#ifndef _PIRATE_RENDERABLE_OBJECT_H_
#define _PIRATE_RENDERABLE_OBJECT_H_

#include "VideoDriverFwd.h"
#include <vector>

FWD_DECLARE(Material)
FWD_DECLARE(MeshBuffer)

FWD_DECLARE(RenderableObject)

class RenderableObject {
public:
	unsigned int GetNumMeshBuffers() { return (unsigned int)m_MeshBuffers.size(); }
	MeshBufferPtr GetMeshBuffer(unsigned int i) { return m_MeshBuffers[i]; }
	MaterialPtr GetMaterial(unsigned int i) { return m_Materials[i]; }

	void Use(VideoDriverPtr pDriver) const;

	static RenderableObjectPtr Create(MeshBufferPtr* ppMeshBuffers, MaterialPtr* ppMaterials, unsigned int NumMeshBuffer)
	{
		return RenderableObjectPtr(new RenderableObject(ppMeshBuffers, ppMaterials, NumMeshBuffer));
	}

private:
	RenderableObject(MeshBufferPtr* ppMeshBuffers, MaterialPtr* ppMaterials, unsigned int NumMeshBuffer);

	std::vector<MeshBufferPtr> m_MeshBuffers;
	std::vector<MaterialPtr> m_Materials;
};

#endif