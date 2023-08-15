// --------------------------------------------------------------------
//	Expression
// ====================================================================
//	2023/July/29th	t.hara
// --------------------------------------------------------------------
#pragma once

#include <string>
#include <vector>
#include "expression_operator_add.h"

// --------------------------------------------------------------------
void CEXPRESSION_OPERATOR_ADD::optimization( void ) {
	
}

// --------------------------------------------------------------------
void CEXPRESSION_OPERATOR_ADD::compile( CCOMPILE_INFO *p_this ) {
	CASSEMBLER_LINE asm_line;

	//	��ɍ�������
	this->p_left->compile( p_this );

	asm_line.type = CMNEMONIC_TYPE::PUSH;
	asm_line.operand1.type = COPERAND_TYPE::REGISTER;
	asm_line.operand1.s_value = "HL";
	asm_line.operand2.type = COPERAND_TYPE::NONE;
	asm_line.operand2.s_value = "";
	p_this->assembler_list.body.push_back( asm_line );

	this->p_right->compile( p_this );

	//	���̉��Z�q�̉��Z���ʂ̌^�����߂�
	if( this->p_left->type == this->p_right->type ) {
		//	���E�̍��������^�Ȃ�A���̌^���p��
		this->type = this->p_left->type;
	}
	else if( this->p_left->type == CEXPRESSION_TYPE::STRING || this->p_right->type == CEXPRESSION_TYPE::STRING ) {
		//	���E�̍����قȂ�^�ŁA�Е���������^�Ȃ�G���[
		p_this->errors.add( TYPE_MISMATCH, p_this->list.get_line_no() );
		return;
	}
	else if( this->p_left->type > this->p_right->type ) {
		//	���̕����^���傫���̂ō����̗p
		this->type = this->p_left->type;
	}
	else {
		//	�E�̕����^���傫���̂ŉE���̗p
		this->type = this->p_right->type;
	}
	if( this->type == CEXPRESSION_TYPE::INTEGER ) {
		//	���̉��Z�q�������̏ꍇ
		asm_line.type = CMNEMONIC_TYPE::POP;
		asm_line.operand1.type = COPERAND_TYPE::REGISTER;
		asm_line.operand1.s_value = "DE";
		asm_line.operand2.type = COPERAND_TYPE::NONE;
		asm_line.operand2.s_value = "";
		p_this->assembler_list.body.push_back( asm_line );

		asm_line.type = CMNEMONIC_TYPE::ADD;
		asm_line.operand1.type = COPERAND_TYPE::REGISTER;
		asm_line.operand1.s_value = "HL";
		asm_line.operand2.type = COPERAND_TYPE::REGISTER;
		asm_line.operand2.s_value = "DE";
		p_this->assembler_list.body.push_back( asm_line );
	}
}