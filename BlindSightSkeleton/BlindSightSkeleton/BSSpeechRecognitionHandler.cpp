#include "BSSpeechRecognitionHandler.h"



void PXCAPI BSSpeechRecognitionHandler::OnRecognition(const PXCSpeechRecognition::RecognitionData * data)
{
	printConsole(L"OK");
	while (true)
	{
		;
	}
	return;
}

BSSpeechRecognitionHandler::BSSpeechRecognitionHandler()
{
}


BSSpeechRecognitionHandler::~BSSpeechRecognitionHandler()
{
}
