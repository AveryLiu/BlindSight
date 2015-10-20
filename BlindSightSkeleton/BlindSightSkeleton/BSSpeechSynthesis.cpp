#include "BSSpeechSynthesis.h"



BSSpeechSynthesis::BSSpeechSynthesis()
{
	InitializeCriticalSection(&queue_lock);
}


BSSpeechSynthesis::~BSSpeechSynthesis()
{
}

void BSSpeechSynthesis::pushQueue(OutputMessage outMsg)
{
	EnterCriticalSection(&queue_lock);
	this->outQueue.push(outMsg);
	LeaveCriticalSection(&queue_lock);
	return;
}

BSSpeechSynthesis::OutputMessage BSSpeechSynthesis::getItemFromQueue() 
{
	OutputMessage msg;
	EnterCriticalSection(&queue_lock);
	if (!this->outQueue.empty()) {
		msg = outQueue.front();
		outQueue.pop();
	}
	LeaveCriticalSection(&queue_lock);
	return msg;
}

void BSSpeechSynthesis::speakAloud(pxcCHAR * sentense)
{
}

void BSSpeechSynthesis::start()
{
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)synthesisThread, NULL, 0, NULL);
}

void BSSpeechSynthesis::stop()
{
	;
}

void BSSpeechSynthesis::isRunning()
{
	;
}

DWORD WINAPI::synthesisThread()
{
	BSController* controller = BSController::getInstance();
	BSSpeechSynthesis* speechSynthesis = controller->speechSynthesis;

	while (true) {
		BSSpeechSynthesis::OutputMessage msg = speechSynthesis->getItemFromQueue();
		if (msg.msgID == 0) break; 
		if (msg.sentence != NULL) {
			printConsole(msg.sentence);
			speechSynthesis->speakAloud(msg.sentence);
		}
		else
		{
			printConsole(L"Message hasn't been received.");
		}
	
	}
	return 0;
}