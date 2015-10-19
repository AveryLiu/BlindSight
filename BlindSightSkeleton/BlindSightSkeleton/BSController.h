#include "BSObjectTracker.h"
#include "BSSpeechRecognizer.h"
#include "BSSpeechSythesis.h"
#include <Windows.h>
#include <queue>
// Recoginizing work in this thread
DWORD WINAPI RecThread();

class BSController
{
public:
	static BSController& getInstance() {
		static BSController controller;
		return controller;
	}
	
	void initialize();
	void startController();
	void stopController();
	
	enum BSState { READY, OBJECT_TRACKING, SPEECH_RECOGNIZING, SPEECH_SYNTHESIZING };

	// Message queue for interchanging data
	std::queue <BSState> messageQueue;

protected:
	BSController();
private:
	BSState state;
	HWND handle;

	BSObjectTracker objectTracker;
	BSSpeechSythesis speechSythesis;
};
