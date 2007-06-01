#include <set>
#include "..\..\Pirate\Pirate.h"
#include "..\..\Pirate\BSPHelper.h"
#include <process.h>

using namespace Pirate;

void main()
{
	Printer::pLogger = new Logger(NULL);

	FileSystem fileSys;
	FileReader* file = fileSys.CreateAndOpenFile("../../Media/d1_trainstation_02/d1_trainstation_02.bsp");

	dheader_t header;
	dface_t* pFaces;
	texinfo_t* pTexinfos;
	dtexdata_t* pTexData;
	c8* pTexStringData;
	s32* pTexStringTable;

	file->Read(&header, sizeof(dheader_t));
	char* pIdentString = (char*)&header.ident;
	//	std::cout<<pIdentString<<" "<<header.version<<" "<<header.mapRevision<<std::endl;
	stringc idString(pIdentString);
	if (!idString.equalsn("VBSP", 4) || header.version<19)
	{
		Printer::Log("Not an available HL2 bsp file format", ELL_WARNING);
		return;
	}

	file->Seek(header.lumps[LUMP_FACES].fileofs);
	pFaces = (dface_t*)malloc(header.lumps[LUMP_FACES].filelen);
	file->Read(pFaces, header.lumps[LUMP_FACES].filelen);

	file->Seek(header.lumps[LUMP_TEXINFO].fileofs);
	pTexinfos = (texinfo_t*)malloc(header.lumps[LUMP_TEXINFO].filelen);
	file->Read(pTexinfos, header.lumps[LUMP_TEXINFO].filelen);

	file->Seek((header.lumps[LUMP_TEXDATA]).fileofs);
	pTexData = (dtexdata_t*)malloc(header.lumps[LUMP_TEXDATA].filelen);
	file->Read(pTexData, header.lumps[LUMP_TEXDATA].filelen);

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
/*
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
*/
	s32 faceCount = header.lumps[LUMP_FACES].filelen / sizeof(dface_t);

	stringc fullname = file->GetFileName();
	stringc relpath = "\"";
	s32 pathend = fullname.findLast('/');
	if (pathend == -1)
		pathend = fullname.findLast('\\');
	if (pathend != -1)
		relpath += fullname.subString(0, pathend + 1) + "materials/" + "\"";

	std::set<stringc> sourceVMT;
	std::set<stringc> mapVMT;
	std::set<stringc> textures;
	std::set<stringc> bumpmaps;
	for (int j=0; j<faceCount; j++)
	{
		stringc textureName = pTexStringData + pTexStringTable[pTexData[pTexinfos[pFaces[j].texinfo].texdata].nameStringTableID];
		stringc vmtName = textureName + ".vmt";
		stringc vtfName = textureName + ".vtf";
		if (textureName.find("maps/") == -1)
		{
			stringc prefix = "root/hl2/materials/";
			sourceVMT.insert(prefix + vmtName);
		}
		else
		{
			stringc prefix = "materials/";
			mapVMT.insert(prefix + vmtName);
		}
	}	

	stringc extractName;
	u32 epc = 0;
	for (std::set<stringc>::iterator itr = mapVMT.begin(); itr!=mapVMT.end(); itr++, epc++ )
	{
		extractName += " -e \"";
		extractName += ((stringc)*itr);
		extractName += "\"";
		if (epc == 20)
		{
			_spawnl(_P_WAIT, "HLExtract.exe", "HLExtract.exe", "-p", "\"F:\\Pirate3D\\Media\\d1_trainstation_02\\d1_trainstation_02.bsp\"", 
					"-d", relpath.c_str(), extractName.c_str(), NULL);
			extractName = "";
			epc = 0;
		}
	}

	if (epc != 1)
	{
		_spawnl(_P_WAIT, "HLExtract.exe", "HLExtract.exe", "-p", "\"F:\\Pirate3D\\Media\\d1_trainstation_02\\d1_trainstation_02.bsp\"", 
			"-d", relpath.c_str(), extractName.c_str(), NULL);
	}

	for (std::set<stringc>::iterator itr = mapVMT.begin(); itr!=mapVMT.end(); itr++ )
	{
		stringc dataPath = "../../Media/d1_trainstation_02/materials/";
		extractName = ((stringc)*itr);
		s32 pos = extractName.findLast('/');
		extractName = extractName.subString(pos+1, extractName.size()-pos);
		FileReader* vmtfile = CreateReadFile((dataPath + extractName).c_str());
		c8 buf[1024];
		vmtfile->Read(buf, vmtfile->GetSize());
		vmtfile->Drop();
		stringc vmtText = buf;
		vmtText.make_lower();
		pos = vmtText.find("include");
		if (pos == -1)
			continue;
		
		pos = vmtText.findNext('"', pos+9);
		stringc incVmt = "root/hl2/";
		incVmt += vmtText.subString(pos+1, vmtText.findNext('"', pos+1)-pos-1);
		sourceVMT.insert(incVmt);
	}

	extractName = "";
	epc = 0;
	for (std::set<stringc>::iterator itr = sourceVMT.begin(); itr!=sourceVMT.end(); itr++, epc++ )
	{
		extractName += " -e \"";
		extractName += ((stringc)*itr);
		extractName += "\"";
		if (epc == 20)
		{
			_spawnl(_P_WAIT, "HLExtract.exe", "HLExtract.exe", "-p", "\"D:\\Steam\\steamapps\\source materials.gcf\"", 
				"-d", relpath.c_str(), extractName.c_str(), NULL);
			extractName = "";
			epc = 0;
		}
	}

	if (epc != 1)
	{
		_spawnl(_P_WAIT, "HLExtract.exe", "HLExtract.exe", "-p", "\"D:\\Steam\\steamapps\\source materials.gcf\"", 
			"-d", relpath.c_str(), extractName.c_str(), NULL);
	}

	for (std::set<stringc>::iterator itr = sourceVMT.begin(); itr!=sourceVMT.end(); itr++ )
	{
		stringc dataPath = "../../Media/d1_trainstation_02/materials/";
		extractName = ((stringc)*itr);
		s32 pos = extractName.findLast('/');
		extractName = extractName.subString(pos+1, extractName.size()-pos);
		extractName.make_lower();
		FileReader* vmtfile = CreateReadFile((dataPath + extractName).c_str());
		c8 buf[4096];
		vmtfile->Read(buf, vmtfile->GetSize());
		vmtfile->Drop();
		stringc vmtText = buf;
		vmtText.make_lower();
		pos = vmtText.find("$basetexture");
		if (pos == -1)
			continue;

		pos = vmtText.findNext('"', pos+13);
		stringc basetexture = "root/hl2/materials/";
		basetexture += (vmtText.subString(pos+1, vmtText.findNext('"', pos+1)-pos-1) + ".vtf");
		textures.insert(basetexture);

		pos = vmtText.find("$bumpmap");
		if (pos == -1)
			continue;

		pos = vmtText.findNext('"', pos+9);
		stringc bumpmap = "root/hl2/materials/";
		bumpmap += (vmtText.subString(pos+1, vmtText.findNext('"', pos+1)-pos-1) + ".vtf");
		bumpmaps.insert(bumpmap);
	}

	extractName = "";
	epc = 0;
	for (std::set<stringc>::iterator itr = textures.begin(); itr!=textures.end(); itr++, epc++ )
	{
		extractName += " -e \"";
		extractName += ((stringc)*itr);
		extractName += "\"";
		if (epc == 20)
		{
			_spawnl(_P_WAIT, "HLExtract.exe", "HLExtract.exe", "-p", "\"D:\\Steam\\steamapps\\source materials.gcf\"", 
				"-d", relpath.c_str(), extractName.c_str(), NULL);
			extractName = "";
			epc = 0;
		}
	}
	if (epc != 1)
		_spawnl(_P_WAIT, "HLExtract.exe", "HLExtract.exe", "-p", "\"D:\\Steam\\steamapps\\source materials.gcf\"", 
				"-d", relpath.c_str(), extractName.c_str(), NULL);

	extractName = "";
	epc = 0;
	for (std::set<stringc>::iterator itr = bumpmaps.begin(); itr!=bumpmaps.end(); itr++, epc++ )
	{
		extractName += " -e \"";
		extractName += ((stringc)*itr);
		extractName += "\"";
		if (epc == 20)
		{
			_spawnl(_P_WAIT, "HLExtract.exe", "HLExtract.exe", "-p", "\"D:\\Steam\\steamapps\\source materials.gcf\"", 
				"-d", relpath.c_str(), extractName.c_str(), NULL);
			extractName = "";
			epc = 0;
		}
	}
	if (epc != 1)
		_spawnl(_P_WAIT, "HLExtract.exe", "HLExtract.exe", "-p", "\"D:\\Steam\\steamapps\\source materials.gcf\"", 
				"-d", relpath.c_str(), extractName.c_str(), NULL);


	free(pFaces);
	free(pTexinfos);
	free(pTexData);
	free(pTexStringData);
	free(pTexStringTable);
	free(pGameLumpHdr);
	free(pEntities);

	file->Drop();
	Printer::pLogger->Drop();
}