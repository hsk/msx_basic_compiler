// --------------------------------------------------------------------
//	Expression
// ====================================================================
//	2023/July/29th	t.hara
// --------------------------------------------------------------------
#include "expression.h"

#include "expression_operator_eqv.h"
#include "expression_operator_imp.h"
#include "expression_operator_xor.h"
#include "expression_operator_or.h"
#include "expression_operator_and.h"
#include "expression_operator_not.h"
#include "expression_operator_equ.h"
#include "expression_operator_neq.h"
#include "expression_operator_gt.h"
#include "expression_operator_ge.h"
#include "expression_operator_lt.h"
#include "expression_operator_le.h"
#include "expression_operator_add.h"
#include "expression_operator_sub.h"
#include "expression_operator_intdiv.h"
#include "expression_operator_mod.h"
#include "expression_operator_mul.h"
#include "expression_operator_div.h"
#include "expression_operator_minus.h"
#include "expression_operator_power.h"
#include "expression_function.h"
#include "expression_term.h"
#include "expression_variable.h"

#include "expression_abs.h"
#include "expression_asc.h"
#include "expression_atn.h"
#include "expression_base.h"
#include "expression_bin.h"
#include "expression_cdbl.h"
#include "expression_chr.h"
#include "expression_chrhex.h"
#include "expression_cint.h"
#include "expression_cos.h"
#include "expression_csng.h"
#include "expression_csrlin.h"
#include "expression_cvd.h"
#include "expression_cvi.h"
#include "expression_cvs.h"
#include "expression_exp.h"
#include "expression_fix.h"
#include "expression_fre.h"
#include "expression_hex.h"
#include "expression_hexchr.h"
#include "expression_inkey.h"
#include "expression_inp.h"
#include "expression_input.h"
#include "expression_int.h"
#include "expression_instr.h"
#include "expression_irnd.h"
#include "expression_left.h"
#include "expression_len.h"
#include "expression_log.h"
#include "expression_mid.h"
#include "expression_mkd.h"
#include "expression_mki.h"
#include "expression_mks.h"
#include "expression_oct.h"
#include "expression_pad.h"
#include "expression_peek.h"
#include "expression_peekw.h"
#include "expression_peeks.h"
#include "expression_right.h"
#include "expression_rnd.h"
#include "expression_sgn.h"
#include "expression_str.h"
#include "expression_string.h"
#include "expression_sgn.h"
#include "expression_sin.h"
#include "expression_space.h"
#include "expression_stick.h"
#include "expression_strig.h"
#include "expression_tan.h"
#include "expression_time.h"
#include "expression_usr.h"
#include "expression_val.h"
#include "expression_varptr.h"
#include "expression_vdp.h"
#include "expression_vpeek.h"
#include "expression_vpeeks.h"

// --------------------------------------------------------------------
void CEXPRESSION::optimization( void ) {
}

// --------------------------------------------------------------------
void CEXPRESSION_NODE::type_adjust_2op( CCOMPILE_INFO *p_info, CEXPRESSION_NODE *p_left, CEXPRESSION_NODE *p_right ) {
	CASSEMBLER_LINE asm_line;

	if( p_left->type == p_right->type ) {
		//	左右の項が同じ型の場合
		if( p_left->type == CEXPRESSION_TYPE::INTEGER ) {
			this->type = CEXPRESSION_TYPE::INTEGER;
		}
		else if( p_left->type == CEXPRESSION_TYPE::STRING ) {
			this->type = CEXPRESSION_TYPE::STRING;
		}
		else if( p_right->type == CEXPRESSION_TYPE::SINGLE_REAL ) {
			p_info->assembler_list.add_label( "work_arg", "0x0f847" );
			p_info->assembler_list.add_label( "work_dac", "0x0f7f6" );
			p_info->assembler_list.activate_pop_single_real_dac();
			this->type = CEXPRESSION_TYPE::DOUBLE_REAL;	//	倍精度に昇格
			//	右項を ARG へ
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "DE", COPERAND_TYPE::CONSTANT, "work_arg" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "BC", COPERAND_TYPE::CONSTANT, "4" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LDIR, CCONDITION::NONE, COPERAND_TYPE::NONE, "", COPERAND_TYPE::NONE, "" );
			p_info->assembler_list.body.push_back( asm_line );
			//	左項を DAC へ
			asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::CONSTANT, "pop_single_real_dac", COPERAND_TYPE::NONE, "" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY, "[work_dac+4]", COPERAND_TYPE::REGISTER, "HL" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY, "[work_dac+6]", COPERAND_TYPE::REGISTER, "HL" );
			p_info->assembler_list.body.push_back( asm_line );
		}
		else {
			p_info->assembler_list.activate_pop_double_real_dac();
			this->type = CEXPRESSION_TYPE::DOUBLE_REAL;
			p_info->assembler_list.add_label( "work_arg", "0x0f847" );
			p_info->assembler_list.add_label( "work_dac", "0x0f7f6" );
			//	右項を ARG へ
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "DE", COPERAND_TYPE::CONSTANT, "work_arg" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "BC", COPERAND_TYPE::CONSTANT, "8" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LDIR, CCONDITION::NONE, COPERAND_TYPE::NONE, "", COPERAND_TYPE::NONE, "" );
			p_info->assembler_list.body.push_back( asm_line );
			//	左項を DAC へ
			asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::CONSTANT, "pop_double_real_dac", COPERAND_TYPE::NONE, "" );
			p_info->assembler_list.body.push_back( asm_line );
		}
	}
	else if( p_left->type == CEXPRESSION_TYPE::STRING || p_right->type == CEXPRESSION_TYPE::STRING ) {
		//	左右の項が異なる型で、片方が文字列型ならエラー
		p_info->errors.add( TYPE_MISMATCH, p_info->list.get_line_no() );
		return;
	}
	else {
		//	倍精度で計算するので倍精度に昇格
		this->type = CEXPRESSION_TYPE::DOUBLE_REAL;
		//	右項は倍精度にして ARG へ転送
		if( p_right->type == CEXPRESSION_TYPE::INTEGER ) {
			//	整数から倍精度実数に昇格
			p_info->assembler_list.add_label( "bios_maf", "0x02c4d" );
			p_info->assembler_list.add_label( "bios_frcdbl", "0x0303a" );
			p_info->assembler_list.add_label( "work_dac", "0x0f7f6" );
			p_info->assembler_list.add_label( "work_valtyp", "0x0f663" );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY, "[work_dac + 2]", COPERAND_TYPE::REGISTER, "HL" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "A", COPERAND_TYPE::CONSTANT, "2" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY, "[work_valtyp]", COPERAND_TYPE::REGISTER, "A" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::CONSTANT, "bios_frcdbl", COPERAND_TYPE::NONE, "" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::CONSTANT, "bios_maf", COPERAND_TYPE::NONE, "" );
			p_info->assembler_list.body.push_back( asm_line );
		}
		else if( p_right->type == CEXPRESSION_TYPE::SINGLE_REAL ) {
			p_info->assembler_list.activate_ld_arg_single_real();
			asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::CONSTANT, "ld_arg_single_real", COPERAND_TYPE::NONE, "" );
			p_info->assembler_list.body.push_back( asm_line );
		}
		else {
			p_info->assembler_list.activate_ld_arg_double_real();
			asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::CONSTANT, "ld_arg_double_real", COPERAND_TYPE::NONE, "" );
			p_info->assembler_list.body.push_back( asm_line );
		}

		//	左項は倍精度にして DAC へ転送
		if( p_left->type == CEXPRESSION_TYPE::INTEGER ) {
			//	整数から倍精度実数に昇格
			p_info->assembler_list.add_label( "bios_frcdbl", "0x0303a" );
			p_info->assembler_list.add_label( "work_dac", "0x0f7f6" );
			p_info->assembler_list.add_label( "work_valtyp", "0x0f663" );
			asm_line.set( CMNEMONIC_TYPE::POP, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "HL", COPERAND_TYPE::NONE, "" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY, "[work_dac + 2]", COPERAND_TYPE::REGISTER, "HL" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "A", COPERAND_TYPE::CONSTANT, "2" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY, "[work_valtyp]", COPERAND_TYPE::REGISTER, "A" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::CONSTANT, "bios_frcdbl", COPERAND_TYPE::NONE, "" );
			p_info->assembler_list.body.push_back( asm_line );
		}
		else if( p_left->type == CEXPRESSION_TYPE::SINGLE_REAL ) {
			p_info->assembler_list.activate_pop_single_real_dac();
			asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::CONSTANT, "pop_single_real_dac", COPERAND_TYPE::NONE, "" );
			p_info->assembler_list.body.push_back( asm_line );
		}
		else {
			p_info->assembler_list.activate_pop_double_real_dac();
			asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::CONSTANT, "pop_double_real_dac", COPERAND_TYPE::NONE, "" );
			p_info->assembler_list.body.push_back( asm_line );
		}
	}
}

// --------------------------------------------------------------------
void CEXPRESSION_NODE::convert_type( CCOMPILE_INFO *p_info, CEXPRESSION_TYPE target, CEXPRESSION_TYPE current ) {
	CASSEMBLER_LINE asm_line;

	if( target == current ) {
		return;
	}
	if( target == CEXPRESSION_TYPE::EXTENDED_INTEGER ) {
		if( current == CEXPRESSION_TYPE::INTEGER ) {
			//	変換の必要なし
			return;
		}
		if( current == CEXPRESSION_TYPE::SINGLE_REAL ) {
			p_info->assembler_list.activate_convert_to_integer_from_sngle_real( &(p_info->constants) );
			asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::CONSTANT, "convert_to_integer_from_sngle_real", COPERAND_TYPE::NONE, "" );
			p_info->assembler_list.body.push_back( asm_line );
			return;
		}
		if( current == CEXPRESSION_TYPE::DOUBLE_REAL ) {
			p_info->assembler_list.activate_convert_to_integer_from_double_real( &(p_info->constants) );
			asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::CONSTANT, "convert_to_integer_from_double_real", COPERAND_TYPE::NONE, "" );
			p_info->assembler_list.body.push_back( asm_line );
			return;
		}
		p_info->errors.add( TYPE_MISMATCH, p_info->list.get_line_no() );
		return;
	}

	p_info->assembler_list.add_label( "work_dac", "0x0f7f6" );

	if( target == CEXPRESSION_TYPE::INTEGER ) {
		p_info->assembler_list.add_label( "work_valtyp", "0x0f663" );
		p_info->assembler_list.add_label( "bios_frcint", "0x02f8a" );

		asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "DE", COPERAND_TYPE::CONSTANT, "work_dac" );
		p_info->assembler_list.body.push_back( asm_line );
		if( current == CEXPRESSION_TYPE::SINGLE_REAL ) {
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "BC", COPERAND_TYPE::CONSTANT, "4" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LDIR, CCONDITION::NONE, COPERAND_TYPE::NONE, "", COPERAND_TYPE::NONE, "" ); 
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::CONSTANT, "[work_dac+4]", COPERAND_TYPE::REGISTER, "BC" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::CONSTANT, "[work_dac+6]", COPERAND_TYPE::REGISTER, "BC" );
			p_info->assembler_list.body.push_back( asm_line );
		}
		else {
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "BC", COPERAND_TYPE::CONSTANT, "8" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LDIR, CCONDITION::NONE, COPERAND_TYPE::NONE, "", COPERAND_TYPE::NONE, "" ); 
			p_info->assembler_list.body.push_back( asm_line );
		}
		asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "A", COPERAND_TYPE::CONSTANT, "8" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY, "[work_valtyp]", COPERAND_TYPE::REGISTER, "A" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::CONSTANT, "bios_frcint", COPERAND_TYPE::NONE, "" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "HL", COPERAND_TYPE::MEMORY, "[work_dac + 2]" );
		p_info->assembler_list.body.push_back( asm_line );
	}
	else if( target == CEXPRESSION_TYPE::SINGLE_REAL ) {
		p_info->assembler_list.add_label( "work_valtyp", "0x0f663" );
		p_info->assembler_list.add_label( "bios_frcsng", "0x02fb2" );
		if( current == CEXPRESSION_TYPE::INTEGER ) {
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY, "[work_dac + 2]", COPERAND_TYPE::REGISTER, "HL" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "A", COPERAND_TYPE::REGISTER, "2" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY, "[work_valtyp]", COPERAND_TYPE::REGISTER, "A" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::CONSTANT, "bios_frcsng", COPERAND_TYPE::NONE, "" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "HL", COPERAND_TYPE::MEMORY, "work_dac" );
			p_info->assembler_list.body.push_back( asm_line );
		}
		else if( current == CEXPRESSION_TYPE::DOUBLE_REAL ) {
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "DE", COPERAND_TYPE::REGISTER, "work_dac" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "BC", COPERAND_TYPE::REGISTER, "8" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LDIR, CCONDITION::NONE, COPERAND_TYPE::NONE, "", COPERAND_TYPE::NONE, "" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "A", COPERAND_TYPE::REGISTER, "8" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY, "[work_valtyp]", COPERAND_TYPE::REGISTER, "A" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::CONSTANT, "bios_frcsng", COPERAND_TYPE::NONE, "" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "HL", COPERAND_TYPE::MEMORY, "work_dac" );
			p_info->assembler_list.body.push_back( asm_line );
		}
	}
	else if( target == CEXPRESSION_TYPE::DOUBLE_REAL ) {
		p_info->assembler_list.add_label( "work_valtyp", "0x0f663" );
		p_info->assembler_list.add_label( "bios_frcdbl", "0x0303a" );
		if( current == CEXPRESSION_TYPE::INTEGER ) {
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY, "[work_dac + 2]", COPERAND_TYPE::REGISTER, "HL" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "A", COPERAND_TYPE::REGISTER, "2" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY, "[work_valtyp]", COPERAND_TYPE::REGISTER, "A" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::CONSTANT, "bios_frcdbl", COPERAND_TYPE::NONE, "" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "HL", COPERAND_TYPE::MEMORY, "work_dac" );
			p_info->assembler_list.body.push_back( asm_line );
		}
		else if( current == CEXPRESSION_TYPE::SINGLE_REAL ) {
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "DE", COPERAND_TYPE::REGISTER, "work_dac" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "BC", COPERAND_TYPE::REGISTER, "4" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LDIR, CCONDITION::NONE, COPERAND_TYPE::NONE, "", COPERAND_TYPE::NONE, "" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "A", COPERAND_TYPE::REGISTER, "4" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY, "[work_valtyp]", COPERAND_TYPE::REGISTER, "A" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::CONSTANT, "bios_frcdbl", COPERAND_TYPE::NONE, "" );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "HL", COPERAND_TYPE::MEMORY, "work_dac" );
			p_info->assembler_list.body.push_back( asm_line );
		}
	}
	else {
	}
}

// --------------------------------------------------------------------
bool CEXPRESSION::check_word( CCOMPILE_INFO *p_info, std::string s, int error_id ) {

	if( p_info->list.is_command_end() || p_info->list.p_position->s_word != s ) {
		if( error_id != CERROR_ID::NO_ERROR ) {
			p_info->errors.add( CERROR_ID( error_id ), p_info->list.get_line_no() );	//	あるべき閉じ括弧
		}
		return false;
	}
	p_info->list.p_position++;
	return true;
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_term( CCOMPILE_INFO *p_info ) {
	CEXPRESSION_NODE *p_result;
	std::string s_operator;

	if( p_info->list.is_command_end() ) {
		return nullptr;															//	値が無い場合は nullptr を返す
	}
	s_operator = p_info->list.p_position->s_word;
	if( s_operator == "(" && p_info->list.p_position->type == CBASIC_WORD_TYPE::SYMBOL ) {
		p_info->list.p_position++;
		p_result = this->makeup_node_operator_eqv( p_info );
		if( p_info->list.is_command_end() || p_info->list.p_position->s_word != ")" || p_info->list.p_position->type != CBASIC_WORD_TYPE::SYMBOL ) {
			p_info->errors.add( MISSING_OPERAND, p_info->list.get_line_no() );	//	あるべき閉じ括弧
			return p_result;
		}
		p_info->list.p_position++;
		return p_result;
	}
	else if( s_operator == "ABS" ) {
		CEXPRESSION_ABS *p_term = new CEXPRESSION_ABS;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "ASC" ) {
		CEXPRESSION_ASC *p_term = new CEXPRESSION_ASC;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "ATN" ) {
		CEXPRESSION_ATN *p_term = new CEXPRESSION_ATN;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "BASE" ) {
		CEXPRESSION_BASE *p_term = new CEXPRESSION_BASE;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "BIN$" ) {
		CEXPRESSION_BIN *p_term = new CEXPRESSION_BIN;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "CDBL" ) {
		CEXPRESSION_CDBL *p_term = new CEXPRESSION_CDBL;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "CHR$" ) {
		CEXPRESSION_CHR *p_term = new CEXPRESSION_CHR;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "CHRHEX$" ) {
		CEXPRESSION_CHRHEX *p_term = new CEXPRESSION_CHRHEX;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
		}
	else if( s_operator == "CINT" ) {
		CEXPRESSION_CINT *p_term = new CEXPRESSION_CINT;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "COS" ) {
		CEXPRESSION_COS *p_term = new CEXPRESSION_COS;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "CSNG" ) {
		CEXPRESSION_CSNG *p_term = new CEXPRESSION_CSNG;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "CSRLIN" ) {
		CEXPRESSION_CSRLIN *p_term = new CEXPRESSION_CSRLIN;
		p_result = p_term;
		p_info->list.p_position++;
		return p_result;
	}
	else if( s_operator == "CVD" ) {
		CEXPRESSION_CVD *p_term = new CEXPRESSION_CVD;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "CVI" ) {
		CEXPRESSION_CVI *p_term = new CEXPRESSION_CVI;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "CVS" ) {
		CEXPRESSION_CVS *p_term = new CEXPRESSION_CVS;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "EXP" ) {
		CEXPRESSION_EXP *p_term = new CEXPRESSION_EXP;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "FIX" ) {
		CEXPRESSION_FIX *p_term = new CEXPRESSION_FIX;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "FRE" ) {
		CEXPRESSION_FRE *p_term = new CEXPRESSION_FRE;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "IRND" ) {
		CEXPRESSION_IRND *p_term = new CEXPRESSION_IRND;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
		}
	else if( s_operator == "HEX$" ) {
		CEXPRESSION_HEX *p_term = new CEXPRESSION_HEX;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "HEXCHR$" ) {
		CEXPRESSION_HEXCHR *p_term = new CEXPRESSION_HEXCHR;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
		}
	else if( s_operator == "INKEY$" ) {
		CEXPRESSION_INKEY *p_term = new CEXPRESSION_INKEY;
		p_result = p_term;
		p_info->list.p_position++;
		return p_result;
	}
	else if( s_operator == "INP" ) {
		CEXPRESSION_INP *p_term = new CEXPRESSION_INP;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "INPUT$" ) {
		CEXPRESSION_INPUT *p_term = new CEXPRESSION_INPUT;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
		}
	else if( s_operator == "INT" ) {
		CEXPRESSION_INT *p_term = new CEXPRESSION_INT;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "INSTR" ) {
		CEXPRESSION_INSTR *p_term = new CEXPRESSION_INSTR;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand1 = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ",", MISSING_OPERAND ) ) {
			return p_result;
		}
		p_term->p_operand2 = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "LEFT$" ) {
		CEXPRESSION_LEFT *p_term = new CEXPRESSION_LEFT;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand1 = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ",", MISSING_OPERAND ) ) {
			return p_result;
		}
		p_term->p_operand2 = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "LEN" ) {
		CEXPRESSION_LEN *p_term = new CEXPRESSION_LEN;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "LOG" ) {
		CEXPRESSION_LOG *p_term = new CEXPRESSION_LOG;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "MID$" ) {
		CEXPRESSION_MID *p_term = new CEXPRESSION_MID;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand1 = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ",", MISSING_OPERAND ) ) {
			return p_result;
		}
		p_term->p_operand2 = this->makeup_node_operator_eqv( p_info );
		p_term->p_operand3 = nullptr;
		if( this->check_word( p_info, ")", NO_ERROR ) ) {
			//	第3引数が省略されている場合
			return p_result;
		}
		if( !this->check_word( p_info, ",", MISSING_OPERAND ) ) {
			return p_result;
		}
		p_term->p_operand3 = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "MKD$" ) {
		CEXPRESSION_MKD *p_term = new CEXPRESSION_MKD;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
		}
	else if( s_operator == "MKI$" ) {
		CEXPRESSION_MKI *p_term = new CEXPRESSION_MKI;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
		}
	else if( s_operator == "MKS$" ) {
		CEXPRESSION_MKS *p_term = new CEXPRESSION_MKS;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "OCT$" ) {
		CEXPRESSION_OCT *p_term = new CEXPRESSION_OCT;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "PAD" ) {
		CEXPRESSION_PAD *p_term = new CEXPRESSION_PAD;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "PEEK" ) {
		CEXPRESSION_PEEK *p_term = new CEXPRESSION_PEEK;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "PEEKW" ) {
		CEXPRESSION_PEEKW *p_term = new CEXPRESSION_PEEKW;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "PEEKS$" ) {
		CEXPRESSION_PEEKS *p_term = new CEXPRESSION_PEEKS;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand1 = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ",", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand2 = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "RIGHT$" ) {
		CEXPRESSION_RIGHT *p_term = new CEXPRESSION_RIGHT;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand1 = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ",", MISSING_OPERAND ) ) {
			return p_result;
		}
		p_term->p_operand2 = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "RND" ) {
		CEXPRESSION_RND *p_term = new CEXPRESSION_RND;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "SGN" ) {
		CEXPRESSION_SGN *p_term = new CEXPRESSION_SGN;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "SIN" ) {
		CEXPRESSION_SIN *p_term = new CEXPRESSION_SIN;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "SPACE$" ) {
		CEXPRESSION_SPACE *p_term = new CEXPRESSION_SPACE;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
		}
	else if( s_operator == "STICK" ) {
		CEXPRESSION_STICK *p_term = new CEXPRESSION_STICK;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "STRIG" ) {
		CEXPRESSION_STRIG *p_term = new CEXPRESSION_STRIG;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "STR$" ) {
		CEXPRESSION_STR *p_term = new CEXPRESSION_STR;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "STRING$" ) {
		CEXPRESSION_STRING *p_term = new CEXPRESSION_STRING;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand1 = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ",", MISSING_OPERAND ) ) {
			return p_result;
		}
		p_term->p_operand2 = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "TAN" ) {
		CEXPRESSION_TAN *p_term = new CEXPRESSION_TAN;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "TIME" ) {
		CEXPRESSION_TIME *p_term = new CEXPRESSION_TIME;
		p_result = p_term;
		p_info->list.p_position++;
		return p_result;
	}
	else if( s_operator == "USR" ) {
		CEXPRESSION_USR *p_term = new CEXPRESSION_USR;
		p_result = p_term;
		p_info->list.p_position++;
		if( p_info->list.is_command_end() ) {
			p_info->errors.add( SYNTAX_ERROR, p_info->list.get_line_no() );
			delete p_term;
			return nullptr;
		}
		if( p_info->list.p_position->s_word.size() == 1 && isdigit( p_info->list.p_position->s_word[0] & 255 ) ) {
			p_term->n = p_info->list.p_position->s_word[0] - '0';
			p_info->list.p_position++;
		}
		else {
			p_term->n = 0;
		}
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		if( p_info->list.is_command_end() || p_info->list.p_position->s_word.size() != 1 ) {
			p_term->type = CEXPRESSION_TYPE::INTEGER;
		}
		else if( p_info->list.p_position->s_word == "%" ) {
			p_term->type = CEXPRESSION_TYPE::INTEGER;
			p_info->list.p_position++;
		}
		else if( p_info->list.p_position->s_word == "!" ) {
			p_term->type = CEXPRESSION_TYPE::SINGLE_REAL;
			p_info->list.p_position++;
		}
		else if( p_info->list.p_position->s_word == "#" ) {
			p_term->type = CEXPRESSION_TYPE::DOUBLE_REAL;
			p_info->list.p_position++;
		}
		else if( p_info->list.p_position->s_word == "$" ) {
			p_term->type = CEXPRESSION_TYPE::STRING;
			p_info->list.p_position++;
		}
		return p_result;
	}
	else if( s_operator == "VAL" ) {
		CEXPRESSION_VAL *p_term = new CEXPRESSION_VAL;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "VARPTR" ) {
		CEXPRESSION_VARPTR *p_term = new CEXPRESSION_VARPTR;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_position = p_info->list.p_position;
		if( !p_info->list.is_command_end() && p_info->list.p_position->s_word == "#" ) {
			//	VARPTR( #1 ) のような使い方の場合
			p_info->list.p_position++;
			if( p_info->list.is_command_end() ) {
				p_info->errors.add( SYNTAX_ERROR, p_info->list.get_line_no() );
				return p_result;
			}
			int n = std::stoi( p_info->list.p_position->s_word );
			if( n < 0 || n > 15 ) {
				p_info->errors.add( ILLEGAL_FUNCTION_CALL, p_info->list.get_line_no() );
				return p_result;
			}
			p_term->file_number = n;
			p_info->list.p_position++;
		}
		else {
			//	VARPTR( A ) のような使い方の場合
			p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		}
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "VDP" ) {
		CEXPRESSION_VDP *p_term = new CEXPRESSION_VDP;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "VPEEK" ) {
		CEXPRESSION_VPEEK *p_term = new CEXPRESSION_VPEEK;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( s_operator == "VPEEKS$" ) {
		CEXPRESSION_VPEEKS *p_term = new CEXPRESSION_VPEEKS;
		p_result = p_term;
		p_info->list.p_position++;
		if( !this->check_word( p_info, "(", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand1 = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ",", SYNTAX_ERROR ) ) {
			delete p_term;
			return nullptr;
		}
		p_term->p_operand2 = this->makeup_node_operator_eqv( p_info );
		if( !this->check_word( p_info, ")", MISSING_OPERAND ) ) {
			return p_result;
		}
		return p_result;
	}
	else if( p_info->list.p_position->type == CBASIC_WORD_TYPE::UNKNOWN_NAME ) {
		//	変数の場合
		CEXPRESSION_VARIABLE *p_term = new CEXPRESSION_VARIABLE;
		CVARIABLE variable = p_info->variable_manager.get_variable_info( p_info, p_term->exp_list );
		p_term->variable = variable;
		p_term->is_variable = true;
		p_result = p_term;
		return p_result;
	}
	else if( p_info->list.p_position->type == CBASIC_WORD_TYPE::INTEGER ) {
		//	整数定数の場合
		CEXPRESSION_TERM *p_term = new CEXPRESSION_TERM;
		p_term->type = CEXPRESSION_TYPE::INTEGER;
		p_term->s_value = s_operator;
		p_result = p_term;
		p_info->list.p_position++;
		return p_result;
	}
	else if( p_info->list.p_position->type == CBASIC_WORD_TYPE::SINGLE_REAL ) {
		//	単精度実数定数の場合
		CEXPRESSION_TERM *p_term = new CEXPRESSION_TERM;
		p_term->type = CEXPRESSION_TYPE::SINGLE_REAL;
		p_term->s_value = s_operator;
		p_result = p_term;
		p_info->list.p_position++;
		return p_result;
	}
	else if( p_info->list.p_position->type == CBASIC_WORD_TYPE::DOUBLE_REAL ) {
		//	倍精度実数定数の場合
		CEXPRESSION_TERM *p_term = new CEXPRESSION_TERM;
		p_term->type = CEXPRESSION_TYPE::DOUBLE_REAL;
		p_term->s_value = s_operator;
		p_result = p_term;
		p_info->list.p_position++;
		return p_result;
	}
	else if( p_info->list.p_position->type == CBASIC_WORD_TYPE::STRING ) {
		//	文字列定数の場合
		CEXPRESSION_TERM *p_term = new CEXPRESSION_TERM;
		p_term->type = CEXPRESSION_TYPE::STRING;
		p_term->s_value = s_operator;
		p_result = p_term;
		p_info->list.p_position++;
		return p_result;
	}
	else if( p_info->list.p_position->type == CBASIC_WORD_TYPE::UNKNOWN_NAME ) {
	}
	else if( s_operator == ":" || s_operator == "," ) {
		return nullptr;
	}
	p_info->errors.add( SYNTAX_ERROR, p_info->list.get_line_no() );
	return nullptr;
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_operator_power( CCOMPILE_INFO *p_info ) {
	CEXPRESSION_NODE *p_left;
	CEXPRESSION_OPERATOR_POWER *p_operator;
	CEXPRESSION_NODE *p_result;
	std::string s_operator;

	//	左項を得る
	p_left = this->makeup_node_term( p_info );
	if( p_left == nullptr ) {
		return nullptr;				//	左項が得られなかった場合
	}
	p_result = p_left;
	while( !p_info->list.is_command_end() ) {
		s_operator = p_info->list.p_position->s_word;
		if( s_operator != "^" || p_info->list.p_position->type != CBASIC_WORD_TYPE::RESERVED_WORD ) {
			//	所望の演算子ではないので左項をそのまま返す
			break;
		}
		p_info->list.p_position++;
		if( p_info->list.is_command_end() ) {
			p_info->errors.add( MISSING_OPERAND, p_info->list.get_line_no() );	//	あるべき右項がない
			break;
		}
		//	この演算子のインスタンスを生成
		p_operator = new CEXPRESSION_OPERATOR_POWER;
		p_operator->p_left = p_result;
		p_operator->p_right = this->makeup_node_term( p_info );
		p_result = p_operator;
	}
	return p_result;
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_operator_minus_plus( CCOMPILE_INFO *p_info ) {
	CEXPRESSION_OPERATOR_MINUS *p_operator_minus;
	CEXPRESSION_NODE *p_result;
	std::string s_operator;

	if( p_info->list.is_command_end() ) {
		return nullptr;
	}
	s_operator = p_info->list.p_position->s_word;
	if( (s_operator != "+" && s_operator != "-") || p_info->list.p_position->type != CBASIC_WORD_TYPE::RESERVED_WORD ) {
		//	所望の演算子ではない
		return this->makeup_node_operator_power( p_info );
	}
	p_info->list.p_position++;
	if( p_info->list.is_command_end() ) {
		p_info->errors.add( MISSING_OPERAND, p_info->list.get_line_no() );	//	あるべき右項がない
		return nullptr;
	}
	//	この演算子の処理
	if( s_operator == "+" ) {
		//	+ は実質何もしない
		return this->makeup_node_operator_minus_plus( p_info );
	}
	else {
		//	if( s_operator == "-" )
		p_operator_minus = new CEXPRESSION_OPERATOR_MINUS;
		p_operator_minus->p_right = this->makeup_node_operator_minus_plus( p_info );
		p_result = p_operator_minus;
	}
	return p_result;
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_operator_mul_div( CCOMPILE_INFO *p_info ) {
	CEXPRESSION_NODE *p_left;
	CEXPRESSION_OPERATOR_MUL *p_operator_mul;
	CEXPRESSION_OPERATOR_DIV *p_operator_div;
	CEXPRESSION_NODE *p_result;
	std::string s_operator;

	//	左項を得る
	p_left = this->makeup_node_operator_minus_plus( p_info );
	if( p_left == nullptr ) {
		return nullptr;				//	左項が得られなかった場合
	}
	p_result = p_left;
	while( !p_info->list.is_command_end() ) {
		s_operator = p_info->list.p_position->s_word;
		if( (s_operator != "*" && s_operator != "/") || p_info->list.p_position->type != CBASIC_WORD_TYPE::RESERVED_WORD ) {
			//	所望の演算子ではないので左項をそのまま返す
			break;
		}
		p_info->list.p_position++;
		if( p_info->list.is_command_end() ) {
			p_info->errors.add( MISSING_OPERAND, p_info->list.get_line_no() );	//	あるべき右項がない
			break;
		}
		//	この演算子のインスタンスを生成
		if( s_operator == "*" ) {
			p_operator_mul = new CEXPRESSION_OPERATOR_MUL;
			p_operator_mul->p_left = p_result;
			p_operator_mul->p_right = this->makeup_node_operator_minus_plus( p_info );
			p_result = p_operator_mul;
		}
		else {
			//	if( s_operator == "-" )
			p_operator_div = new CEXPRESSION_OPERATOR_DIV;
			p_operator_div->p_left = p_result;
			p_operator_div->p_right = this->makeup_node_operator_minus_plus( p_info );
			p_result = p_operator_div;
		}
	}
	return p_result;
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_operator_intdiv( CCOMPILE_INFO *p_info ) {
	CEXPRESSION_NODE *p_left;
	CEXPRESSION_OPERATOR_INTDIV *p_operator;
	CEXPRESSION_NODE *p_result;
	std::string s_operator;

	//	左項を得る
	p_left = this->makeup_node_operator_mul_div( p_info );
	if( p_left == nullptr ) {
		return nullptr;				//	左項が得られなかった場合
	}
	p_result = p_left;
	while( !p_info->list.is_command_end() ) {
		s_operator = p_info->list.p_position->s_word;
		if( s_operator != "\\" || p_info->list.p_position->type != CBASIC_WORD_TYPE::RESERVED_WORD ) {
			//	所望の演算子ではないので左項をそのまま返す
			break;
		}
		p_info->list.p_position++;
		if( p_info->list.is_command_end() ) {
			p_info->errors.add( MISSING_OPERAND, p_info->list.get_line_no() );	//	あるべき右項がない
			break;
		}
		//	この演算子のインスタンスを生成
		p_operator = new CEXPRESSION_OPERATOR_INTDIV;
		p_operator->p_left = p_result;
		p_operator->p_right = this->makeup_node_operator_mul_div( p_info );
		p_result = p_operator;
	}
	return p_result;
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_operator_mod( CCOMPILE_INFO *p_info ) {
	CEXPRESSION_NODE *p_left;
	CEXPRESSION_OPERATOR_MOD *p_operator;
	CEXPRESSION_NODE *p_result;
	std::string s_operator;

	//	左項を得る
	p_left = this->makeup_node_operator_intdiv( p_info );
	if( p_left == nullptr ) {
		return nullptr;				//	左項が得られなかった場合
	}
	p_result = p_left;
	while( !p_info->list.is_command_end() ) {
		s_operator = p_info->list.p_position->s_word;
		if( s_operator != "MOD" ) {
			//	所望の演算子ではないので左項をそのまま返す
			break;
		}
		p_info->list.p_position++;
		if( p_info->list.is_command_end() ) {
			p_info->errors.add( MISSING_OPERAND, p_info->list.get_line_no() );	//	あるべき右項がない
			break;
		}
		//	この演算子のインスタンスを生成
		p_operator = new CEXPRESSION_OPERATOR_MOD;
		p_operator->p_left = p_result;
		p_operator->p_right = this->makeup_node_operator_intdiv( p_info );
		p_result = p_operator;
	}
	return p_result;
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_operator_add_sub( CCOMPILE_INFO *p_info ) {
	CEXPRESSION_NODE *p_left;
	CEXPRESSION_OPERATOR_ADD *p_operator_add;
	CEXPRESSION_OPERATOR_SUB *p_operator_sub;
	CEXPRESSION_NODE *p_result;
	std::string s_operator;

	//	左項を得る
	p_left = this->makeup_node_operator_mod( p_info );
	if( p_left == nullptr ) {
		return nullptr;				//	左項が得られなかった場合
	}
	p_result = p_left;
	while( !p_info->list.is_command_end() ) {
		s_operator = p_info->list.p_position->s_word;
		if( (s_operator != "+" && s_operator != "-") || p_info->list.p_position->type != CBASIC_WORD_TYPE::RESERVED_WORD ) {
			//	所望の演算子ではないので左項をそのまま返す
			break;
		}
		p_info->list.p_position++;
		if( p_info->list.is_command_end() ) {
			p_info->errors.add( MISSING_OPERAND, p_info->list.get_line_no() );	//	あるべき右項がない
			break;
		}
		//	この演算子のインスタンスを生成
		if( s_operator == "+" ) {
			p_operator_add = new CEXPRESSION_OPERATOR_ADD;
			p_operator_add->p_left = p_result;
			p_operator_add->p_right = this->makeup_node_operator_mod( p_info );
			p_result = p_operator_add;
		}
		else {
			//	if( s_operator == "-" )
			p_operator_sub = new CEXPRESSION_OPERATOR_SUB;
			p_operator_sub->p_left = p_result;
			p_operator_sub->p_right = this->makeup_node_operator_mod( p_info );
			p_result = p_operator_sub;
		}
	}
	return p_result;
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_operator_compare( CCOMPILE_INFO *p_info ) {
	CEXPRESSION_NODE *p_left;
	CEXPRESSION_OPERATOR_EQU *p_operator_equ;
	CEXPRESSION_OPERATOR_NEQ *p_operator_neq;
	CEXPRESSION_OPERATOR_GT *p_operator_gt;
	CEXPRESSION_OPERATOR_GE *p_operator_ge;
	CEXPRESSION_OPERATOR_LT *p_operator_lt;
	CEXPRESSION_OPERATOR_LE *p_operator_le;
	CEXPRESSION_NODE *p_result;
	std::string s_operator, s_operator2;

	//	左項を得る
	p_left = this->makeup_node_operator_add_sub( p_info );
	if( p_left == nullptr ) {
		return nullptr;				//	左項が得られなかった場合
	}
	p_result = p_left;
	while( !p_info->list.is_command_end() ) {
		s_operator = p_info->list.p_position->s_word;
		if( (s_operator != "=" && s_operator != "<" && s_operator != ">") || p_info->list.p_position->type != CBASIC_WORD_TYPE::RESERVED_WORD ) {
			//	所望の演算子ではないので左項をそのまま返す
			break;
		}
		p_info->list.p_position++;
		if( p_info->list.is_command_end() ) {
			p_info->errors.add( MISSING_OPERAND, p_info->list.get_line_no() );	//	あるべき右項がない
			break;
		}
		s_operator2 = p_info->list.p_position->s_word;
		if( s_operator2 == "=" || s_operator2 == "<" || s_operator2 == ">" ) {
			//	<>, ><, >=, =>, <=, =< の場合、2つの単語に分かれてるので統合する
			s_operator = s_operator + s_operator2;
			p_info->list.p_position++;
			if( p_info->list.is_command_end() ) {
				p_info->errors.add( MISSING_OPERAND, p_info->list.get_line_no() );	//	あるべき右項がない
				break;
			}
		}
		//	この演算子のインスタンスを生成
		if( s_operator == "=" ) {
			p_operator_equ = new CEXPRESSION_OPERATOR_EQU;
			p_operator_equ->p_left = p_result;
			p_operator_equ->p_right = this->makeup_node_operator_add_sub( p_info );
			p_result = p_operator_equ;
		}
		else if( s_operator == "<>" || s_operator == "><" ) {
			p_operator_neq = new CEXPRESSION_OPERATOR_NEQ;
			p_operator_neq->p_left = p_result;
			p_operator_neq->p_right = this->makeup_node_operator_add_sub( p_info );
			p_result = p_operator_neq;
		}
		else if( s_operator == ">" ) {
			p_operator_gt = new CEXPRESSION_OPERATOR_GT;
			p_operator_gt->p_left = p_result;
			p_operator_gt->p_right = this->makeup_node_operator_add_sub( p_info );
			p_result = p_operator_gt;
		}
		else if( s_operator == "<" ) {
			p_operator_lt = new CEXPRESSION_OPERATOR_LT;
			p_operator_lt->p_left = p_result;
			p_operator_lt->p_right = this->makeup_node_operator_add_sub( p_info );
			p_result = p_operator_lt;
		}
		else if( s_operator == ">=" || s_operator == "=>" ) {
			p_operator_ge = new CEXPRESSION_OPERATOR_GE;
			p_operator_ge->p_left = p_result;
			p_operator_ge->p_right = this->makeup_node_operator_add_sub( p_info );
			p_result = p_operator_ge;
		}
		else { 
			//	if( s_operator == "<=" || s_operator == "=<" )
			p_operator_le = new CEXPRESSION_OPERATOR_LE;
			p_operator_le->p_left = p_result;
			p_operator_le->p_right = this->makeup_node_operator_add_sub( p_info );
			p_result = p_operator_le;
		}
	}
	return p_result;
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_operator_not( CCOMPILE_INFO *p_info ) {
	CEXPRESSION_OPERATOR_NOT *p_operator;
	std::string s_operator;

	if( p_info->list.is_command_end() ) {
		return nullptr;
	}
	s_operator = p_info->list.p_position->s_word;
	if( s_operator != "NOT" || p_info->list.p_position->type != CBASIC_WORD_TYPE::RESERVED_WORD ) {
		//	所望の演算子ではないので右項をそのまま返す
		return this->makeup_node_operator_compare( p_info );
	}
	p_info->list.p_position++;
	if( p_info->list.is_command_end() ) {
		p_info->errors.add( MISSING_OPERAND, p_info->list.get_line_no() );	//	あるべき右項がない
		return nullptr;
	}
	//	この演算子のインスタンスを生成
	p_operator = new CEXPRESSION_OPERATOR_NOT;
	p_operator->p_right = this->makeup_node_operator_not( p_info );
	return p_operator;
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_operator_and( CCOMPILE_INFO *p_info ) {
	CEXPRESSION_NODE *p_left;
	CEXPRESSION_OPERATOR_AND *p_operator;
	CEXPRESSION_NODE *p_result;
	std::string s_operator;

	//	左項を得る
	p_left = this->makeup_node_operator_not( p_info );
	if( p_left == nullptr ) {
		return nullptr;				//	左項が得られなかった場合
	}
	p_result = p_left;
	while( !p_info->list.is_command_end() ) {
		s_operator = p_info->list.p_position->s_word;
		if( s_operator != "AND" || p_info->list.p_position->type != CBASIC_WORD_TYPE::RESERVED_WORD ) {
			//	所望の演算子ではないので左項をそのまま返す
			break;
		}
		p_info->list.p_position++;
		if( p_info->list.is_command_end() ) {
			p_info->errors.add( MISSING_OPERAND, p_info->list.get_line_no() );	//	あるべき右項がない
			break;
		}
		//	この演算子のインスタンスを生成
		p_operator = new CEXPRESSION_OPERATOR_AND;
		p_operator->p_left = p_result;
		p_operator->p_right = this->makeup_node_operator_not( p_info );
		p_result = p_operator;
	}
	return p_result;
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_operator_or( CCOMPILE_INFO *p_info ) {
	CEXPRESSION_NODE *p_left;
	CEXPRESSION_OPERATOR_OR *p_operator;
	CEXPRESSION_NODE *p_result;
	std::string s_operator;

	//	左項を得る
	p_left = this->makeup_node_operator_and( p_info );
	if( p_left == nullptr ) {
		return nullptr;				//	左項が得られなかった場合
	}
	p_result = p_left;
	while( !p_info->list.is_command_end() ) {
		s_operator = p_info->list.p_position->s_word;
		if( s_operator != "OR" || p_info->list.p_position->type != CBASIC_WORD_TYPE::RESERVED_WORD ) {
			//	所望の演算子ではないので左項をそのまま返す
			break;
		}
		p_info->list.p_position++;
		if( p_info->list.is_command_end() ) {
			p_info->errors.add( MISSING_OPERAND, p_info->list.get_line_no() );	//	あるべき右項がない
			break;
		}
		//	この演算子のインスタンスを生成
		p_operator = new CEXPRESSION_OPERATOR_OR;
		p_operator->p_left = p_result;
		p_operator->p_right = this->makeup_node_operator_and( p_info );
		p_result = p_operator;
	}
	return p_result;
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_operator_xor( CCOMPILE_INFO *p_info ) {
	CEXPRESSION_NODE *p_left;
	CEXPRESSION_OPERATOR_XOR *p_operator;
	CEXPRESSION_NODE *p_result;
	std::string s_operator;

	//	左項を得る
	p_left = this->makeup_node_operator_or( p_info );
	if( p_left == nullptr ) {
		return nullptr;				//	左項が得られなかった場合
	}
	p_result = p_left;
	while( !p_info->list.is_command_end() ) {
		s_operator = p_info->list.p_position->s_word;
		if( s_operator != "XOR" || p_info->list.p_position->type != CBASIC_WORD_TYPE::RESERVED_WORD ) {
			//	所望の演算子ではないので左項をそのまま返す
			break;
		}
		p_info->list.p_position++;
		if( p_info->list.is_command_end() ) {
			p_info->errors.add( MISSING_OPERAND, p_info->list.get_line_no() );	//	あるべき右項がない
			break;
		}
		//	この演算子のインスタンスを生成
		p_operator = new CEXPRESSION_OPERATOR_XOR;
		p_operator->p_left = p_result;
		p_operator->p_right = this->makeup_node_operator_or( p_info );
		p_result = p_operator;
	}
	return p_result;
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_operator_imp( CCOMPILE_INFO *p_info ) {
	CEXPRESSION_NODE *p_left;
	CEXPRESSION_OPERATOR_IMP *p_operator;
	CEXPRESSION_NODE *p_result;
	std::string s_operator;

	//	左項を得る
	p_left = this->makeup_node_operator_xor( p_info );
	if( p_left == nullptr ) {
		return nullptr;				//	左項が得られなかった場合
	}
	p_result = p_left;
	while( !p_info->list.is_command_end() ) {
		s_operator = p_info->list.p_position->s_word;
		if( s_operator != "IMP" || p_info->list.p_position->type != CBASIC_WORD_TYPE::RESERVED_WORD ) {
			//	所望の演算子ではないので左項をそのまま返す
			break;
		}
		p_info->list.p_position++;
		if( p_info->list.is_command_end() ) {
			p_info->errors.add( MISSING_OPERAND, p_info->list.get_line_no() );	//	あるべき右項がない
			break;
		}
		//	この演算子のインスタンスを生成
		p_operator = new CEXPRESSION_OPERATOR_IMP;
		p_operator->p_left = p_result;
		p_operator->p_right = this->makeup_node_operator_xor( p_info );
		p_result = p_operator;
	}
	return p_result;
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_operator_eqv( CCOMPILE_INFO *p_info ) {
	CEXPRESSION_NODE *p_left;
	CEXPRESSION_OPERATOR_EQV *p_operator;
	CEXPRESSION_NODE *p_result;
	std::string s_operator;

	//	左項を得る
	p_left = this->makeup_node_operator_imp( p_info );
	if( p_left == nullptr ) {
		return nullptr;				//	左項が得られなかった場合
	}
	p_result = p_left;
	while( !p_info->list.is_command_end() ) {
		s_operator = p_info->list.p_position->s_word;
		if( s_operator != "EQV" || p_info->list.p_position->type != CBASIC_WORD_TYPE::RESERVED_WORD ) {
			//	所望の演算子ではないので左項をそのまま返す
			break;
		}
		p_info->list.p_position++;
		if( p_info->list.is_command_end() ) {
			p_info->errors.add( MISSING_OPERAND, p_info->list.get_line_no() );	//	あるべき右項がない
			break;
		}
		//	この演算子のインスタンスを生成
		p_operator = new CEXPRESSION_OPERATOR_EQV;
		p_operator->p_left = p_result;
		p_operator->p_right = this->makeup_node_operator_imp( p_info );
		p_result = p_operator;
	}
	return p_result;
}

// --------------------------------------------------------------------
void CEXPRESSION::makeup_node( CCOMPILE_INFO *p_info ) {
	CEXPRESSION_NODE *p;

	if( this->p_top_node != nullptr ) {
		//	既にノード生成済みなら何もしない
		return;
	}

	this->p_top_node = this->makeup_node_operator_eqv( p_info );

	if( this->p_top_node != nullptr && p_info->options.optimize_level >= COPTIMIZE_LEVEL::NODE_ONLY ) {
		//	Node Only 以上の最適化レベルが指定されている場合、NODE の最適化を実施する。
		p = this->p_top_node->optimization( p_info );
		if( p != nullptr ) {
			delete this->p_top_node;
			this->p_top_node = p;
		}
	}
}

// --------------------------------------------------------------------
bool CEXPRESSION::compile( CCOMPILE_INFO *p_info, CEXPRESSION_TYPE target ) {

	this->makeup_node( p_info );
	if( this->p_top_node == nullptr ) {
		return false;
	}
	this->p_top_node->compile( p_info );
	if( target == this->p_top_node->type || target == CEXPRESSION_TYPE::UNKNOWN ) {
		//	型が一致している場合と、変換無しが指定されている場合は、型変換不要
		return true;
	}
	//	型変換
	if( target != CEXPRESSION_TYPE::STRING ) {
		if( this->p_top_node->type == CEXPRESSION_TYPE::STRING ) {
			//	数値型を要求しているのに文字列型が指定されている場合
			p_info->errors.add( TYPE_MISMATCH, p_info->list.get_line_no() );
			return false;
		}
		//	数値型の場合
		this->p_top_node->convert_type( p_info, target, this->p_top_node->type );
	}
	else {
		//	文字列型を要求しているのに数値型が指定されている場合
		p_info->errors.add( TYPE_MISMATCH, p_info->list.get_line_no() );
		return false;
	}
	return true;
}
