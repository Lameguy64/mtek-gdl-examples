/**
 *	Simple sprite set example with MTek-GDL by Lameguy64/TheCodingBrony
 *
 *  This is a very simple example demonstrating how to load a sprite set file and
 *  displaying each individual sprite as an animation.
 *
 *  Because sprites in a sprite set file are referenced by index, it makes procedural
 *  frame-by-frame animations possible in a very versatile manner. Also, the Sprite Mapper
 *	file can generate a header containing index defines so you can reference sprites
 *	indexes by name which makes things much more convenient.
 *
 *  To learn more about how sprite sets are constructed, open setprofile.tpf inside the
 *  project folder with the included Sprite Mapper tool.
 *
 *	To quit the demo, either press RESET on your console (Wii) or just close the program
 *	window (PC).
 *
 */

// Include library
#ifndef PLATFORM_PC
#include <mgdl-wii.h>
#else
#include <mgdl-pc.h>
#include <unistd.h>
#endif


// Main function
int main() {

	#ifdef PLATFORM_WII
	// Init FAT library and system
	fatInitDefault();
	gdl::InitSystem(gdl::LowResMode);
	#else
	// Init render window
	gdl::Init("Sprite Set Example", 320, 240, 32, 0, 0, 2);
	#endif


	// Clear color and set console mode (on Wii platform)
	gdl::SetClearColor(63, 63, 127, 255);
	#ifdef PLATFORM_WII
	gdl::ConsoleMode();
	#endif


	// Create sprite set object and load a sprite set
	#ifdef PLATFORM_PC
	chdir("..");
	#endif
	gdl::SpriteSet sprites;
	if (!sprites.LoadSprites("testdata/pinkie_sprites.tsm", "testdata/", gdl::Nearest, gdl::RGBA8))
		exit(0);


	// Variables for animating the Pinkie Pie sprite
	short frame	=0;
	float fCount=0,pinkieX=0;


	// Main loop
	while(!gdl::pc::WindowShouldClose()) {


		// Animate Pinkie Pie sprite
		if (fCount >= 4) {
			frame = (frame+1)%11;
			fCount = 0;
		}
		fCount += 1*gdl::Delta;

		pinkieX += 1*gdl::Delta;
		if (pinkieX > gdl::ScreenXres + 40) pinkieX = -40;


		// Display the sprite
		gdl::PrepDisplay();

		sprites.Put(pinkieX, gdl::ScreenCenterY+40, frame, gdl::Color::White, gdl::Pivot, gdl::Pivot, 1.f, 0);

		gdl::Display();


	}

}
