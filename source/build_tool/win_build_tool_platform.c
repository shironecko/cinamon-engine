#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <assert.h>

#include "./stb_wrapper.h"
#include "../platform.h"
#include "win_build_tool_lib.h"


enum {
	FG_RED = FOREGROUND_RED,
	FG_GREEN = FOREGROUND_GREEN,
	FG_BLUE = FOREGROUND_BLUE,
	FG_YELLOW = FOREGROUND_RED | FOREGROUND_GREEN,
	FG_PURPLE = FOREGROUND_RED | FOREGROUND_BLUE,
	FG_TEAL = FOREGROUND_GREEN | FOREGROUND_BLUE,
	FG_WHITE = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	FG_BOLD = FOREGROUND_INTENSITY
};

typedef struct {
    build_commands (*get_build_commands)();

    HMODULE library;
} build_tool_lib_info;


LOCAL void reload_build_tool_lib(char *src_lib_path, char *temp_lib_path, build_tool_lib_info *info) {
    OutputDebugStringA("[*] Reloading build tool lib...\n");
    FreeLibrary(info->library);
    *info = (build_tool_lib_info){0};
    if (stb_copyfile(src_lib_path, temp_lib_path) != STB_TRUE) {
        return;
    }

    info->library = LoadLibraryA(temp_lib_path);
    info->get_build_commands = (build_commands (*)())GetProcAddress(info->library, "get_build_commands");

    if (!info->library || !info->get_build_commands) {
        // no load is better than partial load
        if (info->library) {
            FreeLibrary(info->library);
        }
        *info = (build_tool_lib_info){0};
    }
}

void cls(HANDLE hConsole);

int main(int argc, char **argv) {
	HANDLE con_handle = GetStdHandle(STD_OUTPUT_HANDLE);

    build_tool_lib_info build_lib = {0};
    reload_build_tool_lib("win_build_tool_lib.dll", "win_build_tool_lib_tmp.dll", &build_lib);
    assert(build_lib.library && build_lib.get_build_commands);
    build_commands commands = build_lib.get_build_commands();

	int user_choice;
	do {
		SetConsoleTextAttribute(con_handle, FG_WHITE | FG_BOLD);

        printf("{ ");
        for (u32 i = 0; i < commands.commands_count; ++i) {
            printf("[%c]%s ", commands.commands[i].trigger_key, commands.commands[i].description);
        }
        printf("} $>");
		user_choice = _getch();
		printf("\n");

        for (u32 i = 0; i < commands.commands_count; ++i) {
            if (user_choice == commands.commands->trigger_key) {
                b32 should_reload_build_lib = false;
                commands.commands[i].command(&should_reload_build_lib);
                if (should_reload_build_lib) {
                    reload_build_tool_lib("win_build_tool_lib.dll", "win_build_tool_lib_tmp.dll", &build_lib);
                    assert(build_lib.library && build_lib.get_build_commands);
                    build_commands commands = build_lib.get_build_commands();
                }
                break;
            }
        }
	} while (user_choice != 'q');

	SetConsoleTextAttribute(con_handle, FG_WHITE);
	return 0;
}

// lifted from https://msdn.microsoft.com/en-us/library/windows/desktop/ms682022(v=vs.85).aspx
void cls(HANDLE hConsole) {
	COORD coordScreen = {0, 0}; // home for the cursor
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;

	// Get the number of character cells in the current buffer.
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) { return; }

	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

	// Fill the entire screen with blanks.
	if (!FillConsoleOutputCharacter(hConsole,        // Handle to console screen buffer
	                                (TCHAR)' ',      // Character to write to the buffer
	                                dwConSize,       // Number of cells to write
	                                coordScreen,     // Coordinates of first cell
	                                &cCharsWritten)) // Receive number of characters written
	{
		return;
	}

	// Get the current text attribute.
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) { return; }

	// Set the buffer's attributes accordingly.
	if (!FillConsoleOutputAttribute(hConsole,         // Handle to console screen buffer
	                                csbi.wAttributes, // Character attributes to use
	                                dwConSize,        // Number of cells to set attribute
	                                coordScreen,      // Coordinates of first cell
	                                &cCharsWritten))  // Receive number of characters written
	{
		return;
	}

	// Put the cursor at its home coordinates.
	SetConsoleCursorPosition(hConsole, coordScreen);
}
