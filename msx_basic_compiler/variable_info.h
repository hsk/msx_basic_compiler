// --------------------------------------------------------------------
//	Variable information
// ====================================================================
//	2023/Aug/5th  t.hara
// --------------------------------------------------------------------

#include <string>
#include <vector>
#include <map>
#include "assembler/assembler_list.h"

#ifndef __VARIABLE_INFO_H__
#define __VARIABLE_INFO_H__

// --------------------------------------------------------------------
enum class CVARIABLE_TYPE {
	INTEGER,
	SINGLE_REAL,
	DOUBLE_REAL,
	STRING,
	UNKNOWN,
};

// --------------------------------------------------------------------
class CVARIABLE {
public:
	CVARIABLE_TYPE	type;
	std::string		s_name;		//	変数の名前
	std::string		s_label;	//	変数領域のラベル名 ( dictionary のインデックス )
	int				dimension;	//	-1: 要素数不明の配列, 0: 通常変数, 1: 1次元配列, 2: 2次元配列, ....

	CVARIABLE(): type( CVARIABLE_TYPE::DOUBLE_REAL ), s_name(""), dimension(0) {
	}
};

// --------------------------------------------------------------------
class CVARIABLE_INFO {
public:

	//	DEFxxx の情報
	std::vector< CVARIABLE_TYPE > def_types;

	//	使用されている変数のリスト
	std::map< std::string, CVARIABLE > dictionary;

	//	コンストラクタ
	CVARIABLE_INFO() {
		this->def_types.resize( 26 );
		for( auto &p: def_types ) {
			p = CVARIABLE_TYPE::DOUBLE_REAL;
		}
	}

	void dump( CASSEMBLER_LIST &asm_list, COPTIONS options ) {
		std::string s;
		CASSEMBLER_LINE asm_line;

		//	配列でない整数・単精度・倍精度変数
		asm_line.set( CMNEMONIC_TYPE::LABEL, CCONDITION::NONE, COPERAND_TYPE::LABEL, "var_area_start", COPERAND_TYPE::NONE, "" );
		asm_list.variables_area.push_back( asm_line );
		for( auto it = dictionary.begin(); it != dictionary.end(); it++ ) {
			if( it->second.dimension != 0 || it->second.type == CVARIABLE_TYPE::STRING ) {
				continue;
			}
			asm_line.set( CMNEMONIC_TYPE::LABEL, CCONDITION::NONE, COPERAND_TYPE::LABEL, it->first, COPERAND_TYPE::NONE, "" );
			asm_list.variables_area.push_back( asm_line );
			switch( it->second.type ) {
			case CVARIABLE_TYPE::INTEGER:
				asm_line.set( CMNEMONIC_TYPE::DEFW, CCONDITION::NONE, COPERAND_TYPE::CONSTANT, "0", COPERAND_TYPE::NONE, "" );
				asm_list.variables_area.push_back( asm_line );
				break;
			case CVARIABLE_TYPE::SINGLE_REAL:
				asm_line.set( CMNEMONIC_TYPE::DEFW, CCONDITION::NONE, COPERAND_TYPE::CONSTANT, "0, 0", COPERAND_TYPE::NONE, "" );
				asm_list.variables_area.push_back( asm_line );
				break;
			case CVARIABLE_TYPE::DOUBLE_REAL:
				asm_line.set( CMNEMONIC_TYPE::DEFW, CCONDITION::NONE, COPERAND_TYPE::CONSTANT, "0, 0, 0, 0", COPERAND_TYPE::NONE, "" );
				asm_list.variables_area.push_back( asm_line );
				break;
			default:
				break;
			}
		}
		asm_line.set( CMNEMONIC_TYPE::LABEL, CCONDITION::NONE, COPERAND_TYPE::LABEL, "var_area_end", COPERAND_TYPE::NONE, "" );
		asm_list.variables_area.push_back( asm_line );

		//	配列でない文字列
		asm_line.set( CMNEMONIC_TYPE::LABEL, CCONDITION::NONE, COPERAND_TYPE::LABEL, "vars_area_start", COPERAND_TYPE::NONE, "" );
		asm_list.variables_area.push_back( asm_line );
		for( auto it = dictionary.begin(); it != dictionary.end(); it++ ) {
			if( it->second.dimension != 0 || it->second.type != CVARIABLE_TYPE::STRING ) {
				continue;
			}
			asm_line.set( CMNEMONIC_TYPE::LABEL, CCONDITION::NONE, COPERAND_TYPE::LABEL, it->first, COPERAND_TYPE::NONE, "" );
			asm_list.variables_area.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::DEFW, CCONDITION::NONE, COPERAND_TYPE::CONSTANT, "0", COPERAND_TYPE::NONE, "" );		//	アドレス
			asm_list.variables_area.push_back( asm_line );
		}
		asm_line.set( CMNEMONIC_TYPE::LABEL, CCONDITION::NONE, COPERAND_TYPE::LABEL, "vars_area_end", COPERAND_TYPE::NONE, "" );
		asm_list.variables_area.push_back( asm_line );

		//	配列の整数・単精度・倍精度変数
		asm_line.set( CMNEMONIC_TYPE::LABEL, CCONDITION::NONE, COPERAND_TYPE::LABEL, "vara_area_start", COPERAND_TYPE::NONE, "" );
		asm_list.variables_area.push_back( asm_line );
		for( auto it = dictionary.begin(); it != dictionary.end(); it++ ) {
			if( it->second.dimension == 0 || it->second.type == CVARIABLE_TYPE::STRING ) {
				continue;
			}
			asm_line.set( CMNEMONIC_TYPE::LABEL, CCONDITION::NONE, COPERAND_TYPE::LABEL, it->first, COPERAND_TYPE::NONE, "" );
			asm_list.variables_area.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::DEFW, CCONDITION::NONE, COPERAND_TYPE::CONSTANT, "0", COPERAND_TYPE::NONE, "" );		//	アドレス
			asm_list.variables_area.push_back( asm_line );
		}
		asm_line.set( CMNEMONIC_TYPE::LABEL, CCONDITION::NONE, COPERAND_TYPE::LABEL, "vara_area_end", COPERAND_TYPE::NONE, "" );
		asm_list.variables_area.push_back( asm_line );

		//	配列の文字列
		asm_line.set( CMNEMONIC_TYPE::LABEL, CCONDITION::NONE, COPERAND_TYPE::LABEL, "varsa_area_start", COPERAND_TYPE::NONE, "" );
		asm_list.variables_area.push_back( asm_line );
		for( auto it = dictionary.begin(); it != dictionary.end(); it++ ) {
			if( it->second.dimension == 0 || it->second.type != CVARIABLE_TYPE::STRING ) {
				continue;
			}
			asm_line.set( CMNEMONIC_TYPE::LABEL, CCONDITION::NONE, COPERAND_TYPE::LABEL, it->first, COPERAND_TYPE::NONE, "" );
			asm_list.variables_area.push_back( asm_line );
			asm_line.set( CMNEMONIC_TYPE::DEFW, CCONDITION::NONE, COPERAND_TYPE::CONSTANT, "0", COPERAND_TYPE::NONE, "" );		//	アドレス
			asm_list.variables_area.push_back( asm_line );
		}
		asm_line.set( CMNEMONIC_TYPE::LABEL, CCONDITION::NONE, COPERAND_TYPE::LABEL, "varsa_area_end", COPERAND_TYPE::NONE, "" );
		asm_list.variables_area.push_back( asm_line );
	}
};

#endif
