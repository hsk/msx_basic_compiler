// --------------------------------------------------------------------
//	Expression
// ====================================================================
//	2023/July/29th	t.hara
// --------------------------------------------------------------------
#pragma once

#include <string>
#include <vector>
#include "expression_operator_ge.h"

// --------------------------------------------------------------------
void CEXPRESSION_OPERATOR_GE::optimization( CCOMPILE_INFO *p_info ) {
	
	this->p_left->optimization( p_info );
	this->p_right->optimization( p_info );
}

// --------------------------------------------------------------------
void CEXPRESSION_OPERATOR_GE::compile( CCOMPILE_INFO *p_info ) {
	CASSEMBLER_LINE asm_line;
	CEXPRESSION_TYPE compare_type;
	std::string s_label;

	//	先に項を処理
	this->p_left->compile( p_info );
	p_info->assembler_list.push_hl( this->p_left->type );
	this->p_right->compile( p_info );

	//	この演算子の演算結果は必ず整数
	this->type_adjust_2op( p_info, this->p_left, this->p_right );


	//	この演算子の演算結果は必ず整数
	if( this->p_left->type != this->p_right->type && (this->p_left->type == CEXPRESSION_TYPE::STRING || this->p_right->type == CEXPRESSION_TYPE::STRING) ) {
		//	左右の項が異なる型で、片方が文字列型ならエラー
		p_info->errors.add( TYPE_MISMATCH, p_info->list.get_line_no() );
		return;
	}
	if( this->p_left->type == CEXPRESSION_TYPE::STRING ) {
		//	文字列の場合
		s_label = p_info->get_auto_label();
		p_info->assembler_list.add_label( "blib_strcmp", "0x04027" );
		asm_line.set( CMNEMONIC_TYPE::POP, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "DE", COPERAND_TYPE::NONE, "" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::EX, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "DE", COPERAND_TYPE::REGISTER, "HL" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::PUSH, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "HL", COPERAND_TYPE::NONE, "" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::PUSH, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "DE", COPERAND_TYPE::NONE, "" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "IX", COPERAND_TYPE::NONE, "blib_strcmp" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::LABEL, "call_blib", COPERAND_TYPE::NONE, "" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::POP, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "HL", COPERAND_TYPE::NONE, "" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::PUSH, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "AF", COPERAND_TYPE::NONE, "" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::LABEL, "free_string", COPERAND_TYPE::NONE, "" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::POP, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "AF", COPERAND_TYPE::NONE, "" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::POP, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "HL", COPERAND_TYPE::NONE, "" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::PUSH, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "AF", COPERAND_TYPE::NONE, "" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::LABEL, "free_string", COPERAND_TYPE::NONE, "" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::POP, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "AF", COPERAND_TYPE::NONE, "" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "HL", COPERAND_TYPE::CONSTANT, "0" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::JR, CCONDITION::C, COPERAND_TYPE::LABEL, s_label, COPERAND_TYPE::NONE, "" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::DEC, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "HL", COPERAND_TYPE::NONE, "" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::LABEL, CCONDITION::NONE, COPERAND_TYPE::LABEL, s_label, COPERAND_TYPE::NONE, "" );
		p_info->assembler_list.body.push_back( asm_line );
		this->type = CEXPRESSION_TYPE::INTEGER;
		return;
	}
	if( this->p_left->type > this->p_right->type ) {
		//	左の方が型が大きいので、右の方の型を左項に合わせる
		compare_type = this->p_left->type;
	}
	else {
		//	右の方が型が大きいので、左の方の型を右項に合わせる
		compare_type = this->p_right->type;
	}
	if( compare_type == CEXPRESSION_TYPE::INTEGER ) {
		//	この演算子が整数の場合
		asm_line.type = CMNEMONIC_TYPE::POP;
		asm_line.operand1.type = COPERAND_TYPE::REGISTER;
		asm_line.operand1.s_value = "DE";
		asm_line.operand2.type = COPERAND_TYPE::NONE;
		asm_line.operand2.s_value = "";
		p_info->assembler_list.body.push_back( asm_line );

		asm_line.type = CMNEMONIC_TYPE::EX;
		asm_line.operand1.type = COPERAND_TYPE::REGISTER;
		asm_line.operand1.s_value = "DE";
		asm_line.operand2.type = COPERAND_TYPE::REGISTER;
		asm_line.operand2.s_value = "HL";
		p_info->assembler_list.body.push_back( asm_line );

		asm_line.type = CMNEMONIC_TYPE::XOR;
		asm_line.operand1.type = COPERAND_TYPE::REGISTER;
		asm_line.operand1.s_value = "A";
		asm_line.operand2.type = COPERAND_TYPE::REGISTER;
		asm_line.operand2.s_value = "A";
		p_info->assembler_list.body.push_back( asm_line );

		asm_line.type = CMNEMONIC_TYPE::SBC;					//	HL(左項) >= DE(右項) が成立する場合は S=0 (P), しない場合は S=1 (M)
		asm_line.operand1.type = COPERAND_TYPE::REGISTER;
		asm_line.operand1.s_value = "HL";
		asm_line.operand2.type = COPERAND_TYPE::REGISTER;
		asm_line.operand2.s_value = "DE";
		p_info->assembler_list.body.push_back( asm_line );

		std::string s_label = p_info->get_auto_label();
		asm_line.type = CMNEMONIC_TYPE::JP;
		asm_line.condition = CCONDITION::M;
		asm_line.operand1.type = COPERAND_TYPE::LABEL;
		asm_line.operand1.s_value = s_label;
		asm_line.operand2.type = COPERAND_TYPE::NONE;
		asm_line.operand2.s_value = "";
		p_info->assembler_list.body.push_back( asm_line );

		asm_line.type = CMNEMONIC_TYPE::DEC;
		asm_line.condition = CCONDITION::NONE;
		asm_line.operand1.type = COPERAND_TYPE::REGISTER;
		asm_line.operand1.s_value = "A";
		asm_line.operand2.type = COPERAND_TYPE::NONE;
		asm_line.operand2.s_value = "";
		p_info->assembler_list.body.push_back( asm_line );

		asm_line.type = CMNEMONIC_TYPE::LABEL;
		asm_line.operand1.type = COPERAND_TYPE::LABEL;
		asm_line.operand1.s_value = s_label;
		asm_line.operand2.type = COPERAND_TYPE::NONE;
		asm_line.operand2.s_value = "";
		p_info->assembler_list.body.push_back( asm_line );

		asm_line.type = CMNEMONIC_TYPE::LD;						//	HL = 0 または HL = -1
		asm_line.operand1.type = COPERAND_TYPE::REGISTER;
		asm_line.operand1.s_value = "H";
		asm_line.operand2.type = COPERAND_TYPE::REGISTER;
		asm_line.operand2.s_value = "A";
		p_info->assembler_list.body.push_back( asm_line );

		asm_line.type = CMNEMONIC_TYPE::LD;
		asm_line.operand1.type = COPERAND_TYPE::REGISTER;
		asm_line.operand1.s_value = "L";
		asm_line.operand2.type = COPERAND_TYPE::REGISTER;
		asm_line.operand2.s_value = "A";
		p_info->assembler_list.body.push_back( asm_line );
	}
	this->type = CEXPRESSION_TYPE::INTEGER;
}
