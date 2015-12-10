#include "BSSpeechRecognitionHandler.h"
#include "main.h"
#include "BSController.h"
#include "BSSpeechSynthesis.h"
#include "BSFaceRecognitionHandler.h"
#include "BSDistanceDetector.h"
#include <ctime>

pxcCHAR *CharToWChar(char *p)
{
	pxcCHAR *pwcsName;
	int nChars = MultiByteToWideChar(CP_ACP, 0, p, -1, NULL, 0);
	pwcsName = new pxcCHAR[nChars];
	MultiByteToWideChar(CP_ACP, 0, p, -1, (LPWSTR)pwcsName, nChars);

	return pwcsName;
}

/* This handler runs in a seperated thread*/
void PXCAPI BSSpeechRecognitionHandler::OnRecognition(const PXCSpeechRecognition::RecognitionData* data)
{
	BSController* controller = BSController::getInstance();
	BSSpeechSynthesis* speechSynthesis = controller->speechSynthesis;
	BSObjectTracker* objectTracker = controller->objectTracker;
	BSFaceRecognitionHandler* faceRecognitionHandler = controller->faceRecognitionHandler;
	BSDistanceDetector* distanceDetector = controller->distanceDetector;
	BSImageSaver* imageSaver = controller->imageSaver;

	BSSpeechSynthesis::OutputMessage msg;

	switch (data->scores->label)
	{
	case 0:
		printConsole(L"Where's my phone");
		if (controller->getCamera()) {
			msg.sentence = L"Camera setting up, finding the phone.";
			// Setting up camera.
			objectTracker->startTracking(0);
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
		msg.sentence = L"The temperature is thirty two degrees, variable clouds.";
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
	case 6:
	{
		time_t rawtime;
		struct tm timeinfo;
		char dateCharBuffer[80];
		time(&rawtime);
	
		localtime_s(&timeinfo, &rawtime);

		strftime(dateCharBuffer, 80, "It\'s %M past %H on december the fifth, 2015", &timeinfo);
	
		pxcCHAR *datePXCCharBuffer = CharToWChar(dateCharBuffer);

		msg.sentence = datePXCCharBuffer;
		speechSynthesis->pushQueue(msg);

		// Sleep to avoid datePXCCharBuffer being deleted before used.
		Sleep(2500);
		delete datePXCCharBuffer;
		break;
	}
	case 7:
	{
		msg.sentence = L"I'm here! I'm here!";
		speechSynthesis->pushQueue(msg);
		break;
	}
	case 8:
	{
		// Detect the key
		printConsole(L"Where's my Book");
		if (controller->getCamera()) {
			msg.sentence = L"Camera setting up, finding the Book.";
			// Setting up camera.
			objectTracker->startTracking(2);
		}
		else {
			msg.sentence = L"Camera occupied, please stop previous session first.";
		}
		speechSynthesis->pushQueue(msg);
		break;
		
	}
	case 9:
	{
		// Detect the Wallet
		printConsole(L"Where's my Wallet");
		if (controller->getCamera()) {
			msg.sentence = L"Camera setting up, finding the Wallet.";
			// Setting up camera.
			objectTracker->startTracking(1);
		}
		else {
			msg.sentence = L"Camera occupied, please stop previous session first.";
		}
		speechSynthesis->pushQueue(msg);
		break;
	}
	case 10:
	{
		if (controller->tryGetCamera()) {
			if (controller->getCamera()) {
				msg.sentence = L"Camera setting up, reading text for you.";
				imageSaver->start();
				speechSynthesis->pushQueue(msg);
			}
		}
		break;
	}
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

