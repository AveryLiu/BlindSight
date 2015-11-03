BlindSight
==========

How to contribute features to BlindSight
----------------------------------------

#. Open the project.
#. Open class wizard and create a new class, by doing so you've got a header file(.h) and a source code file(.cpp) representing that class.
#. Declare the interfaces(functions that you want others to use) in header file and define them in source code file.
#. Complete your feature code in respective functions and if you want to voice over the message, first be sure to include **BSController.h** and **BSSpeechSynthesis.h** in the .cpp file (Double check you don't make it wrong in .h file, this will raise a linker error due to mutual including header files.) and then follow the demo code below
 
    .. code-block:: cpp
   
            // Get the singleton instance of BSController
            BSController* controller = BSController::getInstance();
	    // Get speechSynthesis object which is attached to BSController
	    BSSpeechSynthesis* speechSynthesis = controller->speechSynthesis;

	    // Your feature code here
	    ...
	    
	    // Construct the message to be pushed 
	    BSSpeechSynthesis::OutputMessage msg;
	    msg.sentence = L"Whatever you want the computer to speak.";
	    
	    // Push the constructed message to output queue of speech synthesis
	    speechSynthesis->pushQueue(msg);

	    // Ok, the voice over work is done and that is a non-blocking operation
	    // if you have other work to complete, do them below.
	    ...

#. Invoke your feature function

   Up to now, we assume you've already implemented the feature code in seperated class files, let's say **BSNewFeature.h** and **BSNewFeature.cpp**. In **BSController.h**, include **BSNewFeature.h** and then declare a public attribute like this

   .. code-block:: cpp
       
       class BSController
       {
       public:
           // Original code 
	   ...
	   ...

	   // Your newly declared attribute below
	   BSNewFeature* newFeature;

	   // Note that the line above only declares a pointer to that object
	   // It does not do any initialization work

	   // Other original code below
	   ...
       } 
   Then initialize that object in **BSController.cpp**

   .. code-block:: cpp

       // You shall init in following function
       BSController::BSController()
       {
           // Original code
	   ...
	   ...

	   // Your init code
	   newFeature = new BSNewFeature();
       }

   You've completed all the initialization work so far. To invoke your feature code, add the new commands in **BSController.cpp** 

   .. code-block:: cpp

       void BSController::initializeAudio()
       {
          ...

	  pxcCHAR *cmds[4] = {L'Where is my key', L'Stop', L'What is the weather', L'New command'};
	  // Don't forget to increase the array size
       }

   and include **BSNewFeature.h** in **BSSpeechRecognitionHandler.cpp** and then

   .. code-block:: cpp

       // Using the BSController to get your new feature object
       BSNewFeature* newFeature = controller->newFeature;
       swich(data->scores->label)
       {
       case 0:
           ...
       case 1:
           ...
       case 2:
           ...
       case 3:
           newFeature->doWork();  // doWork is the feature interface you declared and implemented before
	   break;
       }
