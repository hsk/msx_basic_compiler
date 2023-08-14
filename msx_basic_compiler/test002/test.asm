        DEFB        0xFE
        DEFW        start_address
        DEFW        end_address
        DEFW        start_address
        ORG         0x8010
start_address:
        LD          [save_stack], SP
program_start:
bios_chgclr                     = 0x00062
work_forclr                     = 0x0F3E9
work_bakclr                     = 0x0F3EA
work_bdrclr                     = 0x0F3EB
bios_chgmod                     = 0x0005F
work_rg9sv                      = 0x0ffe8
bios_wrtvdp                     = 0x00047
        LD          HL, 15
        LD          A, L
        LD          [work_forclr], A
        LD          HL, 12
        LD          A, L
        LD          [work_bakclr], A
        LD          HL, 3
        LD          A, L
        LD          [work_bdrclr], A
        CALL        bios_chgclr
        LD          HL, 1
        LD          A, L
        CALL        bios_chgmod
        LD          HL, 3
        LD          A, L
        AND         A, 3
        ADD         A, A
        ADD         A, A
        LD          L, A
        LD          A, [work_rg9sv]
        AND         A, 0xF3
        OR          A, L
        LD          B, A
        LD          C, 9
        CALL        bios_wrtvdp
program_termination:
        LD          SP, [save_stack]
        RET
save_stack:
        DEFW        0
end_address:
