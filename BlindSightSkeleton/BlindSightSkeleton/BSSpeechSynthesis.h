#pragma once
#include <iostream>
#include <pxcspeechsynthesis.h>
#include <queue>
#include <Windows.h>



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
	void speakAloud(pxcCHAR* sentence);

private:
	CRITICAL_SECTION queue_lock;
	std::queue <OutputMessage> outQueue;
	
};

