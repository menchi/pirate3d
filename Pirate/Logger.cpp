#include "Logger.h"
#include "OS.h"
#include "IEventReceiver.h"

namespace Pirate
{

Logger::Logger(IEventReceiver* r) : m_eLogLevel(ELL_INFORMATION), m_pReceiver(r)
{
}

//! Returns the current set log level.
ELOG_LEVEL Logger::GetLogLevel()
{
	return m_eLogLevel;
}

//! Sets a new log level.	virtual void setLogLevel(ELOG_LEVEL ll);
void Logger::SetLogLevel(ELOG_LEVEL ll)
{
	m_eLogLevel = ll;
}

//! Prints out a text into the log
void Logger::Log(const c8* text, ELOG_LEVEL ll)
{
	if (ll < m_eLogLevel)
		return;

	if (m_pReceiver)
	{
		SEvent event;
		event.EventType = EET_LOG_TEXT_EVENT;
		event.LogEvent.Text = text;
		event.LogEvent.Level = ll;
		if (m_pReceiver->OnEvent(event))
			return;
	}

	Printer::Print(text);
}


//! Prints out a text into the log
void Logger::Log(const c8* text, const c8* hint, ELOG_LEVEL ll)
{
	if (ll < m_eLogLevel)
		return;

	stringc s = text;
	s += ": ";
	s += hint;
	Log (s.c_str(), ll);
}

//! Prints out a text into the log
void Logger::Log(const wchar_t* text, ELOG_LEVEL ll)
{
	if (ll < m_eLogLevel)
		return;

	stringc s = text;
	Log(s.c_str(), ll);
}


//! Prints out a text into the log
void Logger::Log(const wchar_t* text, const wchar_t* hint, ELOG_LEVEL ll)
{
	if (ll < m_eLogLevel)
		return;

	stringc s1 = text;
	stringc s2 = hint;
	Log(s1.c_str(), s2.c_str(), ll);
}

//! Sets a new event receiver
void Logger::SetReceiver(IEventReceiver* r)
{
	m_pReceiver = r;
}


} // end namespace Pirate