#include <windows.h>

#include "PluginMod.h"

#include <map>
using namespace std;



//---------------------------------------------- ���j���[�������ꂽ�ꍇ�ɔ����ł���悤�Ƀt�b�N�B
//												 TSMod�̏ꍇ�́ATSMod������Ă���邪�APluginMod�͓��ɂȂɂ����Ȃ��̂ŁA���̎��̑Ή��B



static int idSelectMenu;
// ���j���[�I��
void SwitchSelectingMenuExecute() {

	// PluginMod��ApplicationSelectMenuItem���\�b�h
	// �ǂݍ���DLL�Q�ɑ΂��āAOnApplicationSelectMenuItem������΁A��������s�Ȃ���Ή��
	for ( vector<HMODULE>::iterator it = listOfTargetDLL.begin(); it != listOfTargetDLL.end(); it++ ) {

		// DLL���ǂ߂��̂ł���΁AOnFinalize�֐������݂��邩���`�F�b�N�B
		PFNTARGETMODAPPLICATIONSELECTMENU p_OnApplicationSelectMenuItem = (PFNTARGETMODAPPLICATIONSELECTMENU)GetProcAddress(*it, "OnApplicationSelectMenuItem");

		// OnFinalize�����݂���Ύ��s
		if ( !p_OnApplicationSelectMenuItem ) { continue; }

		((PFNTARGETMODAPPLICATIONSELECTMENU) p_OnApplicationSelectMenuItem)(idSelectMenu);
	}

}


/*
WM_COMMAND����ID�̔���
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
���ID����̊e�X�ŏ������ׂ����e�����s������́A�ȉ��̍s�ɃW�����v���Ă���B����ăv���O�����ł����̗���ɏK���B
004170D9  |> 33C0           XOR     EAX, EAX
004170DB  |. 5D             POP     EBP
004170DC  \. C3             RETN
*/

int pTenshouExeJumpFromToOnTenshouExeWmMessage			=0x41702E;	// �֐��͂��̃A�h���X����AOnTenshouExeCallWmMessage�ւƃW�����v���Ă���B
int pTenshouExeJumpVersionFromToOnTenshouExeWmMessage	=0x4170BE;  // �u�V�ċL�ɂ��āv�̃o�[�W�����\���p
int pTenshouExeReturnLblFromOnTenshouExeWmMessage		=0x4170D9;	// �֐����Ō�܂ł����ƁA����TENSHOU.EXE���ɒ��ڃW�����v����Ƃ����Ȃ�Ƃ��댯�ȍs��w


// �w���v�̃o�[�W�����\���������ꍇ�ɂ����Ɉ�[�W�����v���āA����ɂ����W�����v
// __asm �� je�R�}���h�Ń|�C���^�łȂ��ƃ_���Ȃ��߁B
__declspec( naked ) void WINAPI OnTenshouExeWmMessageVersionInfo() {
	__asm {
		jmp pTenshouExeJumpVersionFromToOnTenshouExeWmMessage
	}
}
#pragma warning(push)
#pragma warning(disable:4414)
// �ȉ��uC4414�v�̃��[�j���O�̔�������R�[�h�������Ă��A�R���p�C�����ɂ͏o�͂���Ȃ��Ȃ�B

// ���̊֐���TENSHOU.EXE�����b�Z�[�W��ǂ݂��I���邽�тɁA���ڎ��s�����B
// TENSHOU.EXE���Ń��b�Z�[�W���\�z�����^�C�~���O�ł���naked�֐����Ă΂��B
__declspec( naked ) void WINAPI OnTenshouExeWmMessage() {
	// �X�^�b�N�ɂ��߂Ă���
	__asm {
		// ���XTENSHOU.EXE���ɂ��������̂�������
		// �o�[�W������񂾂����ꍇ�́AOnTenshouExeWmMessageVersionInfo �o�R�ł��̂܂܃o�[�W�����\���_�C�A���O�̃A�h���X��
		cmp eax, 0x81							// ���j���[���\�[�X��129 == 0x81 �̓o�[�W�������
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

	// eax�ɂ� CallWindowProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )�̂�����wParam�̒l�������Ă���
	// �����ۑ����Ă���
	__asm {
		mov idSelectMenu, eax		// eax���R�s�[���Ă���
	}

	// �ǂ̃��j���[���I�΂ꂽ�̂�����BTENSHOU.EXE�Ƀf�t�H�ł��������̂́ATENSHOU.EXE�� __asm�̐擪�Q�s�ł���Ă�B
	SwitchSelectingMenuExecute();


	// �X�^�b�N�Ɉ����o��
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

// �ȏ�uC4414�v�̃��[�j���O�̔�������R�[�h�������Ă��A�R���p�C�����ɂ͏o�͂���Ȃ��Ȃ�B
#pragma warning(pop)

/*
0041702E  |. 3D 81000000    CMP     EAX, 81							; �� OnTenshouExeWmMessage��
							�� ���������s���ɏ��������Ă��K�v������B
00417033  |. 0F84 85000000  JE      TENSHOU.004170BE				; �� nop(=90) ���T��
*/

char cmdOnTenshouExeJumpFromWmMessage[6]="\xE9";


void WriteAsmJumperOnTenshouExeWmMessage() {
	// �܂��A�h���X�𐔎��Ƃ��Ĉ���
	int iAddress = (int)OnTenshouExeWmMessage;
	int SubAddress = iAddress - ( pTenshouExeJumpFromToOnTenshouExeWmMessage + 5 )	;

	// �T�Ƃ����̂́A0041702E  -E9 ????????  JMP TSMod.JmpOnTenshouExeWmMessage  �̖��߂ɕK�v�ȃo�C�g���B�v����ɂT�o�C�g�����Ǝ��̃j�[���j�b�N���ߌQ�Ɉړ�����̂��B�����Ă�������̍������W�����v����ۂ̖ړI�i�Ƃ��ė��p�����B
	memcpy(cmdOnTenshouExeJumpFromWmMessage+1, &SubAddress, 4); // +1 ��E9�̎�����4�o�C�g�����������邩��B


	// �\�z�����j�[���j�b�N���߂�TENSHOU.EXE�̃������ɏ���������
	WriteProcessMemory( GetCurrentProcess(), (LPVOID)(pTenshouExeJumpFromToOnTenshouExeWmMessage), cmdOnTenshouExeJumpFromWmMessage, 5, NULL); // 5�o�C�g�̂ݏ�������
}
