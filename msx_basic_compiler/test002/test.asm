; ------------------------------------------------------------------------
; Compiled by MSX-BACON from test.asc
; ------------------------------------------------------------------------
; 
bios_syntax_error               = 0x4055
bios_calslt                     = 0x001C
bios_enaslt                     = 0x0024
work_mainrom                    = 0xFCC1
work_blibslot                   = 0xF3D3
signature                       = 0x4010
work_dac                        = 0x0f7f6
work_dac_int                    = 0x0f7f8
work_valtyp                     = 0x0f663
bios_frcdbl                     = 0x0303a
; BSAVE header -----------------------------------------------------------
        DEFB        0xfe
        DEFW        start_address
        DEFW        end_address
        DEFW        start_address
        ORG         0x8010
start_address:
        LD          [save_stack], SP
        CALL        check_blib
        JP          NZ, bios_syntax_error
        LD          DE, program_start
        JP          program_run
jp_hl:
        JP          HL
program_start:
line_100:
; TEST
        LD          HL, vard_A
        PUSH        HL
        LD          HL, 1
        LD          [work_dac_int], HL
        LD          A, 2
        LD          [work_valtyp], A
        CALL        bios_frcdbl
        LD          HL, work_dac
        POP         DE
        CALL        ld_de_double_real
        LD          HL, data_110
        LD          [data_ptr], HL
program_termination:
        LD          SP, [save_stack]
        RET         
program_run:
        LD          HL, heap_start
        LD          [heap_next], HL
        LD          HL, [save_stack]
        LD          SP, HL
        PUSH        DE
        LD          DE, 256
        XOR         A, A
        SBC         HL, DE
        LD          [heap_end], HL
        RET         
check_blib:
        LD          a, [work_blibslot]
        LD          h, 0x40
        CALL        bios_enaslt
        LD          bc, 8
        LD          hl, signature
        LD          de, signature_ref
_check_blib_loop:
        LD          a, [de]
        INC         de
        CPI         
        JR          NZ, _check_blib_exit
        JP          PE, _check_blib_loop
_check_blib_exit:
        PUSH        af
        LD          a, [work_mainrom]
        LD          h, 0x40
        CALL        bios_enaslt
        EI          
        POP         af
        RET         
signature_ref:
        DEFB        "BACONLIB"
call_blib:
        LD          iy, [work_blibslot - 1]
        JP          bios_calslt
ld_de_double_real:
        LD          BC, 8
        LDIR        
        RET         
data_110:
        DEFW        str_0
        DEFW        str_1
        DEFW        str_2
        DEFW        str_3
        DEFW        str_4
        DEFW        str_0
data_120:
        DEFW        str_5
        DEFW        str_6
data_140:
        DEFW        str_7
str_0:
        DEFB        0x03, 0x31, 0x32, 0x33
str_1:
        DEFB        0x05, 0x26, 0x48, 0x46, 0x30, 0x30
str_2:
        DEFB        0x03, 0x41, 0x42, 0x43
str_3:
        DEFB        0x03, 0x44, 0x45, 0x46
str_4:
        DEFB        0x02, 0x2D, 0x31
str_5:
        DEFB        0x02, 0x31, 0x30
str_6:
        DEFB        0x02, 0x32, 0x30
str_7:
        DEFB        0x08, 0x52, 0x45, 0x4D, 0x20, 0x54, 0x45, 0x53, 0x54
save_stack:
        DEFW        0
heap_next:
        DEFW        0
heap_end:
        DEFW        0
heap_move_size:
        DEFW        0
heap_remap_address:
        DEFW        0
data_ptr:
        DEFW        data_110
var_area_start:
vard_A:
        DEFW        0, 0, 0, 0
var_area_end:
vars_area_start:
vars_area_end:
vara_area_start:
vara_area_end:
varsa_area_start:
varsa_area_end:
heap_start:
end_address:
