
#include <windows.h>

#include "PluginMod.h"


// PluginModが自分でフックしてTenshou.exeから呼び出してもらう
void WINAPI OnTenshouExeMainGameStartExecute() {

	// 実行対象となっているDLLを順に捜査して…
	for ( vector<HMODULE>::iterator it = listOfTargetDLL.begin(); it != listOfTargetDLL.end(); it++ ) {

		// 対象のDLLにMainGameStart関数が存在するかをチェック。
		PFNTARGETMODMAINGAMESTART p_MainGameStart = (PFNTARGETMODMAINGAMESTART)GetProcAddress(*it, "OnMainGameStart");

		if ( !p_MainGameStart ) {
			continue;
		}

		((PFNTARGETMODMAINGAMESTART) p_MainGameStart)();
	}
}


/*
0044193D  /$ 83EC 10        SUB     ESP, 10				←ここからメインゲームスタートと考えて良い
00441940  |. 56             PUSH    ESI
00441941  |. 57             PUSH    EDI
00441942  |. 68 00020000    PUSH    200
00441947  |. E8 55560200    CALL    TENSHOU.00466FA1
				└ここを JMP TSMod.OnTSExeMainGameStart と書き換えてやる実際の処理
0044194C  |. 83C4 04        ADD     ESP, 4
*/
int pTenshouExeJumpFromToOnTenshouExeMainGameStart		=0x441947; // 関数はこのアドレスから、OnTenshouExeMainGameStartへとジャンプしてくる。
int pTenshouExeJumpCallFromToOnTenshouExeMainGameStart  =0x466FA1; // 元々TENSHOU.EXE内にあったCALL先
int pTenshouExeReturnLblFromOnTenshouExeMainGameStart	=0x44194C; // 関数が最後までいくと、このTENSHOU.EXE内に直接ジャンプするというなんとも危険な行為w
// この関数はTENSHOU.EXEがメッセージを読みを終えるたびに、直接実行される。// TENSHOU.EXE内でメッセージが構築されるタイミングでこのnaked関数が呼ばれる。
__declspec( naked ) void WINAPI OnTenshouExeMainGameStart() {
	// スタックにためておく
	__asm {

		// 元々TENSHOU.EXE内にあったものをここで
		call pTenshouExeJumpCallFromToOnTenshouExeMainGameStart

		push eax
		push ebx
		push ecx
		push edx
		push esp
		push ebp
		push esi
		push edi
	}
	
	OnTenshouExeMainGameStartExecute();

	// スタックに引き出す
	__asm {
		pop edi
		pop esi
		pop ebp
		pop esp
		pop edx
		pop ecx
		pop ebx
		pop eax

		jmp pTenshouExeReturnLblFromOnTenshouExeMainGameStart
	}
}

// 元の命令が5バイト、以後の関数で生まれる命令が合計５バイトなので… 最後１つ使わない
char cmdOnTenshouExeJumpFromMainGameStart[6]="\xE9";

// ニーモニック書き換え用
void WriteAsmJumperOnTenshouExeMainGameStart() {
	// まずアドレスを数字として扱う
	int iAddress = (int)OnTenshouExeMainGameStart;
	int SubAddress = iAddress - ( pTenshouExeJumpFromToOnTenshouExeMainGameStart + 5 )	;

	memcpy(cmdOnTenshouExeJumpFromMainGameStart+1, &SubAddress, 4); // +1 はE9の次から4バイト分書き換えるから。

	// 構築したニーモニック命令をTENSHOU.EXEのメモリに書き換える
	WriteProcessMemory( GetCurrentProcess(), (LPVOID)(pTenshouExeJumpFromToOnTenshouExeMainGameStart), cmdOnTenshouExeJumpFromMainGameStart, 5, NULL); //5バイトのみ書き込む
}




