#include "BSController.h"
#include <iostream>
#include "main.h"

BSController* BSController::_controller;

void BSController::initializeAudio()
{
	// Select the audio device
	source->ScanDevices();
	int deviceNum = source->QueryDeviceNum();
	if (deviceNum > MAX_AUDIO_DEVICES) {
		printConsole(L"Audio devices exceed the maximun supported number.");
		return;
	}
	PXCAudioSource::DeviceInfo dinfo[MAX_AUDIO_DEVICES] = { 0 };

	printConsole(L"\nGathering audio devices information");
	for (int d = 0; d < deviceNum; d++) {
		source->QueryDeviceInfo(d, &dinfo[d]);
		std::cout << (char)(d + '1');
		std::wcout << L" " << dinfo[d].name << std::endl;
	}
	printConsole(L"Please select an audio device");
	char choice;
	while (true) {
		std::cin >> choice;
		if (choice >= '1' && choice <= (deviceNum + '0')) {
			source->SetDevice(&dinfo[choice - '0' - 1]);

			break;
		}
		printConsole(L"Input Error");
	}

	// Set the language
	speechRecognition->QueryProfile(0, &pinfo);
	pinfo.language = PXCSpeechRecognition::LANGUAGE_US_ENGLISH;
	speechRecognition->SetProfile(&pinfo);

	// Set command and control
	// TODO: load the commands from files.
	pxcCHAR *cmds[3] = { L"Where's my key", L"Stop", L"What is the weather" };
	speechRecognition->BuildGrammarFromStringList(1, cmds, 0, 3);
	speechRecognition->SetGrammar(1);
}

void BSController::initializeCamera()
{
	;
}

BSController * BSController::getInstance()
{
	if (BSController::_controller == NULL) {
		BSController::_controller = new BSController();
	}
	return BSController::_controller;
}

void BSController::initialize()
{
	
	initializeAudio();
	initializeCamera();

	// Load 3D tracking map file
}

void BSController::startController()
{
	// Start synthesis thread
	speechSynthesis->start();
	// Start recognintion thread
	speechRecognition->StartRec(source, &speechRecognitionHandler);
}

void BSController::stopController()
{
	return;
}

BSController::BSController()
{
	// Init critical section
	InitializeCriticalSection(&getCameraLock);
	InitializeCriticalSection(&releaseCameraLock);

	session = PXCSession::CreateInstance();
	if (!session) {
		printConsole(L"Failed to create session");
		return;
	}

	source = session->CreateAudioSource();
	if (!source) {
		printConsole(L"Failed to create source");
		return;
	}

	session->CreateImpl <PXCSpeechRecognition> (&speechRecognition);
	if (!speechRecognition) {
		printConsole(L"Failed to create speech recognition module");
		return;
	}

	// Init speech synthesis
	speechSynthesis = new BSSpeechSynthesis();
	objectTracker = new BSObjectTracker();
}

BSController::~BSController()
{
	session->Release();
}

int BSController::getCamera()
{
	EnterCriticalSection(&getCameraLock);
	int result = 0;
	if (lock_camera == 1) {
		lock_camera--;
		result = 1;
	}
	LeaveCriticalSection(&getCameraLock);
	return result;
}

int BSController::tryGetCamera()
{
	return lock_camera;
}

int BSController::releaseCamera()
{
	EnterCriticalSection(&releaseCameraLock);
	int result = 0;
	if (lock_camera == 0) {
		lock_camera++;
		result = 1;
	}
	LeaveCriticalSection(&releaseCameraLock);
	return result;
}

