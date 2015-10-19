#include <iostream>
#include "BSController.h"

void printConsole(char *info)
{
   std::cout << info << std::endl;
}

void printWelcomeInfo()
{
	printConsole("#### BlindSight Beta Verison ####");
	printConsole("Created by team BlindSight");
	printConsole("Initializing...");
}

void printMenu()
{
	printConsole("0. List device info");
	printConsole("1. Terminate");
}

int main(int argc, const char *argv[])
{
	char choice;
	bool isRunning = true;
	printWelcomeInfo();

	BSController controller;
	controller.initialize();

	printConsole("Starting the BlindSight controller...");
	controller.startBlindSight();
	while (isRunning)
	{
		printMenu();
		std::cin >> choice;
		switch (choice)
		{
		case '0':
			break;
		case '1':
			isRunning = false;
		default:
			printConsole("operation wasn't proceessed.");
			break;
		}
	}
	
	controller.stopBlindSight();
	return 0;
}