// --------------------------------------------------------------------
//	Expression
// ====================================================================
//	2023/July/29th	t.hara
// --------------------------------------------------------------------
#pragma once

#include <string>
#include <vector>
#include "expression_operator_lt.h"

// --------------------------------------------------------------------
void CEXPRESSION_OPERATOR_LT::optimization( CCOMPILE_INFO *p_info ) {
	
	this->p_left->optimization( p_info );
	this->p_right->optimization( p_info );
}

// --------------------------------------------------------------------
void CEXPRESSION_OPERATOR_LT::compile( CCOMPILE_INFO *p_info ) {
	CASSEMBLER_LINE asm_line;
	CEXPRESSION_TYPE compare_type;

	//	先に項を処理
	this->p_left->compile( p_info );

	p_info->assembler_list.push_hl( this->p_left->type );

	this->p_right->compile( p_info );

	//	この演算子の演算結果は必ず整数
	this->type = CEXPRESSION_TYPE::INTEGER;
	if( this->p_left->type != this->p_right->type && (this->p_left->type == CEXPRESSION_TYPE::STRING || this->p_right->type == CEXPRESSION_TYPE::STRING) ) {
		//	左右の項が異なる型で、片方が文字列型ならエラー
		p_info->errors.add( TYPE_MISMATCH, p_info->list.get_line_no() );
		return;
	}
	else if( this->p_left->type > this->p_right->type ) {
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

		asm_line.type = CMNEMONIC_TYPE::SBC;					//	HL(左項) < DE(右項) が成立する場合は Cy=1, しない場合は Cy=0
		asm_line.operand1.type = COPERAND_TYPE::REGISTER;
		asm_line.operand1.s_value = "HL";
		asm_line.operand2.type = COPERAND_TYPE::REGISTER;
		asm_line.operand2.s_value = "DE";
		p_info->assembler_list.body.push_back( asm_line );

		std::string s_label = p_info->get_auto_label();
		asm_line.type = CMNEMONIC_TYPE::JR;
		asm_line.condition = CCONDITION::NC;
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
}
