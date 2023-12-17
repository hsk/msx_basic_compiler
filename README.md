# msx_basic_compiler
MSX-BASICコンパイラ「BACON」

-------------------------------------------------------------------------------
MSX-BASICとの相違点

## 1. 実行時エラーは最小限
	実行時に検出しないエラーが存在します。 
	エラー処理を省略するが、ある程度高速化に寄与しています。

## 2. DEFINT, DEFSNG, DEFDBL, DEFSTR
	MSX-BASIC では、命令の記述位置で、変数型記述子省略時の型を変更するが、
	BACON では、コンパイル時にソースコード全体から DEFINT, DEFSNG, DEFDBL, DEFSTR
	を検索し、下優先でトータルの「型記述子省略時の型」を求め、それをソース全体に
	適用します。

## 3. VARPTR
	配列変数のアドレスを取得することも可能ですが、配列変数のアドレスは文字列操作後
	に変化する場合があります。
	100 A$="1":B$=A$+"2":DIM B(10):C=VARPTR(B(0)):B$="2"+A$
	このようなコードは、B$ には、A$+"2" を格納するメモリを確保後にそのアドレスを保持
	させますが、2回目の B$="2"+A$ の時に、最初の A$+"2" の結果を格納したメモリを解放
	してガベージコレクションが作動します。
	すると、その間で確保している配列変数 B(10) は、メモリを移動してしまうため、C に
	保存した B(0) のアドレスはズレてしまいます。
	DIM は文字列操作する前に実行しておけば、移動しなくなります。

## 4. ソースコード関連の命令は使用不可能
	LIST, RENUM, TRON, TROFF, MERGE など。

-------------------------------------------------------------------------------
### Special thanks to "MSX-BACON のデバッグに協力して頂いた方々"
```
nkt360さん (@nkt360)
ちくわ帝国さん (@chikuwa_empire)
b.p.sさん (@BasicProgrammer)
naoさん (@nyaonyao21)
SAILORMAN BRAVO BROSさん（@brapunch2000）
```
	※順不同、（）は X（旧Twitter）のアカウント
