#include "Timer.h"

/* Null, because instance will be initialized on demand. */
Timer *Timer::instance = NULL;

Timer* Timer::getInstance() {
	if (instance == NULL) {
		// If no object is created at all, it will create one and store the pointer pointing on itself.
		instance = new Timer();
	}

	// If an object instance is available, it will return the pointer pointing to the existing object.
	return instance;
}

Timer::Timer() {
	timeout = 0;
	timeoutFlag = false;
	timerFlag = false;
	isRunning = false;

	callbackEnabled = false;
}

Timer::~Timer() {

}

void Timer::initialize() {
	timer = timerBegin(0, 80, true);
	timerAttachInterrupt(timer, &onTimer, true);
	timerAlarmWrite(timer, getTimeout() * 1000000, true);
	enable();
}

void Timer::setCallbackFunction(void (*function)()) {
	callbackEnabled = true;
	callbackFunctionPtr = function;
}

bool Timer::getCallback() {
	return callbackEnabled;
}

void Timer::onTimer() {
	Serial.println("[timer] isr startet at " + String(millis()));
	Serial.println("[timer] isr running on core " + String(xPortGetCoreID()));
	Timer::getInstance()->interruptServiceRoutine();
	Serial.println("[timer] isr ended at " + String(millis()));
}

void Timer::interruptServiceRoutine() {

	if (!getCallback()) {
		if (timeoutFlag == true) {
#ifdef TIMER_DEBUG
			Serial.println(
					"[timer] Warning: Resetting timeout flag is required!");
#endif
		} else {
			timeoutFlag = true;
		}

#ifdef TIMER_DEBUG
		Serial.println("[timer] callback function call not enabled.");
		Serial.print("[timer] Interrupt event called. Timer flag: ");
		Serial.println(timerFlag);
#endif

		timerFlag = !timerFlag;
	} else {
		interruptServiceRoutineCallback();
	}
}

void Timer::interruptServiceRoutineCallback() {
	// call extern function
	(*callbackFunctionPtr)();
}

void Timer::enable() {
	timerAlarmEnable(timer);
}

void Timer::disable() {
	timerAlarmDisable(timer);
}

bool Timer::start() {
	if (timeout == 0 || isRunning == true) {
#ifdef TIMER_DEBUG
		Serial.println(
				"[timer] Error: timeout parameter is 0 or timer already running!");
#endif

		return false;
	}
	timeoutFlag = false;
	timerFlag = false;
	isRunning = true;

	initialize();

#ifdef TIMER_DEBUG
	Serial.println("[timer] Timer started.");
#endif

	return true;
}

bool Timer::stop() {
	disable();

	timeout = 0;
	timeoutFlag = false;
	timerFlag = false;
	isRunning = false;

#ifdef TIMER_DEBUG
	Serial.println("[timer] Resetting flags...");
	Serial.println("[timer] Timer stopped.");
#endif

	return true;
}

void Timer::restart(float timeoutInSeconds) {
#ifdef TIMER_DEBUG
	Serial.println("[timer] Restart timer...");
#endif
	stop();

	setTimeout(timeoutInSeconds);
	start();
}

void Timer::setTimeout(float timeoutInSeconds) {
#ifdef TIMER_DEBUG
	Serial.print("[timer] timeout parameter will be set to ");
	Serial.print(timeoutInSeconds);
	Serial.println(" seconds.");
#endif

	timeout = timeoutInSeconds;
}

float Timer::getTimeout() {
	return timeout;
}

void Timer::clearTimeoutFlag() {
	timeoutFlag = false;

#ifdef TIMER_DEBUG
	Serial.println("[timer] Resetting timeout flag...");
#endif
}

bool Timer::getTimeoutFlag() {
	return timeoutFlag;
}
