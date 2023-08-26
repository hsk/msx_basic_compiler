// --------------------------------------------------------------------
//	Expression operator Term
// ====================================================================
//	2023/July/29th	t.hara
// --------------------------------------------------------------------
#include <string>
#include <vector>
#include "expression.h"

#ifndef __EXPRESSION_TERM_H__
#define __CEXPRESSION_TERM_H__

// --------------------------------------------------------------------
//	関数呼び出し
class CEXPRESSION_TERM: public CEXPRESSION_NODE {
public:
	CEXPRESSION_TERM() {
		this->is_constant = true;
	}

	~CEXPRESSION_TERM() {
		this->release();
	}

	void optimization( CCOMPILE_INFO *p_this );

	void compile( CCOMPILE_INFO *p_this );

	void release( void ) {
	}
};

#endif
