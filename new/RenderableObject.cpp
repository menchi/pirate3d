#include "RenderableObject.h"

RenderableObject::RenderableObject(MeshBufferPtr* ppMeshBuffers, MaterialPtr* ppMaterials, unsigned int NumMeshBuffer)
{
	m_MeshBuffers.resize(NumMeshBuffer);
	for (unsigned int i=0; i<NumMeshBuffer; ++i)
	{
		m_MeshBuffers[i] = ppMeshBuffers[i];
		m_Materials[i] = ppMaterials[i];
	}
}

void RenderableObject::Use(VideoDriverPtr pDriver) const
{

}