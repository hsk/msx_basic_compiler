// --------------------------------------------------------------------
//	Expression STRING$
// ====================================================================
//	2023/July/29th	t.hara
// --------------------------------------------------------------------
#include <string>
#include <vector>
#include "expression.h"

#ifndef __EXPRESSION_STRING_H__
#define __EXPRESSION_STRING_H__

// --------------------------------------------------------------------
//	関数呼び出し
class CEXPRESSION_STRING: public CEXPRESSION_NODE {
public:
	~CEXPRESSION_STRING() {
		this->release();
	}
	CEXPRESSION_NODE *p_operand1 = nullptr;
	CEXPRESSION_NODE *p_operand2 = nullptr;

	CEXPRESSION_NODE* optimization( CCOMPILE_INFO *p_info );

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
