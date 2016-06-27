/**
 *	Ogg playback example with scope visualization with MTek-GDL by Lameguy64/TheCodingBrony
 *
 *	The scope visualization uses vertex	arrays and render-to-texture based effects.
 *
 */
 

// Include library
#include <mgdl-wii.h>
#include <mgdl-wii/mgdl-globals-internal.h>


// OGG file to play
#define	OGGFILE	"testdata/desperation.ogg"


// Prototypes
void init();
void resetCallback();


// Reset flag
bool resetPressed=false;


// Main loop
int main() {

	
	// Init system
	init();
	gdl::ConsoleMode();
	
	
	// Load font image and then bind it to a handler
	gdl::Image	fontImage;
	gdl::FFont	font;
	
	if (!fontImage.LoadImage("testdata/font8x16.png", gdl::Nearest, gdl::RGB5A3))
		exit(0);
		
	font.BindSheet(fontImage, 8, 16);
	
	
	// Create screen image for the visualization effect
	gdl::Image	screenImage;
	screenImage.Create(gdl::wii::rmode->fbWidth, gdl::wii::rmode->efbHeight, gdl::Linear, gdl::RGB565);
	
	
	// Play OGG file
	printf("Now attempting to play %s...", OGGFILE);
	if (!gdl::PlayMusic(OGGFILE, true)) {
		printf("File not found! Exiting...");
		exit(0);
	}
	
	
	// Prepare vertex and color arrays for the scope visualization
	gdl::wii::VERT2s16	scopeLine[2][256];
	GXColor				scopeColor[2][256];
	
	for(short s=0; s<256; s++) {
		for(short i=0; i<2; i++) {
			scopeLine[i][s].x = gdl::ScreenXres*((float)s/256.f);
			scopeLine[i][s].y = gdl::ScreenCenterY;
			scopeColor[i][s].r = 0;
			scopeColor[i][s].g = 0;
			scopeColor[i][s].b = 0;
			if (i==0) scopeColor[i][s].r = 255; else scopeColor[i][s].b = 255;
			scopeColor[i][s].a = 255;
		}
	}
	
	short	*oldSamplePtr=NULL;
	int		sample=0;
	
	
	// Main loop
	while(1) {
		
		
		// Exit when reset is pressed
		if (resetPressed) exit(0);
		
		
		// Prepare display
		gdl::PrepDisplay();
		
		
		// Draw last frame but slightly faded to add a cool mist effect
		screenImage.PutS(-4, -4, gdl::ScreenXres+3, gdl::ScreenYres+3, RGBA(255, 255, 255, 224));
		
		
		// Get buffer pointer and then convert the samples into peaks
		short	*samplePtr=(short*)GetBufferOgg();
		
		if (oldSamplePtr != samplePtr) sample=0;
		oldSamplePtr = samplePtr;
		
		if (samplePtr != NULL) {
			for(short i=0; i<256; i++) {
				scopeLine[0][i].y = gdl::ScreenCenterY+(samplePtr[sample]/256);
				scopeLine[1][i].y = gdl::ScreenCenterY+(samplePtr[sample+1]/256);
				scopeColor[0][i].g = 255*((float)abs(samplePtr[sample])/32768);
				scopeColor[1][i].g = 255*((float)abs(samplePtr[sample+1])/32768);
				sample+=(2*2);
			}
		}
		
		DCFlushRange(scopeLine, sizeof(gdl::wii::VERT2s16)*(256*2));
		DCFlushRange(scopeColor, sizeof(GXColor)*(256*2));
		
		
		// Draw peaks
		GX_SetLineWidth(20, 1);
		
		GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
		
		GX_SetVtxDesc(GX_VA_POS, GX_INDEX8);
		GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX8);
		GX_SetVtxDesc(GX_VA_TEX0, GX_NONE);
		
		GX_SetArray(GX_VA_POS, scopeLine[0], 2*sizeof(s16));
		GX_SetArray(GX_VA_CLR0, scopeColor[0], 4*sizeof(u8));
		
		GX_Begin(GX_LINESTRIP, GX_VTXFMT0, 256);
		for(short v=0; v<256; v++) {
			GX_Position1x8(v);
			GX_Color1x8(0);
		}
		GX_End();
		
		GX_SetArray(GX_VA_POS, scopeLine[1], 2*sizeof(s16));
		GX_SetArray(GX_VA_CLR0, scopeColor[1], 4*sizeof(u8));
		
		GX_Begin(GX_LINESTRIP, GX_VTXFMT0, 256);
		for(short v=0; v<256; v++) {
			GX_Position1x8(v);
			GX_Color1x8(0);
		}
		GX_End();
		
		
		// Capture screen to image for the visualization
		screenImage.Texture.CopyFromScreen(0, 0, gdl::wii::rmode->fbWidth, gdl::wii::rmode->efbHeight, false);
		
		
		// Draw stats
		char strBuff[32];
		sprintf(strBuff, "TIME: %d:%d.%d", (GetTimeOgg()/1000)/60, (GetTimeOgg()/1000)%60, (GetTimeOgg()/10)%100);
		font.DrawText(strBuff, 8, 32, 1, 0xffffffff);
		
		font.DrawText("Press RESET to quit...", 8, 48, 1, 0xffffffff);
		
		
		// Display
		gdl::Display();
		
		
	}
	
	
}

void init() {
	
	
	// Init system
	fatInitDefault();
	gdl::InitSystem(0);
	
	
	// Set system callback
	SYS_SetResetCallback(resetCallback);
	
	
}

void resetCallback() {
	
	resetPressed=true;
	
}