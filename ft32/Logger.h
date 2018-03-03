/*
 * Logger.h
 *
 *  Created on: Mar 1, 2018
 *      Author: joseph
 */

#ifndef LOGGER_H_
#define LOGGER_H_

class Logger {
public:
	~Logger();

	/* Static access method. */
	static Logger* getInstance();

private:
	Logger();
	static Logger* instance;

	int mBaudrate;
};

#endif /* LOGGER_H_ */
