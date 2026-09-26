#include "ConsoleManager.h"

#include <unistd.h>

// GetConsoleProcessList: Orbiter owns the console alone when it was not started from a terminal
bool ConsoleManager::IsConsoleExclusive(void) {
    return !isatty(STDIN_FILENO);
}

// GetConsoleWindow/ShowWindow: the launching terminal is not an Orbiter window, nothing to show or hide
void ConsoleManager::ShowConsole(bool show)
{
}
