// SPDX-License-Identifier: Apache-2.0

.text
.balign 16
.global configure_streams_fixup
.global logical_high_speed_fixup

.equ VIDEO_STABILIZATION_TAG, 0x10011
.equ FPS_RANGE_TAG, 0x10005
.equ HIGH_SPEED_CONFIGURATIONS_TAG, 0x10023
.equ CAPABILITIES_TAG, 0xc000c
.equ CHARACTERISTICS_KEYS_TAG, 0xc000f
.equ HIGH_SPEED_CAPABILITY, 9
.equ LOGICAL_CAMERA_CAPABILITY, 11
.equ OPLUS_VIDEO_30_MODE, 0x800b
.equ OPLUS_VIDEO_60_MODE, 0x8021
.equ OPLUS_VIDEO_8K_MODE, 0x80a9

configure_streams_fixup:
    stp x29, x30, [sp, #-80]!
    stp x0, x1, [sp, #16]
    mov x29, sp
    cbz x0, configure_done
    cbz x1, configure_done
    ldr w8, [x1, #16]
    cbnz w8, configure_done
    ldr x0, [x1, #24]
    cbz x0, configure_done
    mov w1, #(VIDEO_STABILIZATION_TAG & 0xffff)
    movk w1, #(VIDEO_STABILIZATION_TAG >> 16), lsl #16
    add x2, sp, #32
    bl find_entry
    cbnz w0, configure_done
    ldr x8, [sp, #48]
    cmp x8, #1
    b.ne configure_done
    ldr x8, [sp, #56]
    ldrb w8, [x8]
    cmp w8, #1
    b.ne configure_done
    ldr x1, [sp, #24]
    ldr x0, [x1, #24]
    mov w1, #(FPS_RANGE_TAG & 0xffff)
    movk w1, #(FPS_RANGE_TAG >> 16), lsl #16
    add x2, sp, #32
    bl find_entry
    cbnz w0, configure_done
    ldr x8, [sp, #48]
    cmp x8, #2
    b.ne configure_done
    ldr x8, [sp, #56]
    ldp w13, w14, [x8]
    cmp w13, #1
    b.lt configure_done
    cmp w13, w14
    b.gt configure_done
    cmp w14, #30
    b.eq configure_fps_valid
    cmp w14, #60
    b.ne configure_done
configure_fps_valid:
    ldr x1, [sp, #24]
    ldr w9, [x1]
    ldr x10, [x1, #8]
    cbz w9, configure_done
    mov w11, wzr
configure_stream_loop:
    ldr x12, [x10], #8
    ldr w8, [x12]
    cbnz w8, configure_done
    ldr w8, [x12, #16]
    tbz w8, #16, configure_next_stream
    ldr w8, [x12, #4]
    mov w15, #7680
    cmp w8, w15
    b.eq configure_8k
    cmp w8, #3840
    b.ne configure_next_stream
    ldr w8, [x12, #8]
    cmp w8, #2160
    b.ne configure_next_stream
    mov w8, #OPLUS_VIDEO_8K_MODE
    cmp w11, w8
    b.eq configure_next_stream
    mov w11, #OPLUS_VIDEO_30_MODE
    mov w8, #OPLUS_VIDEO_60_MODE
    cmp w14, #60
    csel w11, w8, w11, eq
    b configure_next_stream
configure_8k:
    ldr w8, [x12, #8]
    mov w15, #4320
    cmp w8, w15
    b.ne configure_done
    cmp w14, #30
    b.ne configure_done
    mov w11, #OPLUS_VIDEO_8K_MODE
configure_next_stream:
    subs w9, w9, #1
    b.ne configure_stream_loop
    cbz w11, configure_done
    str w11, [x1, #16]
configure_done:
    ldp x0, x1, [sp, #16]
    ldp x29, x30, [sp], #80
    sub sp, sp, #0x70
    b configure_resume

.balign 16
logical_high_speed_fixup:
    paciasp
    sub sp, sp, #0x1000
    stp x29, x30, [sp, #-128]!
    stp x19, x20, [sp, #16]
    mov x29, sp
    mov x19, x1
    bl init_capabilities
    cbz x19, metadata_done
    ldr x19, [x19, #16]
    cbz x19, metadata_done
    mov x0, x19
    mov w1, #(CAPABILITIES_TAG & 0xffff)
    movk w1, #(CAPABILITIES_TAG >> 16), lsl #16
    add x2, sp, #32
    bl find_entry
    cbnz w0, metadata_done
    ldr x9, [sp, #48]
    cbz x9, metadata_done
    cmp x9, #256
    b.hi metadata_done
    ldr x10, [sp, #56]
    add x2, sp, #128
    mov x3, xzr
    mov w11, wzr
capability_loop:
    ldrb w8, [x10], #1
    cmp w8, #LOGICAL_CAMERA_CAPABILITY
    b.ne capability_check_high_speed
    orr w11, w11, #1
capability_check_high_speed:
    cmp w8, #HIGH_SPEED_CAPABILITY
    b.ne capability_keep
    orr w11, w11, #2
    b capability_next
capability_keep:
    strb w8, [x2, x3]
    add x3, x3, #1
capability_next:
    subs x9, x9, #1
    b.ne capability_loop
    cmp w11, #3
    b.ne metadata_done
    mov x0, x19
    ldr x1, [sp, #32]
    mov x4, xzr
    bl update_entry
    cbnz w0, metadata_done
    mov x0, x19
    mov w1, #(CHARACTERISTICS_KEYS_TAG & 0xffff)
    movk w1, #(CHARACTERISTICS_KEYS_TAG >> 16), lsl #16
    add x2, sp, #32
    bl find_entry
    cbnz w0, remove_high_speed_config
    ldr x9, [sp, #48]
    cmp x9, #1024
    b.hi remove_high_speed_config
    ldr x10, [sp, #56]
    add x2, sp, #128
    mov x3, xzr
    mov w12, #(HIGH_SPEED_CONFIGURATIONS_TAG & 0xffff)
    movk w12, #(HIGH_SPEED_CONFIGURATIONS_TAG >> 16), lsl #16
    cbz x9, remove_high_speed_config
characteristic_loop:
    ldr w8, [x10], #4
    cmp w8, w12
    b.eq characteristic_next
    str w8, [x2, x3, lsl #2]
    add x3, x3, #1
characteristic_next:
    subs x9, x9, #1
    b.ne characteristic_loop
    mov x0, x19
    ldr x1, [sp, #32]
    mov x4, xzr
    bl update_entry
remove_high_speed_config:
    mov x0, x19
    mov w1, #(HIGH_SPEED_CONFIGURATIONS_TAG & 0xffff)
    movk w1, #(HIGH_SPEED_CONFIGURATIONS_TAG >> 16), lsl #16
    add x2, sp, #32
    bl find_entry
    cbnz w0, metadata_done
    mov x0, x19
    ldr x1, [sp, #32]
    bl delete_entry
metadata_done:
    ldp x19, x20, [sp, #16]
    ldp x29, x30, [sp], #128
    add sp, sp, #0x1000
    autiasp
    ret
