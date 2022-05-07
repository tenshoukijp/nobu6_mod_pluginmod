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


// �e�L�X�g�t�@�C����ǂݍ��ނ��A�������́A***Mod.dll�Ƃ����t�@�C����T���āA
// ���̕������ALoadLibrary����B�S��void OnInitialize()���\�b�h�����݂���Ƃ����O��œ��삷��B
// �����P�x�����Ăяo�����B�e�X��DLL�͎����ōl����tenshou.exe�t�b�N���A����̐����ύX����K�v�����邾�낤�B
void WINAPI OnInitialize(HANDLE hCallerDLL) {

	// TSMod.dll�����W���[���Ƃ��ēW�J����Ă���Ȃ�΁A����͓��삷��K�v�͂Ȃ��B
	// TSMod.dll�͂���PluginMod�̋@�\�����ׂē���Ă���B
	HMODULE hTSMod = GetModuleHandle("TSMod.dll");
	if ( hTSMod && GetProcAddress( hTSMod, "DirectDrawCreate" ) ) {
		return;
	}

	// ---------------------------------------------------------------------------------------------------
	// TSMod.dll�����W���[���Ƃ��ēW�J����Ă��Ȃ��Ȃ�΁A����PluginMod�����͂Ńj�[���j�b�N�����������Ȃ���΂Ȃ�Ȃ��B
	// ����������ړI�́A�Ƃ���^�C�~���O��tenshou.exe��PluginMod�̓���̊֐����Ăяo���悤�ɂ��邽�߂��B
	// 
	// �ETSMod.dll�����݂��Ă���΁ATSMod.dll��tenshou.exe���t�b�N���Ă���A��������eMod�̊֐��𒼂ɌĂяo���B
	// �ETSMod.dll�����݂��Ă��Ȃ���΁APluginMod�͎��͂�tenshou.exe���t�b�N���Atenshou.exe��PluginMod.dll�̊֐����Ăяo���A
	//   ���̊֐�������A�eMod�̊֐����Ăяo���B

	// �j�[���j�b�N������������B
	ReWriteAsmNemonics();


	// ---------------------------------------------------------------------------------------------------
	// �v���O�C���̖��O���e�L�X�g�ɏ����āA�Œ艻���邱�Ƃ��\�ł���B(Mod�ǂݍ��݌딚�h�~��A�������S����̕K�v������ꍇ�Ɍ���g���΂悢)
	char *filename = "PluginMod.ini";
	ifstream ifPluginModSettings(filename);

	// �ݒ�t�@�C��������ꍇ�̓��X�g�ɒǉ�
	if (ifPluginModSettings ) {

		string buf = "";
		// �P�s���ǂݍ���
		while( getline(ifPluginModSettings, buf ) ) {

			// DLL�͂���H
			HMODULE hTaretModule = LoadLibraryA(buf.c_str());

			if (!hTaretModule) {
				continue;
			}
			// ���݂��Ă����Ȃ�A�ǂ񂶂�����킯�ł��āA��ő������邽�ߓo�^���Ă����B
			listOfTargetDLL.push_back(hTaretModule);

		}

	// �ݒ�t�@�C�������݂��Ȃ��ꍇ�ɂ́A�������E����B(�قƂ�ǂ��蓾�Ȃ��Ƃ͎v�����A�딚����\��������)
	} else {

		WIN32_FIND_DATA ffd;
		// *Mod.dll�Ƃ������O�̃t�@�C���B
		HANDLE hFileList = FindFirstFile("*Mod.dll", &ffd);
		// ���݂����ꍇ�A�ǂݍ��ݑΏۂƂ��邪�ATSMod.dll��PluginMod.dll��ScenarioMod.dll�͏��O�B
		if ( hFileList != INVALID_HANDLE_VALUE ) {
			do {
				char filename[512] = "";
				strcpy(filename, _strlwr(ffd.cFileName));
				if ( strcmp( filename, "tsmod.dll")==0 ) { continue; }
				if ( strcmp( filename, "pluginmod.dll")==0 ) { continue; }
				if ( strcmp( filename, "scenariomod.dll")==0 ) { continue; }

				// DLL�͂���H
				HMODULE hTaretModule = LoadLibraryA(filename);

				if (!hTaretModule) {
					continue;
				}

				// ���݂��Ă����Ȃ�A�ǂ񂶂�����킯�ł��āA��ő������邽�ߓo�^���Ă����B
				listOfTargetDLL.push_back(hTaretModule);

			} while (FindNextFile(hFileList, &ffd));
		}


		hFileList = FindFirstFile("*Mod.cpl", &ffd);
		// ���݂����ꍇ�A�ǂݍ��ݑΏۂƂ���B
		if ( hFileList != INVALID_HANDLE_VALUE ) {
			do {
				char filename[512] = "";
				strcpy(filename, _strlwr(ffd.cFileName));

				// DLL�͂���H
				HMODULE hTaretModule = LoadLibraryA(filename);

				if (!hTaretModule) {
					continue;
				}

				// ���݂��Ă����Ȃ�A�ǂ񂶂�����킯�ł��āA��ő������邽�ߓo�^���Ă����B
				listOfTargetDLL.push_back(hTaretModule);

			} while (FindNextFile(hFileList, &ffd));
		}

		FindClose(hFileList);

	}

	// �ǂݍ���DLL�Q�ɑ΂��āAOnInitialize������΁A��������s�Ȃ���Ή��
	for ( vector<HMODULE>::iterator it = listOfTargetDLL.begin(); it != listOfTargetDLL.end(); it++ ) {

		// DLL���ǂ߂��̂ł���΁AOnInitialize�֐������݂��邩���`�F�b�N�B
		PFNTARGETMODINITIALIZE p_OnInitialize = (PFNTARGETMODINITIALIZE)GetProcAddress(*it, "OnInitialize");

		// OnInitialize�����݂��Ȃ��̂ł���΁A�ΏۊO�B����B
		if ( !p_OnInitialize ) {
			FreeLibrary(*it);
			continue;
		}

		((PFNTARGETMODINITIALIZE) p_OnInitialize)(hPluginMod);
	}


	/*
	�N����Ă΂ꂽ���̃`�F�b�N�B
	char szFullDLLName[1024] = ""; // ������p�X�������Ƃ��c ���o�������낗
	GetModuleFileNameA((HMODULE)hCallerDLL, szFullDLLName, sizeof(szFullDLLName)-1);
	MessageBox(NULL, szFullDLLName, szFullDLLName, NULL);
	*/

}


