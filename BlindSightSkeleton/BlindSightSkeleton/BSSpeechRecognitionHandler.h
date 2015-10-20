#pragma once
#include <pxcspeechrecognition.h>
#include "main.h"
class BSSpeechRecognitionHandler : public PXCSpeechRecognition::Handler
{
public:
	virtual void PXCAPI OnRecognition(const PXCSpeechRecognition::RecognitionData* data);
	BSSpeechRecognitionHandler();
	~BSSpeechRecognitionHandler();
};

