
#include <windows.h>

#include "PluginMod.h"


// PluginMod�������Ńt�b�N����Tenshou.exe����Ăяo���Ă��炤
void WINAPI OnTenshouExeMainGameStartExecute() {

	// ���s�ΏۂƂȂ��Ă���DLL�����ɑ{�����āc
	for ( vector<HMODULE>::iterator it = listOfTargetDLL.begin(); it != listOfTargetDLL.end(); it++ ) {

		// �Ώۂ�DLL��MainGameStart�֐������݂��邩���`�F�b�N�B
		PFNTARGETMODMAINGAMESTART p_MainGameStart = (PFNTARGETMODMAINGAMESTART)GetProcAddress(*it, "OnMainGameStart");

		if ( !p_MainGameStart ) {
			continue;
		}

		((PFNTARGETMODMAINGAMESTART) p_MainGameStart)();
	}
}


/*
0044193D  /$ 83EC 10        SUB     ESP, 10				���������烁�C���Q�[���X�^�[�g�ƍl���ėǂ�
00441940  |. 56             PUSH    ESI
00441941  |. 57             PUSH    EDI
00441942  |. 68 00020000    PUSH    200
00441947  |. E8 55560200    CALL    TENSHOU.00466FA1
				�������� JMP TSMod.OnTSExeMainGameStart �Ə��������Ă����ۂ̏���
0044194C  |. 83C4 04        ADD     ESP, 4
*/
int pTenshouExeJumpFromToOnTenshouExeMainGameStart		=0x441947; // �֐��͂��̃A�h���X����AOnTenshouExeMainGameStart�ւƃW�����v���Ă���B
int pTenshouExeJumpCallFromToOnTenshouExeMainGameStart  =0x466FA1; // ���XTENSHOU.EXE���ɂ�����CALL��
int pTenshouExeReturnLblFromOnTenshouExeMainGameStart	=0x44194C; // �֐����Ō�܂ł����ƁA����TENSHOU.EXE���ɒ��ڃW�����v����Ƃ����Ȃ�Ƃ��댯�ȍs��w
// ���̊֐���TENSHOU.EXE�����b�Z�[�W��ǂ݂��I���邽�тɁA���ڎ��s�����B// TENSHOU.EXE���Ń��b�Z�[�W���\�z�����^�C�~���O�ł���naked�֐����Ă΂��B
__declspec( naked ) void WINAPI OnTenshouExeMainGameStart() {
	// �X�^�b�N�ɂ��߂Ă���
	__asm {

		// ���XTENSHOU.EXE���ɂ��������̂�������
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

		jmp pTenshouExeReturnLblFromOnTenshouExeMainGameStart
	}
}

// ���̖��߂�5�o�C�g�A�Ȍ�̊֐��Ő��܂�閽�߂����v�T�o�C�g�Ȃ̂Łc �Ō�P�g��Ȃ�
char cmdOnTenshouExeJumpFromMainGameStart[6]="\xE9";

// �j�[���j�b�N���������p
void WriteAsmJumperOnTenshouExeMainGameStart() {
	// �܂��A�h���X�𐔎��Ƃ��Ĉ���
	int iAddress = (int)OnTenshouExeMainGameStart;
	int SubAddress = iAddress - ( pTenshouExeJumpFromToOnTenshouExeMainGameStart + 5 )	;

	memcpy(cmdOnTenshouExeJumpFromMainGameStart+1, &SubAddress, 4); // +1 ��E9�̎�����4�o�C�g�����������邩��B

	// �\�z�����j�[���j�b�N���߂�TENSHOU.EXE�̃������ɏ���������
	WriteProcessMemory( GetCurrentProcess(), (LPVOID)(pTenshouExeJumpFromToOnTenshouExeMainGameStart), cmdOnTenshouExeJumpFromMainGameStart, 5, NULL); //5�o�C�g�̂ݏ�������
}




