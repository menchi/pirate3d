#include "RenderableObject.h"
#include "MeshBuffer.h"
#include "Material.h"
#include "VideoDriver.h"

//-----------------------------------------------------------------------------
RenderableObject::RenderableObject(MeshBufferPtr* ppMeshBuffers, MaterialPtr* ppMaterials, unsigned int NumMeshBuffer)
{
	m_MeshBuffers.resize(NumMeshBuffer);
	m_Materials.resize(NumMeshBuffer);
	for (unsigned int i=0; i<NumMeshBuffer; ++i)
	{
		m_MeshBuffers[i] = ppMeshBuffers[i];
		m_Materials[i] = ppMaterials[i];
	}
}
//-----------------------------------------------------------------------------
void RenderableObject::Use(VideoDriverPtr pDriver) const
{
	pDriver->Begin();

	const unsigned int n = (unsigned int)m_MeshBuffers.size();
	for (unsigned int i=0; i<n; ++i)
	{
		MeshBufferPtr pMeshBuffer = m_MeshBuffers[i];
		pDriver->SetShaderProgram(m_Materials[i]->GetShaderProgram());
		pDriver->SetVertexDeclaration(pMeshBuffer->GetVertexDeclaration());
		for (unsigned int j=0; j<pMeshBuffer->GetNumVertexBuffers(); j++)
		{
			const MeshBuffer::StreamIndexArray& StreamNumbers = pMeshBuffer->GetStreamIndices();
			const MeshBuffer::DriverVertexBufferArray& DriverVertexBuffers = pMeshBuffer->GetDriverVertexBuffers();
			const MeshBuffer::VertexBufferArray& VertexBuffers = pMeshBuffer->GetVertexBuffers();
			pDriver->SetVertexStream(StreamNumbers[j], DriverVertexBuffers[j], VertexBuffers[j]->GetVertexSize());
			pDriver->DrawIndexedTriangleList(pMeshBuffer->GetDriverIndexBuffer(), VertexBuffers[j]->GetNumVertices(), 
				pMeshBuffer->GetIndexBuffer()->GetNumIndices()/3);
		}
	}

	pDriver->End();
}
//-----------------------------------------------------------------------------