#include "BSController.h"



void BSController::initialize()
{
	return;
}

void BSController::startController()
{
	CreateThread(
		NULL,
		0,(LPTHREAD_START_ROUTINE)RecThread,
		NULL,
		0,NULL);
	return;
}

void BSController::stopController()
{
	return;
}

BSController::BSController()
{
}

DWORD WINAPI RecThread()
{
	
	return 0;
}
