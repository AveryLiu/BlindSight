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

	//Create an instance of the PXCSpeechSynthesis
	PXCSpeechSynthesis *tts = 0;
	pxcStatus sts = session->CreateImpl<PXCSpeechSynthesis>(&tts);
	if (sts != pxcStatus::PXC_STATUS_NO_ERROR) {
		wprintf_s(L"Failed to create an instance of the PXCSpeechSynthesis\n");
		return;
	}
	//Initialize the Module
	PXCSpeechSynthesis::ProfileInfo pinfo;
	tts->QueryProfile(0, &pinfo);
	pinfo.language = PXCSpeechSynthesis::LANGUAGE_US_ENGLISH;
	sts = tts->SetProfile(&pinfo);
	if (sts != pxcStatus::PXC_STATUS_NO_ERROR) {
		wprintf_s(L"Failed to Initialize the Module\n");
		return;
	}

	// Synthesize the text string
	tts->BuildSentence(1, sentence);
	// Retrieve the synthesized speech
	int nbuffers = tts->QueryBufferNum(1);
	for (int i = 0; i<nbuffers; i++) {
		PXCAudio *audio = tts->QueryBuffer(1, i);
		// send audio to the audio output device
		VoiceOut vo(&pinfo);
		vo.RenderAudio(audio);

		// Clean up
		tts->ReleaseSentence(1);
	}
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