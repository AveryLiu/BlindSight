#include <iostream>
#include "BSController.h"

void printConsole(pxcCHAR *info)
{
   std::wcout << info << std::endl;
}

void printWelcomeInfo()
{
	printConsole(L"#### BlindSight Beta Verison ####");
	printConsole(L"Created by team BlindSight");
}

void printMenu()
{
	printConsole(L"1. Terminate");
}

int main(int argc, const char *argv[])
{
	char choice;
	bool mainThreadIsRunning = true;
	printWelcomeInfo();

	printConsole(L"\nIni1tializing");
	BSController* controller = BSController::getInstance();
	controller->initialize();

	printConsole(L"Starting the BlindSight controller");
	controller->startController();

	while (mainThreadIsRunning)
	{
		printMenu();
		std::cin >> choice;
		switch (choice)
		{
		case '0':
			break;
		case '1':
			mainThreadIsRunning = false;
		default:
			printConsole(L"operation wasn't proceessed.");
			break;
		}
	}
	
	controller->stopController();

	return 0;
}