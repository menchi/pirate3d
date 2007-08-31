#ifndef _PIRATE_VERTEX_ATTRIBUTES_H_
#define _PIRATE_VERTEX_ATTRIBUTES_H_

template<typename T, unsigned int NumValues>
struct DeclType {};

template<class UsageType, unsigned char UsageIndex>
struct DeclUsage {};

template<typename T, unsigned int NumValues, class UsageType, unsigned char UsageIndex>
struct VertexAttribute {
	typedef DeclType<T, NumValues> decl_type;
	typedef DeclUsage<UsageType, UsageIndex> decl_usage;

	T v_[NumValues];
	T& operator[] (unsigned int i) { return v_[i]; }
};

struct VertexUsage {
	struct Position;
	struct Normal;
	struct TexCoord;
	struct Color;
};

typedef VertexAttribute<float, 2, VertexUsage::Position, 0> POSITION2f;
typedef VertexAttribute<float, 4, VertexUsage::Color, 0> COLOR4f;

#endif