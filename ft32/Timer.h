/*
 * Timer.h
 *
 *  Created on: Apr 03, 2019
 *      Author: joseph
 *      
 *      links: https://forum.arduino.cc/index.php?topic=83211.0
 *             https://techtutorialsx.com/2017/10/07/esp32-arduino-timer-interrupts/
 *             https://stackoverflow.com/questions/7134197/error-with-address-of-parenthesized-member-function
 *             https://esp32.com/viewtopic.php?t=5003
 *             https://stackoverflow.com/questions/13331447/static-function-call-non-static-function-in-c
 *             https://lastminuteengineers.com/handling-esp32-gpio-interrupts-tutorial/
 *             https://de.wikibooks.org/wiki/C%2B%2B-Programmierung:_Entwurfsmuster:_Singleton
 */

#ifndef Timer_H_
#define Timer_H_

#define TIMER_DEBUG

#include <Arduino.h>

class Timer {
public:
    /** singelton method to create an instance (object) of the class
     *  instance = 0: If no object is created at all, it will create one and store the pointer pointing on itself.
     *
     *  @param  ---
     *  @return If an object instance is available, it will return the pointer pointing to the existing object.
    */
    static Timer* getInstance();

    /** ...
     *
     *  @param  ---
     *  @return ---
    */
    void setCallbackFunction(void (*)());

    /** ...
     *
     *  @param  ---
     *  @return ---
    */
    bool getCallback();

    /** ...
     *
     *  @param  ---
     *  @return ---
    */
    bool start();

    /** ...
     *
     *  @param  ---
     *  @return ---
    */
    bool stop();

    /** ...
     *
     *  @param  ---
     *  @return ---
    */
    void restart(float timeoutInSeconds);

    /** ...
     *
     *  @param  ---
     *  @return ---
    */
    void setTimeout(float timeoutInSeconds);

    /** ...
     *
     *  @param  ---
     *  @return ---
    */
    float getTimeout();

    /** ...
     *
     *  @param  ---
     *  @return ---
    */
    void clearTimeoutFlag();

    /** ...
     *
     *  @param  ---
     *  @return ---
    */
    bool getTimeoutFlag();  
private: 
    float timeout;
    bool timeoutFlag;
    bool timerFlag;
    bool isRunning;

    hw_timer_t* timer = NULL;
    static Timer* instance;
    bool callbackEnabled;

    /** Default constructor
     *
     *  @param  ---
     *  @return ---
    */
    Timer();

    /** Default destructor
     *
     *  @param  ---
     *  @return ---
    */
    ~Timer();

    /** ...
     *
     *  @param  ---
     *  @return ---
    */
    void initialize();

     /** ...
     *
     *  @param  ---
     *  @return ---
    */
    static void onTimer();
    
    /** ...
     *
     *  @param  ---
     *  @return ---
    */
    void interruptServiceRoutine();

    /** ...
     *
     *  @param  ---
     *  @return ---
    */
    void interruptServiceRoutineCallback();

    /** ...
     *
     *  @param  ---
     *  @return ---
    */
    void enable();

    /** ...
     *
     *  @param  ---
     *  @return ---
    */
    void disable();

    /** nested extern crazy function call pointer
     *
     *  @param  ---
     *  @return ---
    */
    void (*callbackFunctionPtr)();
};

#endif /* Timer_H_ */
