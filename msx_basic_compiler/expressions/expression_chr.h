// --------------------------------------------------------------------
//	Expression CHR$
// ====================================================================
//	2023/July/29th	t.hara
// --------------------------------------------------------------------
#include <string>
#include <vector>
#include "expression.h"

#ifndef __EXPRESSION_CHR_H__
#define __EXPRESSION_CHR_H__

// --------------------------------------------------------------------
//	�֐��Ăяo��
class CEXPRESSION_CHR: public CEXPRESSION_NODE {
public:
	~CEXPRESSION_CHR() {
		this->release();
	}
	CEXPRESSION_NODE *p_operand = nullptr;

	void optimization( CCOMPILE_INFO *p_info );

	void compile( CCOMPILE_INFO *p_info );

	void release( void ) {
		if( this->p_operand != nullptr ) {
			delete this->p_operand;
			this->p_operand = nullptr;
		}
	}
};

#endif