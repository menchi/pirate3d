#include <iostream>
#include "optimize.h"

using namespace OptimizedModel;

void main()
{
	FileSystem fileSys;
	FileReader* pReader = fileSys.CreateAndOpenFile("../../Media/oildrum001.dx90.vtx");
/*
	studiohdr_t* pMDLFile = (studiohdr_t*)new byte[pReader->GetSize()];
	g_pActiveStudioHdr = pMDLFile;
	pReader->Read(pMDLFile, pReader->GetSize());
	mstudiobodyparts_t* pBodyparts = pMDLFile->pBodypart(0);
	mstudiomodel_t* pModel = pBodyparts->pModel(0);
	const mstudio_modelvertexdata_t* pVertexdata = pModel->GetVertexData();
	for (int i=0; i<pModel->numvertices; i++)
	{
		Vector* p = pVertexdata->Position(i);
		Vector* n = pVertexdata->Normal(i);
		Vector2D* texcoord = pVertexdata->Texcoord(i);
		Vector4D* tangent = pVertexdata->TangentS(i);
		std::cout<<"Position: "<<p->x<<" "<<p->y<<" "<<p->z<<std::endl;
		std::cout<<"Normal: "<<n->x<<" "<<n->y<<" "<<n->z<<std::endl;
		std::cout<<"Texcoord: "<<texcoord->x<<" "<<texcoord->y<<std::endl;
		std::cout<<"TangentS: "<<tangent->x<<" "<<tangent->y<<" "<<tangent->z<<" "<<tangent->w<<std::endl<<std::endl;
	}
*/
	FileHeader_t* pVtxFile = (FileHeader_t*)new byte[pReader->GetSize()];
	pReader->Read(pVtxFile, pReader->GetSize());
	BodyPartHeader_t* pBodypart = pVtxFile->pBodyPart(0);
	ModelHeader_t* pModel = pBodypart->pModel(0);
	ModelLODHeader_t* pLod = pModel->pLOD(0);
	MeshHeader_t* pMesh = pLod->pMesh(0);
	StripGroupHeader_t* pStripGroup = pMesh->pStripGroup(0);
	for (int i=0; i<pStripGroup->numStrips; i++)
	{
		StripHeader_t* v = pStripGroup->pStrip(i);
		std::cout<<v->numIndices<<" "<<v->indexOffset<<" "<<v->numVerts<<" "<<v->vertOffset<<" "
				 <<v->numBones<<" "<<(u16)v->flags<<std::endl;
	}

	pReader->Drop();
	delete[] pVtxFile;
}