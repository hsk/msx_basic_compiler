// --------------------------------------------------------------------
//	Expression
// ====================================================================
//	2023/July/29th	t.hara
// --------------------------------------------------------------------
#pragma once

#include <string>
#include <vector>
#include "expression_operator_power.h"

// --------------------------------------------------------------------
void CEXPRESSION_OPERATOR_POWER::optimization( void ) {
	
}

// --------------------------------------------------------------------
void CEXPRESSION_OPERATOR_POWER::compile( CCOMPILE_INFO *p_this ) {
	CASSEMBLER_LINE asm_line;

	//	先に項を処理
	this->p_left->compile( p_this );

	asm_line.type = CMNEMONIC_TYPE::PUSH;
	asm_line.operand1.type = COPERAND_TYPE::REGISTER;
	asm_line.operand1.s_value = "HL";
	asm_line.operand2.type = COPERAND_TYPE::NONE;
	asm_line.operand2.s_value = "";
	p_this->assembler_list.body.push_back( asm_line );

	this->p_right->compile( p_this );

	if( this->p_left->type == CEXPRESSION_TYPE::STRING || this->p_right->type == CEXPRESSION_TYPE::STRING ) {
		//	この演算子は文字列型には適用できない
		p_this->errors.add( TYPE_MISMATCH, p_this->list.get_line_no() );
		return;
	}
	//	この演算子の演算結果の型は、必ず倍精度
	this->type = CEXPRESSION_TYPE::DOUBLE_REAL;
	if( this->p_left->type == this->p_right->type ) {
		if( this->p_left->type == CEXPRESSION_TYPE::INTEGER ) {
			//	整数型の場合
			p_this->assembler_list.add_label( "bios_intexp", "0x0383f" );

			asm_line.type = CMNEMONIC_TYPE::POP;
			asm_line.operand1.type = COPERAND_TYPE::REGISTER;
			asm_line.operand1.s_value = "DE";
			asm_line.operand2.type = COPERAND_TYPE::NONE;
			asm_line.operand2.s_value = "";
			p_this->assembler_list.body.push_back( asm_line );

			asm_line.type = CMNEMONIC_TYPE::EX;
			asm_line.operand1.type = COPERAND_TYPE::REGISTER;
			asm_line.operand1.s_value = "DE";
			asm_line.operand2.type = COPERAND_TYPE::REGISTER;
			asm_line.operand2.s_value = "HL";
			p_this->assembler_list.body.push_back( asm_line );

			asm_line.type = CMNEMONIC_TYPE::CALL;
			asm_line.operand1.type = COPERAND_TYPE::REGISTER;
			asm_line.operand1.s_value = "bios_intexp";
			asm_line.operand2.type = COPERAND_TYPE::NONE;
			asm_line.operand2.s_value = "";
			p_this->assembler_list.body.push_back( asm_line );
		}
		else if( this->p_left->type == CEXPRESSION_TYPE::SINGLE_REAL ) {
		}
		else {
		}
	}
	else if( this->p_left->type > this->p_right->type ) {
		//	左の方が型が大きいので左を採用
		this->type = this->p_left->type;
	}
	else {
		//	右の方が型が大きいので右を採用
		this->type = this->p_right->type;
	}
}
