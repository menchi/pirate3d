#include "RenderableObject.h"

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

void RenderableObject::Use(VideoDriverPtr pDriver) const
{
	const unsigned int n = (unsigned int)m_MeshBuffers.size();
	for (unsigned int i=0; i<n; ++i)
	{
		pDriver->SetShaderProgram(m_Materials[i]->GetShaderProgram());
		pDriver->SetVertexDeclaration(m_MeshBuffers[i]->GetVertexDeclaration());
		MeshBufferPtr pMeshBuffer = m_MeshBuffers[i];
		unsigned int StreamNumber = pMeshBuffer->GetDriverVertexBuffer(i).first;
		DriverVertexBufferPtr pDriverVB = pMeshBuffer->GetDriverVertexBuffer(i).second;
		unsigned int Stride = pMeshBuffer->GetVertexBuffer(i).second->GetVertexSize();
		pDriver->SetVertexStream(StreamNumber, pDriverVB, Stride);
		unsigned int NumVertices = pMeshBuffer->GetVertexBuffer(i).second->GetNumVertices();
		unsigned int NumIndices = pMeshBuffer->GetIndexBuffer()->GetNumIndices();
		pDriver->DrawIndexedTriangleList(m_MeshBuffers[i]->GetDriverIndexBuffer(), NumVertices, NumIndices/3);
	}
}