// --------------------------------------------------------------------
//	MSX-BASIC compiler
// ====================================================================
//	2023/July/20th  t.hara 
// --------------------------------------------------------------------

#include "assembler_list.h"
#include <cstdio>

// --------------------------------------------------------------------
bool CASSEMBLER_LIST::save_sub( FILE *p_file, const std::vector< CASSEMBLER_LINE > &list, COUTPUT_TYPES output_type ) {

	for( auto p: list ) {
		p.save( p_file, output_type );
	}
	return true;
}

// --------------------------------------------------------------------
void CASSEMBLER_LIST::add_label( const std::string s_name, const std::string s_value ) {

	//	既に存在しているラベルなのか調べる
	for( auto p: this->label_list ) {
		if( p == s_name ) {
			//	存在している場合は何もしない
			return;
		}
	}
	//	リストに追加する
	this->label_list.push_back( s_name );

	CASSEMBLER_LINE asm_line;
	asm_line.type = CMNEMONIC_TYPE::CONSTANT;
	asm_line.operand1.s_value = s_name;
	asm_line.operand1.type = COPERAND_TYPE::LABEL;
	asm_line.operand2.s_value = s_value;
	asm_line.operand2.type = COPERAND_TYPE::CONSTANT;
	this->header.push_back( asm_line );
}

// --------------------------------------------------------------------
bool CASSEMBLER_LIST::save( const std::string s_file_name, COUTPUT_TYPES output_type ) {
	FILE *p_file;
	bool result = true;

	fopen_s( &p_file, s_file_name.c_str(), "w" );
	if( p_file == NULL ) {
		fprintf( stderr, "ERROR: Cannot create the '%s'.\n", s_file_name.c_str() );
		return false;
	}

	result &= this->save_sub( p_file, this->header, output_type );
	result &= this->save_sub( p_file, this->body, output_type );
	result &= this->save_sub( p_file, this->subroutines, output_type );
	result &= this->save_sub( p_file, this->datas, output_type );
	result &= this->save_sub( p_file, this->variables_area, output_type );
	result &= this->save_sub( p_file, this->footer, output_type );
	fclose( p_file );

	return result;
}
