/*
 *
 * This is the only way to debug for now...
 *
 * R4 = 0 will cause FLDR continue
 * R4 = other will cause power down
 *
 */


.arm

.global fldr_init
.global fldr_main

.global _start
_start:
    str lr, _return_addr
    push {r1-r12}
    ldr r0, _init
    blx r0
    ldr r0, _main
    blx r0
    pop {r1-r12}
    mov r4, r0
    ldr r0, _return_addr
    bx  r0

.global _abort
_abort:
    .word 0xfefefefe

_init:
.word fldr_init + 1
_main:
.word fldr_main + 1

.global _return_addr
_return_addr:
.word 0

