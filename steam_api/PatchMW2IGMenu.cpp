// ==========================================================
// IW4M project
// 
// Component: clientdll
// Sub-component: steam_api
// Purpose: load out-of-game menu data from in-game.
//
// Initial author: NTAuthority
// Started: 2011-07-16 (copied from M1 code)
// ==========================================================

#include "StdInc.h"

CallHook ingameMenuHook;
DWORD ingameMenuHookLoc = 0x41C178;

void IngameMenuHookFunc(void* menuData, void* menuFile, int num)
{
	DWORD ui_addMenu = 0x4533C0;
	DWORD loadMenuFile = 0x641460;

	const char* filename = "ui_mp/menus.txt";

	__asm
	{
		push num
		push menuFile
		push menuData
		call ui_addMenu
		add esp, 0Ch

		push filename
		call loadMenuFile
		add esp, 4h

		push 1
		push eax
		push menuData
		call ui_addMenu
		add esp, 0Ch
	}
}

void __declspec(naked) IngameMenuHookStub()
{
	__asm
	{
		jmp IngameMenuHookFunc
	}
}

void PatchMW2_InGameMenu()
{
	ingameMenuHook.initialize(ingameMenuHookLoc, IngameMenuHookStub);
	ingameMenuHook.installHook();
}