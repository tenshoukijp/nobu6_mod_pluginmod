
#include <windows.h>

#include "PluginMod.h"


void OnTenshouExeSobaChangeExecute() {

	// PluginMod系の０個以上（複数可能）なＤＬＬのメインゲーム開始時のメソッドを呼び出す。
	// 実行対象となっているDLLを順に捜査して…
	for ( vector<HMODULE>::iterator it = listOfTargetDLL.begin(); it != listOfTargetDLL.end(); it++ ) {

		// 対象のDLLにMainGameStart関数が存在するかをチェック。
		PFNTARGETMODCHANGESOBA p_ChangeSoba = (PFNTARGETMODCHANGESOBA)GetProcAddress(*it, "OnSobaChange");

		if ( !p_ChangeSoba ) {
			continue;
		}

		((PFNTARGETMODCHANGESOBA) p_ChangeSoba)();
	}
}


/*
0044476F  |. 83C4 04        ADD     ESP, 4
00444772  |. E8 03230200    CALL    TENSHOU.00466A7A
00444777  |. 8BF0           MOV     ESI, EAX
00444779  |. E8 6B2C0200    CALL    TENSHOU.004673E9
				└ここを JMP TSMod.OnTSExeSobaChange と書き換えてやる実際の処理
0044477E  |. B9 03000000    MOV     ECX, 3

 */
int pTenshouExeJumpFromToOnTenshouExeSobaChange    =0x444779; // 関数はこのアドレスから、OnTenshouExeSobaChangeへとジャンプしてくる。
int pTenshouExeJumpCallFromToOnTenshouExeSobaChange=0x4673E9; // 元々TENSHOU.EXE内にあったCALL先
int pTenshouExeReturnLblFromOnTenshouExeSobaChange =0x44477E; // 関数が最後までいくと、このTENSHOU.EXE内に直接ジャンプするというなんとも危険な行為w
// この関数はTENSHOU.EXEがメッセージを読みを終えるたびに、直接実行される。
// TENSHOU.EXE内でメッセージが構築されるタイミングでこのnaked関数が呼ばれる。
__declspec( naked ) void WINAPI OnTenshouExeSobaChange() {
	// スタックにためておく
	__asm {

		push eax
		push ebx
		push ecx
		push edx
		push esp
		push ebp
		push esi
		push edi
	}
	
	// ゲームデータをプレイヤーターンが変わった段階で読み込み更新しておく
	OnTenshouExeSobaChangeExecute();

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

		// 元々TENSHOU.EXE内にあったものをここで
		call pTenshouExeJumpCallFromToOnTenshouExeSobaChange

		jmp pTenshouExeReturnLblFromOnTenshouExeSobaChange
	}
}

// 元の命令が5バイト、以後の関数で生まれる命令が合計５バイトなので… 最後１つ使わない
char cmdOnTenshouExeJumpFromSobaChange[6]="\xE9";

// ニーモニック書き換え用
void WriteAsmJumperOnTenshouExeSobaChange() {
	// まずアドレスを数字として扱う
	int iAddress = (int)OnTenshouExeSobaChange;
	int SubAddress = iAddress - ( pTenshouExeJumpFromToOnTenshouExeSobaChange + 5 )	;

	memcpy(cmdOnTenshouExeJumpFromSobaChange+1, &SubAddress, 4); // +1 はE9の次から4バイト分書き換えるから。

	// 構築したニーモニック命令をTENSHOU.EXEのメモリに書き換える
	WriteProcessMemory( GetCurrentProcess(), (LPVOID)(pTenshouExeJumpFromToOnTenshouExeSobaChange), cmdOnTenshouExeJumpFromSobaChange, 5, NULL); //5バイトのみ書き込む
}



