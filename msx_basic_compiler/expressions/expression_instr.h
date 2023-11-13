// --------------------------------------------------------------------
//	Expression INSTR
// ====================================================================
//	2023/July/29th	t.hara
// --------------------------------------------------------------------
#include <string>
#include <vector>
#include "expression.h"

#ifndef __EXPRESSION_INSTR_H__
#define __EXPRESSION_INSTR_H__

// --------------------------------------------------------------------
//	�֐��Ăяo��
class CEXPRESSION_INSTR: public CEXPRESSION_NODE {
public:
	~CEXPRESSION_INSTR() {
		this->release();
	}
	CEXPRESSION_NODE *p_operand1 = nullptr;
	CEXPRESSION_NODE *p_operand2 = nullptr;

	void optimization( CCOMPILE_INFO *p_info );

	void compile( CCOMPILE_INFO *p_info );

	void release( void ) {
		if( this->p_operand1 != nullptr ) {
			delete this->p_operand1;
			this->p_operand1 = nullptr;
		}
		if( this->p_operand2 != nullptr ) {
			delete this->p_operand2;
			this->p_operand2 = nullptr;
		}
	}
};

#endif