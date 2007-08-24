#ifndef _PIRATE_CANVAS_H_
#define _PIRATE_CANVAS_H_

#include "VideoDriverFwd.h"

class Canvas {
public:
	Canvas(VideoDriverPtr pVideoDriver) : m_pVideoDriver(pVideoDriver) {}

	VideoDriverPtr GetVideoDriver() { return m_pVideoDriver.lock(); }

	Canvas& operator << (Canvas& (*op)(Canvas&))
	{
		return (*op)(*this);
	}

	template<class T> Canvas& operator << (const T& tool)
	{
		tool.Use(m_pVideoDriver.lock());
		return *this;
	}

	template<class T> Canvas& operator << (const std::tr1::shared_ptr<T> tool)
	{
		tool->Use(m_pVideoDriver.lock());
		return *this;
	}

private:
	VideoDriverWeakPtr m_pVideoDriver;
};

Canvas& endf(Canvas& canvas);
Canvas& erase(Canvas& canvas);

#endif