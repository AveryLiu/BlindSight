#include "BSSpeechRecognitionHandler.h"
#include "main.h"
#include "BSController.h"
#include "BSSpeechSynthesis.h"
#include "BSFaceRecognitionHandler.h"
#include "BSDistanceDetector.h"

/* This handler runs in a seperated thread*/
void PXCAPI BSSpeechRecognitionHandler::OnRecognition(const PXCSpeechRecognition::RecognitionData* data)
{
	BSController* controller = BSController::getInstance();
	BSSpeechSynthesis* speechSynthesis = controller->speechSynthesis;
	BSObjectTracker* objectTracker = controller->objectTracker;
	BSFaceRecognitionHandler* faceRecognitionHandler = controller->faceRecognitionHandler;
	BSDistanceDetector* distanceDetector = controller->distanceDetector;

	BSSpeechSynthesis::OutputMessage msg;

	switch (data->scores->label)
	{
	case 0:
		printConsole(L"Where's my key");
		if (controller->getCamera()) {
			msg.sentence = L"Camera setting up, finding the key.";
			// Setting up camera.
			objectTracker->startTracking();
		}
		else {
			msg.sentence = L"Camera occupied, please stop previous session first.";
		}
		speechSynthesis->pushQueue(msg);
		break;
	case 1:
		msg.msgID = 0;
		speechSynthesis->pushQueue(msg);
		printConsole(L"Stop");
		break;
	case 2:
		msg.sentence = L"The weather is sunny.";
		speechSynthesis->pushQueue(msg);
		printConsole(L"What is the weather");
		break;
	case 3:
		if (controller->getCamera()) {
			msg.sentence = L"Camera setting up, Recognizing Faces.";
			// Setting up camera.
			faceRecognitionHandler->startRecognition();
		}
		else {
			msg.sentence = L"Camera occupied, please stop previous session first.";
		}
		speechSynthesis->pushQueue(msg);
		break;
	case 4:
		if (controller->getCamera()) {
			msg.sentence = L"Camera setting up, Learning Faces.";
			// Setting up camera.
			faceRecognitionHandler->startLearningFaces();
		}
		else {
			msg.sentence = L"Camera occupied, please stop previous session first.";
		}
		speechSynthesis->pushQueue(msg);
		break;
	case 5:
		if (controller->getCamera()) {
			msg.sentence = L"Camera setting up, detecting distance.";
			// Setting up camera.
			distanceDetector->start();
		}
		else {
			msg.sentence = L"Camera occupied, please stop previous session first.";
		}
		speechSynthesis->pushQueue(msg);
		break;
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
