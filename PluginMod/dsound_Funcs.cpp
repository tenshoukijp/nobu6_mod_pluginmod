// dsound_Funcs.cpp : ��{�f�t�H���g�̏����փW�����v�B�ꕔ����������
//

#include <windows.h>
#include "dsound.h"
#include "PluginMod.h"

HINSTANCE hPluginMod = NULL;


// PluginMod.dll��OnInitialize�֐������s����B
bool TryLoadPluginMod() {
	hPluginMod = LoadLibrary( "PluginMod.dll");

	if ( !hPluginMod ) {
		return false;
	}

	// ���C�u�����ǂ߂Ă���֐��ǂݍ���
	PFNPLUGINMODINITIALIZE p_InitiaLize = (PFNPLUGINMODINITIALIZE)GetProcAddress( hPluginMod, "OnInitialize" );

	if (!p_InitiaLize) {
		return false;
	}

	// �֐��ǂ߂Ă�����s
	// �����Ƃ��āA����DLL�̃n���h����n���B��{�I�ɂ́A�ǂ��̒N���ǂ񂾂̂��A���O�Ƃ��̃������A�h���X���킩��悤�ɂ��邽�߁B
	((PFNPLUGINMODINITIALIZE) p_InitiaLize)(hCurDSound);
	return true;
}

// ��ԃV���[�Y�����̓V�ċL����N������Ă�̂��`�F�b�N�B�V�ċL����N������Ă��邱�Ƃ��m���炵���Ȃ�APluginMod�Ăяo�������݂�B

BOOL isCheckProcess = FALSE;
void CheckProcess() {
	// ���łɃ`�F�b�N�ς݂Ȃ�Q�x�Ǝ��s���Ȃ��B
	if ( isCheckProcess ) {
		return;
	}
	isCheckProcess = TRUE;

	// TSMod.dll�����łɃ��W���[���Ƃ��ēW�J����Ă���΁Adsound.dll�͓��삷��K�v���Ȃ��BTSMod.dll�����ׂčs���B
	// tenshou.exe��TSMod.dll���V���O���g���n�ł��邽�߁A���̊ȈՔ���ŏ\���ł���B
	HMODULE hTSMod = GetModuleHandle("TSMod.dll");
	if ( hTSMod && GetProcAddress( hTSMod, "DirectDrawCreate" ) ) {
		return;
	}

	/* �����ǂ݁B�V�ċL�̃v���Z�X�łȂ��ꏊ�ɃA�N�Z�X���邩������Ȃ��̂ŁA����ŁB

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

	// �����ǂ܂��\�z�s�\�Ȃ̂ŁANULL�ł�������ԕ��B
	szStringBuf[strlen(szCheckString)] = NULL;

	// �`�F�b�N�p�����񂪓������Ȃ��Ȃ�΁c��ԃV���[�Y��Tenshou.exe�ł͂Ȃ��B�������������directsound�p�̃��C�u�������U�����Ă�̂ŁA
	// �܂������Ⴄ�v���O��������Ă΂�Ă�\���������B
	if ( strncmp(szCheckString, szStringBuf, strlen(szCheckString))!=0 ) {
		return;
	}

	// �����܂ł���΁A�܂������ĊԈႢ�Ȃ��A�V�ċL�̃v���Z�X��ԂŌĂ΂�Ă���B
	// ������TSMod.dll�Ȃ��ŁB
	// ���Ȕ�����������A�m�؃R�[�h�̂��߂ɏ�L�̂悤�ȍH�v���K�v�ł���B
	TryLoadPluginMod();
}


__declspec( naked ) void WINAPI d_DirectSoundCreate() {
	_asm{
		call CheckProcess; // naked�Ȃ̂ŁA�֐��ɏo���Ȃ��ƃ��[�J���ϐ�����܂Ƃ��Ɏg���Ȃ���
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
