// --------------------------------------------------------------------
//	Expression
// ====================================================================
//	2023/July/29th	t.hara
// --------------------------------------------------------------------
#include <string>
#include <vector>
#include "expression_cint.h"

// --------------------------------------------------------------------
CEXPRESSION_NODE* CEXPRESSION_CINT::optimization( CCOMPILE_INFO *p_info ) {
	CEXPRESSION_NODE* p;

	p = this->p_operand->optimization( p_info );
	if( p != nullptr ) {
		delete this->p_operand;
		this->p_operand = p;
	}
	return nullptr;
}

// --------------------------------------------------------------------
void CEXPRESSION_CINT::compile( CCOMPILE_INFO *p_info ) {

	this->p_operand->compile( p_info );
	this->convert_type( p_info, CEXPRESSION_TYPE::INTEGER, this->p_operand->type );
	this->type = CEXPRESSION_TYPE::INTEGER;
}
