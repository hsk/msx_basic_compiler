// --------------------------------------------------------------------
//	Expression
// ====================================================================
//	2023/July/29th	t.hara
// --------------------------------------------------------------------
#pragma once

#include <string>
#include <vector>
#include "expression_abs.h"

// --------------------------------------------------------------------
void CEXPRESSION_ABS::optimization( CCOMPILE_INFO *p_this ) {

	this->p_operand->optimization( p_this );
}

// --------------------------------------------------------------------
void CEXPRESSION_ABS::compile( CCOMPILE_INFO *p_this ) {
	CASSEMBLER_LINE asm_line;

	//	先に引数を処理
	this->p_operand->compile( p_this );

	if( this->p_operand->type == CEXPRESSION_TYPE::STRING ) {
		p_this->errors.add( TYPE_MISMATCH, p_this->list.get_line_no() );
		return;
	}
	p_this->assembler_list.add_label( "work_valtyp", "0x0f663" );
	p_this->assembler_list.add_label( "work_dac", "0x0f7f6" );
	p_this->assembler_list.add_label( "bios_vmovfm", "0x02f08" );
	p_this->assembler_list.add_label( "bios_absfn", "0x02e82" );
	p_this->assembler_list.add_label( "bios_frcsng", "0x0303a" );
	if( this->p_operand->type == CEXPRESSION_TYPE::INTEGER ) {
		asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "A", COPERAND_TYPE::CONSTANT, "2" );
		p_this->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY_CONSTANT, "[work_valtyp]", COPERAND_TYPE::MEMORY_REGISTER, "A" );
		p_this->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY_CONSTANT, "[work_dac + 2]", COPERAND_TYPE::MEMORY_REGISTER, "HL" );
		p_this->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::LABEL, "bios_absfn", COPERAND_TYPE::NONE, "" );
		p_this->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::LABEL, "bios_frcsng", COPERAND_TYPE::NONE, "" );
		p_this->assembler_list.body.push_back( asm_line );
		this->type = CEXPRESSION_TYPE::SINGLE_REAL;
	}
	else if( this->p_operand->type == CEXPRESSION_TYPE::SINGLE_REAL ) {
		asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "A", COPERAND_TYPE::CONSTANT, "4" );
		p_this->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY_CONSTANT, "[work_valtyp]", COPERAND_TYPE::MEMORY_REGISTER, "A" );
		p_this->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::LABEL, "bios_vmovfm", COPERAND_TYPE::NONE, "" );
		p_this->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::LABEL, "bios_absfn", COPERAND_TYPE::NONE, "" );
		p_this->assembler_list.body.push_back( asm_line );
		this->type = CEXPRESSION_TYPE::SINGLE_REAL;
	}
	else {
		asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "A", COPERAND_TYPE::CONSTANT, "8" );
		p_this->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY_CONSTANT, "[work_valtyp]", COPERAND_TYPE::MEMORY_REGISTER, "A" );
		p_this->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::LABEL, "bios_vmovfm", COPERAND_TYPE::NONE, "" );
		p_this->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::CALL, CCONDITION::NONE, COPERAND_TYPE::LABEL, "bios_absfn", COPERAND_TYPE::NONE, "" );
		p_this->assembler_list.body.push_back( asm_line );
		this->type = CEXPRESSION_TYPE::DOUBLE_REAL;
	}
	asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "HL", COPERAND_TYPE::CONSTANT, "work_dac" );
	p_this->assembler_list.body.push_back( asm_line );
}
