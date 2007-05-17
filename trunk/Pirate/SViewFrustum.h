#ifndef _PIRATE_S_VIEW_FRUSTUM_H_
#define _PIRATE_S_VIEW_FRUSTUM_H_

#include "aabbox3d.h"
#include "matrix4.h"

namespace Pirate
{

//! Defines the view frustum. Thats the space viewed by the camera.
/** The view frustum is enclosed by 6 planes. These six planes share
four points. A bounding box around these four points is also stored in
this structure.
*/
struct SViewFrustum
{
	enum VFPLANES
	{
		//! Far plane of the frustum. Thats the plane farest away from the eye.
		VF_FAR_PLANE = 0,
		//! Near plane of the frustum. Thats the plane nearest to the eye.
		VF_NEAR_PLANE,
		//! Left plane of the frustum.
		VF_LEFT_PLANE,
		//! Right plane of the frustum.
		VF_RIGHT_PLANE,
		//! Bottom plane of the frustum.
		VF_BOTTOM_PLANE,
		//! Top plane of the frustum.
		VF_TOP_PLANE,

		//! Amount of planes enclosing the view frustum. Should be 6.
		VF_PLANE_COUNT
	};

	//! Default Constructor
	SViewFrustum() {};

	//! This constructor creates a view frustum based on a projection and/or
	//! view matrix.
	SViewFrustum(const matrix4& mat);

	//! This constructor creates a view frustum based on a projection
	//! and/or view matrix.
	void SetFrom(const matrix4& mat);

	//! the position of the camera
	vector3df m_CameraPosition;

	//! all planes enclosing the view frustum.
	plane3d<f32> planes[VF_PLANE_COUNT];


	//! transforms the frustum by the matrix
	//! \param mat: Matrix by which the view frustum is transformed.
	void Transform(const matrix4 &mat);

	//! returns the point which is on the far left upper corner inside the
	//! the view frustum.
	vector3df GetFarLeftUp() const;

	//! returns the point which is on the far left bottom corner inside the
	//! the view frustum.
	vector3df GetFarLeftDown() const;

	//! returns the point which is on the far right top corner inside the
	//! the view frustum.
	vector3df GetFarRightUp() const;

	//! returns the point which is on the far right bottom corner inside the
	//! the view frustum.
	vector3df GetFarRightDown() const;

	//! returns a bounding box enclosing the whole view frustum
	const aabbox3d<f32> &GetBoundingBox() const;

	//! recalculates the bounding box member based on the planes
	inline void RecalculateBoundingBox();

	//! bouding box around the view frustum
	aabbox3d<f32> m_BoundingBox;

	//! Hold a copy of important transform matrices
	enum E_TRANSFORMATION_STATE
	{
		//! View transformation
		ETS_VIEW = 0,
		//! Projection transformation
		ETS_PROJECTION,

		ETS_COUNT
	};

	matrix4 Matrices[ETS_COUNT];
};


//! transforms the frustum by the matrix
//! \param Matrix by which the view frustum is transformed.
inline void SViewFrustum::Transform(const matrix4 &mat)
{
	for (int i=0; i<VF_PLANE_COUNT; ++i)
		mat.transformPlane(planes[i]);

	mat.transformVect(m_CameraPosition);
	RecalculateBoundingBox();
}


//! returns the point which is on the far left upper corner inside the
//! the view frustum.
inline vector3df SViewFrustum::GetFarLeftUp() const
{
	vector3df p;
	planes[SViewFrustum::VF_FAR_PLANE].getIntersectionWithPlanes(
		planes[SViewFrustum::VF_TOP_PLANE],
		planes[SViewFrustum::VF_LEFT_PLANE], p);

	return p;
}

//! returns the point which is on the far left bottom corner inside the
//! the view frustum.
inline vector3df SViewFrustum::GetFarLeftDown() const
{
	vector3df p;
	planes[SViewFrustum::VF_FAR_PLANE].getIntersectionWithPlanes(
		planes[SViewFrustum::VF_BOTTOM_PLANE],
		planes[SViewFrustum::VF_LEFT_PLANE], p);

	return p;
}

//! returns the point which is on the far right top corner inside the
//! the view frustum.
inline vector3df SViewFrustum::GetFarRightUp() const
{
	vector3df p;
	planes[SViewFrustum::VF_FAR_PLANE].getIntersectionWithPlanes(
		planes[SViewFrustum::VF_TOP_PLANE],
		planes[SViewFrustum::VF_RIGHT_PLANE], p);

	return p;
}

//! returns the point which is on the far right bottom corner inside the
//! the view frustum.
inline vector3df SViewFrustum::GetFarRightDown() const
{
	vector3df p;
	planes[SViewFrustum::VF_FAR_PLANE].getIntersectionWithPlanes(
		planes[SViewFrustum::VF_BOTTOM_PLANE],
		planes[SViewFrustum::VF_RIGHT_PLANE], p);

	return p;
}

//! returns a bounding box enclosing the whole view frustum
inline const aabbox3d<f32> &SViewFrustum::GetBoundingBox() const
{
	return m_BoundingBox;
}

//! recalculates the bounding box member based on the planes
inline void SViewFrustum::RecalculateBoundingBox()
{
	m_BoundingBox.reset ( m_CameraPosition );

	m_BoundingBox.addInternalPoint(GetFarLeftUp());
	m_BoundingBox.addInternalPoint(GetFarRightUp());
	m_BoundingBox.addInternalPoint(GetFarLeftDown());
	m_BoundingBox.addInternalPoint(GetFarRightDown());
}

//! This constructor creates a view frustum based on a projection
//! and/or view matrix.
inline SViewFrustum::SViewFrustum(const matrix4& mat)
{
	SetFrom ( mat );
}


//! This constructor creates a view frustum based on a projection
//! and/or view matrix.
inline void SViewFrustum::SetFrom(const matrix4& mat)
{
	// left clipping plane
	planes[VF_LEFT_PLANE].Normal.X	= mat[3 ] + mat[0];
	planes[VF_LEFT_PLANE].Normal.Y	= mat[7 ] + mat[4];
	planes[VF_LEFT_PLANE].Normal.Z	= mat[11] + mat[8];
	planes[VF_LEFT_PLANE].D		= mat[15] + mat[12];

	// right clipping plane
	planes[VF_RIGHT_PLANE].Normal.X = mat[3 ] - mat[0];
	planes[VF_RIGHT_PLANE].Normal.Y = mat[7 ] - mat[4];
	planes[VF_RIGHT_PLANE].Normal.Z = mat[11] - mat[8];
	planes[VF_RIGHT_PLANE].D =        mat[15] - mat[12];

	// top clipping plane
	planes[VF_TOP_PLANE].Normal.X = mat[3 ] - mat[1];
	planes[VF_TOP_PLANE].Normal.Y = mat[7 ] - mat[5];
	planes[VF_TOP_PLANE].Normal.Z = mat[11] - mat[9];
	planes[VF_TOP_PLANE].D =        mat[15] - mat[13];

	// bottom clipping plane
	planes[VF_BOTTOM_PLANE].Normal.X = mat[3 ] + mat[1];
	planes[VF_BOTTOM_PLANE].Normal.Y = mat[7 ] + mat[5];
	planes[VF_BOTTOM_PLANE].Normal.Z = mat[11] + mat[9];
	planes[VF_BOTTOM_PLANE].D =        mat[15] + mat[13];

	// far clipping plane
	planes[VF_FAR_PLANE].Normal.X = mat[3 ] - mat[2];
	planes[VF_FAR_PLANE].Normal.Y = mat[7 ] - mat[6];
	planes[VF_FAR_PLANE].Normal.Z = mat[11] - mat[10];
	planes[VF_FAR_PLANE].D =        mat[15] - mat[14];

	// near clipping plane
	planes[VF_NEAR_PLANE].Normal.X = mat[2];
	planes[VF_NEAR_PLANE].Normal.Y = mat[6];
	planes[VF_NEAR_PLANE].Normal.Z = mat[10];
	planes[VF_NEAR_PLANE].D =        mat[14];


	// normalize normals
	u32 i;
	for ( i=0; i != 6; ++i)
	{
		const f32 len = - reciprocal_squareroot ( planes[i].Normal.getLengthSQ() );
		planes[i].Normal *= len;
		planes[i].D *= len;
	}

	// make bounding box
	RecalculateBoundingBox();
}


} // end namespace

#endif