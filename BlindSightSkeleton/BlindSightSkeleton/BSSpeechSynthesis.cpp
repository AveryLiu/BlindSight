#include "BSSpeechSynthesis.h"
#include "VoiceOut.h"


BSSpeechSynthesis::BSSpeechSynthesis()
{
	InitializeCriticalSection(&queue_lock);
}


BSSpeechSynthesis::~BSSpeechSynthesis()
{
	;
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

void BSSpeechSynthesis::speakAloud(pxcCHAR * sentence)
{
	BSController *controller = BSController::getInstance();
	PXCSession *session = controller->session;

	// Create a PXCSpeechSynthesis instance
	PXCSpeechSynthesis *synth = 0;
	pxcStatus sts = session->CreateImpl<PXCSpeechSynthesis>(&synth);

	// Configure the module
	PXCSpeechSynthesis::ProfileInfo pinfo;
	synth->QueryProfile(0, &pinfo);
	pinfo.language = PXCSpeechSynthesis::LANGUAGE_US_ENGLISH;
	sts = synth->SetProfile(&pinfo);
	if (sts < PXC_STATUS_NO_ERROR) {
		synth->Release();
		return;
	}

	synth->BuildSentence(1, sentence);

	// Render the sentence or write it to vo's internal memory buffer
	VoiceOut vo(&pinfo);
	int nbuffers = synth->QueryBufferNum(1);
	for (int i = 0; i < nbuffers; i++) {
		PXCAudio *sample = synth->QueryBuffer(1, i);
		vo.RenderAudio(sample);
	}

	synth->Release();
	return;
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