#ifndef _OS_OPERATOR_H_INCLUDED_
#define _OS_OPERATOR_H_INCLUDED_

#include "pirateString.h"
#include "RefObject.h"

namespace Pirate
{

//! The Operating system operator provides operation system specific methods and informations.
class OSOperator : public virtual RefObject
{
public:
	// constructor
	OSOperator(const c8* osversion);

	//! destructor
	~OSOperator();

	//! returns the current operation system version as string.
	const wchar_t* GetOperationSystemVersion();

	//! gets the processor speed in megahertz
	//! \param Mhz:
	//! \return Returns true if successful, false if not
	BOOL GetProcessorSpeedMHz(u32* MHz);

	//! gets the total and available system RAM in kB
	//! \param Total: will contain the total system memory
	//! \param Avail: will contain the available memory
	//! \return Returns true if successful, false if not
	BOOL GetSystemMemory(u32* Total, u32* Avail);

private:
	stringw OperationSystem;
};

} // end namespace

#endif