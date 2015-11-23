#include "BSDistanceDetector.h"
#include "BSObjectTracker.h"
#include "BSController.h"
#include "BSSpeechSynthesis.h"
#include "pxcprojection.h"


/* Threading control */
volatile bool g_stop2 = true;

/* Detection Threshold */
int distance = 100000;

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

	/* Set Module */
	/*sts = senseMgr->EnableTracker(); // is this necessary?
	if (sts < PXC_STATUS_NO_ERROR)
	{
	printConsole(L"Failed to enable tracking module"); // and this
	return;
	} */

	senseMgr->EnableStream(PXCCapture::STREAM_TYPE_DEPTH, 320, 240, 30);

	if (senseMgr->Init() >= PXC_STATUS_NO_ERROR)
	{
		senseMgr->StreamFrames(true);
		printConsole(L"Streaming");

		PXCProjection * projection = captureMgr->QueryDevice()->CreateProjection(); // of voor de init?
		if (projection == NULL) {
			printConsole(L"Unable to create a projection.");
			return;
		}

		bool coordArrayCreated = false;
		PXCPoint3DF32 *vertices = NULL;
		while (!g_stop2) {
			if (senseMgr->AcquireFrame(true) < PXC_STATUS_NO_ERROR)
				break;

			PXCCapture::Sample * sample = senseMgr->QuerySample();
			if (sample == NULL) {
				printConsole(L"Unable to obtain a sample.");
				break;
			}

			PXCImage * depthMap = sample->depth; // does this work?
			if (depthMap == NULL) {
				printConsole(L"Unable to obtain depth map.");
				break;
			}

			PXCImage::ImageInfo dinfo = depthMap->QueryInfo(); // pointer?
			int width = dinfo.width;
			int height = dinfo.height;

			if (!coordArrayCreated) {
				// Assumption: width and height are obtained from resolution
				vertices = new PXCPoint3DF32[width*height];
				coordArrayCreated = true;
			}

			pxcStatus sts = projection->QueryVertices(depthMap, &vertices[0]);
			if (sts < PXC_STATUS_NO_ERROR) {
				printConsole(L"An error occured obtaining vertices from depth map.");
				break;
			}

			// Find minimum distance
			int minDist = 10000000;
			for (int i = 0; i < width * height; i++) {
				if (vertices[i].z < minDist) {
					minDist = vertices[i].z;
				}
			}

			printf("Minimum depth found = %d", minDist);

			//if threshold of closest depth crossed, push message to queue
			//sleep for a while after detection

			senseMgr->ReleaseFrame();
		}
		if (vertices != NULL) {
			delete vertices;
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