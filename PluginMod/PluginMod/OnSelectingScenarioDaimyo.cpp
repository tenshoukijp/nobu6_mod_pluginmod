
#include <windows.h>

#include "PluginMod.h"


// PluginModが自分でフックしてTenshou.exeから呼び出してもらう
void WINAPI OnTenshouExeSelectingScenarioDaimyoExecute() {

	// 実行対象となっているDLLを順に捜査して…
	for ( vector<HMODULE>::iterator it = listOfTargetDLL.begin(); it != listOfTargetDLL.end(); it++ ) {

		// 対象のDLLにOnSelectingScenarioDaimyoStart関数が存在するかをチェック。
		PFNTARGETMODSELECTINGSCENARIODAIMYOSTART p_SelectingScenarioDaimyoStart = (PFNTARGETMODSELECTINGSCENARIODAIMYOSTART)GetProcAddress(*it, "OnSelectingScenarioDaimyoStart");

		if ( !p_SelectingScenarioDaimyoStart ) {
			continue;
		}

		((PFNTARGETMODSELECTINGSCENARIODAIMYOSTART) p_SelectingScenarioDaimyoStart)();
	}
}


/*
0049D289  |. E8 87A7F8FF    CALL    TENSHOU.00427A15                 ; \TENSHOU.00427A15
0049D28E  |. E8 4160F8FF    CALL    TENSHOU.004232D4
				└ここを JMP TSMod.OnTSExeSelectingScenarioDaimyo と書き換えてやる実際の処理
0049D293  |. E8 3FE6FDFF    CALL    TENSHOU.0047B8D7
*/
int pTenshouExeJumpFromToOnTenshouExeSelectingScenarioDaimyo	 =0x49D28E; // 関数はこのアドレスから、OnTenshouExeSelectingScenarioDaimyoへとジャンプしてくる。
int pTenshouExeJumpCallFromToOnTenshouExeSelectingScenarioDaimyo =0x4232D4; // 元々TENSHOU.EXE内にあったCALL先
int pTenshouExeReturnLblFromOnTenshouExeSelectingScenarioDaimyo  =0x49D293; // 関数が最後までいくと、このTENSHOU.EXE内に直接ジャンプするというなんとも危険な行為w
// この関数はTENSHOU.EXEがメッセージを読みを終えるたびに、直接実行される。
// TENSHOU.EXE内でメッセージが構築されるタイミングでこのnaked関数が呼ばれる。
__declspec( naked ) void WINAPI OnTenshouExeSelectingScenarioDaimyo() {
	// スタックにためておく
	__asm {
		// 元々TENSHOU.EXE内にあったものをここで
		call pTenshouExeJumpCallFromToOnTenshouExeSelectingScenarioDaimyo

		push eax
		push ebx
		push ecx
		push edx
		push esp
		push ebp
		push esi
		push edi
	}
	
	OnTenshouExeSelectingScenarioDaimyoExecute();

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

		jmp pTenshouExeReturnLblFromOnTenshouExeSelectingScenarioDaimyo
	}
}

// 元の命令が5バイト、以後の関数で生まれる命令が合計５バイトなので… 最後１つ使わない
char cmdOnTenshouExeJumpFromSelectingScenarioDaimyo[6]="\xE9";

// ニーモニック書き換え用
void WriteAsmJumperOnTenshouExeSelectingScenarioDaimyo() {
	// まずアドレスを数字として扱う
	int iAddress = (int)OnTenshouExeSelectingScenarioDaimyo;
	int SubAddress = iAddress - ( pTenshouExeJumpFromToOnTenshouExeSelectingScenarioDaimyo + 5 )	;

	memcpy(cmdOnTenshouExeJumpFromSelectingScenarioDaimyo+1, &SubAddress, 4); // +1 はE9の次から4バイト分書き換えるから。

	// 構築したニーモニック命令をTENSHOU.EXEのメモリに書き換える
	WriteProcessMemory( GetCurrentProcess(), (LPVOID)(pTenshouExeJumpFromToOnTenshouExeSelectingScenarioDaimyo), cmdOnTenshouExeJumpFromSelectingScenarioDaimyo, 5, NULL); //5バイトのみ書き込む
}




