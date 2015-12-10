#include "BSDistanceDetector.h"
#include "BSObjectTracker.h"
#include "BSController.h"
#include "BSSpeechSynthesis.h"


/* Threading control */
volatile bool g_stop2 = true;

BSDistanceDetector::BSDistanceDetector()
{
}


BSDistanceDetector::~BSDistanceDetector()
{
}

void BSDistanceDetector::start()
{
	//BSController* controller = BSController::getInstance();
	//PXCSession* session = controller->session;

	g_stop2 = false;
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DistanceDetectorPipeline, (LPVOID)0, 0, 0);
}

void BSDistanceDetector::stop()
{
	g_stop2 = true;
}

bool UnsafeScanForMinimumDistanceMillimetres(PXCImage::ImageData imageData,
	int minimumDistanceMm,
	long long length) {

//	bool alert = false;
	
//	{
	//	long long *ptr = (long long*)imageData.planes[0];

	//	for (long long i = 0; ((i < length) && !alert); i++, ptr++)
	//	{
	//		alert = (*ptr > 0) && (*ptr < minimumDistanceMm);
	//	}
	//}
	//return (alert);
	bool alert = false;
	int a = 0;
	{
		long long *ptr = (long long*)imageData.planes[0];

		for (long long i = 0; ((i < length) && !alert); i++, ptr++)
		{
			
			if ((*ptr > 0) && (*ptr < minimumDistanceMm)) a+=1;
		}
		a;
		length;
		if (a > 0.005 * length) alert = true;
	}
	return (alert);
}

void DistanceDetectorPipeline()
{
	//PXCTracker *pTracker;
	BSController* controller = BSController::getInstance();
	BSSpeechSynthesis* speechSynthesis = controller->speechSynthesis;

	PXCSession* session = controller->session;
	PXCSenseManager *senseMgr = session->CreateSenseManager();



	if (!senseMgr)
	{
		printConsole(L"Failed to create an SDK SenseManager");
		return;
	}

	/* Set Mode & Source */
	pxcStatus sts = PXC_STATUS_NO_ERROR;
	PXCCaptureManager *captureMgr = senseMgr->QueryCaptureManager(); //no need to Release it is released with senseMgr
	pxcCHAR *device = NULL;

	// Live streaming
	device = L"Intel(R) RealSense(TM) 3D Camera";
	captureMgr->FilterByDeviceInfo(device, 0, 0);

	bool stsFlag = true;

	if (senseMgr->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, 640, 480) < 0) {
		printConsole(L"Failed to enable stream.");
	}
	PXCCapture::Sample *sample = senseMgr->QuerySample();
	if (sample == NULL)
	{
		printConsole(L"Sample is null (might not be a problem)\n");
		return;
	}

	sts = senseMgr->Init();

	if (sts >= PXC_STATUS_NO_ERROR)
	{
		//senseMgr->StreamFrames(true);
		printConsole(L"Streaming");

		while (!g_stop2)
		{
			if (senseMgr->AcquireFrame(true) < PXC_STATUS_NO_ERROR)
				break;

			sample = senseMgr->QuerySample();

			PXCImage::ImageData *imageData = new PXCImage::ImageData;

			sample->depth->AcquireAccess(PXCImage::Access::ACCESS_READ, PXCImage::PixelFormat::PIXEL_FORMAT_DEPTH, imageData);
			int minimumDistance = 150; //mm
			if (UnsafeScanForMinimumDistanceMillimetres(
				*imageData,
				minimumDistance, (100*10)))
				//(long long)(sample->depth->QueryInfo().width * sample->depth->QueryInfo().height)))
			{
				printConsole(L"Saw something within 100mm of the camera, PANIC!");
				BSSpeechSynthesis::OutputMessage msg;
				msg.sentence = L"Warning! Object detected"; // not sure if this works, maybe function needed
				speechSynthesis->pushQueue(msg);
				Sleep(2500);
			}
			else {
				printConsole(L"Nothing detected");
			}

			senseMgr->ReleaseFrame();
		}
	}
	else
	{
		printConsole(L"Init Failed");
		stsFlag = false;
	}

	senseMgr->Close();
	senseMgr->Release();
	if (stsFlag)
		printConsole(L"Stopped");
}