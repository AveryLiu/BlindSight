#pragma once

void recognitionPipeline();

class BSFaceRecognitionHandler
{
public:
	BSFaceRecognitionHandler();
	~BSFaceRecognitionHandler();
	void startRecognition();
	void stopRecognition();
	void startLearningFaces();
	void stopLearningFaces();

	
};

