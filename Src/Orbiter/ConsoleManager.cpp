#include "ConsoleManager.h"

#ifndef __linux__
#include <windows.h>
#else // __linux__
#include <unistd.h>
#endif // __linux__

#ifdef __linux__
// GetConsoleProcessList: Orbiter owns the console alone when it was not started from a terminal
#endif // __linux__
bool ConsoleManager::IsConsoleExclusive(void) {
#ifndef __linux__
    DWORD pids[2];
    DWORD num_pids = GetConsoleProcessList(pids, 2);
    return num_pids <= 1;
#else // __linux__
    return !isatty(STDIN_FILENO);
#endif // __linux__
}

#ifdef __linux__
// GetConsoleWindow/ShowWindow: the launching terminal is not an Orbiter window, nothing to show or hide
#endif // __linux__
void ConsoleManager::ShowConsole(bool show)
{
#ifndef __linux__
    HWND wnd = GetConsoleWindow();
    if (wnd)
        ShowWindow(wnd, show ? SW_SHOW : SW_HIDE);
#endif // !__linux__
}
