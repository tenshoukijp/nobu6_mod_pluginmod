#include <windows.h>

#include "PluginMod.h"

#include <map>
using namespace std;



//---------------------------------------------- メニューが押された場合に反応できるようにフック。
//												 TSModの場合は、TSModがやってくれるが、PluginModは特になにもしないので、その時の対応。



static int idSelectMenu;
// メニュー選択
void SwitchSelectingMenuExecute() {

	// PluginModのApplicationSelectMenuItemメソッド
	// 読み込んだDLL群に対して、OnApplicationSelectMenuItemがあれば、それを実行なければ解放
	for ( vector<HMODULE>::iterator it = listOfTargetDLL.begin(); it != listOfTargetDLL.end(); it++ ) {

		// DLLが読めたのであれば、OnFinalize関数が存在するかをチェック。
		PFNTARGETMODAPPLICATIONSELECTMENU p_OnApplicationSelectMenuItem = (PFNTARGETMODAPPLICATIONSELECTMENU)GetProcAddress(*it, "OnApplicationSelectMenuItem");

		// OnFinalizeが存在すれば実行
		if ( !p_OnApplicationSelectMenuItem ) { continue; }

		((PFNTARGETMODAPPLICATIONSELECTMENU) p_OnApplicationSelectMenuItem)(idSelectMenu);
	}

}


/*
WM_COMMAND時のIDの判定
00417013  |. 0FB74424 10    MOVZX   EAX, WORD PTR SS:[ESP+10]
00417018  |. 8BEC           MOV     EBP, ESP
0041701A  |. 83F8 64        CMP     EAX, 64
0041701D  |. 74 1F          JE      SHORT TENSHOU.0041703E
0041701F  |. 83F8 6F        CMP     EAX, 6F
00417022  |. 74 31          JE      SHORT TENSHOU.00417055
00417024  |. 83F8 70        CMP     EAX, 70
00417027  |. 74 49          JE      SHORT TENSHOU.00417072
00417029  |. 83F8 79        CMP     EAX, 79
0041702C  |. 74 6B          JE      SHORT TENSHOU.00417099
0041702E  |. 3D 81000000    CMP     EAX, 81
00417033  |. 0F84 85000000  JE      TENSHOU.004170BE
00417039  |. E9 9B000000    JMP     TENSHOU.004170D9
*/
/*
上のID分岐の各々で処理すべき内容を実行した後は、以下の行にジャンプしてくる。よってプログラムでもこの流れに習う。
004170D9  |> 33C0           XOR     EAX, EAX
004170DB  |. 5D             POP     EBP
004170DC  \. C3             RETN
*/

int pTenshouExeJumpFromToOnTenshouExeWmMessage			=0x41702E;	// 関数はこのアドレスから、OnTenshouExeCallWmMessageへとジャンプしてくる。
int pTenshouExeJumpVersionFromToOnTenshouExeWmMessage	=0x4170BE;  // 「天翔記について」のバージョン表示用
int pTenshouExeReturnLblFromOnTenshouExeWmMessage		=0x4170D9;	// 関数が最後までいくと、このTENSHOU.EXE内に直接ジャンプするというなんとも危険な行為w


// ヘルプのバージョン表示が来た場合にここに一端ジャンプして、さらにすぐジャンプ
// __asm の jeコマンドでポインタでないとダメなため。
__declspec( naked ) void WINAPI OnTenshouExeWmMessageVersionInfo() {
	__asm {
		jmp pTenshouExeJumpVersionFromToOnTenshouExeWmMessage
	}
}
#pragma warning(push)
#pragma warning(disable:4414)
// 以下「C4414」のワーニングの発生するコードがあっても、コンパイル時には出力されなくなる。

// この関数はTENSHOU.EXEがメッセージを読みを終えるたびに、直接実行される。
// TENSHOU.EXE内でメッセージが構築されるタイミングでこのnaked関数が呼ばれる。
__declspec( naked ) void WINAPI OnTenshouExeWmMessage() {
	// スタックにためておく
	__asm {
		// 元々TENSHOU.EXE内にあったものをここで
		// バージョン情報だった場合は、OnTenshouExeWmMessageVersionInfo 経由でそのままバージョン表示ダイアログのアドレスへ
		cmp eax, 0x81							// メニューリソースの129 == 0x81 はバージョン情報
		je OnTenshouExeWmMessageVersionInfo		// JE  TENSHOU.004170BE 

		push eax
		push ebx
		push ecx
		push edx
		push esp
		push ebp
		push esi
		push edi
	}

	// eaxには CallWindowProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )のうちのwParamの値が入っている
	// これを保存しておく
	__asm {
		mov idSelectMenu, eax		// eaxをコピーしておく
	}

	// どのメニューが選ばれたのか判定。TENSHOU.EXEにデフォであったものは、TENSHOU.EXEと __asmの先頭２行でやってる。
	SwitchSelectingMenuExecute();


	// スタックに引き出す
	__asm {
		pop edi
		pop esi
		pop ebp
		pop esp
		pop ecx
		pop edx
		pop ebx
		pop eax

		jmp pTenshouExeReturnLblFromOnTenshouExeWmMessage
	}
}

// 以上「C4414」のワーニングの発生するコードがあっても、コンパイル時には出力されなくなる。
#pragma warning(pop)

/*
0041702E  |. 3D 81000000    CMP     EAX, 81							; ⇒ OnTenshouExeWmMessageへ
							└ ここを実行時に書き換えてやる必要がある。
00417033  |. 0F84 85000000  JE      TENSHOU.004170BE				; ⇒ nop(=90) が５つへ
*/

char cmdOnTenshouExeJumpFromWmMessage[6]="\xE9";


void WriteAsmJumperOnTenshouExeWmMessage() {
	// まずアドレスを数字として扱う
	int iAddress = (int)OnTenshouExeWmMessage;
	int SubAddress = iAddress - ( pTenshouExeJumpFromToOnTenshouExeWmMessage + 5 )	;

	// ５というのは、0041702E  -E9 ????????  JMP TSMod.JmpOnTenshouExeWmMessage  の命令に必要なバイト数。要するに５バイト足すと次のニーモニック命令群に移動するのだ。そしてそこからの差分がジャンプする際の目的格として利用される。
	memcpy(cmdOnTenshouExeJumpFromWmMessage+1, &SubAddress, 4); // +1 はE9の次から4バイト分書き換えるから。


	// 構築したニーモニック命令をTENSHOU.EXEのメモリに書き換える
	WriteProcessMemory( GetCurrentProcess(), (LPVOID)(pTenshouExeJumpFromToOnTenshouExeWmMessage), cmdOnTenshouExeJumpFromWmMessage, 5, NULL); // 5バイトのみ書き込む
}
