; =============================================================================
;	MSX-BACON Library
; -----------------------------------------------------------------------------
;	Copyright (C)2023 HRA!
; =============================================================================

chget		:= 0x009F
rslreg		:= 0x0138
calbas		:= 0x0159
errhand		:= 0x406F					; BIOS の BASICエラー処理ルーチン E にエラーコード。戻ってこない。
blibslot	:= 0xF3D3
putpnt		:= 0xF3F8
getpnt		:= 0xF3FA
buf			:= 0xF55E
fnkstr		:= 0xF87F					; ファンクションキーの文字列 16文字 x 10個
exptbl		:= 0xFCC1

; BASIC error codes
error_syntax					:= 2
error_illegal_function_call		:= 5
error_overflow					:= 6
error_out_of_memory				:= 7
error_subscript_out_of_range	:= 9
error_redimensioned_array		:= 10
error_division_by_zero			:= 11
error_type_mismatch				:= 13
error_out_of_string_space		:= 14
error_string_too_long			:= 15
error_device_IO					:= 19

			org		0x4000

			db		"AB"				; +0000 id
			dw		init_address		; +0002 init
			dw		0					; +0004 statement
			dw		0					; +0006 device
			dw		0					; +0008 text
			dw		0					; +000A reserved0
			dw		0					; +000C reserved1
			dw		0					; +000E reserved2

			db		"BACONLIB"			; Singnature

; =============================================================================
;	MSX-BACON Library Routine public entries
;		sub_xxxx なアドレスは、今後の修正などで変更になる恐れがありますので、
;		blib_entries にある blib_xxxx を CALL すること。
;		ここのアドレス値は維持されます。
; =============================================================================
blib_entries::
	blib_key_list:
			jp		sub_key_list
	blib_iotget_int:
			jp		sub_iotget_int
	blib_iotget_str:
			jp		sub_iotget_str
	blib_iotput_int:
			jp		sub_iotput_int
	blib_iotput_str:
			jp		sub_iotput_str
	blib_strcmp:
			jp		sub_strcmp
	blib_inkey:
			jp		sub_inkey
	blib_right:
			jp		sub_right
	blib_left:
			jp		sub_left
	blib_mid:
			jp		sub_mid

; =============================================================================
;	ROMカートリッジで用意した場合の初期化ルーチン
;	bacon_loader でロードした場合も、ロード時にここが呼ばれる
; =============================================================================
init_address::
			scope	update_blibslt
			; BLIBSLOT を更新する
			; -- プライマリスロットレジスタを読んで、Page1 のスロットを得る
			call	rslreg				; Get Primary Slot Register
			rrca
			rrca
			and		a, 0b0000_0011
			ld		b, a
			; -- そのスロットが拡張されているか調べる
			add		a, exptbl & 255
			ld		l, a
			ld		h, exptbl >> 8		; HL = EXPTBL + A
			ld		a, [hl]
			and		a, 0x80
			or		a, b
			ld		b, a
			inc		hl
			inc		hl
			inc		hl
			inc		hl
			ld		a, [hl]
			and		a, 0b0000_1100
			or		a, b
			ld		[blibslot], a
			ret
			endscope

; =============================================================================
;	KEY LIST
;	input:
;		none
;	output:
;		none
;	break:
;		A, B, D, E, H, L, F
;	comment:
;		MSX-BASIC の KEY LIST 相当の動作
; =============================================================================
			scope	sub_key_list
sub_key_list::
			ld		hl, fnkstr			; ファンクションキー文字列の場所
			ld		de, 16				; 1つのキーは 16文字
			ld		b, 10				; 10個のキーを表示する
	_loop_key:
			push	hl
	_loop_char:
			ld		a, [hl]
			inc		hl
			or		a, a
			jr		z, _exit_char
			cp		a, 2				; グラフィック文字のプレコード以外のコントロール文字は置換する
			jr		c, _no_replace
			cp		a, 32
			jr		nc, _no_replace
			ld		a, 32				; スペースに置換
	_no_replace:
			rst		0x18				; 1文字表示。レジスタ全保存。
			jr		_loop_char
	_exit_char:
			ld		a, 13
			rst		0x18
			ld		a, 10
			rst		0x18
			pop		hl
			add		hl, de
			djnz	_loop_key
			ret
			endscope

; =============================================================================
;	_send_device_path
;	input:
;		HL ... デバイスパス文字列 (BASIC形式)
;	output:
;		none
;	break:
;		A, B, C, D, E, H, L, F
;	comment:
;		デバイスパス文字列を送信する。
; =============================================================================
			scope	_send_device_path
_send_device_path::
			ld		c, 8
			; デバイスパス送信開始コマンド
			ld		a, 0xe0
			out		[c], a
			ld		a, 1
			out		[c], a
			ld		a, 0x53
			out		[c], a
			; このまま _send_string へ続く
			endscope

; =============================================================================
;	_send_string
;	input:
;		HL ... 送信文字列 (BASIC形式)
;	output:
;		none
;	break:
;		A, B, C, D, E, H, L, F
;	comment:
;		送信文字列を送信する。
; =============================================================================
			scope	_send_string
_send_string::
			ld		a, 0xc0
			out		[c], a
			; 文字列の長さを取得
			ld		a, [hl]
	_iot_send_string_loop1:
			ld		b, a
			cp		a, 64							; 64文字以上の場合は特別処理を実施する
			jr		c, _iot_send_string_skip
			sub		a, 63
			ld		b, 0x7f							; 特別処理を示すコード
	_iot_send_string_skip:
			out		[c], b
			ld		d, a
			ld		a, b
			and		a, 0x3f
			ld		b, a
	_iot_send_string_loop2:
			inc		hl
			ld		a, [hl]
			out		[c], a
			djnz	_iot_send_string_loop2
			ld		a, d
			sub		a, 63
			jr		z, _iot_send_string_exit
			jr		nc, _iot_send_string_loop1
	_iot_send_string_exit:
			in		a, [c]
			rlca									; エラーなら cf = 1, 正常なら cf = 0
			ret		nc
			; エラー
			ld		e, error_device_IO
			ld		ix, errhand
			jp		calbas
			endscope

; =============================================================================
;	CALL IOTGET( DEVPATH$, INT_VAR )
;	input:
;		HL ... デバイスパス文字列 (BASIC形式)
;	output:
;		HL
;	break:
;		A, B, C, D, E, H, L, F
;	comment:
;		IoT-BASIC の _IOTGET( HL, DE ) 相当の動作
; =============================================================================
			scope	sub_iotget_int
sub_iotget_int::
			call	_send_device_path
			; 受信コマンド送信
			ld		a, 0xe0
			out		[c], a
			ld		a, 0x01
			out		[c], a
			; 整数型識別コード送信
			ld		a, 0x01
			out		[c], a
			; 受信開始
			ld		a, 0x80
			out		[c], a
			in		a, [c]				; 多分長さ 2 が返ってくる
			in		l, [c]
			in		h, [c]
			ret
			endscope

; =============================================================================
;	CALL IOTGET( DEVPATH$, STR_VAR$ )
;	input:
;		HL ... デバイスパス文字列 (BASIC形式)
;	output:
;		HL ... 得られた文字列のアドレス (BASIC形式)
;	break:
;		A, B, C, D, E, H, L, F
;	comment:
;		IoT-BASIC の _IOTGET( HL, DE ) 相当の動作
; =============================================================================
			scope	sub_iotget_str
sub_iotget_str::
			call	_send_device_path
			; 受信コマンド送信
			ld		a, 0xe0
			out		[c], a
			ld		a, 0x01
			out		[c], a
			; 文字列型識別コード送信
			ld		a, 0x03
			out		[c], a
			; 受信開始
			ld		a, 0x80
			out		[c], a
			in		b, [c]				; 文字列の長さ
			ld		hl, buf
			ld		[hl], b
	_loop:
			inc		hl
			in		a, [c]
			ld		[hl], a
			djnz	_loop
			ld		hl, buf
			ret
			endscope

; =============================================================================
;	CALL IOTPUT( DEVPATH$, INT )
;	input:
;		HL ... デバイスパス文字列 (BASIC形式)
;		DE ... 送信する数値
;	output:
;		none
;	break:
;		A, B, C, D, E, H, L, F
;	comment:
;		IoT-BASIC の _IOTPUT( HL, DE ) 相当の動作
; =============================================================================
			scope	sub_iotput_int
sub_iotput_int::
			push	de
			call	_send_device_path
			; 受信コマンド送信
			ld		a, 0xe0
			out		[c], a
			ld		a, 0x01
			out		[c], a
			; 整数型識別コード送信
			ld		a, 0x01
			out		[c], a
			; 送信開始
			pop		de
			ld		a, 0xc0
			out		[c], a
			ld		a, 0x02			; 長さ 2
			out		[c], a
			out		[c], e
			out		[c], d
			xor		a, a
			out		[c], a
			ret
			endscope

; =============================================================================
;	CALL IOTPUT( DEVPATH$, STR$ )
;	input:
;		HL ... デバイスパス文字列 (BASIC形式)
;		DE ... 送信する文字列 (BASIC形式)
;	output:
;		none
;	break:
;		A, B, C, D, E, H, L, F
;	comment:
;		IoT-BASIC の _IOTPUT( HL, DE ) 相当の動作
; =============================================================================
			scope	sub_iotput_str
sub_iotput_str::
			push	de
			call	_send_device_path
			; 受信コマンド送信
			ld		a, 0xe0
			out		[c], a
			ld		a, 0x01
			out		[c], a
			; 文字列型識別コード送信
			ld		a, 0x03
			out		[c], a
			; 送信開始
			pop		hl
			jp		_send_string
			endscope

; =============================================================================
;	STRCMP
;	input:
;		HL ... 文字列1 (BASIC形式)
;		DE ... 文字列2 (BASIC形式)
;	output:
;		HL < DE ... C = 1, Z = 0
;		HL = DE ... C = 0, Z = 1
;		HL > DE ... C = 0, Z = 0
;	break:
;		A, B, C, D, E, H, L, F
;	comment:
;		文字列 HL - DE を求める
; =============================================================================
			scope	sub_strcmp
sub_strcmp::
			ld		b, [hl]				; B = [HL] の長さ
			ld		a, [de]				; C = [DE] の長さ
			ld		c, a
			inc		hl
			inc		de
			ex		de, hl

			inc		b
			inc		c
			dec		c
			jr		z, _loop_end
			jr		_loop_start
	_loop:
			ld		a, [de]
			cp		a, [hl]
			ret		c
			ret		nz
			inc		hl
			inc		de
			dec		c
			jr		z, _loop_end
	_loop_start:
			djnz	_loop
			; C がまだ残っているので HL < DE と判断。
			scf
			ret
	_loop_end:
			dec		b
			ret
			endscope

; =============================================================================
;	INKEY$
;	input:
;		none
;	output:
;		HL .... 入力された文字 (BASIC形式)
;	break:
;		A, B, C, D, E, H, L, F
;	comment:
;		何も入力されていなければ "" が返る
; =============================================================================
			scope	sub_inkey
sub_inkey::
			di
			ld		hl, [getpnt]
			ld		a, [putpnt]
			sub		a, l
			jr		z, no_key
		found_key:
			call	chget
			ld		hl, buf+1
			ld		[hl], a
			dec		hl
			ld		[hl], 1
			ret
		no_key:
			ld		hl, buf
			ld		[hl], a
			ei
			ret
			endscope

; =============================================================================
;	RIGHT$( STR$, N )
;	input:
;		HL .... STR$ (BASIC形式)
;		C ..... 文字数 N
;	output:
;		HL .... 切り取られた文字列 (BASIC形式)
;	break:
;		A, B, C, D, E, H, L, F
;	comment:
;		何も入力されていなければ "" が返る
; =============================================================================
			scope	sub_right
sub_right::
			ex		de, hl
			ld		hl, buf
			ld		[hl], c
			inc		c
			dec		c
			ret		z				; もし、N=0 なら長さ 0 の文字列を返す

			ld		a, [de]			; 文字列の長さ
			inc		de
			cp		a, c			; もし、STR$の長さよりも N の方が大きければ、まるまる返す
			jr		nc, skip
			ld		c, a
			ld		[hl], c			; 長さを更新
	skip:

			ex		de, hl			; HL = ターゲット文字列, DE = BUF
			sub		a, c			; A = 文字列長 - N  ※かならず0以上。負にはならない。
			ld		e, a			; DE = 文字列長 - N
			ld		d, 0
			ld		b, d
			add		hl, de
			ld		de, buf + 1
			ldir

			ld		hl, buf
			ret
			endscope

; =============================================================================
;	LEFT$( STR$, N )
;	input:
;		HL .... STR$ (BASIC形式)
;		C ..... 文字数 N
;	output:
;		HL .... 切り取られた文字列 (BASIC形式)
;	break:
;		A, B, C, D, E, H, L, F
;	comment:
;		何も入力されていなければ "" が返る
; =============================================================================
			scope	sub_left
sub_left::
			ex		de, hl			; DE = STR$
			ld		hl, buf			; HL = BUF
			ld		[hl], c			; 切り取る長さを設定
			inc		c
			dec		c
			ret		z				; もし、N=0 なら長さ 0 の文字列を返す

			ld		a, [de]			; 文字列の長さ
			inc		de
			cp		a, c			; もし、STR$の長さよりも N の方が大きければ、まるまる返す
			jr		nc, skip
			ld		c, a
	skip:
			ld		[hl], c
			ld		a, c
			or		a, a
			ret		z
			ld		b, 0
			inc		hl
			ex		de, hl
			ldir
			ld		hl, buf
			ret
			endscope

; =============================================================================
;	MID$( STR$, N, M )
;	input:
;		HL .... STR$ (BASIC形式)
;		B ..... 位置 N (先頭の文字は 1)
;		C ..... 文字数 M (切り出し長)
;	output:
;		HL .... 切り取られた文字列 (BASIC形式)
;	break:
;		A, B, C, D, E, H, L, F
;	comment:
;		何も入力されていなければ "" が返る
; =============================================================================
			scope	sub_mid
sub_mid::
			ld		a, [hl]
			cp		a, b
			jr		c, ret_blank			; 位置が右からはみ出してる場合は空文字列を返す
			or		a, a
			jr		z, ret_blank			; そもそも空文字列が指定されてる場合は空文字列を返す
			inc		c
			dec		c
			jr		z, ret_blank			; そもそも 切り出し長 0 が指定されてる場合は空文字列を返す

			; 切り出し先頭位置へ移動
			ld		e, b
			ld		d, 0
			add		hl, de					; HL = HL + (B - 1) + 1    : 文字列長を示す 1byte のフィールドがあるので +1

			; 残りの文字数と、文字数 M を比較する
			dec		b
			sub		a, b
			jr		z, ret_blank			; 残りの文字数が 0 なら空文字列を返す
			cp		a, c
			jr		nc, skip				; 残りの文字数の方が少なかった場合は、そのまま。多かった場合は、切り出し長を残りの文字数に置換
			ld		c, a
	skip:
			ld		a, c
			ld		de, buf
			ld		[de], a
			or		a, a
			ret		z
			ld		b, 0
			inc		de
			ldir
			ld		hl, buf
			ret

	ret_blank:
			ld		hl, buf
			ld		[hl], 0
			ret
			endscope
