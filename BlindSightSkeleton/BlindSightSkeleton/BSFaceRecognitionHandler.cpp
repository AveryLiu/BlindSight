#include "BSFaceRecognitionHandler.h"
#include "BSController.h"
#include "BSSpeechSynthesis.h"
#include <fstream>

typedef std::basic_ofstream<unsigned char, std::char_traits<unsigned char>> uofstream;
typedef std::basic_ifstream<unsigned char, std::char_traits<unsigned char>> uifstream;

/* Threading control */
volatile bool g_recognize_stop = true;
volatile bool g_learning_stop = true;

BSFaceRecognitionHandler::BSFaceRecognitionHandler()
{
}


BSFaceRecognitionHandler::~BSFaceRecognitionHandler()
{
}

void BSFaceRecognitionHandler::startRecognition()
{
	BSController* controller = BSController::getInstance();
	PXCSession* session = controller->session;

	g_recognize_stop = false;
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)recognitionPipeline, (LPVOID)0, 0, 0);
}

void BSFaceRecognitionHandler::stopRecognition()
{
	g_recognize_stop = true;
}

void BSFaceRecognitionHandler::startLearningFaces()
{
	BSController* controller = BSController::getInstance();
	PXCSession* session = controller->session;

	g_learning_stop = false;
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)recognitionPipeline, (LPVOID)0, 0, 0);
}

void BSFaceRecognitionHandler::stopLearningFaces()
{
	g_learning_stop = true;
}

void recognitionPipeline()
{
	PXCFaceModule *faceModule;
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

	// Init
	printConsole(L"Init Started");

	/* Set Module */
	sts = senseMgr->EnableFace();  //CHANGE BACK LATER
	if (sts < PXC_STATUS_NO_ERROR)
	{
		printConsole(L"Failed to enable Face module");
		return;
	}

	// Get a face instance (or inside the AcquireFrame/ReleaseFrame loop) for configuration.
	faceModule = senseMgr->QueryFace();
	if (faceModule == NULL)
	{
		printConsole(L"Failed to Query FaceModule\n");
		return;
	}

	//TODO - load recognition database: example 76
	//if no database exists yet, create one
	//else: load database
	PXCFaceConfiguration *cfg = faceModule->CreateActiveConfiguration();
	if (cfg == NULL)
	{
		printConsole(L"Failed to Create Active Configuration instance\n");
		return;
	}

	cfg->SetTrackingMode(PXCFaceConfiguration::FACE_MODE_COLOR_PLUS_DEPTH);
	cfg->ApplyChanges();
	sts = senseMgr->Init();

	PXCFaceConfiguration::RecognitionConfiguration *rcfg = cfg->QueryRecognition();
	if (rcfg == NULL)
	{
	printConsole(L"Failed to Query Recognition Configuration instance\n");
	return;
	} // Difference with sdk code
	// Enable face recognition
	rcfg->Enable(); // Is it necessary to call this function before creating/loading a database?

	// Make it effective
	cfg->QueryRecognition()->SetRegistrationMode(PXCFaceConfiguration::RecognitionConfiguration::REGISTRATION_MODE_CONTINUOUS);

	cfg->ApplyChanges();


	PXCFaceData *fdata = faceModule->CreateOutput();
	if (fdata == NULL) {
		printConsole(L"No FaceData found.");
		return;
	}

	if (sts >= PXC_STATUS_NO_ERROR)
	{
		printConsole(L"Streaming");
		// Check if both flags are false, in which case something is wrong
		/*if (!g_learning_stop & !g_recognize_stop)
		{
			g_learning_stop = true;
			g_recognize_stop = true;
			printConsole(L"Learning and recognition cannot be done at the same time. Stopped both processes.");
		}*/

			// configure for learning
		while (!g_learning_stop) {
			if (senseMgr->AcquireFrame(true) < PXC_STATUS_NO_ERROR)
				break;
			fdata->Update();
			pxcI32 nfaces = fdata->QueryNumberOfDetectedFaces();

			if (nfaces > 0) {
				if (fdata->QueryFaceByIndex(0)) {
					fdata->QueryFaceByIndex(0)->QueryRecognition()->RegisterUser();
					g_learning_stop = true;
					
					BSSpeechSynthesis::OutputMessage msg;
					msg.sentence = L"Nice to meet you.";
					speechSynthesis->pushQueue(msg);
				}

			}
			senseMgr->ReleaseFrame();
		}
		
		uifstream infile;
		char databaseName[20] = "database.dat";
		infile.open(databaseName, std::ios::binary | std::ios::in);
		if (infile.good()) {
			//TODO - load database into buffer file, obtain size; see below / example 76
			//pxcI32 nbytes = rmd->QueryDatabaseSize();
			//pxcBYTE *buffer = new pxcBYTE[nbytes];
			// Load the database buffer back.
			//rcfg->SetDatabaseBuffer(buffer, nbytes);
		}

		else { //no database present
			   // Create a recognition database
			PXCFaceConfiguration::RecognitionConfiguration::RecognitionStorageDesc desc = {};
			desc.maxUsers = 10;
			rcfg->CreateStorage(L"MyDB", &desc);
			rcfg->UseStorage(L"MyDB");


			//TODO - save and close database: example 76
			if (fdata != NULL) {
				// allocate the buffer to save the database
				PXCFaceData::RecognitionModuleData *rmd = fdata->QueryRecognitionModule();
				pxcI32 nbytes = rmd->QueryDatabaseSize();
				pxcBYTE *buffer = new pxcBYTE[nbytes];

				// retrieve the database buffer
				rmd->QueryDatabaseBuffer(buffer);

				// TODO - Save the buffer to a file. Apply industry standard encryption to protect privacy.
				uofstream outfile;
				outfile.open(databaseName, std::ios::binary | std::ios::out);
				outfile.write(reinterpret_cast<const unsigned char *>(&nbytes), sizeof(nbytes));
				outfile.write(buffer, sizeof(buffer));
			}
		}
			g_recognize_stop = false;
			while (!g_recognize_stop) {
				for (int i = 0; i < 100; i++)
					if (senseMgr->AcquireFrame(true) < PXC_STATUS_NO_ERROR)
						break;

				fdata->Update();

				// fdata is a PXCFaceData instance
				pxcI32 nfaces = fdata->QueryNumberOfDetectedFaces();
				printf("%d\n", nfaces);
				for (pxcI32 i = 0; i < nfaces; i++) {
					// Retrieve the recognition data instance
					PXCFaceData::Face *face = fdata->QueryFaceByIndex(i);
					PXCFaceData::RecognitionData *rdata = face->QueryRecognition();

					// recognize the current face?
					pxcI32 uid = rdata->QueryUserID();
					if (uid >= 0) {
						// do something with the recognized user.
						BSSpeechSynthesis::OutputMessage msg;
						msg.sentence = L"Person found.";
						speechSynthesis->pushQueue(msg);
						printConsole(L"Gotta catch em all.");
						Sleep(2500);
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

	cfg->Release();
	senseMgr->Close();
	senseMgr->Release();
	if (stsFlag)
		printConsole(L"Stopped");
}


