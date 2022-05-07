
#include <windows.h>
#include "PluginMod.h"

HANDLE hPluginMod = NULL;

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved ) {

	switch( ul_reason_for_call )
	{
	case DLL_PROCESS_ATTACH:
		hPluginMod = hModule;
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		// �o�^����������B�܂����ۂ̓_�����������B
		// ��d�N�����ɂ͖���������炢�͂��Ă����B
		for ( vector<HMODULE>::iterator it = listOfTargetDLL.begin(); it != listOfTargetDLL.end(); it++ ) {
			FreeLibrary(*it);
		}

		break;
	default:
		break;
	}
	return TRUE;
}
