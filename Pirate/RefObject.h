#ifndef _PIRATE_REF_OBJECT_H_
#define _PIRATE_REF_OBJECT_H_

#include "pirateTypes.h"

namespace Pirate
{

class RefObject
{
public:
	RefObject() : m_iReferenceCounter(1), m_pDebugName(NULL)
	{
	}

	virtual ~RefObject()
	{
	}

	void Grab() { ++m_iReferenceCounter; }

	BOOL Drop()
	{
		_PIRATE_DEBUG_BREAK_IF(m_iReferenceCounter <= 0) // someone is doing bad reference counting.

		--m_iReferenceCounter;
		if (!m_iReferenceCounter)
		{
			delete this;
			return TRUE;
		}

		return FALSE;
	}

	const c8* GetDebugName() const
	{
		return m_pDebugName;
	} 

protected:
	void SetDebugName(const c8* newName)
	{
		m_pDebugName = newName;
	}

private:
	s32	m_iReferenceCounter;
	const c8* m_pDebugName;
};

}

#endif