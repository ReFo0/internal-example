#include "memory.h"
#include <iostream>



void examplevoid() 
{
	read<uintptr_t>(0x0); // example 
	write<uintptr_t>(0x0, 0); // example 

}

BOOL APIENTRY DllMain(HMODULE hmodule, DWORD call, LPVOID reversed)
{
	if (call == DLL_PROCESS_ATTACH)
		show_console();
	    examplevoid();

	return TRUE;
}

