#ifndef _PIRATE_BSP_FILE_LOADER_
#define _PIRATE_BSP_FILE_LOADER_

#include "RefObject.h"

namespace Pirate
{

struct SMesh;
class D3D9Driver;
class FileSystem;

//! Meshloader capable of loading bsp level.
class BspFileLoader : public virtual RefObject
{
public:

	//! Constructor
	BspFileLoader(FileSystem* fs, D3D9Driver* driver);

	//! destructor
	~BspFileLoader();

	//! Returns true if the file maybe is able to be loaded by this class.
	/** This decision should be based only on the file extension (e.g. ".cob") */
	BOOL IsALoadableFileExtension(const c8* fileName);

	//! Creates/loads an animated mesh from the file.
	/** \return Pointer to the created mesh. Returns 0 if loading failed.
	If you no longer need the mesh, you should call IAnimatedMesh::drop().
	See IUnknown::drop() for more information. */
	SMesh* CreateMesh(FileReader* file);

private:

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

	struct dvertex_t
	{
		Vector	point;
	};

	// note that edge 0 is never used, because negative edge nums are used for
	// counterclockwise use of the edge in a face
	struct dedge_t
	{
		unsigned short	v[2];		// vertex numbers
	};

	#define	MAXLIGHTMAPS	4
#ifndef byte
	typedef unsigned char	byte;
#endif

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

	FileSystem* m_pFileSystem;
	D3D9Driver* m_pDriver;
};


} // end namespace 

#endif