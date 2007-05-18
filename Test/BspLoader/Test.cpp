#include <iostream>
#include "..\..\Pirate\Pirate.h"

using namespace Pirate;

enum
{
	LUMP_ENTITIES					= 0,	// *
	LUMP_PLANES						= 1,	// *
	LUMP_TEXDATA					= 2,	// JAY: This is texdata now, previously LUMP_TEXTURES
	LUMP_VERTEXES					= 3,	// *
	LUMP_VISIBILITY					= 4,	// *
	LUMP_NODES						= 5,	// *
	LUMP_TEXINFO					= 6,	// *
	LUMP_FACES						= 7,	// *
	LUMP_LIGHTING					= 8,	// *
	LUMP_OCCLUSION					= 9,
	LUMP_LEAFS						= 10,	// *
	// UNUSED
	LUMP_EDGES						= 12,	// *
	LUMP_SURFEDGES					= 13,	// *
	LUMP_MODELS						= 14,	// *
	LUMP_WORLDLIGHTS				= 15,	// 
	LUMP_LEAFFACES					= 16,	// *
	LUMP_LEAFBRUSHES				= 17,	// *
	LUMP_BRUSHES					= 18,	// *
	LUMP_BRUSHSIDES					= 19,	// *
	LUMP_AREAS						= 20,	// *
	LUMP_AREAPORTALS				= 21,	// *
	LUMP_PORTALS					= 22,
	LUMP_CLUSTERS					= 23,
	LUMP_PORTALVERTS				= 24,
	LUMP_CLUSTERPORTALS				= 25,
	LUMP_DISPINFO					= 26,
	LUMP_ORIGINALFACES				= 27,
	// UNUSED
	LUMP_PHYSCOLLIDE				= 29,
	LUMP_VERTNORMALS				= 30,
	LUMP_VERTNORMALINDICES			= 31,
	LUMP_DISP_LIGHTMAP_ALPHAS		= 32,
	LUMP_DISP_VERTS					= 33,		// CDispVerts
	LUMP_DISP_LIGHTMAP_SAMPLE_POSITIONS = 34,	// For each displacement
	//     For each lightmap sample
	//         byte for index
	//         if 255, then index = next byte + 255
	//         3 bytes for barycentric coordinates
	// The game lump is a method of adding game-specific lumps
	// FIXME: Eventually, all lumps could use the game lump system
	LUMP_GAME_LUMP					= 35,
	LUMP_LEAFWATERDATA				= 36,
	LUMP_PRIMITIVES					= 37,
	LUMP_PRIMVERTS					= 38,
	LUMP_PRIMINDICES				= 39,
	// A pak file can be embedded in a .bsp now, and the file system will search the pak
	//  file first for any referenced names, before deferring to the game directory 
	//  file system/pak files and finally the base directory file system/pak files.
	LUMP_PAKFILE					= 40,
	LUMP_CLIPPORTALVERTS			= 41,
	// A map can have a number of cubemap entities in it which cause cubemap renders
	// to be taken after running vrad.
	LUMP_CUBEMAPS					= 42,
	LUMP_TEXDATA_STRING_DATA		= 43,
	LUMP_TEXDATA_STRING_TABLE		= 44,
	LUMP_OVERLAYS					= 45,
	LUMP_LEAFMINDISTTOWATER			= 46,
	LUMP_FACE_MACRO_TEXTURE_INFO	= 47,
	LUMP_DISP_TRIS					= 48,
	LUMP_PHYSCOLLIDESURFACE			= 49,	// deprecated.  We no longer use win32-speicifc havok compression on terrain
	LUMP_WATEROVERLAYS              = 50,
	LUMP_LIGHTMAPPAGES				= 51,	// xbox: alternate lightdata implementation
	LUMP_LIGHTMAPPAGEINFOS          = 52,	// xbox: indexed by faces for alternate lightdata

	// optional lumps for HDR
	LUMP_LIGHTING_HDR				= 53,
	LUMP_WORLDLIGHTS_HDR			= 54,
	LUMP_LEAF_AMBIENT_LIGHTING_HDR	= 55,	// NOTE: this data overrides part of the data stored in LUMP_LEAFS.
	LUMP_LEAF_AMBIENT_LIGHTING		= 56,	// NOTE: this data overrides part of the data stored in LUMP_LEAFS.

	LUMP_XZIPPAKFILE				= 57,   // xbox: maps may now have xzp's instead of zips stored. 
	LUMP_FACES_HDR					= 58,	// HDR maps may have different face data.
	LUMP_MAP_FLAGS                  = 59,   // extended level-wide flags. not present in all levels

};

#define	HEADER_LUMPS		64

struct lump_t
{
	int		fileofs, filelen;
	int		version;		// default to zero
	char	fourCC[4];		// default to ( char )0, ( char )0, ( char )0, ( char )0
};

struct dheader_t
{
	int			ident;
	int			version;	
	lump_t		lumps[HEADER_LUMPS];
	int			mapRevision;				// the map's revision (iteration, version) number (added BSPVERSION 6)
};

typedef struct texinfo_s
{
	float		textureVecsTexelsPerWorldUnits[2][4];			// [s/t][xyz offset]
	float		lightmapVecsLuxelsPerWorldUnits[2][4];			// [s/t][xyz offset] - length is in units of texels/area
	int			flags;				// miptex flags + overrides
	int			texdata;			// Pointer to texture name, size, etc.
} texinfo_t;

struct Vector
{
	float _v[3];
};

struct dtexdata_t
{
	Vector		reflectivity;
	int			nameStringTableID;				// index into g_StringTable for the texture name
	int			width, height;					// source image
	int			view_width, view_height;		//
};

#define	MAXLIGHTMAPS	4

typedef unsigned char	byte;

struct dface_t
{
	unsigned short	planenum;
	byte		side;	// faces opposite to the node's plane direction
	byte		onNode; // 1 of on node, 0 if in leaf

	int			firstedge;		// we must support > 64k edges
	short		numedges;	
	short		texinfo;
	// This is a union under the assumption that a fog volume boundary (ie. water surface)
	// isn't a displacement map.
	// FIXME: These should be made a union with a flags or type field for which one it is
	// if we can add more to this.
	//	union
	//	{
	short       dispinfo;
	// This is only for surfaces that are the boundaries of fog volumes
	// (ie. water surfaces)
	// All of the rest of the surfaces can look at their leaf to find out
	// what fog volume they are in.
	short		surfaceFogVolumeID;
	//	};

	// lighting info
	byte		styles[MAXLIGHTMAPS];
	int			lightofs;		// start of [numstyles*surfsize] samples
	float       area;

	// TODO: make these unsigned chars?
	int			m_LightmapTextureMinsInLuxels[2];
	int			m_LightmapTextureSizeInLuxels[2];

	int origFace;				// reference the original face this face was derived from


public:

	unsigned short GetNumPrims() const;
	void SetNumPrims( unsigned short nPrims );
	bool AreDynamicShadowsEnabled();
	void SetDynamicShadowsEnabled( bool bEnabled );

	// non-polygon primitives (strips and lists)
public:
	unsigned short m_NumPrims;	// Top bit, if set, disables shadows on this surface (this is why there are accessors).

public:
	unsigned short	firstPrimID; 

	unsigned int	smoothingGroups;
};

// compressed color format 
struct ColorRGBExp32
{
	byte r, g, b;
	signed char exponent;
};

typedef struct
{
	byte			page;			// lightmap page [0..?]
	byte			offset[2];		// offset into page (s,t)
	byte			pad;			// unused
	ColorRGBExp32	avgColor;		// average used for runtime lighting calcs
} dlightmappageinfo_t;

void main()
{
	FileSystem fileSys;
	FileReader* pReader = fileSys.CreateAndOpenFile("..\\..\\Media\\FirstMap.bsp");

	dheader_t header;
	pReader->Read(&header, sizeof(dheader_t));
	char* pIdentString = (char*)&header.ident;
	std::cout<<pIdentString<<" "<<header.version<<" "<<header.mapRevision<<std::endl;

	texinfo_t texinfo;
	int n = header.lumps[LUMP_TEXINFO].filelen/sizeof(texinfo_t);
	pReader->Seek(header.lumps[LUMP_TEXINFO].fileofs);
	for (int i=0; i<n; i++)
	{
		pReader->Read(&texinfo, sizeof(texinfo_t));
		std::cout<<texinfo.textureVecsTexelsPerWorldUnits[0][0]<<" "<<texinfo.textureVecsTexelsPerWorldUnits[0][1]<<" "
				 <<texinfo.textureVecsTexelsPerWorldUnits[0][2]<<" "<<texinfo.textureVecsTexelsPerWorldUnits[0][3]<<" "
				 <<texinfo.textureVecsTexelsPerWorldUnits[1][0]<<" "<<texinfo.textureVecsTexelsPerWorldUnits[1][1]<<" "
				 <<texinfo.textureVecsTexelsPerWorldUnits[1][2]<<" "<<texinfo.textureVecsTexelsPerWorldUnits[1][3]<<std::endl
				 <<texinfo.lightmapVecsLuxelsPerWorldUnits[0][0]<<" "<<texinfo.lightmapVecsLuxelsPerWorldUnits[0][1]<<" "
				 <<texinfo.lightmapVecsLuxelsPerWorldUnits[0][2]<<" "<<texinfo.lightmapVecsLuxelsPerWorldUnits[0][3]<<" "
				 <<texinfo.lightmapVecsLuxelsPerWorldUnits[1][0]<<" "<<texinfo.lightmapVecsLuxelsPerWorldUnits[1][1]<<" "
				 <<texinfo.lightmapVecsLuxelsPerWorldUnits[1][2]<<" "<<texinfo.lightmapVecsLuxelsPerWorldUnits[1][3]<<std::endl
				 <<texinfo.flags<<" "<<texinfo.texdata<<std::endl<<std::endl;
	}
	n = header.lumps[LUMP_TEXDATA].filelen/sizeof(dtexdata_t);
	pReader->Seek(header.lumps[LUMP_TEXDATA].fileofs);
	for (int i=0; i<n; i++)
	{
		dtexdata_t texdata;
		pReader->Read(&texdata, sizeof(dtexdata_t));
		std::cout<<texdata.nameStringTableID<<" "<<texdata.view_width<<" "<<texdata.view_height<<" "
			<<texdata.width<<" "<<texdata.height<<std::endl;
	}
	std::cout<<std::endl;

	pReader->Seek(header.lumps[LUMP_TEXDATA_STRING_DATA].fileofs);
	char texstring[1024];
	pReader->Read(texstring, header.lumps[LUMP_TEXDATA_STRING_DATA].filelen);

	n = header.lumps[LUMP_TEXDATA_STRING_TABLE].filelen/sizeof(int);
	pReader->Seek(header.lumps[LUMP_TEXDATA_STRING_TABLE].fileofs);
	for (int i=0; i<n; i++)
	{
		int stable;
		pReader->Read(&stable, sizeof(int));
		char* pTexName = &texstring[stable];
		std::cout<<stable<<" "<<pTexName<<" "<<std::endl;
	}
	std::cout<<std::endl;

	n = header.lumps[LUMP_VERTEXES].filelen/sizeof(Vector);
	pReader->Seek(header.lumps[LUMP_VERTEXES].fileofs);
	for (int i=0; i<n; i++)
	{
		Vector v;
		pReader->Read(&v, sizeof(Vector));
		std::cout<<v._v[0]<<" "<<v._v[1]<<" "<<v._v[2]<<std::endl;
	}
	std::cout<<std::endl;

	n = header.lumps[LUMP_FACES].filelen/sizeof(dface_t);
	pReader->Seek(header.lumps[LUMP_FACES].fileofs);
	for (int i=0; i<n; i++)
	{
		dface_t face;
		pReader->Read(&face, sizeof(dface_t));
		std::cout<<face.planenum<<" "<<(bool)face.side<<" "<<(bool)face.onNode<<" "<<face.firstedge<<" "<<face.numedges<<"  "
				 <<face.m_LightmapTextureMinsInLuxels[0]<<" "<<face.m_LightmapTextureMinsInLuxels[1]<<" "
				 <<face.m_LightmapTextureSizeInLuxels[0]<<" "<<face.m_LightmapTextureSizeInLuxels[1]<<"  "
				 <<face.lightofs<<std::endl;
	}
	std::cout<<std::endl;

	n = header.lumps[LUMP_LIGHTING].filelen;
	pReader->Seek(header.lumps[LUMP_LIGHTING].fileofs);
	std::cout<<n<<std::endl;
	for (int i=0; i<29; i++)
	{
		ColorRGBExp32 rgbe;
		pReader->Read(&rgbe, sizeof(ColorRGBExp32));
		std::cout<<(u32)rgbe.r<<" "<<(u32)rgbe.g<<" "<<(u32)rgbe.b<<" "<<(s32)rgbe.exponent<<std::endl;
	}

	n = header.lumps[LUMP_LIGHTMAPPAGEINFOS].filelen;
	pReader->Seek(header.lumps[LUMP_LIGHTMAPPAGEINFOS].fileofs);
	std::cout<<n<<std::endl;
}