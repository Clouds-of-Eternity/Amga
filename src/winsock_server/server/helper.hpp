#pragma once
#include <windows.h>

bool IsKeyDown(WORD virtual_key)
{
	HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
	printf("Enter is key down\n");


	if (!handle)
		return false;

	DWORD read_count;
	INPUT_RECORD record;
	PeekConsoleInput(handle, &record, 1, &read_count);
	if (record.EventType == KEY_EVENT
		&& ((KEY_EVENT_RECORD&)(record.Event)).wVirtualKeyCode == virtual_key
		&& ((KEY_EVENT_RECORD&)(record.Event)).bKeyDown)
		return true;

	printf("Exit is key down false\n");
	return false;
}