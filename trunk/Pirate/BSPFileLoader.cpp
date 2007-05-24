#include "D3D9Driver.h"
#include "SMesh.h"
#include "FileSystem.h"
#include "OS.h"
#include "BSPHelper.h"
#include "BSPFileLoader.h"

namespace Pirate
{

D3DVERTEXELEMENT9 BspVertexDecl[] = 
{
	{0,  0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12,  D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	{0, 20,  D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
	D3DDECL_END()
};

struct BSP_VT
{
	float pos[3];
	float uv[2];
	float lightuv[2];
	BSP_VT(float x, float y, float z, float u, float v, float lu, float lv)
	{
		pos[0] = x; pos[1] = y; pos[2] = z;
		uv[0] = u; uv[1] = v;
		lightuv[0] = lu; lightuv[1] = lv;
	}
};

//! Constructor
BspFileLoader::BspFileLoader(FileSystem* fs, D3D9Driver* driver)
: m_pFileSystem(fs), m_pDriver(driver)
{
	if (m_pFileSystem)
		m_pFileSystem->Grab();

	if (m_pDriver)
		m_pDriver->Grab();
}



//! destructor
BspFileLoader::~BspFileLoader()
{
	if (m_pFileSystem)
		m_pFileSystem->Drop();

	if (m_pDriver)
		m_pDriver->Drop();
}

//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".bsp")
BOOL BspFileLoader::IsALoadableFileExtension(const c8* filename)
{
	return strstr(filename, ".bsp")!=0;
}

//! creates/loads an animated mesh from the file.
//! \return Pointer to the created mesh. Returns 0 if loading failed.
//! If you no longer need the mesh, you should call IAnimatedMesh::drop().
//! See IUnknown::drop() for more information.
SMesh* BspFileLoader::CreateMesh(FileReader* file)
{
	dheader_t header;
	dvertex_t* pVertices;
	dedge_t* pEdges;
	s32* pSurfEdges;
	dface_t* pFaces;
	texinfo_t* pTexinfos;
	dtexdata_t* pTexData;
	ColorRGBExp32* pLightmaps;
	c8* pTexStringData;
	s32* pTexStringTable;

	file->Read(&header, sizeof(dheader_t));
	char* pIdentString = (char*)&header.ident;
	//	std::cout<<pIdentString<<" "<<header.version<<" "<<header.mapRevision<<std::endl;
	stringc idString(pIdentString);
	if (!idString.equalsn("VBSP", 4) || header.version<20)
	{
		Printer::Log("Not an available HL2 bsp file format", ELL_WARNING);
		return NULL;
	}

	file->Seek(header.lumps[LUMP_VERTEXES].fileofs);
	pVertices = (dvertex_t*)malloc(header.lumps[LUMP_VERTEXES].filelen);
	file->Read(pVertices, header.lumps[LUMP_VERTEXES].filelen);

	file->Seek(header.lumps[LUMP_EDGES].fileofs);
	pEdges = (dedge_t*)malloc(header.lumps[LUMP_EDGES].filelen);
	file->Read(pEdges, header.lumps[LUMP_EDGES].filelen);

	file->Seek(header.lumps[LUMP_SURFEDGES].fileofs);
	pSurfEdges = (s32*)malloc(header.lumps[LUMP_SURFEDGES].filelen);
	file->Read(pSurfEdges, header.lumps[LUMP_SURFEDGES].filelen);

	file->Seek(header.lumps[LUMP_FACES].fileofs);
	pFaces = (dface_t*)malloc(header.lumps[LUMP_FACES].filelen);
	file->Read(pFaces, header.lumps[LUMP_FACES].filelen);

	file->Seek(header.lumps[LUMP_TEXINFO].fileofs);
	pTexinfos = (texinfo_t*)malloc(header.lumps[LUMP_TEXINFO].filelen);
	file->Read(pTexinfos, header.lumps[LUMP_TEXINFO].filelen);

	file->Seek((header.lumps[LUMP_TEXDATA]).fileofs);
	pTexData = (dtexdata_t*)malloc(header.lumps[LUMP_TEXDATA].filelen);
	file->Read(pTexData, header.lumps[LUMP_TEXDATA].filelen);

	file->Seek((header.lumps[LUMP_LIGHTING]).fileofs);
	pLightmaps = (ColorRGBExp32*)malloc(header.lumps[LUMP_LIGHTING].filelen);
	file->Read(pLightmaps, header.lumps[LUMP_LIGHTING].filelen);

	file->Seek((header.lumps[LUMP_TEXDATA_STRING_DATA]).fileofs);
	pTexStringData = (c8*)malloc(header.lumps[LUMP_TEXDATA_STRING_DATA].filelen);
	file->Read(pTexStringData, header.lumps[LUMP_TEXDATA_STRING_DATA].filelen);

	file->Seek((header.lumps[LUMP_TEXDATA_STRING_TABLE]).fileofs);
	pTexStringTable = (s32*)malloc(header.lumps[LUMP_TEXDATA_STRING_TABLE].filelen);
	file->Read(pTexStringTable, header.lumps[LUMP_TEXDATA_STRING_TABLE].filelen);

	file->Seek((header.lumps[LUMP_ENTITIES]).fileofs);
	char* pEntities = (char*)malloc(header.lumps[LUMP_ENTITIES].filelen);
	file->Read(pEntities, header.lumps[LUMP_ENTITIES].filelen);

	file->Seek((header.lumps[LUMP_GAME_LUMP]).fileofs);
	dgamelumpheader_t* pGameLumpHdr = (dgamelumpheader_t*)malloc(header.lumps[LUMP_GAME_LUMP].filelen);
	file->Read(pGameLumpHdr, header.lumps[LUMP_GAME_LUMP].filelen);

	dgamelump_t* pGameLumps = (dgamelump_t*)(pGameLumpHdr + 1);

	for (int i=0; i<pGameLumpHdr->lumpCount; i++)
	{
		if (pGameLumps[i].id == 'sprp')
		{
			int* dictEntries = (int*)((c8*)pGameLumpHdr + (pGameLumps[i].fileofs - header.lumps[LUMP_GAME_LUMP].fileofs));
			StaticPropDictLump_t* pStaticPropDict = (StaticPropDictLump_t*)(dictEntries + 1);
			int* leafEntries = (int*)(pStaticPropDict + *dictEntries);
			StaticPropLeafLump_t* pStaticPropLeaf = (StaticPropLeafLump_t*)(leafEntries + 1);
			int* lumpEntries = (int*)(pStaticPropLeaf + *leafEntries);
			StaticPropLump_t* pStaticPropLumps = (StaticPropLump_t*)(lumpEntries + 1);
			for (int j=0; j<*lumpEntries; j++)
			{
				EntityInfo eInfo;
				eInfo.ModelName = pStaticPropDict[pStaticPropLumps->m_PropType].m_Name;
				eInfo.Origin = vector3df(pStaticPropLumps->m_Origin._v[0], pStaticPropLumps->m_Origin._v[1], pStaticPropLumps->m_Origin._v[2]);
				eInfo.Angles = vector3df(pStaticPropLumps->m_Angles.z, pStaticPropLumps->m_Angles.x, pStaticPropLumps->m_Angles.y);
				m_Entities.push_back(eInfo);
				pStaticPropLumps++;
			}
		}
	}

	stringc entString = pEntities;
	s32 seeker = 0;
	s32 start = entString.findNext('{', seeker);
	while (start != -1)
	{
		s32 end = entString.findNext('}', start + 1);
		seeker = end + 1;
		stringc entity = entString.subString(start, end - start + 1);

		if (!ParseModels(entity)) 
			ParsePlayerStartInfo(entity);

		start = entString.findNext('{', seeker);
	}

	s32 vertexCount = header.lumps[LUMP_VERTEXES].filelen / sizeof(dvertex_t);
	s32 faceCount = header.lumps[LUMP_FACES].filelen / sizeof(dface_t);
	array<BSP_VT> tmpVertices;
	array<array<u32>> tmpIndices(faceCount);
	for (int j=0; j<faceCount; j++)
	{
		s32 surfedgeStart = pFaces[j].firstedge;
		s32 surfedgeEnd = surfedgeStart + pFaces[j].numedges - 1;
		s16 texinfo = pFaces[j].texinfo;
		s32 texW = pTexData[pTexinfos[texinfo].texdata].width;
		s32 texH = pTexData[pTexinfos[texinfo].texdata].height;
		s32 lightmapW = pFaces[j].m_LightmapTextureSizeInLuxels[0];
		s32 lightmapH = pFaces[j].m_LightmapTextureSizeInLuxels[1];
		array<u32> faceIndices((pFaces[j].numedges-2)*3);
		for (s32 i=surfedgeStart; i<=surfedgeEnd; i++)
		{
			s16 edge = pSurfEdges[i];
			s16 v1, v2;
			if (edge<0)
			{
				edge = -edge;
				v1 = pEdges[edge].v[1];
				v2 = pEdges[edge].v[0];
			}
			else
			{
				v1 = pEdges[edge].v[0];
				v2 = pEdges[edge].v[1];
			}

			_PIRATE_DEBUG_BREAK_IF(v1<0 || v1>=vertexCount || v2<0 || v2>=vertexCount)

			float tc1 = pTexinfos[texinfo].textureVecsTexelsPerWorldUnits[0][0] * pVertices[v1].point._v[0] + 
				pTexinfos[texinfo].textureVecsTexelsPerWorldUnits[0][1] * pVertices[v1].point._v[1] + 
				pTexinfos[texinfo].textureVecsTexelsPerWorldUnits[0][2] * pVertices[v1].point._v[2] + 
				pTexinfos[texinfo].textureVecsTexelsPerWorldUnits[0][3];
			float tc2 = pTexinfos[texinfo].textureVecsTexelsPerWorldUnits[1][0] * pVertices[v1].point._v[0] + 
				pTexinfos[texinfo].textureVecsTexelsPerWorldUnits[1][1] * pVertices[v1].point._v[1] + 
				pTexinfos[texinfo].textureVecsTexelsPerWorldUnits[1][2] * pVertices[v1].point._v[2] + 
				pTexinfos[texinfo].textureVecsTexelsPerWorldUnits[1][3];

			float tc3 = pTexinfos[texinfo].lightmapVecsLuxelsPerWorldUnits[0][0] * pVertices[v1].point._v[0] + 
				pTexinfos[texinfo].lightmapVecsLuxelsPerWorldUnits[0][1] * pVertices[v1].point._v[1] + 
				pTexinfos[texinfo].lightmapVecsLuxelsPerWorldUnits[0][2] * pVertices[v1].point._v[2] + 
				pTexinfos[texinfo].lightmapVecsLuxelsPerWorldUnits[0][3] - pFaces[j].m_LightmapTextureMinsInLuxels[0];
			float tc4 = pTexinfos[texinfo].lightmapVecsLuxelsPerWorldUnits[1][0] * pVertices[v1].point._v[0] + 
				pTexinfos[texinfo].lightmapVecsLuxelsPerWorldUnits[1][1] * pVertices[v1].point._v[1] + 
				pTexinfos[texinfo].lightmapVecsLuxelsPerWorldUnits[1][2] * pVertices[v1].point._v[2] + 
				pTexinfos[texinfo].lightmapVecsLuxelsPerWorldUnits[1][3] - pFaces[j].m_LightmapTextureMinsInLuxels[1];

			tc1 /= texW;
			tc2 /= texH;
			tc3 /= lightmapW;
			tc4 /= lightmapH;

			tmpVertices.push_back(BSP_VT(pVertices[v1].point._v[0], pVertices[v1].point._v[1], pVertices[v1].point._v[2],
										 tc1, tc2, tc3, tc4));

			if (i == surfedgeStart)
			{
				faceIndices.push_back(tmpVertices.size()-1);
			}
			else if (i < surfedgeEnd)
			{
				if (i > surfedgeStart+1)
					faceIndices.push_back(faceIndices[0]);
				faceIndices.push_back(tmpVertices.size()-1);
				faceIndices.push_back(tmpVertices.size());
			}
		}

		tmpIndices.push_back(faceIndices);
	}

	stringc fullname = file->GetFileName();
	stringc relpath = "";
	s32 pathend = fullname.findLast('/');
	if (pathend == -1)
		pathend = fullname.findLast('\\');
	if (pathend != -1)
		relpath = fullname.subString(0, pathend + 1);

	SMesh* pMesh = new SMesh();
	for (int j=0; j<faceCount; j++)
	{
		SD3D9MeshBuffer* pMB = NULL;
		if (j==0)
		{
			pMB = new SD3D9MeshBuffer(m_pDriver, BspVertexDecl);
			pMB->CreateVertexBuffer(0, sizeof(BSP_VT), tmpVertices.size(), D3DUSAGE_WRITEONLY);
			BSP_VT* pVert;
			pMB->GetVertexBuffer(0)->Lock(0, 0, (void**)&pVert, 0);
			memcpy(pVert, tmpVertices.const_pointer(), sizeof(BSP_VT)*tmpVertices.size());
			pMB->GetVertexBuffer(0)->Unlock();
		}
		else
		{
			pMB = new SD3D9MeshBuffer(m_pDriver, pMesh->GetMeshBuffer(0)->GetVertexType());
			pMB->SetVertexBuffer(0, tmpVertices.size(), pMesh->GetMeshBuffer(0)->GetVertexBuffer(0));
		}

		s32 indexBufferSize = (pFaces[j].numedges - 2) * 3 * sizeof(u32);
		pMB->CreateIndexBuffer((pFaces[j].numedges - 2) * 3);
		u32* pIndices;
		pMB->GetIndexBuffer()->Lock(0, 0, (void**)&pIndices, 0);
		memcpy(pIndices, tmpIndices[j].const_pointer(), indexBufferSize);
		pMB->GetIndexBuffer()->Unlock();

		stringc textureName = pTexStringData + pTexStringTable[pTexData[pTexinfos[pFaces[j].texinfo].texdata].nameStringTableID];
		s32 lastSlash = textureName.findLast('/');
		textureName = textureName.c_str() + lastSlash + 1;
		textureName = relpath + textureName + ".tga";
		D3D9Texture* pTexture = m_pDriver->GetTexture(textureName.c_str());
		pMB->m_Material.Textures[0] = pTexture;

		stringc lightmapName = pFaces[j].lightofs;
		s32 lightmapW = pFaces[j].m_LightmapTextureSizeInLuxels[0]+1;
		s32 lightmapH = pFaces[j].m_LightmapTextureSizeInLuxels[1]+1;
		m_pDriver->SetTextureCreationFlag(ETCF_CREATE_MIP_MAPS, FALSE);
		D3D9Texture* pD3DLightmap = m_pDriver->AddTexture(dimension2di(lightmapW, lightmapH), lightmapName.c_str(), ECF_A8R8G8B8);
		s32 luxelCount = lightmapW * lightmapH;
		ColorRGBExp32* pLightmap = (ColorRGBExp32*)(((byte*)pLightmaps) + pFaces[j].lightofs);
		u8* pLuxels = (u8*)pD3DLightmap->Lock();
		for (s32 i=0; i<luxelCount; i++)
		{
			pLuxels[i*4]   = (u8)(powf((round(clamp<float>(pLightmap[i].b * powf(2.0f, pLightmap[i].exponent), 0.0f, 255.0f)))
				/ 255.f, 1.f/2.2f) *255.f);
			pLuxels[i*4+1] = (u8)(powf((round(clamp<float>(pLightmap[i].g * powf(2.0f, pLightmap[i].exponent), 0.0f, 255.0f)))
				/ 255.f, 1.f/2.2f) *255.f);
			pLuxels[i*4+2] = (u8)(powf((round(clamp<float>(pLightmap[i].r * powf(2.0f, pLightmap[i].exponent), 0.0f, 255.0f)))
				/ 255.f, 1.f/2.2f) *255.f);
			pLuxels[i*4+3] = 255;
		}
		pD3DLightmap->Unlock();
		m_pDriver->SetTextureCreationFlag(ETCF_CREATE_MIP_MAPS, TRUE);
		pMB->m_Material.Textures[1] = pD3DLightmap;
		pMB->m_Material.TextureWrap[1] = D3DTADDRESS_CLAMP;

		pMesh->AddMeshBuffer(pMB);
		pMB->Drop();
	}

	free(pVertices);
	free(pEdges);
	free(pSurfEdges);
	free(pFaces);
	free(pTexinfos);
	free(pTexData);
	free(pLightmaps);
	free(pTexStringData);
	free(pTexStringTable);
	free(pGameLumpHdr);
	free(pEntities);

	return pMesh;
}

const s32 BspFileLoader::GetEntityCount() const
{
	return m_Entities.size();
}

const BspFileLoader::EntityInfo& BspFileLoader::GetEntity(s32 i) const
{
	return m_Entities[i];
}

BOOL BspFileLoader::ParseModels(const stringc& entity)
{
	s32 start  = entity.find("model");

	if (start == -1)
		return FALSE;

	start = entity.findNext('"', start + 6);
	s32 end = entity.findNext('"', start + 1);
	EntityInfo eInfo;
	eInfo.ModelName = entity.subString(start, end - start);

	if (eInfo.ModelName[0] == '*')
		return FALSE;

	char tmpString[32];
	float tmpVector[3];
	int i = 0;

	start = entity.find("origin");
	start = entity.findNext('"', start + 7) + 1;

	while (entity[start] != '"')
	{
		if (entity[start] != ' ')
		{
			end = start;
			while (entity[end] != ' ')
			{
				tmpString[end-start] = entity[end];
				end++;
			}
			tmpString[end-start] = 0;
			tmpVector[i++] = (float)atof(tmpString);
			if (i == 3)
				break;
		}
		start = end + 1;
	}

	eInfo.Origin.X = tmpVector[0];
	eInfo.Origin.Y = tmpVector[1];
	eInfo.Origin.Z = tmpVector[2];

	i = 0;
	start = entity.find("angles");
	start = entity.findNext('"', start + 7) + 1;

	while (entity[start] != '"')
	{
		if (entity[start] != ' ')
		{
			end = start;
			while (entity[end] != ' ')
			{
				tmpString[end-start] = entity[end];
				end++;
			}
			tmpString[end-start] = 0;
			tmpVector[i++] = (float)atof(tmpString);
			if (i == 3)
				break;
		}
		start++;
	}

	eInfo.Angles.X = tmpVector[0];
	eInfo.Angles.Y = tmpVector[1];
	eInfo.Angles.Z = tmpVector[2];

	m_Entities.push_back(eInfo);

	return TRUE;
}

BOOL BspFileLoader::ParsePlayerStartInfo(const stringc& entity)
{
	s32 start  = entity.find("info_player_start");
	s32 end = start;

	if (start == -1)
		return FALSE;

	EntityInfo eInfo;
	eInfo.ModelName = "info_player_start";

	char tmpString[32];
	float tmpVector[3];
	int i = 0;

	start = entity.find("origin");
	start = entity.findNext('"', start + 7) + 1;

	while (entity[start] != '"')
	{
		if (entity[start] != ' ')
		{
			end = start;
			while (entity[end] != ' ')
			{
				tmpString[end-start] = entity[end];
				end++;
			}
			tmpString[end-start] = 0;
			tmpVector[i++] = (float)atof(tmpString);
			if (i == 3)
				break;
		}
		start = end + 1;
	}

	eInfo.Origin.X = tmpVector[0];
	eInfo.Origin.Y = tmpVector[1];
	eInfo.Origin.Z = tmpVector[2];

	i = 0;
	start = entity.find("angles");
	start = entity.findNext('"', start + 7) + 1;

	while (entity[start] != '"')
	{
		if (entity[start] != ' ')
		{
			end = start;
			while (entity[end] != ' ')
			{
				tmpString[end-start] = entity[end];
				end++;
			}
			tmpString[end-start] = 0;
			tmpVector[i++] = (float)atof(tmpString);
			if (i == 3)
				break;
		}
		start++;
	}

	eInfo.Angles.X = tmpVector[0];
	eInfo.Angles.Y = tmpVector[1];
	eInfo.Angles.Z = tmpVector[2];

	m_Entities.push_back(eInfo);

	return TRUE;
}

}