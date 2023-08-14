// --------------------------------------------------------------------
//	Expression
// ====================================================================
//	2023/July/29th	t.hara
// --------------------------------------------------------------------
#pragma once

#include <string>
#include <vector>
#include "expression_operator_div.h"

// --------------------------------------------------------------------
void CEXPRESSION_OPERATOR_DIV::optimization( void ) {
	
}

// --------------------------------------------------------------------
void CEXPRESSION_OPERATOR_DIV::compile( CCOMPILE_INFO *p_this ) {
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

	//	この演算子の演算結果の型を決める
	if( this->p_left->type == CEXPRESSION_TYPE::STRING || this->p_right->type == CEXPRESSION_TYPE::STRING ) {
		//	この演算子は文字列型には適用できない
		p_this->errors.add( TYPE_MISMATCH, p_this->list.get_line_no() );
		return;
	}
	else if( this->p_left->type == this->p_right->type ) {
		//	左右の項が同じ型なら、その型を継承
		this->type = this->p_left->type;
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
