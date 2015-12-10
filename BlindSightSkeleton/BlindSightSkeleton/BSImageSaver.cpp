#include "BSImageSaver.h"
#include "BSController.h"
#include <opencv.hpp>
#include <core.hpp>
#include <imgcodecs.hpp>
#include <highgui.hpp>


//Marcio's includes
#include <baseapi.h>

#include <leptonica\allheaders.h>
#include <string>

using namespace cv;
using namespace std;

string filename = "C:\\image\\image.bmp";

/* Threading control */
volatile bool g_stop3;

void BSImageSaver::start()
{
	BSController* controller = BSController::getInstance();
	PXCSession* session = controller->session;

	g_stop3 = false;
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ImageSaverPipeline, (LPVOID)0, 0, 0);
}

void BSImageSaver::stop()
{
	g_stop3 = true;
}

void ConvertPXCImageToOpenCVMat(PXCImage *inImg, Mat *outImg) {
	int cvDataType = 0;
	int cvDataWidth = 1;

	PXCImage::ImageData data;
	inImg->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB32, &data);
	PXCImage::ImageInfo imgInfo = inImg->QueryInfo();

	switch (data.format) {
		/* STREAM_TYPE_COLOR */
	case PXCImage::PIXEL_FORMAT_YUY2: /* YUY2 image  */
	case PXCImage::PIXEL_FORMAT_NV12: /* NV12 image */
		throw(0); // Not implemented
		break;
	case PXCImage::PIXEL_FORMAT_RGB32: /* BGRA layout on a little-endian machine */
		cvDataType = CV_8UC4;
		cvDataWidth = 4;
		break;
	case PXCImage::PIXEL_FORMAT_RGB24: /* BGR layout on a little-endian machine */
		cvDataType = CV_8UC3;
		cvDataWidth = 3;
		break;
	case PXCImage::PIXEL_FORMAT_Y8:  /* 8-Bit Gray Image, or IR 8-bit */
		cvDataType = CV_8U;
		cvDataWidth = 1;
		break;

		/* STREAM_TYPE_DEPTH */
	case PXCImage::PIXEL_FORMAT_DEPTH: /* 16-bit unsigned integer with precision mm. */
	case PXCImage::PIXEL_FORMAT_DEPTH_RAW: /* 16-bit unsigned integer with device specific precision (call device->QueryDepthUnit()) */
		cvDataType = CV_16U;
		cvDataWidth = 2;
		break;
	case PXCImage::PIXEL_FORMAT_DEPTH_F32: /* 32-bit float-point with precision mm. */
		cvDataType = CV_32F;
		cvDataWidth = 4;
		break;

		/* STREAM_TYPE_IR */
	case PXCImage::PIXEL_FORMAT_Y16:          /* 16-Bit Gray Image */
		cvDataType = CV_16U;
		cvDataWidth = 2;
		break;
	case PXCImage::PIXEL_FORMAT_Y8_IR_RELATIVE:    /* Relative IR Image */
		cvDataType = CV_8U;
		cvDataWidth = 1;
		break;
	}

	// suppose that no other planes
	if (data.planes[1] != NULL) printf("Hello"); // not implemented
												 // suppose that no sub pixel padding needed
	if (data.pitches[0] % cvDataWidth != 0) printf("Hello"); // not implemented
	outImg = new Mat(imgInfo.height, data.pitches[0] / cvDataWidth, cvDataType);
	//outImg->create(imgInfo.height, data.pitches[0] / cvDataWidth, cvDataType);

	memcpy(outImg->data, data.planes[0], imgInfo.height*imgInfo.width*cvDataWidth*sizeof(pxcBYTE));

	//inImg->ReleaseAccess(&data);
}

char* readText(string filename)
{
	char *outText;

	tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
	// Initialize tesseract-ocr with English, without specifying tessdata path
	if (api->Init(NULL, "eng")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}

	// Open input image with leptonica library
	const char * c = filename.c_str();
	Pix *image = pixRead(c);

	api->SetImage(image);

	// Get OCR result
	outText = api->GetUTF8Text();
	printf("\n%s\n", outText);

	//delete [] outText;
	pixDestroy(&image);

	// Destroy used object and release memory
	api->End();

	return outText;
}

void ImageSaverPipeline() {

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

	// Select the color stream
	if (senseMgr->EnableStream(PXCCapture::STREAM_TYPE_COLOR, 640, 480) < 0) {
		printConsole(L"Failed to enable stream.");
	}

	PXCCapture::Sample *sample = senseMgr->QuerySample();
	if (sample == NULL)
	{
		printConsole(L"Sample is null (might not be a problem)\n");
		return;
	}

	// Initialize and Stream Samples
	if (senseMgr->Init() < 0) {
		printConsole(L"Init failed");
	}

	printConsole(L"Initialized");
	int counter = 0;
	while (!g_stop3) {
		// This function blocks until a color sample is ready
		if (senseMgr->AcquireFrame(true)<PXC_STATUS_NO_ERROR) break;
		//printConsole(L"Streaming");

		counter = counter + 1;
		if (counter > 30) {
			printConsole(L"Streaming");
			// retrieve the sample
			sample = senseMgr->QuerySample();

			// work on the image sample->color
			PXCImage *image = sample->color;

			if (image != NULL) {

				Mat *outImg = NULL;
				//ConvertPXCImageToOpenCVMat(image, outImg);


				int cvDataType = 0;
				int cvDataWidth = 1;

				PXCImage::ImageData data;
				image->AcquireAccess(PXCImage::ACCESS_READ, PXCImage::PIXEL_FORMAT_RGB32, &data);
				PXCImage::ImageInfo imgInfo = image->QueryInfo();

				switch (data.format) {
					/* STREAM_TYPE_COLOR */
				case PXCImage::PIXEL_FORMAT_YUY2: /* YUY2 image  */
				case PXCImage::PIXEL_FORMAT_NV12: /* NV12 image */
					throw(0); // Not implemented
					break;
				case PXCImage::PIXEL_FORMAT_RGB32: /* BGRA layout on a little-endian machine */
					cvDataType = CV_8UC4;
					cvDataWidth = 4;
					break;
				case PXCImage::PIXEL_FORMAT_RGB24: /* BGR layout on a little-endian machine */
					cvDataType = CV_8UC3;
					cvDataWidth = 3;
					break;
				case PXCImage::PIXEL_FORMAT_Y8:  /* 8-Bit Gray Image, or IR 8-bit */
					cvDataType = CV_8U;
					cvDataWidth = 1;
					break;

					/* STREAM_TYPE_DEPTH */
				case PXCImage::PIXEL_FORMAT_DEPTH: /* 16-bit unsigned integer with precision mm. */
				case PXCImage::PIXEL_FORMAT_DEPTH_RAW: /* 16-bit unsigned integer with device specific precision (call device->QueryDepthUnit()) */
					cvDataType = CV_16U;
					cvDataWidth = 2;
					break;
				case PXCImage::PIXEL_FORMAT_DEPTH_F32: /* 32-bit float-point with precision mm. */
					cvDataType = CV_32F;
					cvDataWidth = 4;
					break;

					/* STREAM_TYPE_IR */
				case PXCImage::PIXEL_FORMAT_Y16:          /* 16-Bit Gray Image */
					cvDataType = CV_16U;
					cvDataWidth = 2;
					break;
				case PXCImage::PIXEL_FORMAT_Y8_IR_RELATIVE:    /* Relative IR Image */
					cvDataType = CV_8U;
					cvDataWidth = 1;
					break;
				}

				// suppose that no other planes
				if (data.planes[1] != NULL) printf("Hello"); // not implemented
															 // suppose that no sub pixel padding needed
				if (data.pitches[0] % cvDataWidth != 0) printf("Hello"); // not implemented
				outImg = new Mat(imgInfo.height, data.pitches[0] / cvDataWidth, cvDataType);
				//outImg->create(imgInfo.height, data.pitches[0] / cvDataWidth, cvDataType);

				memcpy(outImg->data, data.planes[0], imgInfo.height*imgInfo.width*cvDataWidth*sizeof(pxcBYTE));

				image->ReleaseAccess(&data);


				if (outImg != NULL) {
					//write image to file
					Mat img;
					cvtColor(*outImg, img, CV_BGR2GRAY);
					bool writingSuccesful = imwrite(filename, *outImg);
					if (writingSuccesful) {
						char *str = readText(filename);
						
						BSSpeechSynthesis::OutputMessage msg;
						msg.sentence = CharToWChar(str); // not sure if this works, maybe function needed
						speechSynthesis->pushQueue(msg);
						g_stop3 = true;
				
					}

				}

			}
		}
		senseMgr->ReleaseFrame();
	}

	// Close down
	senseMgr->Release();
	controller->releaseCamera();
}





BSImageSaver::BSImageSaver()
{
}


BSImageSaver::~BSImageSaver()
{
}
