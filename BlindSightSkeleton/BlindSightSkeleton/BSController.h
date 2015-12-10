#pragma once
#include "BSObjectTracker.h"
#include "BSSpeechRecognitionHandler.h"
#include "BSSpeechSynthesis.h"
#include "BSFaceRecognitionHandler.h"
#include "BSDistanceDetector.h"
#include "BSImageSaver.h"
#include <pxcspeechrecognition.h>
#include <pxcsensemanager.h>
#include <Windows.h>
#include <queue>
#include <map>

// Macros
#define MAX_AUDIO_DEVICES 5

class BSController
{
public:
	// Singleton pattern
	static BSController* getInstance();

	void initialize();
	void startController();
	void stopController();

	// Properities from SDK
	PXCSession *session;
	PXCAudioSource *source;
	PXCSpeechRecognition *speechRecognition;
	PXCSpeechRecognition::ProfileInfo pinfo;

	int getCamera();
	int tryGetCamera();
	int releaseCamera();

	BSObjectTracker* objectTracker;
	BSSpeechSynthesis* speechSynthesis;
	BSFaceRecognitionHandler* faceRecognitionHandler;
	BSDistanceDetector* distanceDetector;
	BSImageSaver* imageSaver;

protected:
	BSController();
	~BSController();

private:
	static BSController* _controller;

	// Camera management operations
	CRITICAL_SECTION getCameraLock;
	CRITICAL_SECTION releaseCameraLock;
	int lock_camera = 1;

	void initializeAudio();
	void initializeCamera();

	BSSpeechRecognitionHandler speechRecognitionHandler;
};


