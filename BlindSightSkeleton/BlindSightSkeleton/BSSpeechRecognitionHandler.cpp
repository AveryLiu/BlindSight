#include "BSSpeechRecognitionHandler.h"
#include "main.h"


void PXCAPI BSSpeechRecognitionHandler::OnRecognition(const PXCSpeechRecognition::RecognitionData * data)
{
	switch (data->scores->label)
	{
	case 0:
		printConsole(L"Find my key");
		break;
	case 1:
		printConsole(L"Stop");
		break;
	case 2:
		printConsole(L"What is the weather");
	default:
		break;
	}
	return;
}

BSSpeechRecognitionHandler::BSSpeechRecognitionHandler()
{
}


BSSpeechRecognitionHandler::~BSSpeechRecognitionHandler()
{
}
