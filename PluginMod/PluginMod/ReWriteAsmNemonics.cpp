#include "ReWriteAsmNemonics.h"

void ReWriteAsmNemonics() {
	WriteAsmJumperOnTenshouExeAfterChangeWindowSize();
	WriteAsmJumperOnTenshouExeViewingTopMenu();
	WriteAsmJumperOnTenshouExeMainGameStart();
	WriteAsmJumperOnTenshouExeSelectingScenarioDaimyo();
	WriteAsmJumperOnTenshouExeSobaChange();
	WriteAsmJumperOnTenshouExeSelectingScenario();
	WriteAsmJumperOnTenshouExeWritingScenarioDetail1st();
	WriteAsmJumperOnTenshouExeWritingScenarioDetail2nd();
	WriteAsmJumperOnTenshouExeWritingScenarioDetail3rd();
	WriteAsmJumperOnTenshouExeViewingScenarioTitleList();
	WriteAsmJumperOnTenshouExeGlobalFreeMemoryInGameEnd();
	WriteAsmJumperOnTenshouExeWmMessage();
}
