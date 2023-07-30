// --------------------------------------------------------------------
//	Expression operator +
// ====================================================================
//	2023/July/29th	t.hara
// --------------------------------------------------------------------
#pragma once

#include <string>
#include <vector>
#include "compiler.h"
#include "expression.h"

// --------------------------------------------------------------------
//	関数呼び出し
class CEXPRESSION_FUNCTION: public CEXPRESSION_NODE {
public:
	void optimization( void );

	void compile( CCOMPILER *p_this );
};
