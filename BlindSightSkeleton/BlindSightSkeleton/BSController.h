#include "BSObjectTracker.h"
#include "BSSpeechRecognizer.h"
#include "BSSpeechSythesis.h"
#include "BSSpeechRecognitionHandler.h"
#include <pxcspeechrecognition.h>
#include <pxcsensemanager.h>
#include <Windows.h>
#include <queue>
#include <map>

// Macros
#define MAX_AUDIO_DEVICES 5

// Recoginizing work in this thread
DWORD WINAPI RecThread();

class BSController
{
public:
	// Singleton pattern
	static BSController& getInstance() {
		static BSController controller;
		return controller;
	}
	
	void initialize();
	void startController();
	void stopController();

	// Properities related to the SDK
	PXCSession *session;
	PXCAudioSource *source;
	PXCSpeechRecognition *speechRecognition;
	PXCSpeechRecognition::ProfileInfo pinfo;

protected:
	BSController();
	~BSController();

private:
	// Camera management operations
	LPCRITICAL_SECTION getCameraLock;
	LPCRITICAL_SECTION realseCameraLock;
	int lock_camera = 1;
	int getCamera();
	int releaseCamera();

	void initializeAudio();
	void initializeCamera();

	BSObjectTracker objectTracker;
	BSSpeechSythesis speechSythesis;
	BSSpeechRecognitionHandler speechRecognitionHandler;
};


