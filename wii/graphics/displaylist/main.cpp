/**	Display list example with MTek-GDL by Lameguy64/TheCodingBrony
 *
 *	This example demonstrates how to use display lists for efficient graphics
 *	rendering by rendering a box with multiple textures assigned to it through
 *	display lists. It also demonstrates the use of vertex arrays as well.
 *
 *	Display lists and vertex arrays are a must when rendering 3D
 *	geometry that has a large number of polygons for best performance.
 *
 *	Unfortunately, there is no way to calculate the exact size required for
 *	the display list to be created. The best way so far is to just estimate
 *	the size of the display list which must be larger than necessary and then
 *	trim it down to the size returned by GX_EndDispList() with realloc() after
 *	building the list.
 *
 *	The most interesting thing about display lists with the GX is that most
 *	GX commands and multiple GX_Begin()/GX_End() pairs can be stuffed into a
 *	single display list as demonstrated in this example which are very useful
 *	for rendering 3D scenery especially when it has multiple textures. However,
 *	display lists cannot be called within a display list and another display
 *	list cannot be built while building another display list.
 *
 */


// Include library
#include <mgdl-wii.h>


// Vertexes for our box (must be 32-byte aligned)
gdl::wii::VERT3f32 verts[] ATTRIBUTE_ALIGN(32) = {
	// Front
	-100.f	,	100.f	,	100.f,
	100.f	,	100.f	,	100.f,
	100.f	,	-100.f	,	100.f,
	-100.f	,	-100.f	,	100.f,

	// Back
	100.f	,	100.f	,	-100.f,
	-100.f	,	100.f	,	-100.f,
	-100.f	,	-100.f	,	-100.f,
	100.f	,	-100.f	,	-100.f,
};



// Texcoords for our box (must be 32-byte aligned)
gdl::wii::TEX2f32 tcoords[] ATTRIBUTE_ALIGN(32) = {
	0.f, 0.f,
	1.f, 0.f,
	1.f, 1.f,
	0.f, 1.f,
};


// Main function
int main() {

	// Init system
	fatInitDefault();
	gdl::InitSystem(0);
	gdl::SetClearColor(0x00, 0x3f, 0x7f, 0xff);

	gdl::ConsoleMode();


	// Load textures
	gdl::Image texture[4];

	if (!texture[0].LoadImageMipmapped("testdata/mel_tex_0.png", gdl::LN_MM_LN, gdl::Linear, gdl::RGBA8, 9))
		exit(0);
	texture[0].Texture.SetWrapMode(gdl::Repeat, gdl::Repeat);

	if (!texture[1].LoadImageMipmapped("testdata/mel_tex_1.png", gdl::LN_MM_LN, gdl::Linear, gdl::RGBA8, 9))
		exit(0);
	texture[1].Texture.SetWrapMode(gdl::Repeat, gdl::Repeat);

	if (!texture[2].LoadImageMipmapped("testdata/mel_tex_2.png", gdl::LN_MM_LN, gdl::Linear, gdl::RGBA8, 9))
		exit(0);
	texture[2].Texture.SetWrapMode(gdl::Repeat, gdl::Repeat);

	if (!texture[3].LoadImageMipmapped("testdata/mel_tex_3.png", gdl::LN_MM_LN, gdl::Linear, gdl::RGBA8, 9))
		exit(0);
	texture[3].Texture.SetWrapMode(gdl::Repeat, gdl::Repeat);


	// Set vertex format for our 3D box
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_POS, GX_POS_XYZ,	GX_F32,	0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);


	// Allocate buffer for the display list
	int  listSize = 1024*32;
	void *listBuff = memalign(32, listSize);
	DCInvalidateRange(listBuff, listSize);


	// Begin building the display list
	GX_BeginDispList(listBuff, listSize);

		// Front
		GX_LoadTexObj(texture[0].Texture.TexObj(), GX_TEXMAP0);
		GX_Begin(GX_QUADS, GX_VTXFMT1, 4);

			GX_Position1x8(0);	// Reference vertexes by index
			GX_TexCoord1x8(0);

			GX_Position1x8(1);
			GX_TexCoord1x8(1);

			GX_Position1x8(2);
			GX_TexCoord1x8(2);

			GX_Position1x8(3);
			GX_TexCoord1x8(3);

		GX_End();

		// Left
		GX_LoadTexObj(texture[1].Texture.TexObj(), GX_TEXMAP0);
		GX_Begin(GX_QUADS, GX_VTXFMT1, 4);

			GX_Position1x8(5);
			GX_TexCoord1x8(0);

			GX_Position1x8(0);
			GX_TexCoord1x8(1);

			GX_Position1x8(3);
			GX_TexCoord1x8(2);

			GX_Position1x8(6);
			GX_TexCoord1x8(3);

		GX_End();

		// Back
		GX_LoadTexObj(texture[2].Texture.TexObj(), GX_TEXMAP0);
		GX_Begin(GX_QUADS, GX_VTXFMT1, 4);

			GX_Position1x8(4);
			GX_TexCoord1x8(0);

			GX_Position1x8(5);
			GX_TexCoord1x8(1);

			GX_Position1x8(6);
			GX_TexCoord1x8(2);

			GX_Position1x8(7);
			GX_TexCoord1x8(3);

		GX_End();

		// Right
		GX_LoadTexObj(texture[3].Texture.TexObj(), GX_TEXMAP0);
		GX_Begin(GX_QUADS, GX_VTXFMT1, 4);

			GX_Position1x8(1);
			GX_TexCoord1x8(0);

			GX_Position1x8(4);
			GX_TexCoord1x8(1);

			GX_Position1x8(7);
			GX_TexCoord1x8(2);

			GX_Position1x8(2);
			GX_TexCoord1x8(3);

		GX_End();

	// End display list building and get new list size (required for GX_CallDispList())
	listSize = GX_EndDispList();

	// Trim the display list to the size returned by GX_EndDispList() to save memory
	realloc(listBuff, listSize);


	// Stuff for rotating the box
	guVector tempRot;
	Mtx		 tempMtx;
	float	 angle=0;
	float	 loop=0;


	// Main loop
	while(1) {

		// Prepare for rendering
		gdl::PrepDisplay();


		// Set 3D mode
		gdl::Set3DMode(1000);


		// Make the textures scroll indefinitely (to demonstrate one of the many benefits of vertex arrays on the GX)
		tcoords[0].u = 0.f+loop;
		tcoords[1].u = 1.f+loop;
		tcoords[2].u = 1.f+loop;
		tcoords[3].u = 0.f+loop;
		DCFlushRange(tcoords, sizeof(gdl::wii::TEX2f32)*4);

		loop += 0.4f/texture[0].Texture.TXsize();
		if (loop > 1) loop -= 1.f;


		// Move the box forward and then rotate it around
		guMtxApplyTrans(gdl::wii::ModelMtx, gdl::wii::ModelMtx, 0.f, 0.f, -300.f);

		tempRot.x = 1;	tempRot.y = 0;	tempRot.z = 0;
		guMtxRotAxisDeg(tempMtx, &tempRot, angle);
		guMtxConcat(gdl::wii::ModelMtx, tempMtx, gdl::wii::ModelMtx);

		tempRot.x = 0;	tempRot.y = 1;	tempRot.z = 0;
		guMtxRotAxisDeg(tempMtx, &tempRot, angle);
		guMtxConcat(gdl::wii::ModelMtx, tempMtx, gdl::wii::ModelMtx);

		GX_LoadPosMtxImm(gdl::wii::ModelMtx, GX_PNMTX0);

		angle += 0.4f;


		// Set appropriate TEV operator to GX_REPLACE since we're not using color arrays
		GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);

		// Set vertex descriptors since we specify vertexes by index
		GX_ClearVtxDesc();
		GX_SetVtxDesc(GX_VA_POS, GX_INDEX8);
		GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX8);

		// Set the vertex arrays for the display list
		GX_SetArray(GX_VA_POS, verts, 3*sizeof(f32));
		GX_SetArray(GX_VA_TEX0, tcoords, 2*sizeof(f32));

		// Execute the display list
		GX_CallDispList(listBuff, listSize);


		// Reset back to 2D mode
		gdl::Set2DMode(100);


		// Display
		gdl::Display();

	}

}
