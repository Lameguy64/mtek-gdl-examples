#ifndef _PLYLIB_H
#define _PLYLIB_H

#define PLY_ERR_NOTFOUND    1
#define PLY_ERR_NOTPLY      2
#define PLY_ERR_INCOMPLETE  3
#define PLY_ERR_VERTEXPROP  4
#define PLY_ERR_INVALIDPOLY 5

namespace ply {

	GXColor DefaultCol ATTRIBUTE_ALIGN(32) = { 0xff, 0xff, 0xff, 0xff };

	// For vertexes and normals
	typedef struct {
		float x;
		float y;
		float z;
	} VERTEX3f;

	// For texture coordinates
	typedef struct {
		float u;
		float v;
	} TCOORD2f;

	// For vertex colors
	typedef struct {
		u_char r;
		u_char g;
		u_char b;
	} COLOR3b;

	// For faces
	typedef struct {
		u_short v0;
		u_short v1;
		u_short v2;
	} INDEX;


	// Simply initializes a vertex format for 3D coordinates into slot 1
	void Init();


	class Model {
	public:

		void	*vList;	// Vertex list
		void	*nList;	// Normal list
		void	*tList;	// Texcoord list
		void	*cList;	// Color list
		INDEX	*iList;	// Index list

		u_short	vertCount;	// Vertex count
		u_short	faceCount;	// Face count

		void	*dList;		// Display list pointer
		int		listSize;	// Display list size

		// Constructor and deconstructor
		Model();
		virtual	~Model();

		// Loads a PLY model ready to be rendered (make sure that GX_VTXFMT1 has been properly set up)
		int		LoadModel(const char *fileName);

		// Flushes vertices useful when you procedurally alter the vertices (such as vertex animation)
		void	FlushVertexes();

		// Renders the model (make sure that the current projection matrix is set for 3D rendering)
		void	RenderModel();

	};


}

#endif // _PLYLIB_H
