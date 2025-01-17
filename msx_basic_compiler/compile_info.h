// --------------------------------------------------------------------
//	MSX-BASIC compiler
// ====================================================================
//	2023/July/20th  t.hara 
// --------------------------------------------------------------------

#include "basic_types.h"
#include "basic_list.h"
#include "error_list.h"
#include "variable_info.h"
#include "constant_info.h"
#include "assembler/assembler_list.h"
#include "variable_manager.h"

#ifndef __COMPILE_INFO_H__
#define __COMPILE_INFO_H__

class CCOMPILE_INFO {
private:
	// ----------------------------------------------------------------
	//	自動生成ラベルのインデックス
	// ----------------------------------------------------------------
	int					auto_label_index;

public:
	CCOMPILE_INFO(): auto_label_index(0) {
	}

	class CCOMPILER		*p_compiler = nullptr;

	// ----------------------------------------------------------------
	//	BASICのソースコードとその参照位置を保持
	// ----------------------------------------------------------------
	CBASIC_LIST			list;

	// ----------------------------------------------------------------
	//	変数を保持
	// ----------------------------------------------------------------
	CVARIABLE_INFO		variables;

	// ----------------------------------------------------------------
	//	定数を保持
	// ----------------------------------------------------------------
	CCONSTANT_INFO		constants;

	// ----------------------------------------------------------------
	//	発見した全てのエラーを保持
	// ----------------------------------------------------------------
	CERROR_LIST			errors;

	// ----------------------------------------------------------------
	//	コンパイル結果を保持
	// ----------------------------------------------------------------
	CASSEMBLER_LIST		assembler_list;

	// ----------------------------------------------------------------
	//	コンパイルオプションを保持
	// ----------------------------------------------------------------
	COPTIONS			options;

	// ----------------------------------------------------------------
	//	変数の管理
	// ----------------------------------------------------------------
	CVARIABLE_MANAGER	variable_manager;

	// ----------------------------------------------------------------
	//	ラベルの自動生成
	// ----------------------------------------------------------------
	std::string get_auto_label( void ) {
		std::string s;
		s = "_pt" + std::to_string( this->auto_label_index );
		this->auto_label_index++;
		return s;
	}

	// ----------------------------------------------------------------
	//	FOR文のループ変数スタック
	// ----------------------------------------------------------------
	std::vector< CVARIABLE > for_variable_array;
};

// --------------------------------------------------------------------
class CCOMPILER_CONTAINER {
public:
	// ----------------------------------------------------------------
	//	このコンテナが対応する記述だけをコンパイルする
	//	このコンテナが対応する記述だった場合は、true を返す
	virtual bool exec( CCOMPILE_INFO *p_info ) = 0;
};

#endif
