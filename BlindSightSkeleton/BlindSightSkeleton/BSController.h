#include "BSObjectTracker.h"
#include "BSSpeechRecognizer.h"
#include "BSSpeechSythesis.h"
#include "BSState.h"

class BSController
{
public:
	BSController();
	~BSController();
	void initialize();
	void startBlindSight();
	void stopBlindSight();

private:
	BSState state;
};
