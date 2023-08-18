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
#include "expression_csrlin.h"
#include "expression_str.h"
#include "expression_time.h"

// --------------------------------------------------------------------
void CEXPRESSION::optimization( void ) {
}

// --------------------------------------------------------------------
void CEXPRESSION_NODE::type_adjust_2op( CCOMPILE_INFO *p_this, CEXPRESSION_NODE *p_left, CEXPRESSION_NODE *p_right ) {
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
			p_this->assembler_list.add_label( "work_dac", "0x0f7f6" );
			p_this->assembler_list.activate_pop_single_real_arg();
			this->type = CEXPRESSION_TYPE::DOUBLE_REAL;	//	倍精度に昇格
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "DE", COPERAND_TYPE::CONSTANT, "work_dac" );
			p_this->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "BC", COPERAND_TYPE::CONSTANT, "4" );
			p_this->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LDIR, CCONDITION::NONE, COPERAND_TYPE::NONE, "", COPERAND_TYPE::NONE, "" );
			p_this->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::LABEL, "pop_single_real_arg", COPERAND_TYPE::NONE, "" );
			p_this->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY_CONSTANT, "[work_dac+4]", COPERAND_TYPE::REGISTER, "HL" );
			p_this->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY_CONSTANT, "[work_dac+6]", COPERAND_TYPE::REGISTER, "HL" );
			p_this->assembler_list.body.push_back( asm_line );
		}
		else {
			p_this->assembler_list.activate_pop_double_real_arg();
			this->type = CEXPRESSION_TYPE::DOUBLE_REAL;
			p_this->assembler_list.add_label( "work_arg", "0x0f847" );
			p_this->assembler_list.add_label( "work_dac", "0x0f7f6" );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "DE", COPERAND_TYPE::LABEL, "work_dac" );
			p_this->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "BC", COPERAND_TYPE::CONSTANT, "8" );
			p_this->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LDIR, CCONDITION::NONE, COPERAND_TYPE::NONE, "", COPERAND_TYPE::NONE, "" );
			p_this->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "pop_single_real_arg", COPERAND_TYPE::NONE, "" );
			p_this->assembler_list.body.push_back( asm_line );
		}
	}
	else if( p_left->type == CEXPRESSION_TYPE::STRING || p_right->type == CEXPRESSION_TYPE::STRING ) {
		//	左右の項が異なる型で、片方が文字列型ならエラー
		p_this->errors.add( TYPE_MISMATCH, p_this->list.get_line_no() );
		return;
	}
	else {
		//	倍精度で計算するので倍精度に昇格
		this->type = CEXPRESSION_TYPE::DOUBLE_REAL;
		//	右項は倍精度にして ARG へ転送
		if( p_right->type == CEXPRESSION_TYPE::INTEGER ) {
			//	整数から倍精度実数に昇格
			p_this->assembler_list.add_label( "bios_maf", "0x02c4d" );
			p_this->assembler_list.add_label( "bios_frcdbl", "0x0303a" );
			p_this->assembler_list.add_label( "work_dac_int", "0x0f7f8" );
			p_this->assembler_list.add_label( "work_valtyp", "0x0f663" );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY_CONSTANT, "[work_dac_int]", COPERAND_TYPE::REGISTER, "HL" );
			p_this->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "A", COPERAND_TYPE::CONSTANT, "2" );
			p_this->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY_CONSTANT, "[work_valtyp]", COPERAND_TYPE::REGISTER, "A" );
			p_this->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::LABEL, "bios_frcdbl", COPERAND_TYPE::NONE, "" );
			p_this->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::LABEL, "bios_maf", COPERAND_TYPE::NONE, "" );
			p_this->assembler_list.body.push_back( asm_line );
		}
		else if( p_right->type == CEXPRESSION_TYPE::SINGLE_REAL ) {
			p_this->assembler_list.activate_ld_arg_single_real();
			asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::LABEL, "ld_arg_single_real", COPERAND_TYPE::NONE, "" );
			p_this->assembler_list.body.push_back( asm_line );
		}
		else {
			p_this->assembler_list.activate_ld_arg_double_real();
			asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::LABEL, "ld_arg_double_real", COPERAND_TYPE::NONE, "" );
			p_this->assembler_list.body.push_back( asm_line );
		}

		//	左項は倍精度にして DAC へ転送
		if( p_left->type == CEXPRESSION_TYPE::INTEGER ) {
			//	整数から倍精度実数に昇格
			p_this->assembler_list.add_label( "bios_frcdbl", "0x0303a" );
			p_this->assembler_list.add_label( "work_dac_int", "0x0f7f8" );
			p_this->assembler_list.add_label( "work_valtyp", "0x0f663" );
			asm_line.set( CMNEMONIC_TYPE::POP, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "HL", COPERAND_TYPE::NONE, "" );
			p_this->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY_CONSTANT, "[work_dac_int]", COPERAND_TYPE::REGISTER, "HL" );
			p_this->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "A", COPERAND_TYPE::CONSTANT, "2" );
			p_this->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY_CONSTANT, "[work_valtyp]", COPERAND_TYPE::REGISTER, "A" );
			p_this->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::LABEL, "bios_frcdbl", COPERAND_TYPE::NONE, "" );
			p_this->assembler_list.body.push_back( asm_line );
		}
		else if( p_left->type == CEXPRESSION_TYPE::SINGLE_REAL ) {
			p_this->assembler_list.activate_pop_single_real_dac();
			asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::LABEL, "pop_single_real_dac", COPERAND_TYPE::NONE, "" );
			p_this->assembler_list.body.push_back( asm_line );
		}
		else {
			p_this->assembler_list.activate_pop_double_real_dac();
			asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::LABEL, "pop_double_real_dac", COPERAND_TYPE::NONE, "" );
			p_this->assembler_list.body.push_back( asm_line );
		}
	}
}

// --------------------------------------------------------------------
void CEXPRESSION_NODE::convert_type( CCOMPILE_INFO *p_this, CEXPRESSION_TYPE target, CEXPRESSION_TYPE current ) {
	CASSEMBLER_LINE asm_line;

	if( target == current ) {
		return;
	}
	p_this->assembler_list.add_label( "work_dac", "0x0f7f6" );
	p_this->assembler_list.add_label( "work_dac_int", "0x0f7f8" );

	if( target == CEXPRESSION_TYPE::INTEGER ) {
		p_this->assembler_list.add_label( "work_valtyp", "0x0f663" );
		p_this->assembler_list.add_label( "bios_frcint", "0x02f8a" );

		asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "DE", COPERAND_TYPE::CONSTANT, "work_dac" );
		p_this->assembler_list.body.push_back( asm_line );
		if( current == CEXPRESSION_TYPE::SINGLE_REAL ) {
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "BC", COPERAND_TYPE::CONSTANT, "4" );
			p_this->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LDIR, CCONDITION::NONE, COPERAND_TYPE::NONE, "", COPERAND_TYPE::NONE, "" ); 
			p_this->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::CONSTANT, "[work_dac+4]", COPERAND_TYPE::REGISTER, "BC" );
			p_this->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::CONSTANT, "[work_dac+6]", COPERAND_TYPE::REGISTER, "BC" );
			p_this->assembler_list.body.push_back( asm_line );
		}
		else {
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "BC", COPERAND_TYPE::CONSTANT, "8" );
			p_this->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LDIR, CCONDITION::NONE, COPERAND_TYPE::NONE, "", COPERAND_TYPE::NONE, "" ); 
			p_this->assembler_list.body.push_back( asm_line );
		}
		asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "A", COPERAND_TYPE::CONSTANT, "8" );
		p_this->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY_CONSTANT, "[work_valtyp]", COPERAND_TYPE::REGISTER, "A" );
		p_this->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::LABEL, "bios_frcint", COPERAND_TYPE::NONE, "" );
		p_this->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "HL", COPERAND_TYPE::MEMORY_CONSTANT, "[work_dac_int]" );
		p_this->assembler_list.body.push_back( asm_line );
	}
	else {
	}
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_term( CCOMPILE_INFO *p_this ) {
	CEXPRESSION_NODE *p_result;
	std::string s_operator;

	if( p_this->list.is_command_end() ) {
		return nullptr;															//	値が無い場合は nullptr を返す
	}
	s_operator = p_this->list.p_position->s_word;
	if( s_operator == "(" ) {
		p_this->list.p_position++;
		p_result = this->makeup_node_operator_eqv( p_this );
		if( p_this->list.is_command_end() || p_this->list.p_position->s_word != ")" ) {
			p_this->errors.add( MISSING_OPERAND, p_this->list.get_line_no() );	//	あるべき閉じ括弧
			return p_result;
		}
		p_this->list.p_position++;
		return p_result;
	}
	else if( s_operator == "CSRLIN" ) {
		CEXPRESSION_CSRLIN *p_term = new CEXPRESSION_CSRLIN;
		p_result = p_term;
		p_this->list.p_position++;
		return p_result;
	}
	else if( s_operator == "STR$" ) {
		CEXPRESSION_STR *p_term = new CEXPRESSION_STR;
		p_result = p_term;
		p_this->list.p_position++;
		p_term->p_operand = this->makeup_node_operator_eqv( p_this );
		if( p_this->list.is_command_end() || p_this->list.p_position->s_word != ")" ) {
			p_this->errors.add( MISSING_OPERAND, p_this->list.get_line_no() );	//	あるべき閉じ括弧
			return p_result;
		}
		p_this->list.p_position++;
		return p_result;
	}
	else if( s_operator == "TIME" ) {
		CEXPRESSION_TIME *p_term = new CEXPRESSION_TIME;
		p_result = p_term;
		p_this->list.p_position++;
		return p_result;
	}
	else if( p_this->list.p_position->type == CBASIC_WORD_TYPE::INTEGER ) {
		CEXPRESSION_TERM *p_term = new CEXPRESSION_TERM;
		p_term->type = CEXPRESSION_TYPE::INTEGER;
		p_term->s_value = s_operator;
		p_result = p_term;
		p_this->list.p_position++;
		return p_result;
	}
	else if( p_this->list.p_position->type == CBASIC_WORD_TYPE::SINGLE_REAL ) {
		CEXPRESSION_TERM *p_term = new CEXPRESSION_TERM;
		p_term->type = CEXPRESSION_TYPE::SINGLE_REAL;
		p_term->s_value = s_operator;
		p_result = p_term;
		p_this->list.p_position++;
		return p_result;
	}
	else if( p_this->list.p_position->type == CBASIC_WORD_TYPE::DOUBLE_REAL ) {
		CEXPRESSION_TERM *p_term = new CEXPRESSION_TERM;
		p_term->type = CEXPRESSION_TYPE::DOUBLE_REAL;
		p_term->s_value = s_operator;
		p_result = p_term;
		p_this->list.p_position++;
		return p_result;
	}
	else if( p_this->list.p_position->type == CBASIC_WORD_TYPE::STRING ) {
		CEXPRESSION_TERM *p_term = new CEXPRESSION_TERM;
		p_term->type = CEXPRESSION_TYPE::STRING;
		p_term->s_value = s_operator;
		p_result = p_term;
		p_this->list.p_position++;
		return p_result;
	}
	else if( p_this->list.p_position->type == CBASIC_WORD_TYPE::UNKNOWN_NAME ) {
	}
	else if( s_operator == ":" || s_operator == "," ) {
		return nullptr;
	}
	p_this->errors.add( SYNTAX_ERROR, p_this->list.get_line_no() );
	return nullptr;
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_operator_power( CCOMPILE_INFO *p_this ) {
	CEXPRESSION_NODE *p_left;
	CEXPRESSION_OPERATOR_POWER *p_operator;
	CEXPRESSION_NODE *p_result;
	std::string s_operator;

	//	左項を得る
	p_left = this->makeup_node_term( p_this );
	if( p_left == nullptr ) {
		return nullptr;				//	左項が得られなかった場合
	}
	p_result = p_left;
	while( !p_this->list.is_command_end() ) {
		s_operator = p_this->list.p_position->s_word;
		if( s_operator != "^" ) {
			//	所望の演算子ではないので左項をそのまま返す
			break;
		}
		p_this->list.p_position++;
		if( p_this->list.is_command_end() ) {
			p_this->errors.add( MISSING_OPERAND, p_this->list.get_line_no() );	//	あるべき右項がない
			break;
		}
		//	この演算子のインスタンスを生成
		p_operator = new CEXPRESSION_OPERATOR_POWER;
		p_operator->p_left = p_result;
		p_operator->p_right = this->makeup_node_term( p_this );
		p_result = p_operator;
	}
	return p_result;
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_operator_minus_plus( CCOMPILE_INFO *p_this ) {
	CEXPRESSION_OPERATOR_MINUS *p_operator_minus;
	CEXPRESSION_NODE *p_result;
	std::string s_operator;

	if( p_this->list.is_command_end() ) {
		return nullptr;
	}
	s_operator = p_this->list.p_position->s_word;
	if( s_operator != "+" && s_operator != "-" ) {
		//	所望の演算子ではない
		return this->makeup_node_operator_power( p_this );
	}
	p_this->list.p_position++;
	if( p_this->list.is_command_end() ) {
		p_this->errors.add( MISSING_OPERAND, p_this->list.get_line_no() );	//	あるべき右項がない
		return nullptr;
	}
	//	この演算子の処理
	if( s_operator == "+" ) {
		//	+ は実質何もしない
		return this->makeup_node_operator_minus_plus( p_this );
	}
	else {
		//	if( s_operator == "-" )
		p_operator_minus = new CEXPRESSION_OPERATOR_MINUS;
		p_operator_minus->p_right = this->makeup_node_operator_minus_plus( p_this );
		p_result = p_operator_minus;
	}
	return p_result;
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_operator_mul_div( CCOMPILE_INFO *p_this ) {
	CEXPRESSION_NODE *p_left;
	CEXPRESSION_OPERATOR_MUL *p_operator_mul;
	CEXPRESSION_OPERATOR_DIV *p_operator_div;
	CEXPRESSION_NODE *p_result;
	std::string s_operator;

	//	左項を得る
	p_left = this->makeup_node_operator_minus_plus( p_this );
	if( p_left == nullptr ) {
		return nullptr;				//	左項が得られなかった場合
	}
	p_result = p_left;
	while( !p_this->list.is_command_end() ) {
		s_operator = p_this->list.p_position->s_word;
		if( s_operator != "*" && s_operator != "/" ) {
			//	所望の演算子ではないので左項をそのまま返す
			break;
		}
		p_this->list.p_position++;
		if( p_this->list.is_command_end() ) {
			p_this->errors.add( MISSING_OPERAND, p_this->list.get_line_no() );	//	あるべき右項がない
			break;
		}
		//	この演算子のインスタンスを生成
		if( s_operator == "*" ) {
			p_operator_mul = new CEXPRESSION_OPERATOR_MUL;
			p_operator_mul->p_left = p_result;
			p_operator_mul->p_right = this->makeup_node_operator_minus_plus( p_this );
			p_result = p_operator_mul;
		}
		else {
			//	if( s_operator == "-" )
			p_operator_div = new CEXPRESSION_OPERATOR_DIV;
			p_operator_div->p_left = p_result;
			p_operator_div->p_right = this->makeup_node_operator_minus_plus( p_this );
			p_result = p_operator_div;
		}
	}
	return p_result;
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_operator_intdiv( CCOMPILE_INFO *p_this ) {
	CEXPRESSION_NODE *p_left;
	CEXPRESSION_OPERATOR_INTDIV *p_operator;
	CEXPRESSION_NODE *p_result;
	std::string s_operator;

	//	左項を得る
	p_left = this->makeup_node_operator_mul_div( p_this );
	if( p_left == nullptr ) {
		return nullptr;				//	左項が得られなかった場合
	}
	p_result = p_left;
	while( !p_this->list.is_command_end() ) {
		s_operator = p_this->list.p_position->s_word;
		if( s_operator != "\\" ) {
			//	所望の演算子ではないので左項をそのまま返す
			break;
		}
		p_this->list.p_position++;
		if( p_this->list.is_command_end() ) {
			p_this->errors.add( MISSING_OPERAND, p_this->list.get_line_no() );	//	あるべき右項がない
			break;
		}
		//	この演算子のインスタンスを生成
		p_operator = new CEXPRESSION_OPERATOR_INTDIV;
		p_operator->p_left = p_result;
		p_operator->p_right = this->makeup_node_operator_mul_div( p_this );
		p_result = p_operator;
	}
	return p_result;
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_operator_mod( CCOMPILE_INFO *p_this ) {
	CEXPRESSION_NODE *p_left;
	CEXPRESSION_OPERATOR_MOD *p_operator;
	CEXPRESSION_NODE *p_result;
	std::string s_operator;

	//	左項を得る
	p_left = this->makeup_node_operator_intdiv( p_this );
	if( p_left == nullptr ) {
		return nullptr;				//	左項が得られなかった場合
	}
	p_result = p_left;
	while( !p_this->list.is_command_end() ) {
		s_operator = p_this->list.p_position->s_word;
		if( s_operator != "MOD" ) {
			//	所望の演算子ではないので左項をそのまま返す
			break;
		}
		p_this->list.p_position++;
		if( p_this->list.is_command_end() ) {
			p_this->errors.add( MISSING_OPERAND, p_this->list.get_line_no() );	//	あるべき右項がない
			break;
		}
		//	この演算子のインスタンスを生成
		p_operator = new CEXPRESSION_OPERATOR_MOD;
		p_operator->p_left = p_result;
		p_operator->p_right = this->makeup_node_operator_intdiv( p_this );
		p_result = p_operator;
	}
	return p_result;
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_operator_add_sub( CCOMPILE_INFO *p_this ) {
	CEXPRESSION_NODE *p_left;
	CEXPRESSION_OPERATOR_ADD *p_operator_add;
	CEXPRESSION_OPERATOR_SUB *p_operator_sub;
	CEXPRESSION_NODE *p_result;
	std::string s_operator;

	//	左項を得る
	p_left = this->makeup_node_operator_mod( p_this );
	if( p_left == nullptr ) {
		return nullptr;				//	左項が得られなかった場合
	}
	p_result = p_left;
	while( !p_this->list.is_command_end() ) {
		s_operator = p_this->list.p_position->s_word;
		if( s_operator != "+" && s_operator != "-" ) {
			//	所望の演算子ではないので左項をそのまま返す
			break;
		}
		p_this->list.p_position++;
		if( p_this->list.is_command_end() ) {
			p_this->errors.add( MISSING_OPERAND, p_this->list.get_line_no() );	//	あるべき右項がない
			break;
		}
		//	この演算子のインスタンスを生成
		if( s_operator == "+" ) {
			p_operator_add = new CEXPRESSION_OPERATOR_ADD;
			p_operator_add->p_left = p_result;
			p_operator_add->p_right = this->makeup_node_operator_mod( p_this );
			p_result = p_operator_add;
		}
		else {
			//	if( s_operator == "-" )
			p_operator_sub = new CEXPRESSION_OPERATOR_SUB;
			p_operator_sub->p_left = p_result;
			p_operator_sub->p_right = this->makeup_node_operator_mod( p_this );
			p_result = p_operator_sub;
		}
	}
	return p_result;
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_operator_compare( CCOMPILE_INFO *p_this ) {
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
	p_left = this->makeup_node_operator_add_sub( p_this );
	if( p_left == nullptr ) {
		return nullptr;				//	左項が得られなかった場合
	}
	p_result = p_left;
	while( !p_this->list.is_command_end() ) {
		s_operator = p_this->list.p_position->s_word;
		if( s_operator != "=" && s_operator != "<" && s_operator != ">" ) {
			//	所望の演算子ではないので左項をそのまま返す
			break;
		}
		p_this->list.p_position++;
		if( p_this->list.is_command_end() ) {
			p_this->errors.add( MISSING_OPERAND, p_this->list.get_line_no() );	//	あるべき右項がない
			break;
		}
		s_operator2 = p_this->list.p_position->s_word;
		if( s_operator2 == "=" || s_operator2 == "<" || s_operator2 == ">" ) {
			//	<>, ><, >=, =>, <=, =< の場合、2つの単語に分かれてるので統合する
			s_operator = s_operator + s_operator2;
			p_this->list.p_position++;
			if( p_this->list.is_command_end() ) {
				p_this->errors.add( MISSING_OPERAND, p_this->list.get_line_no() );	//	あるべき右項がない
				break;
			}
		}
		//	この演算子のインスタンスを生成
		if( s_operator == "=" ) {
			p_operator_equ = new CEXPRESSION_OPERATOR_EQU;
			p_operator_equ->p_left = p_result;
			p_operator_equ->p_right = this->makeup_node_operator_add_sub( p_this );
			p_result = p_operator_equ;
		}
		else if( s_operator == "<>" || s_operator == "><" ) {
			p_operator_neq = new CEXPRESSION_OPERATOR_NEQ;
			p_operator_neq->p_left = p_result;
			p_operator_neq->p_right = this->makeup_node_operator_add_sub( p_this );
			p_result = p_operator_neq;
		}
		else if( s_operator == ">" ) {
			p_operator_gt = new CEXPRESSION_OPERATOR_GT;
			p_operator_gt->p_left = p_result;
			p_operator_gt->p_right = this->makeup_node_operator_add_sub( p_this );
			p_result = p_operator_gt;
		}
		else if( s_operator == "<" ) {
			p_operator_lt = new CEXPRESSION_OPERATOR_LT;
			p_operator_lt->p_left = p_result;
			p_operator_lt->p_right = this->makeup_node_operator_add_sub( p_this );
			p_result = p_operator_lt;
		}
		else if( s_operator == ">=" || s_operator == "=>" ) {
			p_operator_ge = new CEXPRESSION_OPERATOR_GE;
			p_operator_ge->p_left = p_result;
			p_operator_ge->p_right = this->makeup_node_operator_add_sub( p_this );
			p_result = p_operator_ge;
		}
		else { 
			//	if( s_operator == "<=" || s_operator == "=<" )
			p_operator_le = new CEXPRESSION_OPERATOR_LE;
			p_operator_le->p_left = p_result;
			p_operator_le->p_right = this->makeup_node_operator_add_sub( p_this );
			p_result = p_operator_le;
		}
	}
	return p_result;
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_operator_not( CCOMPILE_INFO *p_this ) {
	CEXPRESSION_OPERATOR_NOT *p_operator;
	std::string s_operator;

	s_operator = p_this->list.p_position->s_word;
	if( s_operator != "NOT" ) {
		//	所望の演算子ではないので右項をそのまま返す
		return this->makeup_node_operator_compare( p_this );
	}
	p_this->list.p_position++;
	if( p_this->list.is_command_end() ) {
		p_this->errors.add( MISSING_OPERAND, p_this->list.get_line_no() );	//	あるべき右項がない
		return nullptr;
	}
	//	この演算子のインスタンスを生成
	p_operator = new CEXPRESSION_OPERATOR_NOT;
	p_operator->p_right = this->makeup_node_operator_not( p_this );
	return p_operator;
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_operator_and( CCOMPILE_INFO *p_this ) {
	CEXPRESSION_NODE *p_left;
	CEXPRESSION_OPERATOR_AND *p_operator;
	CEXPRESSION_NODE *p_result;
	std::string s_operator;

	//	左項を得る
	p_left = this->makeup_node_operator_not( p_this );
	if( p_left == nullptr ) {
		return nullptr;				//	左項が得られなかった場合
	}
	p_result = p_left;
	while( !p_this->list.is_command_end() ) {
		s_operator = p_this->list.p_position->s_word;
		if( s_operator != "AND" ) {
			//	所望の演算子ではないので左項をそのまま返す
			break;
		}
		p_this->list.p_position++;
		if( p_this->list.is_command_end() ) {
			p_this->errors.add( MISSING_OPERAND, p_this->list.get_line_no() );	//	あるべき右項がない
			break;
		}
		//	この演算子のインスタンスを生成
		p_operator = new CEXPRESSION_OPERATOR_AND;
		p_operator->p_left = p_result;
		p_operator->p_right = this->makeup_node_operator_not( p_this );
		p_result = p_operator;
	}
	return p_result;
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_operator_or( CCOMPILE_INFO *p_this ) {
	CEXPRESSION_NODE *p_left;
	CEXPRESSION_OPERATOR_OR *p_operator;
	CEXPRESSION_NODE *p_result;
	std::string s_operator;

	//	左項を得る
	p_left = this->makeup_node_operator_and( p_this );
	if( p_left == nullptr ) {
		return nullptr;				//	左項が得られなかった場合
	}
	p_result = p_left;
	while( !p_this->list.is_command_end() ) {
		s_operator = p_this->list.p_position->s_word;
		if( s_operator != "OR" ) {
			//	所望の演算子ではないので左項をそのまま返す
			break;
		}
		p_this->list.p_position++;
		if( p_this->list.is_command_end() ) {
			p_this->errors.add( MISSING_OPERAND, p_this->list.get_line_no() );	//	あるべき右項がない
			break;
		}
		//	この演算子のインスタンスを生成
		p_operator = new CEXPRESSION_OPERATOR_OR;
		p_operator->p_left = p_result;
		p_operator->p_right = this->makeup_node_operator_and( p_this );
		p_result = p_operator;
	}
	return p_result;
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_operator_xor( CCOMPILE_INFO *p_this ) {
	CEXPRESSION_NODE *p_left;
	CEXPRESSION_OPERATOR_XOR *p_operator;
	CEXPRESSION_NODE *p_result;
	std::string s_operator;

	//	左項を得る
	p_left = this->makeup_node_operator_or( p_this );
	if( p_left == nullptr ) {
		return nullptr;				//	左項が得られなかった場合
	}
	p_result = p_left;
	while( !p_this->list.is_command_end() ) {
		s_operator = p_this->list.p_position->s_word;
		if( s_operator != "XOR" ) {
			//	所望の演算子ではないので左項をそのまま返す
			break;
		}
		p_this->list.p_position++;
		if( p_this->list.is_command_end() ) {
			p_this->errors.add( MISSING_OPERAND, p_this->list.get_line_no() );	//	あるべき右項がない
			break;
		}
		//	この演算子のインスタンスを生成
		p_operator = new CEXPRESSION_OPERATOR_XOR;
		p_operator->p_left = p_result;
		p_operator->p_right = this->makeup_node_operator_or( p_this );
		p_result = p_operator;
	}
	return p_result;
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_operator_imp( CCOMPILE_INFO *p_this ) {
	CEXPRESSION_NODE *p_left;
	CEXPRESSION_OPERATOR_IMP *p_operator;
	CEXPRESSION_NODE *p_result;
	std::string s_operator;

	//	左項を得る
	p_left = this->makeup_node_operator_or( p_this );
	if( p_left == nullptr ) {
		return nullptr;				//	左項が得られなかった場合
	}
	p_result = p_left;
	while( !p_this->list.is_command_end() ) {
		s_operator = p_this->list.p_position->s_word;
		if( s_operator != "IMP" ) {
			//	所望の演算子ではないので左項をそのまま返す
			break;
		}
		p_this->list.p_position++;
		if( p_this->list.is_command_end() ) {
			p_this->errors.add( MISSING_OPERAND, p_this->list.get_line_no() );	//	あるべき右項がない
			break;
		}
		//	この演算子のインスタンスを生成
		p_operator = new CEXPRESSION_OPERATOR_IMP;
		p_operator->p_left = p_result;
		p_operator->p_right = this->makeup_node_operator_or( p_this );
		p_result = p_operator;
	}
	return p_result;
}

// --------------------------------------------------------------------
CEXPRESSION_NODE *CEXPRESSION::makeup_node_operator_eqv( CCOMPILE_INFO *p_this ) {
	CEXPRESSION_NODE *p_left;
	CEXPRESSION_OPERATOR_EQV *p_operator;
	CEXPRESSION_NODE *p_result;
	std::string s_operator;

	//	左項を得る
	p_left = this->makeup_node_operator_imp( p_this );
	if( p_left == nullptr ) {
		return nullptr;				//	左項が得られなかった場合
	}
	p_result = p_left;
	while( !p_this->list.is_command_end() ) {
		s_operator = p_this->list.p_position->s_word;
		if( s_operator != "EQV" ) {
			//	所望の演算子ではないので左項をそのまま返す
			break;
		}
		p_this->list.p_position++;
		if( p_this->list.is_command_end() ) {
			p_this->errors.add( MISSING_OPERAND, p_this->list.get_line_no() );	//	あるべき右項がない
			break;
		}
		//	この演算子のインスタンスを生成
		p_operator = new CEXPRESSION_OPERATOR_EQV;
		p_operator->p_left = p_result;
		p_operator->p_right = this->makeup_node_operator_imp( p_this );
		p_result = p_operator;
	}
	return p_result;
}

// --------------------------------------------------------------------
void CEXPRESSION::makeup_node( CCOMPILE_INFO *p_this ) {

	this->p_top_node = this->makeup_node_operator_eqv( p_this );
}

// --------------------------------------------------------------------
bool CEXPRESSION::compile( CCOMPILE_INFO *p_this, CEXPRESSION_TYPE target ) {

	this->makeup_node( p_this );
	if( this->p_top_node == nullptr ) {
		return false;
	}
	this->p_top_node->compile( p_this );
	if( target == this->p_top_node->type || target == CEXPRESSION_TYPE::UNKNOWN ) {
		//	型が一致している場合と、変換無しが指定されている場合は、型変換不要
		return true;
	}
	//	型変換
	if( target != CEXPRESSION_TYPE::STRING ) {
		if( this->p_top_node->type == CEXPRESSION_TYPE::STRING ) {
			//	数値型を要求しているのに文字列型が指定されている場合
			p_this->errors.add( TYPE_MISMATCH, p_this->list.get_line_no() );
			return false;
		}
		//	数値型の場合
		this->p_top_node->convert_type( p_this, target, this->p_top_node->type );
	}
	else {
		//	文字列型を要求しているのに数値型が指定されている場合
		p_this->errors.add( TYPE_MISMATCH, p_this->list.get_line_no() );
		return false;
	}
	return true;
}
