/**	
 *	Simple tilemap renderer example with MTek-GDL by TheCodingBrony/Lameguy64
 *
 *	A simple yet well optimized tilemap renderer example that also doubles as
 *	a vertex array example which can easily be translated for 3D graphics
 *	programming.
 *	
 *	When it comes to tilemap rendering, it is best to make use of vertex
 *	arrays since it is a lot faster than sending vertexes directly to the GP. 
 *	The following is a simple visualization of how the tile rendering works
 *	in this example:
 *
 *
 *	First, an array of vertices are generated:
 *
 *		+0	+1	+2	+3
 *
 *
 *		+4	+5	+6	+7
 *
 *
 *		+8	+9	+10	+11
 *
 *
 *	Since each vertex is referenced by index, we can draw quads by simply
 *	connecting each point of the quad to an appropriate vertex like so:
 *
 *		+0--+1	+2--+3
 *		|	|	|	|
 *		|	|	|	|
 *		+4--+5--+6--+7
 *		|	|	|
 *		|	|	|
 *		+8--+9--+10	+11 
 *
 *
 *	Tiles are selected through texcoords since tiles are usually packed
 *	together as a simple array of tiles in a form of a large image (also
 *	known as a tile sheet). This method of tile selection is the fastest
 *	since texture objects are not set each time a tile is drawn and not
 *	much matrix calculations are needed.
 *
 *
 *	Controls:
 *		IR Pointer	- Move pointer.
 *		B			- Scroll around (direction is controlled by the pointer)
 *		Home/Reset	- Exit program.
 *
 */


// Include library and neccessary headers
#include <mgdl-wii.h>
#include <wiiuse/wpad.h>
#include "../../common/wiimote.c"


// Struct to hold vertex coordinates
typedef struct {
	short x,y;
} tileVCoord;

// Struct to hold texcoords of our tiles
typedef struct {
	float u0, v0;	// Top-left
	float u1, v1;	// Top-right
	float u2, v2;	// Bottom-right
	float u3, v3;	// Bottom-left
} tileTCoord;


// Struct to hold rendering parameters of our simple tile renderer
typedef struct {
	short		rXdim,rYdim;	// Size of rendering field in tile units (always 1 tile larger than the screen to hide tile pop-in).
	short		tXdim,tYdim;	// Size of tiles in pixels.
	tileVCoord	*vCoords;		// Pointer for our vertex array.
} renderStruct;

typedef struct {
	short		tXdim,tYdim;	// Size of tiles in pixels (not related to the one in renderStruct).
	short		numTiles;		// Number of tiles.
	tileTCoord	*tCoords;		// Pointer for our texcoord array.
	gdl::Image	*sheet;			// Pointer to associated gdl::Image object containing the tile sheet.
} tilesetStruct;


// Function prototypes
void drawTileMap(short scrollX, short scrollY, renderStruct *render, tilesetStruct *tileset, short mapXdim, short mapYdim, short *mapData);
void initRenderer(renderStruct *render, short tileXsize, short tileYsize);
void initTileset(tilesetStruct *tileset, gdl::Image &sheet, short tileXsize, short tileYsize);


// Main function
int main() {
	
	
	// Init system
	fatInitDefault();
	gdl::InitSystem(0);
	gdl::SetClearColor(0x00, 0x3f, 0x7f, 0xff);
	
	// Init Wiimote
	WPAD_Init();
	InitWiimotes(1);
	
	
	// Load pointer image
	gdl::Image pointer;
	pointer.LoadImage("testdata/pointer.png", gdl::Nearest, gdl::RGB5A3);
	
	// Load image of our tiles
	gdl::Image tileImage;
	tileImage.LoadImage("testdata/mel_tiles.png", gdl::Linear, gdl::RGBA8);
	
	
	// Create and initialize renderer struct
	renderStruct renderer;
	initRenderer(&renderer, 64, 64);
	
	// Create and initialize tile set image with the tile sheet image
	tilesetStruct tileset;
	initTileset(&tileset, tileImage, 64, 64);
	
	
	// Generate tilemap
	short tileMap[64][64];
	
	// In this example, tile 0 is treated as air/transparent while tile 1 refers to
	// the first tile in the tile sheet. This is common practice amonst 2D tilemap
	// based game engines.
	for(short ty=0; ty<64; ty++) {
		for(short tx=0; tx<64; tx++) {
			
			tileMap[ty][tx] = (tileset.numTiles+1)*((float)rand()/RAND_MAX);
			
			tileMap[0][tx]	= 4;
			tileMap[63][tx]	= 4;
			
		}
		tileMap[ty][0]	= 4;
		tileMap[ty][63]	= 4;
	}
	
	
	// Camera structure
	struct {
		float x,y;
		float xMove,yMove;
	} Camera={0};
	
	
	// Main loop
	while(1) {
		
		
		// Handle input
		CheckWiimotes();
		
		if (Wiimote[0].status == WPAD_ERR_NONE) {
			
			if (Wiimote[0].data.btns_h & WPAD_BUTTON_HOME)
				gdl::wii::Exit();
			
			if (Wiimote[0].data.btns_h & WPAD_BUTTON_B) {
				Camera.xMove += (float)(Wiimote[0].data.ir.x-gdl::ScreenCenterX)/(gdl::ScreenCenterX/2);
				Camera.yMove += (float)(Wiimote[0].data.ir.y-gdl::ScreenCenterY)/(gdl::ScreenCenterY/2);
			}
			
		}
		
		
		// Calculate camera position
		Camera.x += Camera.xMove*gdl::Delta;
		Camera.y += Camera.yMove*gdl::Delta;
		Camera.xMove *=0.8f;
		Camera.yMove *=0.8f;
		
		
		// Prepare rendering frame
		gdl::PrepDisplay();
		
		
		// Draw the tilemap
		drawTileMap(Camera.x, Camera.y, &renderer, &tileset, 64, 64, &tileMap[0][0]);
		
		
		// Draw the pointer
		if (Wiimote[0].status == WPAD_ERR_NONE) {
			if (Wiimote[0].data.ir.valid) {
				pointer.PutX(Wiimote[0].data.ir.x, Wiimote[0].data.ir.y, 4, 0, Wiimote[0].data.ir.angle, 2, gdl::Color::White);
			}
		}
		
		if (Wiimote[0].data.btns_h & WPAD_BUTTON_2) {
			gdl::SaveScreen("tilemap_cap.png");
		}
		
		// Display frame
		gdl::Display();
		
	}
	
}


// Renders a tilemap
void drawTileMap(short scrollX, short scrollY, renderStruct *render, tilesetStruct *tileset, short mapXdim, short mapYdim, short *mapData) {
	
	
	// Set the texture object of the tile sheet
	GX_LoadTexObj(tileset->sheet->Texture.TexObj(), GX_TEXMAP0);
	GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);
	
	
	// Perform translation
	Mtx tempMtx;
	guMtxTransApply(gdl::wii::ModelMtx, tempMtx, -(scrollX%render->tXdim), -(scrollY%render->tYdim), 0);
	GX_LoadPosMtxImm(tempMtx, GX_PNMTX0);
	
	
	// Prepare vertex descriptors for rendering the tilemap
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_INDEX16);
	GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX16);
	
	// Set the arrays
	GX_SetArray(GX_VA_POS, render->vCoords, 2*sizeof(s16));
	GX_SetArray(GX_VA_TEX0, tileset->tCoords, 2*sizeof(f32));
	
	
	// Calculate the proper tile offset to draw
	short ttx=scrollX/render->tXdim;
	short tty=scrollY/render->tYdim;
	
	
	// Begin rendering the tilemap
	GX_Begin(GX_QUADS, GX_VTXFMT0, 4*((render->rXdim-1)*(render->rYdim-1)));
	
	// Process the tiles
	for(short ty=0; ty<render->rYdim-1; ty++) {
		for (short tx=0; tx<render->rXdim-1; tx++) {
			
			// Calculate offset of the tile to be read
			int toffs=(ttx+tx)+(mapXdim*(tty+ty));
			
			// Only accept non-zero tiles that are within the boundaries of the array
			if (mapData[toffs] && (((ttx+tx) >= 0) && ((tty+ty) >= 0) && ((ttx+tx) < mapXdim) && ((tty+ty) < mapYdim))) {
				
				int vIndex=tx+(render->rXdim*ty);
				int tIndex=4*(mapData[toffs]-1);
				
				// Top-left
				GX_Position1x16(vIndex);
				GX_TexCoord1x16(tIndex);
				
				// Top-right
				GX_Position1x16(vIndex+1);
				GX_TexCoord1x16(tIndex+1);
				
				// Bottom-right
				GX_Position1x16(vIndex+(render->rXdim+1));
				GX_TexCoord1x16(tIndex+2);
				
				// Bottom-left
				GX_Position1x16(vIndex+render->rXdim);
				GX_TexCoord1x16(tIndex+3);
				
			} else { // If tile is not valid, simply reference zeros to the quad
				
				GX_Position1x16(0);
				GX_TexCoord1x16(0);
				
				GX_Position1x16(0);
				GX_TexCoord1x16(0);
				
				GX_Position1x16(0);
				GX_TexCoord1x16(0);
				
				GX_Position1x16(0);
				GX_TexCoord1x16(0);
				
			}
			
		}
	}
	
	// Finish rendering the tilemap
	GX_End();
	
	
	// Revert to previous matrix
	GX_LoadPosMtxImm(gdl::wii::ModelMtx, GX_PNMTX0);
	
	
}


// Initializes a renderStruct and generates an array of vertexes for tilemap drawing
void initRenderer(renderStruct *render, short tileXsize, short tileYsize) {
	
	// Calculate range of tiles to draw (usually a tile larger than the screen to hide tile clipping)
	render->rXdim = ((gdl::ScreenXres+(tileXsize-1))/tileXsize)+2;
	render->rYdim = ((gdl::ScreenXres+(tileYsize-1))/tileYsize)+2;
	
	// Set tile dimensions
	render->tXdim = tileXsize;
	render->tYdim = tileYsize;
	
	// Generate vertex array
	render->vCoords = (tileVCoord*)memalign(32, sizeof(tileVCoord)*(render->rXdim*render->rYdim));
	
	for(short ty=0; ty<render->rYdim; ty++) {
		for(short tx=0; tx<render->rXdim; tx++) {
			
			int index = tx+(render->rXdim*ty);
			
			render->vCoords[index].x = tileXsize*tx;
			render->vCoords[index].y = tileYsize*ty;
			
		}
	}
	
	DCFlushRange(render->vCoords, sizeof(tileVCoord)*(render->rXdim*render->rYdim));
	
}


// Initializes a tilesetStruct, attaches a gdl::Image object to it and generates an array of texcoords
void initTileset(tilesetStruct *tileset, gdl::Image &sheet, short tileXsize, short tileYsize) {

	// Calculate the number of tiles in the sheet and set it to the tilesetStruct
	short tXdim=sheet.Xsize()/tileXsize;
	short tYdim=sheet.Ysize()/tileYsize;
	tileset->numTiles	= tXdim*tYdim;
	
	// Attach tilesheet to the tilesetStruct and generate texcoords
	tileset->sheet		= &sheet;
	tileset->tCoords	= (tileTCoord*)memalign(32, sizeof(tileTCoord)*tileset->numTiles);
	
	for(short ty=0; ty<tYdim; ty++) {
		for(short tx=0; tx<tXdim; tx++) {
			
			int index = tx+(tXdim*ty);
			
			// Top-left
            tileset->tCoords[index].u0 = ((float)(tileXsize*tx)+0.01f)     /sheet.Xsize();
            tileset->tCoords[index].v0 = ((float)(tileYsize*ty)+0.01f)     /sheet.Ysize();
			// Top-right
            tileset->tCoords[index].u1 = (float)((tileXsize*tx)+tileXsize) /sheet.Xsize();
            tileset->tCoords[index].v1 = (float)((tileYsize*ty)+0.01f)     /sheet.Ysize();
			// Bottom-right
			tileset->tCoords[index].u2 = (float)((tileXsize*tx)+tileXsize) /sheet.Xsize();
            tileset->tCoords[index].v2 = (float)((tileYsize*ty)+tileYsize) /sheet.Ysize();
			// Bottom-left
            tileset->tCoords[index].u3 = ((float)(tileXsize*tx)+0.01f)     /sheet.Xsize();
            tileset->tCoords[index].v3 = (float)((tileYsize*ty)+tileYsize) /sheet.Ysize();
			
		}
	}
	
	DCFlushRange(tileset->tCoords, sizeof(tileTCoord)*tileset->numTiles);
	
}