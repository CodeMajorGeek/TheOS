.code16

.global init

init:
    jmp init # Jump to init (loop)

.fill 510 - (.-init), 1, 0 # Fill with 0 to make it 510 bytes long
.word 0xaa55 # Tell the BIOS that is bootable