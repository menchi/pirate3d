#include "D3D9Driver.h"
#include "SMesh.h"
#include "MDLHelper.h"
#include "MDLFileLoader.h"

using namespace OptimizedModel;

namespace Pirate
{

D3DVERTEXELEMENT9 MdlVertexDecl[] = 
{
	{0,  0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,	 0},
	{0, 24,  D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	D3DDECL_END()
};

struct MDL_VT
{
	float Position[3];
	float Normal[3];
	float Texcoord[2];
	MDL_VT(float x, float y, float z, float nx, float ny, float nz, float u, float v)
	{
		Position[0] = x; Position[1] = y; Position[2] = z;
		Normal[0] = nx; Normal[1] = ny; Normal[2] = nz;
		Texcoord[0] = u; Texcoord[1] = v;
	}
};

//! Constructor
MDLFileLoader::MDLFileLoader(FileSystem* fs, D3D9Driver* driver)
	: m_pFileSystem(fs), m_pDriver(driver)
{
	if (m_pFileSystem)
		m_pFileSystem->Grab();

	if (m_pDriver)
		m_pDriver->Grab();
}



//! destructor
MDLFileLoader::~MDLFileLoader()
{
	if (m_pFileSystem)
		m_pFileSystem->Drop();

	if (m_pDriver)
		m_pDriver->Drop();
}

//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".bsp")
BOOL MDLFileLoader::IsALoadableFileExtension(const c8* filename)
{
	return strstr(filename, ".mdl")!=0;
}

//! creates/loads an animated mesh from the file.
//! \return Pointer to the created mesh. Returns 0 if loading failed.
//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
//! See IUnknown::drop() for more information.
SMesh* MDLFileLoader::CreateMesh(FileReader* file)
{
	studiohdr_t* pMDLFile = (studiohdr_t*)new byte[file->GetSize()];
	g_pActiveStudioHdr = pMDLFile;
	file->Read(pMDLFile, file->GetSize());
	mstudiobodyparts_t* pBodyparts = pMDLFile->pBodypart(0);
	mstudiomodel_t* pModel = pBodyparts->pModel(0);
	const mstudio_modelvertexdata_t* pVertexdata = pModel->GetVertexData();

	stringc vtxFile = file->GetFileName();
	vtxFile = vtxFile.subString(0, vtxFile.findLast('.') + 1);
	vtxFile += "dx90.vtx";
	file = m_pFileSystem->CreateAndOpenFile(vtxFile.c_str());

	FileHeader_t* pVtxFile = (FileHeader_t*)new byte[file->GetSize()];
	file->Read(pVtxFile, file->GetSize());
	BodyPartHeader_t* pBodypart = pVtxFile->pBodyPart(0);
	ModelHeader_t* pVtxModel = pBodypart->pModel(0);
	ModelLODHeader_t* pLod = pVtxModel->pLOD(0);

	SMesh* pPirateMesh = new SMesh();
	IDirect3DVertexBuffer9* pVertexBuffer = NULL;

	for (int i=0; i<pLod->numMeshes; i++)
	{
		MeshHeader_t* pMesh = pLod->pMesh(i);
		StripGroupHeader_t* pStripGroup = pMesh->pStripGroup(0);
//		StripHeader_t* pStrip = pStripGroup->pStrip(0);

		SD3D9MeshBuffer* pMeshBuffer = new SD3D9MeshBuffer(m_pDriver, MdlVertexDecl);
		if (i==0)
		{
			pMeshBuffer->CreateVertexBuffer(0, sizeof(MDL_VT), pModel->numvertices, D3DUSAGE_WRITEONLY);
			pVertexBuffer = pMeshBuffer->GetVertexBuffer(0);
			MDL_VT* pVert = NULL;

			pVertexBuffer->Lock(0, 0, (void**)&pVert, 0);
			for (int j=0; j<pModel->numvertices; j++)
			{
				const Vector* p = pVertexdata->Position(j);
				const Vector* n = pVertexdata->Normal(j);
				const Vector2D* t = pVertexdata->Texcoord(j);
				pVert[j].Position[0] = p->x; pVert[j].Position[1] = p->y; pVert[j].Position[2] = p->z;
				pVert[j].Normal[0] = n->x; pVert[j].Normal[1] = n->y; pVert[j].Normal[2] = n->z;
				pVert[j].Texcoord[0] = t->x; pVert[j].Texcoord[1] = t->y;
			}
			pVertexBuffer->Unlock();
		}
		else
		{
			pMeshBuffer->SetVertexBuffer(0, pModel->numvertices, pVertexBuffer);
		}

		mstudiomesh_t* pStudioMesh = pModel->pMesh(i);

		pMeshBuffer->CreateIndexBuffer(pStripGroup->numIndices, D3DUSAGE_WRITEONLY, TRUE);
		u16* pIndices = NULL;

		pMeshBuffer->GetIndexBuffer()->Lock(0, 0, (void**)&pIndices, 0);
		for (int j=0; j<pStripGroup->numIndices; j++)
		{
			pIndices[j] = pStripGroup->pVertex(*(pStripGroup->pIndex(j)))->origMeshVertID + pStudioMesh->vertexoffset;
		}
		pMeshBuffer->GetIndexBuffer()->Unlock();


		mstudiotexture_t* pTexture =  pMDLFile->pTexture(i);
		stringc textureName = file->GetFileName();
		textureName = textureName.subString(0, textureName.findLast('/') + 1);
		textureName = textureName + pTexture->pszName() + ".tga";
		D3D9Texture* pD3DTexture = m_pDriver->GetTexture(textureName.c_str());
		pMeshBuffer->m_Material.Textures[0] = pD3DTexture;

		pPirateMesh->AddMeshBuffer(pMeshBuffer);
		pMeshBuffer->Drop();
	}

	delete[] pVtxFile;
	if (pMDLFile->pVertexBase)
		delete pMDLFile->pVertexBase;
	delete[] pMDLFile;

	file->Drop();

	return pPirateMesh;
}

}