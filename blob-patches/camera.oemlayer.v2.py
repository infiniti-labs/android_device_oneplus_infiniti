# SPDX-License-Identifier: Apache-2.0

import hashlib
import os
import re
import struct
import subprocess
import sys
import tempfile
from pathlib import Path


ORIGINAL_SHA256 = '87457cf6cf90d3e8dc1cbdcaad7d1adf610f98dcb15a288c9b902d76bfa9101d'
PAYLOAD_ADDRESS = 0x2C7000
PAYLOAD_LIMIT = 0x2C8000
CONFIGURE_HOOK = 0x11DA88
METADATA_HOOK = 0x1A8AAC
EXECUTABLE_HEADER = 64 + 2 * 56
ORIGINAL_EXECUTABLE_SIZE = 0x1AAFF0
ORIGINAL_EXECUTABLE_END = 0x2C6FF0
MARKER = b'infiniti-camera-fixup-v1\0'


def branch(source, target):
    distance = target - source
    if distance % 4 or not -(1 << 27) <= distance < (1 << 27):
        raise ValueError('Camera patch branch is out of range')
    return struct.pack('<I', 0x14000000 | ((distance >> 2) & 0x3FFFFFF))


def blob_fixup(ctx, file, file_path, *args, **kwargs):
    source = Path(__file__).with_suffix('.S')
    root = Path(os.environ.get('ANDROID_BUILD_TOP', source.parents[4]))
    tool_dir = (root / 'prebuilts/clang/host/linux-x86/llvm-binutils-stable/llvm-objdump').resolve().parent
    data = bytearray(Path(file_path).read_bytes())
    already_patched = MARKER in data[PAYLOAD_ADDRESS:PAYLOAD_LIMIT]
    original = bytearray(data)
    if already_patched:
        original[CONFIGURE_HOOK:CONFIGURE_HOOK + 4] = bytes.fromhex('ffc301d1')
        original[METADATA_HOOK:METADATA_HOOK + 4] = branch(METADATA_HOOK, 0x2BF1F0)
        original[ORIGINAL_EXECUTABLE_END:PAYLOAD_LIMIT] = bytes(PAYLOAD_LIMIT - ORIGINAL_EXECUTABLE_END)
        struct.pack_into('<QQ', original, EXECUTABLE_HEADER + 32, *(ORIGINAL_EXECUTABLE_SIZE,) * 2)
    if hashlib.sha256(original).hexdigest() != ORIGINAL_SHA256:
        raise ValueError('Camera blob changed; recheck the extraction patch before updating it')

    plt = subprocess.check_output(
        [tool_dir / 'llvm-objdump', '-d', '--section=.plt', file_path], text=True
    )
    definitions = {
        'configure_resume': CONFIGURE_HOOK + 4,
        'init_capabilities': 0x1A9BC0,
    }
    for name, symbol in (
        ('find_entry', 'find_camera_metadata_entry'),
        ('update_entry', 'update_camera_metadata_entry'),
        ('delete_entry', 'delete_camera_metadata_entry'),
    ):
        match = re.search(rf'^([0-9a-f]+) <{symbol}@plt>:', plt, re.MULTILINE)
        if match is None:
            raise ValueError(f'Missing camera metadata entry point: {symbol}')
        definitions[name] = int(match[1], 16)

    temporary_root = Path(os.environ.get('TMPDIR', root / 'out' / '.tmp'))
    temporary_root.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(dir=temporary_root) as temporary:
        obj = Path(temporary) / 'camera.o'
        elf = Path(temporary) / 'camera.elf'
        binary = Path(temporary) / 'camera.bin'
        subprocess.run(
            [tool_dir / 'clang', '--target=aarch64-linux-android', '-c', source, '-o', obj],
            check=True,
        )
        subprocess.run(
            [tool_dir / 'ld.lld', '-static', f'-Ttext={PAYLOAD_ADDRESS:#x}',
             '--entry=configure_streams_fixup',
             *(f'--defsym={name}={address:#x}' for name, address in definitions.items()),
             obj, '-o', elf], check=True,
        )
        symbols = subprocess.check_output([tool_dir / 'llvm-nm', '--format=posix', elf], text=True)
        metadata_address = int(re.search(r'^logical_high_speed_fixup T ([0-9a-f]+)', symbols, re.MULTILINE)[1], 16)
        subprocess.run([tool_dir / 'llvm-objcopy', '-O', 'binary', '--only-section=.text', elf, binary], check=True)
        payload = binary.read_bytes() + MARKER

    if PAYLOAD_ADDRESS + len(payload) > PAYLOAD_LIMIT:
        raise ValueError('Camera patch exceeds executable padding')
    if not PAYLOAD_ADDRESS <= metadata_address < PAYLOAD_ADDRESS + len(payload):
        raise ValueError('Camera metadata patch is outside the payload')
    executable_size = PAYLOAD_ADDRESS + len(payload) - 0x11C000
    if already_patched:
        expected_padding = (
            bytes(PAYLOAD_ADDRESS - ORIGINAL_EXECUTABLE_END) + payload +
            bytes(PAYLOAD_LIMIT - PAYLOAD_ADDRESS - len(payload))
        )
        if (data[ORIGINAL_EXECUTABLE_END:PAYLOAD_LIMIT] != expected_padding or
                struct.unpack_from('<QQ', data, EXECUTABLE_HEADER + 32) != (executable_size,) * 2 or
                data[CONFIGURE_HOOK:CONFIGURE_HOOK + 4] != branch(CONFIGURE_HOOK, PAYLOAD_ADDRESS) or
                data[METADATA_HOOK:METADATA_HOOK + 4] != branch(METADATA_HOOK, metadata_address)):
            raise ValueError('Incomplete camera blob patch')
        return
    if any(data[ORIGINAL_EXECUTABLE_END:PAYLOAD_LIMIT]):
        raise ValueError('Camera executable padding is occupied')
    if data[CONFIGURE_HOOK:CONFIGURE_HOOK + 4] != bytes.fromhex('ffc301d1'):
        raise ValueError('Unexpected configure_streams prologue')
    if data[METADATA_HOOK:METADATA_HOOK + 4] != branch(METADATA_HOOK, 0x2BF1F0):
        raise ValueError('Unexpected static metadata tail call')
    if struct.unpack_from('<QQ', data, EXECUTABLE_HEADER + 32) != (ORIGINAL_EXECUTABLE_SIZE,) * 2:
        raise ValueError('Unexpected camera executable segment')

    data[PAYLOAD_ADDRESS:PAYLOAD_ADDRESS + len(payload)] = payload
    data[CONFIGURE_HOOK:CONFIGURE_HOOK + 4] = branch(CONFIGURE_HOOK, PAYLOAD_ADDRESS)
    data[METADATA_HOOK:METADATA_HOOK + 4] = branch(METADATA_HOOK, metadata_address)
    struct.pack_into('<QQ', data, EXECUTABLE_HEADER + 32, executable_size, executable_size)
    Path(file_path).write_bytes(data)


if __name__ == '__main__':
    blob_fixup(None, None, sys.argv[1])
