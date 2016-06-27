/**
 *	Simple 2D graphics example using MTek-GDL by Lameguy64/TheCodingBrony
 *
 *	This silly little cross-platform example demonstrates loading a couple of
 *	images and then drawing them using Put(), PutX(), and PutS()... It also
 *	demonstrates texture modification by procedurally generating a plasma image
 *	in real-time using gdl::Texture::PokePixel() (on Wii) or glTexSubImage2D()
 *  (on PC).
 *
 */


// Includes for compiling on PC
#ifdef PLATFORM_PC
#include <mgdl-pc.h>
#endif

// Includes for compiling on Wii
#ifdef PLATFORM_WII
#include <mgdl-wii.h>
#include <mgdl-wii/mgdl-globals-internal.h>
#include <wiiuse/wpad.h>
#endif


// Main function
int main() {


	// Init code for PC version
	#ifdef PLATFORM_PC

	// Initialize and create GDL window
	gdl::Init("Simple 2D Stuff", 854, 480, 32, 0, 0);

	#endif


	// Init code for Wii version
	#ifdef PLATFORM_WII

	// Initialize the file system
	fatInitDefault();

	// Initialize MTek-GDL with default settings
	gdl::InitSystem(gdl::ModeAuto, gdl::AspectAuto, gdl::HiRes, gdl::AntiAliasing);
	//gdl::InitSystem(0);

	// Initialize Wiimote library (WPAD)
	WPAD_Init();

	// Enter console mode to display error messages (on console platforms only)
	gdl::ConsoleMode();

	#endif


	// Create 3 image objects for our test images
	gdl::Image	image[3];

	// Font sheet image for the fixed-font handling class
	gdl::Image	fontImage;

	// Fixed-font handling class
	gdl::FFont	font;

	// Some variables
	float angle=0,scaleCount=0;


	// Step a directory back since the test files are located there when compiled for PC
	#ifdef PLATFORM_PC
	chdir("..");
	#endif


	// Load my uber-cute Barb fan-art (unless you're not a fan of my style)
	#ifdef PLATFORM_WII
	if (!image[0].LoadImageMipmapped("testdata/barb.png", gdl::LN_MM_LN, gdl::Linear, gdl::RGBA8, 9))
		exit(0);
	#else
	if (!image[0].LoadImage("testdata/barb.png", gdl::Linear))
		exit(0);
	#endif


	// Load the Trollface :)
	if (!image[1].LoadImage("testdata/trollface.png", gdl::Linear, gdl::IA4))
		exit(0);


	// Create a 128x128 image for the procedurally generated plasma
	image[2].Create(128, 128, gdl::Linear, gdl::RGBA8);


	// Load generic 8x16 system font sheet
	if (!fontImage.LoadImage("testdata/font8x16.png", gdl::Nearest, gdl::I4))
		exit(0);

	// Bind font sheet to our font object so we can print characters with it
	font.BindSheet(fontImage, 8, 16);


	// Generate color table for the plasma effect
	#define J 255.f/360*3
	#define K 255.f/360*2
	#define L 255.f/360*1

	u_int colTable[256];
	for (short c=0; c<256; c++) {

		int r = 255*fabs(sin((J*c)*ROTPI));
		int g = 255*fabs(sin((K*c)*ROTPI));
		int b = 255*fabs(sin((L*c)*ROTPI));

		colTable[c] = RGBA(r, g, b, 255);

    }


	// Main loop
	while(1) {

		#ifdef PLATFORM_WII

		WPAD_ScanPads();

		// Quit if home button is pressed
		if ((WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME))
			gdl::wii::Exit();

		#else

		if ((gdl::pc::WindowShouldClose()) || (gdl::Keyboard::KeyDown(gdl::Keyboard::KEY_ESCAPE)))
			break;

		#endif


		// Prepare display for rendering (this must be called prior to intensive code)
		gdl::PrepDisplay();


		// Generate plasma image
		#ifdef PLATFORM_WII

        for(short py=0; py<128; py++) {
			for(short px=0; px<128; px++) {
				short col = (sin(px*PI/270)*255)+(sin(py*PI/45)*31)+(sin(((py+px)+(4*angle))*PI/90)*63);
				image[2].Texture.PokePixel(px, py, colTable[abs(col)%256]);
			}
		}
		// Flush texture so that updated pixels will be drawn properly
		image[2].Texture.Flush();

		#else

		// Because PokePixel() is very slow on OpenGL, it is best to allocate a texture buffer and then upload its
		// pixels to the video card all at once using glTexSubImage2D().

		u_int imageBuff[128][128];
        for(short py=0; py<128; py++) {
			for(short px=0; px<128; px++) {
				short col = (sin(px*PI/270)*255)+(sin(py*PI/45)*31)+(sin(((py+px)+(4*angle))*PI/90)*63);
                imageBuff[py][px] = colTable[abs(col)%256];
			}
		}
        glBindTexture(GL_TEXTURE_2D, image[2].Texture.TexID());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 128, 128, GL_RGBA, GL_UNSIGNED_BYTE, imageBuff);

		#endif


		// Stretch the procedurally generated plasma graphic to fill the entire screen
		image[2].PutS(0, 0, gdl::ScreenXres, gdl::ScreenYres, gdl::Color::White);

		// Draw Barb picture and make it roto-zoom
		image[0].Put(
			gdl::ScreenCenterX, gdl::ScreenCenterY,
			RGBA(255, 255, 255, 127),
			gdl::Centered, gdl::Centered,
			1.f+(0.8f*(sin(scaleCount*ROTPI))), angle
		);

		// Draw trollface and make it bob up & down (for trolliness)
		image[1].Put(16, (gdl::ScreenYres-120)+(16*(sin((scaleCount*32)*ROTPI))) , gdl::Color::White);


		// Draw text and make it scale in and out
		font.DrawText("The power of MTek-GDL!", gdl::Centered, 32, 2.f+(1.f*(sin((scaleCount*4)*ROTPI))), gdl::Color::White);


		// Display
		gdl::Display();


		// Update counters (with gdl::Delta so that it'll adapt according to your console's current TV system)
		angle		+= 0.1f*gdl::Delta;
		scaleCount	+= 0.5f*gdl::Delta;

	}

}
