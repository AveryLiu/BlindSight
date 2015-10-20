#include "BSSpeechRecognitionHandler.h"
#include "main.h"
#include "BSController.h"
#include "BSSpeechSynthesis.h"

/* This handler runs in a seperated thread*/
void PXCAPI BSSpeechRecognitionHandler::OnRecognition(const PXCSpeechRecognition::RecognitionData* data)
{
	BSController* controller = BSController::getInstance();
	BSSpeechSynthesis* speechSynthesis = controller->speechSynthesis;
	BSSpeechSynthesis::OutputMessage msg;

	switch (data->scores->label)
	{
	case 0:
		printConsole(L"Find my key");
		break;
	case 1:
		msg.msgID = 0;
		speechSynthesis->pushQueue(msg);
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
