#include "BSObjectTracker.h"
#include "BSController.h"
#include "BSSpeechSynthesis.h"

pxcCHAR     g_file[1024] = { 0 };
pxcCHAR     g_buf[20];
std::vector<Model> g_targets(0);
pxcCHAR		g_status[512];

/* Threading control */
volatile bool g_stop = false;

int g_tracking_map_flag = 0;
BSObjectTracker::BSObjectTracker()
{
}


BSObjectTracker::~BSObjectTracker()
{
}

void BSObjectTracker::startTracking(int trackingMapFlag)
{
	BSController* controller = BSController::getInstance();
	PXCSession* session = controller->session;

	if (!g_targets.empty()) {
		g_targets.clear();
	}
	// Load the map file
	pxcCHAR	phoneFile[1024] = L"C:\\Users\\Avery Liu\\Desktop\\myPhone.slam";
	pxcCHAR walletFile[1024] = L"C:\\Users\\Avery Liu\\Desktop\\myWallet.slam";
	pxcCHAR keyFile[1024] = L"C:\\Users\\Avery Liu\\Desktop\\myBook.slam";
	if (trackingMapFlag == 0)
		g_targets.push_back(Model(phoneFile));
	else if (trackingMapFlag == 1)
		g_targets.push_back(Model(walletFile));
	else if (trackingMapFlag == 2)
		g_targets.push_back(Model(keyFile));

	g_stop = false;
	g_tracking_map_flag = trackingMapFlag;
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ObjectTrackingPipeline, (LPVOID)0, 0, 0);
}

void BSObjectTracker::stopTracking()
{
	g_stop = true;
}



// Creates a pipeline which begins tracking as soon as streaming starts
void ObjectTrackingPipeline()
{
	PXCTracker *pTracker;
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
	sts = senseMgr->EnableTracker();
	if (sts < PXC_STATUS_NO_ERROR)
	{
		printConsole(L"Failed to enable tracking module");
		return;
	}

	// Init
	printConsole(L"Init Started");

	pTracker = senseMgr->QueryTracker();
	if (pTracker == NULL)
	{
		printConsole(L"Failed to Query tracking module\n");
		return;
	}

	if (senseMgr->Init() >= PXC_STATUS_NO_ERROR)
	{
		printConsole(L"Streaming");

		// Configure tracking now that the module has been initialized
		pxcUID cosID;
		for (size_t i = 0; i < g_targets.size(); i++)
		{
			g_targets[i].cosIDs.clear();

			// 3-D tracking

			pxcUID firstID, lastID;
			sts = pTracker->Set3DTrack(g_targets[i].model_filename, firstID, lastID);
			while (firstID <= lastID)
			{
				PXCTracker::TrackingValues vals;
				pTracker->QueryTrackingValues(firstID, vals);

				g_targets[i].addCosID(firstID, vals.targetName);
				firstID++;
			}

			if (sts<PXC_STATUS_NO_ERROR)
			{
				printConsole(L"Failed to set tracking configuration!");
				return;
			}
		}

		while (!g_stop)
		{
			if (senseMgr->AcquireFrame(true) < PXC_STATUS_NO_ERROR)
				break;

			/* Display Results */
			PXCTracker::TrackingValues  trackData;
			int updatedTrackingCount = 0;
			const PXCCapture::Sample *sample = senseMgr->QueryTrackerSample();

			if (!sample)
				printConsole(L"Sample not loaded");

			// Loop over all of the registered targets (COS IDs) and see if they are tracked
			for (TargetIterator targetIter = g_targets.begin(); targetIter != g_targets.end(); targetIter++)
			{
				for (TrackingIterator iter = targetIter->cosIDs.begin(); iter != targetIter->cosIDs.end(); iter++)
				{
					pTracker->QueryTrackingValues(iter->cosID, trackData);

					if (PXCTracker::IsTracking(trackData.state))
					{
						updatedTrackingCount += (!iter->isTracking) ? 1 : 0;
						iter->isTracking = true;
						
						BSSpeechSynthesis::OutputMessage msg;
						std::wcout << iter->friendlyName << std::endl;
						
						if (g_tracking_map_flag == 0)
							msg.sentence = L"Phone found.";
						else if (g_tracking_map_flag == 1)
							msg.sentence = L"Wallet found";
						else if (g_tracking_map_flag == 2)
							msg.sentence = L"Book found";
						speechSynthesis->pushQueue(msg);
						printConsole(L"Items found.");

						// Sleep to avoid excessive output.
						Sleep(2500);
					}
					else
					{
						updatedTrackingCount += iter->isTracking ? 1 : 0;
						iter->isTracking = false;
					}
				}
			}
			senseMgr->ReleaseFrame();
		}
	}
	else
	{
		printConsole(L"Init Failed");
		controller->releaseCamera();
		stsFlag = false;
	}

	senseMgr->Close();
	senseMgr->Release();
	if (stsFlag)
		printConsole(L"Stopped");
}

