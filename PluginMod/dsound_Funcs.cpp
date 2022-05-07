// dsound_Funcs.cpp : 基本デフォルトの処理へジャンプ。一部処理を挟む
//

#include <windows.h>
#include "dsound.h"
#include "PluginMod.h"

HINSTANCE hPluginMod = NULL;


// PluginMod.dllのOnInitialize関数を実行する。
bool TryLoadPluginMod() {
	hPluginMod = LoadLibrary( "PluginMod.dll");

	if ( !hPluginMod ) {
		return false;
	}

	// ライブラリ読めてたら関数読み込み
	PFNPLUGINMODINITIALIZE p_InitiaLize = (PFNPLUGINMODINITIALIZE)GetProcAddress( hPluginMod, "OnInitialize" );

	if (!p_InitiaLize) {
		return false;
	}

	// 関数読めてたら実行
	// 引数として、このDLLのハンドルを渡す。基本的には、どこの誰が読んだのか、名前とそのメモリアドレスがわかるようにするため。
	((PFNPLUGINMODINITIALIZE) p_InitiaLize)(hCurDSound);
	return true;
}

// 定番シリーズ相当の天翔記から起動されてるのかチェック。天翔記から起動されていることが確からしいなら、PluginMod呼び出しを試みる。

BOOL isCheckProcess = FALSE;
void CheckProcess() {
	// すでにチェック済みなら２度と実行しない。
	if ( isCheckProcess ) {
		return;
	}
	isCheckProcess = TRUE;

	// TSMod.dllがすでにモジュールとして展開されていれば、dsound.dllは動作する必要がない。TSMod.dllがすべて行う。
	// tenshou.exeもTSMod.dllもシングルトン系であるため、この簡易判定で十分である。
	HMODULE hTSMod = GetModuleHandle("TSMod.dll");
	if ( hTSMod && GetProcAddress( hTSMod, "DirectDrawCreate" ) ) {
		return;
	}

	/* 試し読み。天翔記のプロセスでない場所にアクセスするかもしれないので、これで。

		004B669C  4D 69 63 72 6F 73 6F 66 74 20 56 69 73 75 61 6C  Microsoft Visual
		004B66AC  20 43 2B 2B 20 52 75 6E 74 69 6D 65 20 4C 69 62   C++ Runtime Lib
		004B66BC  72 61 72 79 00                                   rary.
	*/
	char szStringBuf[256] = "";
	const int iCheckStringAddress = 0x4B669C;
	const char szCheckString[256] = "Microsoft Visual C++ Runtime Library";

	BOOL isCanReadProcess = ReadProcessMemory( GetCurrentProcess(), (LPCVOID)iCheckStringAddress, szStringBuf, strlen(szCheckString)+1, NULL);
	if ( !isCanReadProcess ) {
		return;
	}

	// 何が読まれる予想不可能なので、NULLでぐっさり番兵。
	szStringBuf[strlen(szCheckString)] = NULL;

	// チェック用文字列が等しくないならば…定番シリーズのTenshou.exeではない。そもそもこやつはdirectsound用のライブラリを偽装してるので、
	// まったく違うプログラムから呼ばれてる可能性も高い。
	if ( strncmp(szCheckString, szStringBuf, strlen(szCheckString))!=0 ) {
		return;
	}

	// ここまでくれば、まずもって間違いなく、天翔記のプロセス空間で呼ばれている。
	// しかもTSMod.dllなしで。
	// 糞な判定も多いが、確証コードのために上記のような工夫が必要である。
	TryLoadPluginMod();
}


__declspec( naked ) void WINAPI d_DirectSoundCreate() {
	_asm{
		call CheckProcess; // nakedなので、関数に出さないとローカル変数すらまともに使えないｗ
		jmp p_DirectSoundCreate;
	}
}
__declspec( naked ) void WINAPI d_DirectSoundEnumerateA() { _asm{ jmp p_DirectSoundEnumerateA } }
__declspec( naked ) void WINAPI d_DirectSoundEnumerateW() { _asm{ jmp p_DirectSoundEnumerateW } }
__declspec( naked ) void WINAPI d_DllCanUnloadNow() { _asm{ jmp p_DllCanUnloadNow } }
__declspec( naked ) void WINAPI d_DllGetClassObject() { _asm{ jmp p_DllGetClassObject } }
__declspec( naked ) void WINAPI d_DirectSoundCaptureCreate() { _asm{ jmp p_DirectSoundCaptureCreate } }
__declspec( naked ) void WINAPI d_DirectSoundCaptureEnumerateA() { _asm{ jmp p_DirectSoundCaptureEnumerateA } }
__declspec( naked ) void WINAPI d_DirectSoundCaptureEnumerateW() { _asm{ jmp p_DirectSoundCaptureEnumerateW } }
__declspec( naked ) void WINAPI d_GetDeviceID() { _asm{ jmp p_GetDeviceID } }
__declspec( naked ) void WINAPI d_DirectSoundFullDuplexCreate() { _asm{ jmp p_DirectSoundFullDuplexCreate } }
__declspec( naked ) void WINAPI d_DirectSoundCreate8() { _asm{ jmp p_DirectSoundCreate8 } }
__declspec( naked ) void WINAPI d_DirectSoundCaptureCreate8() { _asm{ jmp p_DirectSoundCaptureCreate8 } }
