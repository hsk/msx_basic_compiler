// --------------------------------------------------------------------
//	Compiler collection: Read
// ====================================================================
//	2023/July/25th	t.hara
// --------------------------------------------------------------------
#include "../compiler.h"

#ifndef __READ_H__
#define __READ_H__

class CREAD: public CCOMPILER_CONTAINER {
public:
	bool exec( CCOMPILE_INFO *p_info );
};

#endif
