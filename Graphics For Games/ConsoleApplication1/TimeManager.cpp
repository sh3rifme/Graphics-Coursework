#include "TimeManager.h"

using namespace std;
using namespace chrono;

TimeManager::TimeManager(void) {
}

TimeManager::~TimeManager(void) {
}

double TimeManager::CalcFPS() {
	static double framesPerSecond = 0.0f;
	static double startTime = GetTime();
	static double lastTime = GetTime();	
	static char strFrameRate[50] = {0};
	static double currentFPS = 0.0f;

	currentTime = GetTime();
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;
	++framesPerSecond;

	if (currentTime - startTime > 1.0f) {
		startTime = currentTime;
		currentFPS = framesPerSecond;
		framesPerSecond = 0;
	}
	return currentFPS;
}

double TimeManager::GetTime() {
	auto epoch = system_clock::now().time_since_epoch();
	auto ms = duration_cast<milliseconds>(epoch).count();
	return ms * 0.001;
}