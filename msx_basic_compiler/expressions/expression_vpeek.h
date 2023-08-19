// --------------------------------------------------------------------
//	Expression VPEEK
// ====================================================================
//	2023/July/29th	t.hara
// --------------------------------------------------------------------
#include <string>
#include <vector>
#include "expression.h"

#ifndef __EXPRESSION_VPEEK_H__
#define __EXPRESSION_VPEEK_H__

// --------------------------------------------------------------------
//	�֐��Ăяo��
class CEXPRESSION_VPEEK: public CEXPRESSION_NODE {
public:
	~CEXPRESSION_VPEEK() {
		this->release();
	}
	CEXPRESSION_NODE *p_operand = nullptr;

	void optimization( void );

	void compile( CCOMPILE_INFO *p_this );

	void release( void ) {
		if( this->p_operand != nullptr ) {
			delete this->p_operand;
			this->p_operand = nullptr;
		}
	}
};

#endif