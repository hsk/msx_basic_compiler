// --------------------------------------------------------------------
//	Compiler
// ====================================================================
//	2023/July/23rd	t.hara
// --------------------------------------------------------------------

#include "compiler.h"
#include "collections/beep.h"
#include "collections/bload.h"
#include "collections/bsave.h"
#include "collections/call.h"
#include "collections/clear.h"
#include "collections/cls.h"
#include "collections/color.h"
#include "collections/comment.h"
#include "collections/data.h"
#include "collections/defdbl.h"
#include "collections/defint.h"
#include "collections/defusr.h"
#include "collections/defsng.h"
#include "collections/defstr.h"
#include "collections/dim.h"
#include "collections/end.h"
#include "collections/erase.h"
#include "collections/error.h"
#include "collections/files.h"
#include "collections/for.h"
#include "collections/goto.h"
#include "collections/gosub.h"
#include "collections/if.h"
#include "collections/input.h"
#include "collections/irandomize.h"
#include "collections/key.h"
#include "collections/kill.h"
#include "collections/let.h"
#include "collections/lset.h"
#include "collections/line.h"
#include "collections/locate.h"
#include "collections/name.h"
#include "collections/next.h"
#include "collections/mid.h"
#include "collections/on_goto.h"			//	on gosub もこの中
#include "collections/on_interval.h"
#include "collections/on_key.h"
#include "collections/on_sprite.h"
#include "collections/on_strig.h"
#include "collections/out.h"
#include "collections/play.h"
#include "collections/poke.h"
#include "collections/pokew.h"
#include "collections/pokes.h"
#include "collections/print.h"
#include "collections/pset.h"
#include "collections/put_sprite.h"
#include "collections/read.h"
#include "collections/restore.h"
#include "collections/return.h"
#include "collections/run.h"
#include "collections/screen.h"
#include "collections/setpage.h"
#include "collections/setscroll.h"
#include "collections/sound.h"
#include "collections/swap.h"
#include "collections/vpoke.h"
#include "collections/vpokes.h"
#include "collections/width.h"
#include "variable_manager.h"
#include "./expressions/expression.h"

// --------------------------------------------------------------------
void CCOMPILER::initialize( void ) {

	this->collection.push_back( new CBEEP );
	this->collection.push_back( new CBLOAD );
	this->collection.push_back( new CBSAVE );
	this->collection.push_back( new CCALL );
	this->collection.push_back( new CCLS );
	this->collection.push_back( new CCLEAR );
	this->collection.push_back( new CCOMMENT );
	this->collection.push_back( new CCOLOR );
	this->collection.push_back( new CDATA );
	this->collection.push_back( new CDEFDBL );
	this->collection.push_back( new CDEFINT );
	this->collection.push_back( new CDEFUSR );
	this->collection.push_back( new CDEFSNG );
	this->collection.push_back( new CDEFSTR );
	this->collection.push_back( new CDIM );
	this->collection.push_back( new CEND );
	this->collection.push_back( new CERASE );
	this->collection.push_back( new CERROR );
	this->collection.push_back( new CFOR );
	this->collection.push_back( new CFILES );
	this->collection.push_back( new CGOTO );
	this->collection.push_back( new CGOSUB );
	this->collection.push_back( new CIF );
	this->collection.push_back( new CINPUT );
	this->collection.push_back( new CIRANDOMIZE );
	this->collection.push_back( new CKEY );
	this->collection.push_back( new CKILL );
	this->collection.push_back( new CLET );
	this->collection.push_back( new CLSET );
	this->collection.push_back( new CLINE );
	this->collection.push_back( new CLOCATE );
	this->collection.push_back( new CNAME );
	this->collection.push_back( new CNEXT );
	this->collection.push_back( new CMID );
	this->collection.push_back( new CONINTERVAL );
	this->collection.push_back( new CONKEY );
	this->collection.push_back( new CONSPRITE );
	this->collection.push_back( new CONSTRIG );
	this->collection.push_back( new CONGOTO );			//	ON GOTO/GOSUB は、ON〜GOSUB より後
	this->collection.push_back( new COUT );
	this->collection.push_back( new CPLAY );
	this->collection.push_back( new CPOKE );
	this->collection.push_back( new CPOKEW );
	this->collection.push_back( new CPOKES );
	this->collection.push_back( new CPRINT );
	this->collection.push_back( new CPSET );
	this->collection.push_back( new CPUTSPRITE );
	this->collection.push_back( new CREAD );
	this->collection.push_back( new CRESTORE );
	this->collection.push_back( new CRETURN );
	this->collection.push_back( new CRUN );
	this->collection.push_back( new CSCREEN );
	this->collection.push_back( new CSETPAGE );
	this->collection.push_back( new CSETSCROLL );
	this->collection.push_back( new CSOUND );
	this->collection.push_back( new CSWAP );
	this->collection.push_back( new CVPOKE );
	this->collection.push_back( new CVPOKES );
	this->collection.push_back( new CWIDTH );
}

// --------------------------------------------------------------------
void CCOMPILER::insert_label( void ) {
	CASSEMBLER_LINE asm_line;

	this->info.list.update_current_line_no();
	int current_line_no = this->info.list.get_line_no();
	asm_line.set( "LABEL", "", "line_" + std::to_string( current_line_no ), "" );
	this->info.assembler_list.body.push_back( asm_line );
}

// --------------------------------------------------------------------
void CCOMPILER::line_compile( bool is_top ) {
	CASSEMBLER_LINE asm_line;
	bool do_exec;

	while( !this->info.list.is_line_end() && !(!is_top && this->info.list.p_position->s_word == "ELSE") ) {
		if( this->info.list.p_position->s_word == ":" ) {
			this->info.list.p_position++;
			continue;
		}
		if( this->info.list.p_position->type == CBASIC_WORD_TYPE::LINE_NO && this->info.list.p_position->s_word[0] == '*' ) {
			asm_line.set( "LABEL", "", "label_" + this->info.list.p_position->s_word.substr(1), "" );
			this->info.assembler_list.body.push_back( asm_line );
			this->info.list.p_position++;
			continue;
		}
		do_exec = false;
		//	割り込み処理用のルーチンを呼び出す
		asm_line.set( "CALL", "", "interrupt_process", "" );
		this->info.assembler_list.body.push_back( asm_line );
		//	命令の処理を生成する
		for( auto p: this->collection ) {
			do_exec = p->exec( &(this->info) );
			if( do_exec ) {
				break;
			}
		}
		if( !do_exec ) {
			//	何も処理されなかった場合、Syntax error にしてそのステートメントを読み飛ばす
			this->info.list.update_current_line_no();
			this->info.errors.add( SYNTAX_ERROR, this->info.list.get_line_no() );
			this->info.list.skip_statement();
		}
	}
}

// --------------------------------------------------------------------
//	着目位置の変数名に応じて、その変数のアドレスを取得するコードを生成する
CVARIABLE CCOMPILER::get_variable_address() {
	CASSEMBLER_LINE asm_line;
	CVARIABLE variable;
	std::vector< CEXPRESSION* > exp_list;

	variable = this->info.variable_manager.get_variable_info( &this->info, exp_list );
	if( variable.dimension == 0 ) {
		//	単独変数の場合
		asm_line.set( "LD", "", "HL", variable.s_label );
		this->info.assembler_list.body.push_back( asm_line );
	}
	else {
		//	配列変数の場合
		this->info.variable_manager.compile_array_elements( &this->info, exp_list, variable );
	}
	return variable;
}

// --------------------------------------------------------------------
//	着目位置の変数名に応じて、その変数のアドレスを取得するコードを生成する (配列は除外)
CVARIABLE CCOMPILER::get_variable_address_wo_array( void ) {
	CASSEMBLER_LINE asm_line;
	CVARIABLE variable;
	std::vector< CEXPRESSION* > exp_list;

	variable = this->info.variable_manager.get_variable_info( &this->info, exp_list, false );
	asm_line.set( "LD", "", "HL", variable.s_label );
	this->info.assembler_list.body.push_back( asm_line );
	return variable;
}

// --------------------------------------------------------------------
void CCOMPILER::write_variable_value( CVARIABLE &variable ) {
	CASSEMBLER_LINE asm_line;

	switch( variable.type ) {
	default:
	case CVARIABLE_TYPE::INTEGER:
		//	変数のアドレスを POP
		asm_line.set( "POP", "", "DE", "" );
		this->info.assembler_list.body.push_back( asm_line );
		//	格納する値を DE, 変数のアドレスを HL へ
		asm_line.set( "EX", "", "DE", "HL" );
		this->info.assembler_list.body.push_back( asm_line );
		//	変数へ DE の値を格納
		asm_line.set( "LD", "", "[HL]", "E" );
		this->info.assembler_list.body.push_back( asm_line );
		asm_line.set( "INC", "", "HL", "" );
		this->info.assembler_list.body.push_back( asm_line );
		asm_line.set( "LD", "", "[HL]", "D" );
		this->info.assembler_list.body.push_back( asm_line );
		break;
	case CVARIABLE_TYPE::SINGLE_REAL:
		this->info.assembler_list.activate_ld_de_single_real();
		//	変数のアドレスを POP
		asm_line.set( "POP", "", "DE", "" );
		this->info.assembler_list.body.push_back( asm_line );
		asm_line.set( "CALL", "", "ld_de_single_real", "" );
		this->info.assembler_list.body.push_back( asm_line );
		break;
	case CVARIABLE_TYPE::DOUBLE_REAL:
		this->info.assembler_list.activate_ld_de_double_real();
		//	変数のアドレスを POP
		asm_line.set( "POP", "", "DE", "" );
		this->info.assembler_list.body.push_back( asm_line );
		asm_line.set( "CALL", "", "ld_de_double_real", "" );
		this->info.assembler_list.body.push_back( asm_line );
		break;
	case CVARIABLE_TYPE::STRING:
		//	文字列の演算結果 [HL] を HEAP にコピー
		this->info.assembler_list.activate_free_string();
		//	変数のアドレスを POP
		asm_line.set( "POP", "", "DE", "" );
		this->info.assembler_list.body.push_back( asm_line );
		asm_line.set( "EX", "", "DE", "HL" );
		this->info.assembler_list.body.push_back( asm_line );
		asm_line.set( "LD", "", "C", "[HL]" );
		this->info.assembler_list.body.push_back( asm_line );
		asm_line.set( "LD", "", "[HL]", "E" );
		this->info.assembler_list.body.push_back( asm_line );
		asm_line.set( "INC", "", "HL", "" );
		this->info.assembler_list.body.push_back( asm_line );
		asm_line.set( "LD", "", "B", "[HL]" );
		this->info.assembler_list.body.push_back( asm_line );
		asm_line.set( "LD", "", "[HL]", "D" );
		this->info.assembler_list.body.push_back( asm_line );
		asm_line.set( "LD", "", "L", "C" );
		this->info.assembler_list.body.push_back( asm_line );
		asm_line.set( "LD", "", "H", "B" );
		this->info.assembler_list.body.push_back( asm_line );
		asm_line.set( "CALL", "", "free_string", "" );
		this->info.assembler_list.body.push_back( asm_line );
		break;
	}
}

// --------------------------------------------------------------------
void CCOMPILER::exec_header( std::string s_name ) {
	CASSEMBLER_LINE asm_line;
	char s_buffer[32];

	//	ヘッダーコメント
	asm_line.set( "COMMENT", "", "------------------------------------------------------------------------", "" );
	this->info.assembler_list.header.push_back( asm_line );
	asm_line.set( "COMMENT", "", "Compiled by MSX-BACON from " + s_name, "" );
	this->info.assembler_list.header.push_back( asm_line );
	asm_line.set( "COMMENT", "", "------------------------------------------------------------------------", "" );
	this->info.assembler_list.header.push_back( asm_line );
	asm_line.set( "COMMENT", "", "", "" );
	this->info.assembler_list.header.push_back( asm_line );
	//	BSAVEヘッダー
	asm_line.set( "COMMENT", "", "BSAVE header -----------------------------------------------------------", "" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "DEFB", "", "0xfe", "" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "DEFW", "", "start_address", "" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "DEFW", "", "end_address", "" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "DEFW", "", "start_address", "" );
	this->info.assembler_list.body.push_back( asm_line );
	sprintf( s_buffer, "0x%04X", this->info.options.start_address );
	asm_line.set( "ORG", "", s_buffer, "" );
	this->info.assembler_list.body.push_back( asm_line );
}

// --------------------------------------------------------------------
void CCOMPILER::exec_initializer( std::string s_name ) {
	CASSEMBLER_LINE asm_line;

	this->info.assembler_list.add_label( "work_himem", "0x0FC4A" );

	//	初期化処理 (BACONLIB存在確認)
	asm_line.set( "LABEL", "", "start_address", "" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "LD", "", "SP", "[work_himem]" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "CALL", "", "check_blib", "" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "JP", "NZ", "bios_syntax_error", "" );
	this->info.assembler_list.body.push_back( asm_line );
	//	実行の度の blib 初期化
	this->info.assembler_list.add_label( "blib_init_ncalbas", "0x0404e" );
	asm_line.set( "LD", "", "IX", "blib_init_ncalbas" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "CALL", "", "call_blib", "" );
	this->info.assembler_list.body.push_back( asm_line );
	//	初期化処理 (H.TIMIフック)
	asm_line.set( "LD", "", "HL", "work_h_timi" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "LD", "", "DE", "h_timi_backup" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "LD", "", "BC", "5" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "LDIR" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "DI" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "CALL", "", "setup_h_timi", "" );
	this->info.assembler_list.body.push_back( asm_line );
	//	ON SPRITE の飛び先初期化
	asm_line.set( "LD", "", "HL", "_code_ret" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "LD", "", "[svari_on_sprite_line]", "HL" );
	this->info.assembler_list.body.push_back( asm_line );
	//	ON INTERVAL の飛び先初期化
	asm_line.set( "LD", "", "[svari_on_interval_line]", "HL" );
	this->info.assembler_list.body.push_back( asm_line );
	//	ON KEY の飛び先初期化
	asm_line.set( "LD", "", "[svari_on_key01_line]", "HL" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "LD", "", "HL", "svari_on_key01_line" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "LD", "", "DE", "svari_on_key01_line + 2" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "LD", "", "BC", "20 - 2" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "LDIR" );
	this->info.assembler_list.body.push_back( asm_line );
	//	初期化処理 (H.ERROフック)
	asm_line.set( "CALL", "", "setup_h_erro", "" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "EI" );
	this->info.assembler_list.body.push_back( asm_line );
	//	初期化処理 (プログラム起動)
	asm_line.set( "LD", "", "DE", "program_start" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "JP", "", "program_run", "" );
	this->info.assembler_list.body.push_back( asm_line );
	//	初期化処理 (h.timiフック)
	asm_line.set( "LABEL", "", "setup_h_timi", "" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "LD", "", "HL", "h_timi_handler" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "LD", "", "[work_h_timi + 1]", "HL" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "LD", "", "A", "0xC3" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "LD", "", "[work_h_timi]", "A" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "RET" );
	this->info.assembler_list.body.push_back( asm_line );
	//	初期化処理 (h.erroフック)
	asm_line.set( "LABEL", "", "setup_h_erro", "" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "LD", "", "HL", "work_h_erro" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "LD", "", "DE", "h_erro_backup" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "LD", "", "BC", "5" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "LDIR" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "LD", "", "HL", "h_erro_handler" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "LD", "", "[work_h_erro + 1]", "HL" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "LD", "", "A", "0xC3" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "LD", "", "[work_h_erro]", "A" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "RET" );
	this->info.assembler_list.body.push_back( asm_line );

	asm_line.set( "LABEL", "", "jp_hl", "" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "JP", "", "HL", "" );
	this->info.assembler_list.body.push_back( asm_line );

	asm_line.set( "LABEL", "", "program_start", "" );
	this->info.assembler_list.body.push_back( asm_line );

	//	BLIBチェッカー
	this->info.assembler_list.add_label( "bios_syntax_error", "0x4055" );
	this->info.assembler_list.add_label( "bios_calslt", "0x001C" );
	this->info.assembler_list.add_label( "bios_enaslt", "0x0024" );
	this->info.assembler_list.add_label( "work_mainrom", "0xFCC1" );
	this->info.assembler_list.add_label( "work_blibslot", "0xF3D3" );
	this->info.assembler_list.add_label( "signature", "0x4010" );
	asm_line.set( "LABEL", "", "check_blib", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "a", "[work_blibslot]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "h", "0x40" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "CALL", "", "bios_enaslt", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "bc", "8" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "hl", "signature" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "de", "signature_ref" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LABEL", "", "_check_blib_loop", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "a", "[de]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "INC", "", "de", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "CPI" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "JR", "NZ", "_check_blib_exit", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "JP", "PE", "_check_blib_loop", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LABEL", "", "_check_blib_exit", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "PUSH", "", "af", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "a", "[work_mainrom]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "h", "0x40" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "CALL", "", "bios_enaslt", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "EI" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "POP", "", "af", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "RET" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LABEL", "", "signature_ref", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "DEFB", "", "\"BACONLIB\"", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LABEL", "", "call_blib", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "iy", "[work_blibslot - 1]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "CALL", "", "bios_calslt", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "EI" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "RET" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	//	初期化処理用変数
	asm_line.set( "LABEL", "", "heap_next", "" );
	this->info.assembler_list.variables_area.push_back( asm_line );
	asm_line.set( "DEFW", "", "0", "" );
	this->info.assembler_list.variables_area.push_back( asm_line );
	asm_line.set( "LABEL", "", "heap_end", "" );
	this->info.assembler_list.variables_area.push_back( asm_line );
	asm_line.set( "DEFW", "", "0", "" );
	this->info.assembler_list.variables_area.push_back( asm_line );
	asm_line.set( "LABEL", "", "heap_move_size", "" );
	this->info.assembler_list.variables_area.push_back( asm_line );
	asm_line.set( "DEFW", "", "0", "" );
	this->info.assembler_list.variables_area.push_back( asm_line );
	asm_line.set( "LABEL", "", "heap_remap_address", "" );
	this->info.assembler_list.variables_area.push_back( asm_line );
	asm_line.set( "DEFW", "", "0", "" );
	this->info.assembler_list.variables_area.push_back( asm_line );
}

// --------------------------------------------------------------------
void CCOMPILER::exec_compile_body( void ) {
	CASSEMBLER_LINE asm_line;

	this->info.list.reset_position();
	while( !this->info.list.is_end() ) {
		if( this->info.list.is_line_end() && !this->info.list.is_end() ) {
			//	新しい行なので、ラベルの挿入をチェックする
			this->insert_label();
		}
		this->line_compile( true );
		if( !this->info.list.is_line_end() ) {
			this->info.errors.add( SYNTAX_ERROR, this->info.list.get_line_no() );
			this->info.list.p_position++;
		}
	}
}

// --------------------------------------------------------------------
void CCOMPILER::exec_terminator( void ) {
	CASSEMBLER_LINE asm_line;

	this->info.assembler_list.add_label( "bios_newstt", "0x04601" );
	this->info.assembler_list.add_label( "bios_errhand", "0x0406F" );
	this->info.assembler_list.add_label( "work_himem", "0x0FC4A" );

	//	プログラムの終了処理
	asm_line.set( "LABEL", "", "program_termination", "" );
	this->info.assembler_list.body.push_back( asm_line );
	//	プログラムの終了処理 (H.TIMI復元)
	asm_line.set( "CALL", "", "restore_h_erro", "" );
	this->info.assembler_list.body.push_back( asm_line );
	//	プログラムの終了処理 (H.TIMI復元)
	asm_line.set( "CALL", "", "restore_h_timi", "" );
	this->info.assembler_list.body.push_back( asm_line );
	//	プログラムの終了処理 (スタック復元)
	asm_line.set( "LD", "", "SP", "[work_himem]" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "LD", "", "HL", "_basic_end" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "CALL", "", "bios_newstt", "" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "LABEL", "", "_basic_end", "" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "DEFB", "", "':', 0x81, 0x00", "" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "LABEL", "", "err_return_without_gosub", "" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "LD", "", "E", "3" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "JP", "", "bios_errhand", "" );
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "LABEL", "", "heap_start", "" );
	this->info.assembler_list.footer.push_back( asm_line );
	asm_line.set( "LABEL", "", "end_address", "" );
	this->info.assembler_list.footer.push_back( asm_line );
}

// --------------------------------------------------------------------
void CCOMPILER::exec_data( void ) {

	//	データ参照用のラベル
	if( this->info.list.data_line_no.size() == 0 ) {
		return;
	}

	CASSEMBLER_LINE asm_line;
	std::string s_label;

	s_label = "data_" + std::to_string( this->info.list.data_line_no[0] );
	asm_line.set( "LABEL", "", "data_ptr", "" );
	this->info.assembler_list.variables_area.push_back( asm_line );
	asm_line.set( "DEFW", "", s_label, "" );
	this->info.assembler_list.variables_area.push_back( asm_line );
}

// --------------------------------------------------------------------
void CCOMPILER::exec_sub_run( void ) {
	CASSEMBLER_LINE asm_line;

	//	RUN用サブルーチン
	asm_line.set( "LABEL", "", "program_run", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "HL", "heap_start" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "[heap_next]", "HL" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "SP", "[work_himem]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "HL", "err_return_without_gosub" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "PUSH", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "PUSH", "", "DE", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "DE", std::to_string( this->info.options.stack_size ) );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "XOR", "", "A", "A" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "HL", "[work_himem]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "SBC", "", "HL", "DE" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "[heap_end]", "HL" );
	this->info.assembler_list.subroutines.push_back( asm_line );

	//	RUN用サブルーチンの中で変数領域をクリアする
	int variable_area_bytes = this->info.variables.var_area_size + this->info.variables.vars_area_count + this->info.variables.vara_area_count;
	if( variable_area_bytes == 0 ) {
		//	変数を一切使ってないので初期化不要
	}
	else {
		asm_line.set( "LD", "", "HL", "var_area_start" );
		this->info.assembler_list.subroutines.push_back( asm_line );
		asm_line.set( "LD", "", "DE", "var_area_start + 1" );
		this->info.assembler_list.subroutines.push_back( asm_line );
		asm_line.set( "LD", "", "BC", "varsa_area_end - var_area_start - 1" );
		this->info.assembler_list.subroutines.push_back( asm_line );
		asm_line.set( "LD", "", "[HL]", "0" );
		this->info.assembler_list.subroutines.push_back( asm_line );
		asm_line.set( "LDIR" );
		this->info.assembler_list.subroutines.push_back( asm_line );
	}
	if( this->info.variables.vars_area_count == 0 ) {
		//	文字列変数を一切使ってないので初期化不要
	}
	else if( this->info.variables.vars_area_count == 1 ) {
		asm_line.set( "LD", "", "HL", this->info.constants.s_blank_string );
		this->info.assembler_list.subroutines.push_back( asm_line );
		asm_line.set( "LD", "", "[vars_area_start]", "HL" );
		this->info.assembler_list.subroutines.push_back( asm_line );
	}
	else {
		asm_line.set( "LD", "", "HL", this->info.constants.s_blank_string );
		this->info.assembler_list.subroutines.push_back( asm_line );
		asm_line.set( "LD", "", "[vars_area_start]", "HL" );
		this->info.assembler_list.subroutines.push_back( asm_line );
		asm_line.set( "LD", "", "HL", "vars_area_start" );
		this->info.assembler_list.subroutines.push_back( asm_line );
		asm_line.set( "LD", "", "DE", "vars_area_start + 2" );
		this->info.assembler_list.subroutines.push_back( asm_line );
		asm_line.set( "LD", "", "BC", "vars_area_end - vars_area_start - 2" );
		this->info.assembler_list.subroutines.push_back( asm_line );
		asm_line.set( "LDIR" );
		this->info.assembler_list.subroutines.push_back( asm_line );
	}
	asm_line.set( "RET" );
	this->info.assembler_list.subroutines.push_back( asm_line );
}

// --------------------------------------------------------------------
void CCOMPILER::exec_sub_interrupt_process( void ) {
	CASSEMBLER_LINE asm_line;

	//	割り込みフラグ処理ルーチン
	asm_line.set( "LABEL", "", "interrupt_process", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	//	割り込みフラグ処理ルーチン ( ON SPRITE )
	asm_line.set( "LD", "", "A", "[svarb_on_sprite_running]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "OR", "", "A", "A" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "JR", "NZ", "_skip_on_sprite", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "A", "[svarb_on_sprite_exec]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "OR", "", "A", "A" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "JR", "Z", "_skip_on_sprite", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "[svarb_on_sprite_running]", "A" );
	this->info.assembler_list.subroutines.push_back( asm_line );

	asm_line.set( "LD", "", "HL", "[svari_on_sprite_line]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "PUSH", "", "HL", "" );				//	飛び先へ飛ぶ前に「STACK 1段」 数あわせのダミー
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "PUSH", "", "HL", "" );				//	飛び先へ飛ぶ前に「STACK 2段」 数あわせのダミー
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "PUSH", "", "HL", "" );				//	飛び先へ飛ぶ前に「STACK 3段」 数あわせのダミー
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "CALL", "", "jp_hl", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LABEL", "", "_on_sprite_return_address", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "POP", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "POP", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "POP", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );

	asm_line.set( "XOR", "", "A", "A" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "[svarb_on_sprite_running]", "A" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LABEL", "", "_skip_on_sprite", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );

	//	割り込みフラグ処理ルーチン ( ON INTERVAL )
	asm_line.set( "LD", "", "A", "[svarb_on_interval_exec]" );		//	0:Through, 1:Execute
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "DEC", "", "A", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "JR", "NZ", "_skip_on_interval", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "[svarb_on_interval_exec]", "A" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "HL", "[svari_on_interval_line]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "PUSH", "", "HL", "" );							//	飛び先へ飛ぶ前に「STACK 1段」 数あわせのダミー
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "PUSH", "", "HL", "" );							//	飛び先へ飛ぶ前に「STACK 2段」 数あわせのダミー
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "PUSH", "", "HL", "" );							//	飛び先へ飛ぶ前に「STACK 3段」 数あわせのダミー
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "CALL", "", "jp_hl", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "POP", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "POP", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "POP", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "A", "[svarb_on_interval_mode]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "CP", "", "A", "2" );								//	割り込み保留なら解除する
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "JR", "NZ", "_skip_on_interval", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "DEC", "", "A", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "[svarb_on_interval_mode]", "A" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LABEL", "", "_skip_on_interval", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	//	割り込みフラグ処理ルーチン ( ON STRIG )
	asm_line.set( "LD", "", "HL", "svarf_on_strig0_mode" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "DE", "svari_on_strig0_line" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "B", "5" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	//	-- STRIG(n)
	asm_line.set( "LABEL", "", "_on_strig_loop1", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	//	-- svarf_on_strig0_mode[+00] を確認する
	asm_line.set( "LD", "", "A", "[HL]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "INC", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "DEC", "", "A", "" );									//	1:ON か？
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "JR", "NZ", "_skip_strig1", "" );						// 0:OFF/STOP なら処理をスキップする
	this->info.assembler_list.subroutines.push_back( asm_line );
	//	-- svarf_on_strig0_mode[+01] を確認する
	asm_line.set( "OR", "", "A", "[HL]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "JR", "Z", "_skip_strig1", "" );						// 0x00 なら処理をスキップする
	this->info.assembler_list.subroutines.push_back( asm_line );
	//	-- svarf_on_strig0_mode[+02] を確認する
	asm_line.set( "INC", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "INC", "", "A", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "OR", "", "A", "[HL]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "DEC", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "JR", "NZ", "_skip_strig1", "" );						// 0xFF なら処理をスキップする
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "DEC", "", "A", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "INC", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "[HL]", "A" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "DEC", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	//	-- svari_on_strig0_line を CALL する
	asm_line.set( "PUSH", "", "HL", "" );								// svarf_on_strig0_mode + 01 を保存
	this->info.assembler_list.subroutines.push_back( asm_line );		//	飛び先へ飛ぶ前に「STACK 1段」
	asm_line.set( "EX", "", "DE", "HL" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "E", "[HL]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "INC", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "D", "[HL]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "DEC", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "PUSH", "", "HL", "" );				// svari_on_strig0_line 保存
	this->info.assembler_list.subroutines.push_back( asm_line );																//	飛び先へ飛ぶ前に「STACK 2段」
	asm_line.set( "EX", "", "DE", "HL" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "PUSH", "", "BC", "" );				//	飛び先へ飛ぶ前に「STACK 3段」
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "CALL", "", "jp_hl", "" );				//	割り込みの飛び先
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "POP", "", "BC", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "POP", "", "DE", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "POP", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	//	-- 次へ
	asm_line.set( "LABEL", "", "_skip_strig1", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "INC", "", "DE", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "INC", "", "DE", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "INC", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "INC", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "INC", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "DJNZ", "", "_on_strig_loop1", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	//	割り込みフラグ処理ルーチン (ON KEY)
	asm_line.set( "LD", "", "HL", "svarf_on_key01_mode" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "DE", "svari_on_key01_line" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "B", "0x0A" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LABEL", "", "_on_key_loop1", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "A", "[HL]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "INC", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "AND", "", "A", "[HL]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "JR", "Z", "_skip_key1", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "[HL]", "0" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "PUSH", "", "HL", "" );	//	飛び先へ飛ぶ前に「STACK 1段」
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "EX", "", "DE", "HL" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "E", "[HL]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "INC", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "D", "[HL]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "DEC", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "EX", "", "DE", "HL" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "PUSH", "", "DE", "" );	//	飛び先へ飛ぶ前に「STACK 2段」
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "PUSH", "", "BC", "" );	//	飛び先へ飛ぶ前に「STACK 3段」
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "CALL", "", "jp_hl", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "POP", "", "BC", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "POP", "", "DE", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "POP", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LABEL", "", "_skip_key1", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "INC", "", "DE", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "INC", "", "DE", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "INC", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "INC", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "INC", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "DJNZ", "", "_on_key_loop1", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	//	割り込みフラグ処理ルーチン終わり
	asm_line.set( "RET" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LABEL", "", "interrupt_process_end", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
}

// --------------------------------------------------------------------
void CCOMPILER::exec_sub_h_timi( void ) {
	CASSEMBLER_LINE asm_line;

	//	H.TIMI処理ルーチン
	asm_line.set( "LABEL", "", "h_timi_handler", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "PUSH", "", "AF", "" );		// VDP S#0 の値 (Aレジスタ) を保存
	this->info.assembler_list.subroutines.push_back( asm_line );
	//	H.TIMI処理ルーチン ( ON SPRITE 処理 )
	asm_line.set( "LD", "", "B", "A" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "A", "[svarb_on_sprite_mode]" );	//	0:OFF, 1:ON, 2:STOP
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "OR", "", "A", "A" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "JR", "Z", "_end_of_sprite", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "A", "B" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	//	-- スプライト衝突フラフが立っているか？
	asm_line.set( "AND", "", "A", "0x20" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "[svarb_on_sprite_exec]", "A" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LABEL", "", "_end_of_sprite", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );

	//	H.TIMI処理ルーチン ( ON INTERVAL 処理 )
	asm_line.set( "LD", "", "A", "[svarb_on_interval_mode]" );	//	0:OFF, 1:ON, 2:STOP
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "OR", "", "A", "A" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "JR", "Z", "_end_of_interval", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	//	-- デクリメントカウンターを減算
	asm_line.set( "LD", "", "HL", "[svari_on_interval_counter]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "A", "L" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "OR", "", "A", "H" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "JR", "Z", "_happned_interval", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "DEC", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "[svari_on_interval_counter]", "HL" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "JR", "", "_end_of_interval", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LABEL", "", "_happned_interval", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "A", "[svarb_on_interval_mode]" );			//	0:OFF, 1:ON, 2:STOP
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "DEC", "", "A", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "JR", "NZ", "_end_of_interval", "" );		//	STOP なら保留
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "INC", "", "A", "" );									//	1:Execute
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "[svarb_on_interval_exec]", "A" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "HL", "[svari_on_interval_value]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "[svari_on_interval_counter]", "HL" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LABEL", "", "_end_of_interval", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );

	//	H.TIMI処理ルーチン ( ON STRIG 処理 )
	this->info.assembler_list.add_label( "bios_gttrig", "0x00D8" );
	asm_line.set( "LD", "", "HL", "svarf_on_strig0_mode" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "BC", "0x0500" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	//	-- STRIG(n)
	asm_line.set( "LABEL", "", "_on_strig_loop2", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "A", "[HL]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "INC", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "OR", "", "A", "A" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "JR", "Z", "_skip_strig2", "" );					//	0:OFF なら、処理をスキップ
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "A", "[HL]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "INC", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "[HL]", "A" );							//	1つ前の GTTRIG状態を更新
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "DEC", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "A", "C" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "PUSH", "", "BC", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "CALL", "", "bios_gttrig", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "POP", "", "BC", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "[HL]", "A" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LABEL", "", "_skip_strig2", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "INC", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "INC", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "INC", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "INC", "", "C", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "DJNZ", "", "_on_strig_loop2", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );

	asm_line.set( "LABEL", "", "_end_of_strig", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	//	H.TIMI処理ルーチン ( ON KEY 処理 )
	asm_line.set( "IN", "", "A", "[0xAA]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "AND", "", "A", "0xF0" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "OR", "", "A", "6" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "B", "A" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "OUT", "", "[0xAA]", "A" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "IN", "", "A", "[0xA9]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "OR", "", "A", "0x1E" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "RRCA", "", "", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "C", "A" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "A", "B" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "INC", "", "A", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "OUT", "", "[0xAA]", "A" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "IN", "", "A", "[0xA9]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "OR", "", "A", "0xFC" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "AND", "", "A", "C" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "C", "A" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "HL", "svarf_on_key06_mode" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "B", "0x90" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "CALL", "", "_on_key_sub", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "HL", "svarf_on_key07_mode" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "B", "0xA0" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "CALL", "", "_on_key_sub", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "HL", "svarf_on_key08_mode" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "B", "0xC0" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "CALL", "", "_on_key_sub", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "HL", "svarf_on_key09_mode" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "B", "0x81" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "CALL", "", "_on_key_sub", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "HL", "svarf_on_key10_mode" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "B", "0x82" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "CALL", "", "_on_key_sub", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "A", "C" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "XOR", "", "A", "0x80" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "C", "A" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "HL", "svarf_on_key01_mode" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "B", "0x90" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "CALL", "", "_on_key_sub", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "HL", "svarf_on_key02_mode" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "B", "0xA0" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "CALL", "", "_on_key_sub", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "HL", "svarf_on_key03_mode" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "B", "0xC0" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "CALL", "", "_on_key_sub", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "HL", "svarf_on_key04_mode" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "B", "0x81" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "CALL", "", "_on_key_sub", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "HL", "svarf_on_key05_mode" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "B", "0x82" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "CALL", "", "_on_key_sub", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	//	H.TIMI処理ルーチン終了処理
	asm_line.set( "POP", "", "AF", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "JP", "", "h_timi_backup", "" );		// VDP S#0 の値 (Aレジスタ) を復帰
	this->info.assembler_list.subroutines.push_back( asm_line );
	//	H.TIMI処理ルーチンの中のサブルーチン
	asm_line.set( "LABEL", "", "_on_key_sub", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "A", "[HL]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "AND", "", "A", "B" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "INC", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "INC", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "D", "[HL]" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "[HL]", "0" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "RET", "Z", "", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "AND", "", "A", "C" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "RET", "NZ", "", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "DEC", "", "A", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "[HL]", "A" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "AND", "", "A", "D" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "RET", "NZ", "", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "DEC", "", "HL", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "DEC", "", "A", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "[HL]", "A" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "RET" );
	this->info.assembler_list.subroutines.push_back( asm_line );
}

// --------------------------------------------------------------------
void CCOMPILER::exec_sub_restore_h_timi( void ) {
	CASSEMBLER_LINE asm_line;

	//	H.TIMI復元処理ルーチン
	asm_line.set( "LABEL", "", "restore_h_timi", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "DI" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "HL", "h_timi_backup" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "DE", "work_h_timi" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "BC", "5" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LDIR" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "EI" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "RET" );
	this->info.assembler_list.subroutines.push_back( asm_line );

	//	H.TIMI待避エリア
	asm_line.set( "LABEL", "", "h_timi_backup", "" );
	this->info.assembler_list.variables_area.push_back( asm_line );
	asm_line.set( "DEFB", "", "0, 0, 0, 0, 0", "" );
	this->info.assembler_list.variables_area.push_back( asm_line );
}

// --------------------------------------------------------------------
void CCOMPILER::exec_sub_on_error( void ) {
	CASSEMBLER_LINE asm_line;

	//	H.ERRO復元処理ルーチン
	asm_line.set( "LABEL", "", "restore_h_erro", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "DI" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "HL", "h_erro_backup" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "DE", "work_h_erro" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LD", "", "BC", "5" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LDIR" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "EI" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "LABEL", "", "_code_ret", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "RET" );
	this->info.assembler_list.subroutines.push_back( asm_line );

	//	H.ERRO処理ルーチン
	asm_line.set( "LABEL", "", "h_erro_handler", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );


	asm_line.set( "PUSH", "", "DE", "" );				// Eにエラーコードが入っているので保存
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "CALL", "", "restore_h_timi", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "CALL", "", "restore_h_erro", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "POP", "", "DE", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );
	asm_line.set( "JP", "", "work_h_erro", "" );
	this->info.assembler_list.subroutines.push_back( asm_line );

	//	H.ERRO待避エリア
	asm_line.set( "LABEL", "", "h_erro_backup", "" );
	this->info.assembler_list.variables_area.push_back( asm_line );
	asm_line.set( "DEFB", "", "0, 0, 0, 0, 0", "" );
	this->info.assembler_list.variables_area.push_back( asm_line );
}

// --------------------------------------------------------------------
void CCOMPILER::exec_subroutines( void ) {
	CASSEMBLER_LINE asm_line;

	this->exec_sub_run();
	this->exec_sub_interrupt_process();
	this->exec_sub_h_timi();
	this->exec_sub_restore_h_timi();
	this->exec_sub_on_error();
}

// --------------------------------------------------------------------
bool CCOMPILER::exec( std::string s_name ) {
	CASSEMBLER_LINE asm_line;
	CVARIABLE variable;

	this->info.p_compiler = this;

	//	DEFINT, DEFSNG, DEFDBL, DEFSTR を処理する。
	//	実装をシンプルにするために、途中で変わることは想定しない。
	this->info.list.reset_position();
	this->info.variable_manager.analyze_defvars( &(this->info) );

	//	空文字列を文字列プールに追加
	CSTRING value;
	value.set( "" );
	this->info.constants.s_blank_string = this->info.constants.add( value );

	this->info.assembler_list.add_label( "work_h_timi", "0x0fd9f" );
	this->info.assembler_list.add_label( "work_h_erro", "0x0ffb1" );

	this->exec_header( s_name );
	this->exec_initializer( s_name );
	this->exec_compile_body();
	this->exec_terminator();
	this->exec_data();

	//	割り込みフラグ
	this->info.variable_manager.put_special_variable( &(this->info), "on_interval_mode", CVARIABLE_TYPE::UNSIGNED_BYTE );
	this->info.variable_manager.put_special_variable( &(this->info), "on_interval_exec", CVARIABLE_TYPE::UNSIGNED_BYTE );
	this->info.variable_manager.put_special_variable( &(this->info), "on_interval_line", CVARIABLE_TYPE::INTEGER );
	this->info.variable_manager.put_special_variable( &(this->info), "on_interval_value", CVARIABLE_TYPE::INTEGER );
	this->info.variable_manager.put_special_variable( &(this->info), "on_interval_counter", CVARIABLE_TYPE::INTEGER );
	this->info.variable_manager.put_special_variable( &(this->info), "on_strig0_mode", CVARIABLE_TYPE::SINGLE_REAL );
	this->info.variable_manager.put_special_variable( &(this->info), "on_strig1_mode", CVARIABLE_TYPE::SINGLE_REAL );
	this->info.variable_manager.put_special_variable( &(this->info), "on_strig2_mode", CVARIABLE_TYPE::SINGLE_REAL );
	this->info.variable_manager.put_special_variable( &(this->info), "on_strig3_mode", CVARIABLE_TYPE::SINGLE_REAL );
	this->info.variable_manager.put_special_variable( &(this->info), "on_strig4_mode", CVARIABLE_TYPE::SINGLE_REAL );
	this->info.variable_manager.put_special_variable( &(this->info), "on_strig5_mode_dummy", CVARIABLE_TYPE::SINGLE_REAL );
	this->info.variable_manager.put_special_variable( &(this->info), "on_strig6_mode_dummy", CVARIABLE_TYPE::SINGLE_REAL );
	this->info.variable_manager.put_special_variable( &(this->info), "on_strig7_mode_dummy", CVARIABLE_TYPE::SINGLE_REAL );
	this->info.variable_manager.put_special_variable( &(this->info), "on_strig0_line", CVARIABLE_TYPE::INTEGER );
	this->info.variable_manager.put_special_variable( &(this->info), "on_strig1_line", CVARIABLE_TYPE::INTEGER );
	this->info.variable_manager.put_special_variable( &(this->info), "on_strig2_line", CVARIABLE_TYPE::INTEGER );
	this->info.variable_manager.put_special_variable( &(this->info), "on_strig3_line", CVARIABLE_TYPE::INTEGER );
	this->info.variable_manager.put_special_variable( &(this->info), "on_strig4_line", CVARIABLE_TYPE::INTEGER );
	this->info.variable_manager.put_special_variable( &(this->info), "on_key01_mode", CVARIABLE_TYPE::SINGLE_REAL );
	this->info.variable_manager.put_special_variable( &(this->info), "on_key02_mode", CVARIABLE_TYPE::SINGLE_REAL );
	this->info.variable_manager.put_special_variable( &(this->info), "on_key03_mode", CVARIABLE_TYPE::SINGLE_REAL );
	this->info.variable_manager.put_special_variable( &(this->info), "on_key04_mode", CVARIABLE_TYPE::SINGLE_REAL );
	this->info.variable_manager.put_special_variable( &(this->info), "on_key05_mode", CVARIABLE_TYPE::SINGLE_REAL );
	this->info.variable_manager.put_special_variable( &(this->info), "on_key06_mode", CVARIABLE_TYPE::SINGLE_REAL );
	this->info.variable_manager.put_special_variable( &(this->info), "on_key07_mode", CVARIABLE_TYPE::SINGLE_REAL );
	this->info.variable_manager.put_special_variable( &(this->info), "on_key08_mode", CVARIABLE_TYPE::SINGLE_REAL );
	this->info.variable_manager.put_special_variable( &(this->info), "on_key09_mode", CVARIABLE_TYPE::SINGLE_REAL );
	this->info.variable_manager.put_special_variable( &(this->info), "on_key10_mode", CVARIABLE_TYPE::SINGLE_REAL );
	this->info.variable_manager.put_special_variable( &(this->info), "on_key11_mode_dummy", CVARIABLE_TYPE::SINGLE_REAL );
	this->info.variable_manager.put_special_variable( &(this->info), "on_key12_mode_dummy", CVARIABLE_TYPE::SINGLE_REAL );
	this->info.variable_manager.put_special_variable( &(this->info), "on_key13_mode_dummy", CVARIABLE_TYPE::SINGLE_REAL );
	this->info.variable_manager.put_special_variable( &(this->info), "on_key14_mode_dummy", CVARIABLE_TYPE::SINGLE_REAL );
	this->info.variable_manager.put_special_variable( &(this->info), "on_key15_mode_dummy", CVARIABLE_TYPE::SINGLE_REAL );
	this->info.variable_manager.put_special_variable( &(this->info), "on_key16_mode_dummy", CVARIABLE_TYPE::SINGLE_REAL );
	this->info.variable_manager.put_special_variable( &(this->info), "on_key01_line", CVARIABLE_TYPE::INTEGER );
	this->info.variable_manager.put_special_variable( &(this->info), "on_key02_line", CVARIABLE_TYPE::INTEGER );
	this->info.variable_manager.put_special_variable( &(this->info), "on_key03_line", CVARIABLE_TYPE::INTEGER );
	this->info.variable_manager.put_special_variable( &(this->info), "on_key04_line", CVARIABLE_TYPE::INTEGER );
	this->info.variable_manager.put_special_variable( &(this->info), "on_key05_line", CVARIABLE_TYPE::INTEGER );
	this->info.variable_manager.put_special_variable( &(this->info), "on_key06_line", CVARIABLE_TYPE::INTEGER );
	this->info.variable_manager.put_special_variable( &(this->info), "on_key07_line", CVARIABLE_TYPE::INTEGER );
	this->info.variable_manager.put_special_variable( &(this->info), "on_key08_line", CVARIABLE_TYPE::INTEGER );
	this->info.variable_manager.put_special_variable( &(this->info), "on_key09_line", CVARIABLE_TYPE::INTEGER );
	this->info.variable_manager.put_special_variable( &(this->info), "on_key10_line", CVARIABLE_TYPE::INTEGER );
	this->info.variable_manager.put_special_variable( &(this->info), "on_sprite_mode", CVARIABLE_TYPE::UNSIGNED_BYTE );
	this->info.variable_manager.put_special_variable( &(this->info), "on_sprite_exec", CVARIABLE_TYPE::UNSIGNED_BYTE );
	this->info.variable_manager.put_special_variable( &(this->info), "on_sprite_running", CVARIABLE_TYPE::UNSIGNED_BYTE );
	this->info.variable_manager.put_special_variable( &(this->info), "on_sprite_line", CVARIABLE_TYPE::INTEGER );

	//	変数ダンプ
	this->info.constants.dump( this->info.assembler_list, this->info.options );
	this->info.variables.dump( this->info.assembler_list, this->info.options );

	this->exec_subroutines();

	if( this->info.errors.list.size() ) {
		//	エラーがある場合終了する
		return false;
	}

	//	最適化
	if( this->info.options.optimize_level != COPTIMIZE_LEVEL::NONE ) {
		this->optimize();
	}
	return( this->info.errors.list.size() == 0 );
}

// --------------------------------------------------------------------
void CCOMPILER::optimize( void ) {

	this->optimize_interrupt_process();
	this->optimize_push_pop();
}

// --------------------------------------------------------------------
//	過剰に出し過ぎた call interrupt_process を削減する
void CCOMPILER::optimize_interrupt_process( void ) {
	std::vector< CASSEMBLER_LINE >::iterator p, p_search, p_next;

	//	単純に連続実行されている場合
	for( p = this->info.assembler_list.body.begin(); p != this->info.assembler_list.body.end(); p++ ) {
		if( p->type == CMNEMONIC_TYPE::CALL && p->condition == CCONDITION::NONE && p->operand1.type == COPERAND_TYPE::CONSTANT && p->operand1.s_value == "INTERRUPT_PROCESS" ) {
			for( p_search = p + 1; p_search != this->info.assembler_list.body.end(); ) {
				if( p_search->type == CMNEMONIC_TYPE::CALL && p_search->condition == CCONDITION::NONE && p_search->operand1.type == COPERAND_TYPE::CONSTANT && p_search->operand1.s_value == "INTERRUPT_PROCESS" ) {
					p_next = p_search - 1;
					this->info.assembler_list.body.erase( p_search );
					p_search = p_next + 1;
				}
				else {
					break;
				}
			}
		}
	}

	//		call interrupt_process
	//	; comment
	//	の場合。interrupt_process は必要ない。
	for( p = this->info.assembler_list.body.begin(); p != this->info.assembler_list.body.end(); p++ ) {
		if( p->type == CMNEMONIC_TYPE::CALL && p->condition == CCONDITION::NONE && p->operand1.type == COPERAND_TYPE::CONSTANT && p->operand1.s_value == "INTERRUPT_PROCESS" ) {
			p_search = p + 1;
			if( p_search == this->info.assembler_list.body.end() ) {
				break;
			}
			if( p_search->type == CMNEMONIC_TYPE::COMMENT ) {
				this->info.assembler_list.body.erase( p );
			}
		}
	}
}

// --------------------------------------------------------------------
//	過剰に出し過ぎた push/pop を削減する
void CCOMPILER::optimize_push_pop( void ) {
	std::vector< CASSEMBLER_LINE >::iterator p, p_next, p_back;

	//	push rp; pop rp を削除する
	for( p = this->info.assembler_list.body.begin(); p != this->info.assembler_list.body.end(); p++ ) {
		if( p->type == CMNEMONIC_TYPE::PUSH && p->operand1.type == COPERAND_TYPE::REGISTER ) {
			p_next = p + 1;
			if( p_next == this->info.assembler_list.body.end() ) {
				break;
			}
			if( p_next->type == CMNEMONIC_TYPE::POP && p->operand1.s_value == p_next->operand1.s_value ) {
				p_back = p - 1;
				this->info.assembler_list.body.erase( p_next );
				this->info.assembler_list.body.erase( p );
				p = p_back;
			}
		}
	}

	//	LD HL, xxx
	//	PUSH HL
	//	LD HL, xxx
	//	↓
	//	LD HL, xxx
	//	PUSH HL
	for( p = this->info.assembler_list.body.begin(); p != this->info.assembler_list.body.end(); p++ ) {
		if( p->type == CMNEMONIC_TYPE::LD && p->operand1.type == COPERAND_TYPE::REGISTER && p->operand1.s_value == "HL" && (p->operand2.type == COPERAND_TYPE::CONSTANT || p->operand2.type == COPERAND_TYPE::CONSTANT)) {
			p_next = p + 1;
			if( p_next == this->info.assembler_list.body.end() ) break;
			if( p_next->type != CMNEMONIC_TYPE::PUSH || p_next->operand1.s_value != "HL" ) continue;

			p_next++;
			if( p_next == this->info.assembler_list.body.end() ) break;
			if( p_next->type != CMNEMONIC_TYPE::LD || p_next->operand1.s_value != "HL" || p_next->operand2.s_value != p->operand2.s_value ) continue;

			this->info.assembler_list.body.erase( p_next );
		}
	}

	//	PUSH rp1
	//	POP rp2
	//	↓
	//	LD  rp2l, rp1l
	//	LD  rp2h, rp1h
	std::string s_reg1, s_reg2, s_reg10, s_reg20, s_reg11, s_reg21;
	for( p = this->info.assembler_list.body.begin(); p != this->info.assembler_list.body.end(); p++ ) {
		if( p->type == CMNEMONIC_TYPE::PUSH ) {
			p_next = p + 1;
			if( p_next == this->info.assembler_list.body.end() ) break;
			if( p_next->type != CMNEMONIC_TYPE::POP ) continue;

			s_reg1 = p->operand1.s_value;
			s_reg2 = p_next->operand1.s_value;
			s_reg10 = s_reg1[0];
			s_reg20 = s_reg2[0];
			s_reg11 = s_reg1[1];
			s_reg21 = s_reg2[1];
			p->set(      "LD", "", s_reg21, s_reg11 );
			p_next->set( "LD", "", s_reg20, s_reg10 );
		}
	}

	//	LD HL, constant
	//	LD A, L
	//	↓
	//	LD A, constant
	for( p = this->info.assembler_list.body.begin(); p != this->info.assembler_list.body.end(); p++ ) {
		if( p->type == CMNEMONIC_TYPE::LD && p->operand1.type == COPERAND_TYPE::REGISTER && p->operand1.s_value == "HL" && p->operand2.type == COPERAND_TYPE::CONSTANT ) {
			p_next = p + 1;
			if( p_next == this->info.assembler_list.body.end() ) {
				break;
			}
			if( p_next->type == CMNEMONIC_TYPE::LD && p_next->operand1.type == COPERAND_TYPE::REGISTER && p_next->operand1.s_value == "A" && p_next->operand2.type == COPERAND_TYPE::REGISTER && p_next->operand2.s_value == "L" ) {
				this->info.assembler_list.body.erase( p_next );
				if( p->operand2.s_value == "0" ) {
					p->operand2.s_value = "A";
					p->operand2.type = COPERAND_TYPE::REGISTER;
					p->type = CMNEMONIC_TYPE::XOR;
				}
				p->operand1.s_value = "A";
			}
		}
	}
}

// --------------------------------------------------------------------
void CCOMPILER::put_logical_operation( void ) {
	CASSEMBLER_LINE asm_line;
	const char* s_rop[] = {
		"AND", "OR", "XOR", "PSET", "PRESET", "TAND", "TOR", "TXOR", "TPSET", "TPRESET"
	};
	const int i_rop[] = {
		1, 2, 3, 0, 4, 1+8, 2+8, 3+8, 0+8, 4+8 
	};
	int i, rop = 0;

	if( this->info.list.is_command_end() ) {
		rop = 0;
	}
	else if( this->info.list.p_position->s_word == "," ) {
		this->info.list.p_position++;
		if( this->info.list.is_command_end() ) {
			rop = 0;
			this->info.errors.add( SYNTAX_ERROR, this->info.list.p_position->line_no );
		}
		else {
			for( i = 0; i < (int)(sizeof(s_rop)/sizeof(s_rop[0])); i++ ) {
				if( this->info.list.p_position->s_word == s_rop[i] ) {
					break;
				}
			}
			if(i >= (int)(sizeof( s_rop ) / sizeof( s_rop[ 0 ] ))){
				rop = 0;
				this->info.errors.add( SYNTAX_ERROR, this->info.list.p_position->line_no );
			}
			else {
				rop = i_rop[i];
				this->info.list.p_position++;
			}
		}
	}

	this->info.assembler_list.add_label( "work_logopr", "0x0fB02" );
	if( rop == 0 ) {
		asm_line.set( "XOR", "", "A", "A" );
	}
	else {
		asm_line.set( "LD", "", "A", std::to_string( rop ) );
	}
	this->info.assembler_list.body.push_back( asm_line );
	asm_line.set( "LD", "", "[work_logopr]", "A" );
	this->info.assembler_list.body.push_back( asm_line );
}
