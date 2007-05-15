#ifndef _PIRATE_LOGGER_H_
#define _PIRATE_LOGGER_H_

#include "RefObject.h"
#include "pirateString.h"

namespace Pirate
{

enum ELOG_LEVEL
{
	//! High log level, warnings, errors and important information
	//! texts are printed out.
	ELL_INFORMATION = 0,

	//! Default log level, warnings and errors are printed out
	ELL_WARNING,

	//! Low log level, only errors are printed into the log
	ELL_ERROR,

	//! Nothing is printed out to the log
	ELL_NONE
};

class IEventReceiver;

//! Interface for logging messages, warnings and errors
class Logger : public virtual RefObject
{
public:

	Logger(IEventReceiver* r);

	//! Returns the current set log level.
	ELOG_LEVEL GetLogLevel();

	//! Sets a new log level.	virtual void setLogLevel(ELOG_LEVEL ll);
	void SetLogLevel(ELOG_LEVEL ll);

	//! Prints out a text into the log
	void Log(const c8* text, ELOG_LEVEL ll=ELL_INFORMATION);

	//! Prints out a text into the log
	void Log(const wchar_t* text, ELOG_LEVEL ll=ELL_INFORMATION);

	//! Prints out a text into the log
	void Log(const c8* text, const c8* hint, ELOG_LEVEL ll=ELL_INFORMATION);

	//! Prints out a text into the log
	void Log(const wchar_t* text, const wchar_t* hint, ELOG_LEVEL ll=ELL_INFORMATION);

	//! Sets a new event receiver
	void SetReceiver(IEventReceiver* r);

private:

	ELOG_LEVEL m_eLogLevel;
	IEventReceiver* m_pReceiver;
};

} // end namespace

#endif