
#include <windows.h>

#include "PluginMod.h"


void OnTenshouExeSobaChangeExecute() {

	// PluginMod�n�̂O�ȏ�i�����\�j�Ȃc�k�k�̃��C���Q�[���J�n���̃��\�b�h���Ăяo���B
	// ���s�ΏۂƂȂ��Ă���DLL�����ɑ{�����āc
	for ( vector<HMODULE>::iterator it = listOfTargetDLL.begin(); it != listOfTargetDLL.end(); it++ ) {

		// �Ώۂ�DLL��MainGameStart�֐������݂��邩���`�F�b�N�B
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
				�������� JMP TSMod.OnTSExeSobaChange �Ə��������Ă����ۂ̏���
0044477E  |. B9 03000000    MOV     ECX, 3

 */
int pTenshouExeJumpFromToOnTenshouExeSobaChange    =0x444779; // �֐��͂��̃A�h���X����AOnTenshouExeSobaChange�ւƃW�����v���Ă���B
int pTenshouExeJumpCallFromToOnTenshouExeSobaChange=0x4673E9; // ���XTENSHOU.EXE���ɂ�����CALL��
int pTenshouExeReturnLblFromOnTenshouExeSobaChange =0x44477E; // �֐����Ō�܂ł����ƁA����TENSHOU.EXE���ɒ��ڃW�����v����Ƃ����Ȃ�Ƃ��댯�ȍs��w
// ���̊֐���TENSHOU.EXE�����b�Z�[�W��ǂ݂��I���邽�тɁA���ڎ��s�����B
// TENSHOU.EXE���Ń��b�Z�[�W���\�z�����^�C�~���O�ł���naked�֐����Ă΂��B
__declspec( naked ) void WINAPI OnTenshouExeSobaChange() {
	// �X�^�b�N�ɂ��߂Ă���
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
	
	// �Q�[���f�[�^���v���C���[�^�[�����ς�����i�K�œǂݍ��ݍX�V���Ă���
	OnTenshouExeSobaChangeExecute();

	// �X�^�b�N�Ɉ����o��
	__asm {
		pop edi
		pop esi
		pop ebp
		pop esp
		pop edx
		pop ecx
		pop ebx
		pop eax

		// ���XTENSHOU.EXE���ɂ��������̂�������
		call pTenshouExeJumpCallFromToOnTenshouExeSobaChange

		jmp pTenshouExeReturnLblFromOnTenshouExeSobaChange
	}
}

// ���̖��߂�5�o�C�g�A�Ȍ�̊֐��Ő��܂�閽�߂����v�T�o�C�g�Ȃ̂Łc �Ō�P�g��Ȃ�
char cmdOnTenshouExeJumpFromSobaChange[6]="\xE9";

// �j�[���j�b�N���������p
void WriteAsmJumperOnTenshouExeSobaChange() {
	// �܂��A�h���X�𐔎��Ƃ��Ĉ���
	int iAddress = (int)OnTenshouExeSobaChange;
	int SubAddress = iAddress - ( pTenshouExeJumpFromToOnTenshouExeSobaChange + 5 )	;

	memcpy(cmdOnTenshouExeJumpFromSobaChange+1, &SubAddress, 4); // +1 ��E9�̎�����4�o�C�g�����������邩��B

	// �\�z�����j�[���j�b�N���߂�TENSHOU.EXE�̃������ɏ���������
	WriteProcessMemory( GetCurrentProcess(), (LPVOID)(pTenshouExeJumpFromToOnTenshouExeSobaChange), cmdOnTenshouExeJumpFromSobaChange, 5, NULL); //5�o�C�g�̂ݏ�������
}



