#pragma once
#include <iostream>
#include "BSController.h"
#include <pxcspeechsynthesis.h>
#include "VoiceOut.h"
#include <queue>
#include <Windows.h>

class BSController;



DWORD WINAPI synthesisThread();

class BSSpeechSynthesis
{
public:
	struct OutputMessage
	{
		int msgID = -1; // -1 indicates NULL massage;
		pxcCHAR *sentence = NULL;
	};

	BSSpeechSynthesis();
	~BSSpeechSynthesis();

	void start();
	void isRunning();
	void stop();
	void pushQueue(OutputMessage outMsg);
	OutputMessage getItemFromQueue();
	void speakAloud(pxcCHAR* sentense);

private:
	CRITICAL_SECTION queue_lock;
	std::queue <OutputMessage> outQueue;
	
};

