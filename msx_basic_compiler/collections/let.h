// --------------------------------------------------------------------
//	Compiler collection: Let
// ====================================================================
//	2023/July/25th	t.hara
// --------------------------------------------------------------------
#pragma once

#include "../compiler.h"

class CLET: public CCOMPILER_CONTAINER {
public:
	bool exec( class CCOMPILER *p_this );
};
