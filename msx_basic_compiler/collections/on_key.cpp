// --------------------------------------------------------------------
//	Compiler collection: On Key
// ====================================================================
//	2023/July/25th	t.hara
// --------------------------------------------------------------------

#include "on_key.h"
#include "../expressions/expression.h"

// --------------------------------------------------------------------
//	KEY {ON|OFF|STOP}
void CONKEY::key( CCOMPILE_INFO *p_info ) {
	CASSEMBLER_LINE asm_line;
	int line_no = p_info->list.get_line_no();
	CEXPRESSION exp;

	p_info->list.p_position++;

	if( p_info->list.is_command_end() || p_info->list.p_position->s_word != "(" ) {
		p_info->errors.add( SYNTAX_ERROR, line_no );
		return;
	}
	p_info->list.p_position++;

	if( exp.compile( p_info ) ) {
		asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "A", COPERAND_TYPE::REGISTER, "L" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::AND, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "A", COPERAND_TYPE::CONSTANT, "7" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::ADD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "A", COPERAND_TYPE::REGISTER, "A" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::ADD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "A", COPERAND_TYPE::REGISTER, "A" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "L", COPERAND_TYPE::REGISTER, "A" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "H", COPERAND_TYPE::CONSTANT, "0" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "DE", COPERAND_TYPE::LABEL, "svarf_on_strig0_mode" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::ADD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "HL", COPERAND_TYPE::REGISTER, "DE" );
		p_info->assembler_list.body.push_back( asm_line );
		exp.release();
	}
	else {
		p_info->errors.add( SYNTAX_ERROR, line_no );
		return;
	}

	if( p_info->list.is_command_end() || p_info->list.p_position->s_word != ")" ) {
		p_info->errors.add( SYNTAX_ERROR, line_no );
		return;
	}
	p_info->list.p_position++;

	if( p_info->list.p_position->s_word == "OFF" || p_info->list.p_position->s_word == "STOP" ) {
		asm_line.set( CMNEMONIC_TYPE::XOR, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "A", COPERAND_TYPE::CONSTANT, "0" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::DI, CCONDITION::NONE, COPERAND_TYPE::NONE, "", COPERAND_TYPE::NONE, "" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY_REGISTER, "[HL]", COPERAND_TYPE::REGISTER, "A" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::INC, CCONDITION::NONE, COPERAND_TYPE::MEMORY_REGISTER, "HL", COPERAND_TYPE::NONE, "" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY_REGISTER, "[HL]", COPERAND_TYPE::REGISTER, "A" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::INC, CCONDITION::NONE, COPERAND_TYPE::MEMORY_REGISTER, "HL", COPERAND_TYPE::NONE, "" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY_REGISTER, "[HL]", COPERAND_TYPE::REGISTER, "A" );
		p_info->assembler_list.body.push_back( asm_line );
		asm_line.set( CMNEMONIC_TYPE::EI, CCONDITION::NONE, COPERAND_TYPE::NONE, "", COPERAND_TYPE::NONE, "" );
		p_info->assembler_list.body.push_back( asm_line );
		p_info->list.p_position++;
	}
	else if( p_info->list.p_position->s_word == "ON" ) {
		asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY_REGISTER, "[HL]", COPERAND_TYPE::CONSTANT, "1" );
		p_info->assembler_list.body.push_back( asm_line );
		p_info->list.p_position++;
	}
	else {
		p_info->errors.add( SYNTAX_ERROR, line_no );
	}
}

// --------------------------------------------------------------------
//  ON KEY GOSUB <��ѐ�0>, <��ѐ�1>, ... , <��ѐ�3>, <��ѐ�9>
bool CONKEY::exec( CCOMPILE_INFO *p_info ) {
	CASSEMBLER_LINE asm_line;
	int line_no = p_info->list.get_line_no();
	int i;

	if( p_info->list.p_position->s_word == "KEY" ) {
		//	STRIG(n) {ON|OFF|STOP}
		this->key( p_info );
		return true;
	}
	if( p_info->list.p_position->s_word != "ON" ) {
		return false;
	}
	//	ON STRIG GOSUB line
	p_info->list.p_position++;
	if( p_info->list.is_command_end() ) {
		p_info->errors.add( SYNTAX_ERROR, line_no );
		return true;
	}
	if( p_info->list.p_position->s_word != "KEY" ) {
		p_info->list.p_position--;
		return false;
	}
	p_info->list.p_position++;

	//	��ѐ�
	if( p_info->list.p_position->s_word != "GOSUB" ) {
		p_info->errors.add( SYNTAX_ERROR, line_no );
		return true;
	}
	p_info->list.p_position++;
	if( p_info->list.is_command_end() || p_info->list.p_position->type != CBASIC_WORD_TYPE::LINE_NO ) {
		p_info->errors.add( SYNTAX_ERROR, line_no );
		return true;
	}
	asm_line.set( CMNEMONIC_TYPE::DI, CCONDITION::NONE, COPERAND_TYPE::NONE, "", COPERAND_TYPE::NONE, "" );
	p_info->assembler_list.body.push_back( asm_line );
	for( i = 0; i < 10; i++ ) {
		//	�s�ԍ��̋L�q���Ȃ��ꍇ�̓G���[
		if( p_info->list.is_command_end() ) {
			p_info->errors.add( SYNTAX_ERROR, line_no );
			return true;
		}
		if( p_info->list.p_position->type == CBASIC_WORD_TYPE::LINE_NO ) {
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::REGISTER, "HL", COPERAND_TYPE::LABEL, "line_" + p_info->list.p_position->s_word );
			p_info->assembler_list.body.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::LD, CCONDITION::NONE, COPERAND_TYPE::MEMORY_CONSTANT, "[svari_on_key" + std::to_string(i) + "_line]", COPERAND_TYPE::REGISTER, "HL" );
			p_info->assembler_list.body.push_back( asm_line );
			p_info->list.p_position++;
		}
		if( p_info->list.is_command_end() ) {
			//	�s�ԍ��̂������ŃR�}���h���I����Ă���ΏI���
			break;
		}
		if( p_info->list.p_position->s_word != "," ) {
			p_info->errors.add( SYNTAX_ERROR, line_no );
			return true;
		}
		p_info->list.p_position++;
	}
	asm_line.set( CMNEMONIC_TYPE::EI, CCONDITION::NONE, COPERAND_TYPE::NONE, "", COPERAND_TYPE::NONE, "" );
	p_info->assembler_list.body.push_back( asm_line );
	return true;
}