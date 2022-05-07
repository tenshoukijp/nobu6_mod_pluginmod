#define _CRT_SECURE_NO_WARNINGS 1

#include <windows.h>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>

#include "PluginMod.h"
#include "ReWriteAsmNemonics.h"

using namespace std;

vector<HMODULE> listOfTargetDLL;


// テキストファイルを読み込むか、もしくは、***Mod.dllというファイルを探して、
// その分だけ、LoadLibraryする。全てvoid OnInitialize()メソッドが存在するという前提で動作する。
// ただ１度だけ呼び出される。各々のDLLは自分で考えてtenshou.exeフックし、流れの制御を変更する必要があるだろう。
void WINAPI OnInitialize(HANDLE hCallerDLL) {

	// TSMod.dllがモジュールとして展開されているならば、これは動作する必要はない。
	// TSMod.dllはこのPluginModの機能をすべて内包している。
	HMODULE hTSMod = GetModuleHandle("TSMod.dll");
	if ( hTSMod && GetProcAddress( hTSMod, "DirectDrawCreate" ) ) {
		return;
	}

	// ---------------------------------------------------------------------------------------------------
	// TSMod.dllがモジュールとして展開されていないならば、このPluginModが自力でニーモニックを書き換えなければならない。
	// 書き換える目的は、とあるタイミングでtenshou.exeがPluginModの特定の関数を呼び出すようにするためだ。
	// 
	// ・TSMod.dllが存在していれば、TSMod.dllがtenshou.exeをフックしており、そこから各Modの関数を直に呼び出す。
	// ・TSMod.dllが存在していなければ、PluginModは自力でtenshou.exeをフックし、tenshou.exeがPluginMod.dllの関数を呼び出し、
	//   その関数内から、各Modの関数を呼び出す。

	// ニーモニックを書き換える。
	ReWriteAsmNemonics();


	// ---------------------------------------------------------------------------------------------------
	// プラグインの名前をテキストに書いて、固定化することが可能である。(Mod読み込み誤爆防止や、順序完全制御の必要がある場合に限り使えばよい)
	char *filename = "PluginMod.ini";
	ifstream ifPluginModSettings(filename);

	// 設定ファイルがある場合はリストに追加
	if (ifPluginModSettings ) {

		string buf = "";
		// １行ずつ読み込む
		while( getline(ifPluginModSettings, buf ) ) {

			// DLLはある？
			HMODULE hTaretModule = LoadLibraryA(buf.c_str());

			if (!hTaretModule) {
				continue;
			}
			// 存在していたなら、読んじゃったわけでして、後で走査するため登録しておく。
			listOfTargetDLL.push_back(hTaretModule);

		}

	// 設定ファイルが存在しない場合には、自動収拾する。(ほとんどあり得ないとは思うが、誤爆する可能性がある)
	} else {

		WIN32_FIND_DATA ffd;
		// *Mod.dllという名前のファイル。
		HANDLE hFileList = FindFirstFile("*Mod.dll", &ffd);
		// 存在した場合、読み込み対象とするが、TSMod.dllとPluginMod.dllとScenarioMod.dllは除外。
		if ( hFileList != INVALID_HANDLE_VALUE ) {
			do {
				char filename[512] = "";
				strcpy(filename, _strlwr(ffd.cFileName));
				if ( strcmp( filename, "tsmod.dll")==0 ) { continue; }
				if ( strcmp( filename, "pluginmod.dll")==0 ) { continue; }
				if ( strcmp( filename, "scenariomod.dll")==0 ) { continue; }

				// DLLはある？
				HMODULE hTaretModule = LoadLibraryA(filename);

				if (!hTaretModule) {
					continue;
				}

				// 存在していたなら、読んじゃったわけでして、後で走査するため登録しておく。
				listOfTargetDLL.push_back(hTaretModule);

			} while (FindNextFile(hFileList, &ffd));
		}


		hFileList = FindFirstFile("*Mod.cpl", &ffd);
		// 存在した場合、読み込み対象とする。
		if ( hFileList != INVALID_HANDLE_VALUE ) {
			do {
				char filename[512] = "";
				strcpy(filename, _strlwr(ffd.cFileName));

				// DLLはある？
				HMODULE hTaretModule = LoadLibraryA(filename);

				if (!hTaretModule) {
					continue;
				}

				// 存在していたなら、読んじゃったわけでして、後で走査するため登録しておく。
				listOfTargetDLL.push_back(hTaretModule);

			} while (FindNextFile(hFileList, &ffd));
		}

		FindClose(hFileList);

	}

	// 読み込んだDLL群に対して、OnInitializeがあれば、それを実行なければ解放
	for ( vector<HMODULE>::iterator it = listOfTargetDLL.begin(); it != listOfTargetDLL.end(); it++ ) {

		// DLLが読めたのであれば、OnInitialize関数が存在するかをチェック。
		PFNTARGETMODINITIALIZE p_OnInitialize = (PFNTARGETMODINITIALIZE)GetProcAddress(*it, "OnInitialize");

		// OnInitializeが存在しないのであれば、対象外。解放。
		if ( !p_OnInitialize ) {
			FreeLibrary(*it);
			continue;
		}

		((PFNTARGETMODINITIALIZE) p_OnInitialize)(hPluginMod);
	}


	/*
	誰から呼ばれたかのチェック。
	char szFullDLLName[1024] = ""; // これよりパスが長いとか… ヤバすぎだろｗ
	GetModuleFileNameA((HMODULE)hCallerDLL, szFullDLLName, sizeof(szFullDLLName)-1);
	MessageBox(NULL, szFullDLLName, szFullDLLName, NULL);
	*/

}


