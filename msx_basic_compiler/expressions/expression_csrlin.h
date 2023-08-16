// --------------------------------------------------------------------
//	Expression CSRLIN
// ====================================================================
//	2023/July/29th	t.hara
// --------------------------------------------------------------------
#include <string>
#include <vector>
#include "expression.h"

#ifndef __EXPRESSION_CSRLIN_H__
#define __CEXPRESSION_CSRLIN_H__

// --------------------------------------------------------------------
//	関数呼び出し
class CEXPRESSION_CSRLIN: public CEXPRESSION_NODE {
public:
	~CEXPRESSION_CSRLIN() {
		this->release();
	}

	void optimization( void );

	void compile( CCOMPILE_INFO *p_this );

	void release( void ) {
	}
};

#endif
