/**
 *	Whack-a-Cirra minigame example using MTek-GDL by TheCodingBrony/Lameguy64
 *
 *	UPDATE (06-20-2015): The game now runs in low-res mode and fixed an ordering table system
 *	bug where if you plot a Cirra at the bottom most part of the screen, the game will crash.
 *
 *	A silly but bloody minigame demonstrating the sprite capabilities of the MTek-GDL library.
 *	It also features an ordering table system (blit sort) to quickly sort sprites from top to
 *	bottom order for top-down isometric styled games. This demo also visually demonstrates how
 *	many sprites the library can draw at once (it can handle up to 1500+ sprites before the
 *	framerate dips on the Wii platform).
 *
 *	This example can both be compiled for the Wii and Windows platforms... Use CodeBlocks to
 *	compile for Windows (and maybe Linux/Mac as well).
 *
 *	Controls (PC):
 *		Left-click	- Drop a Cirra into the field.
 *		Right-click	- Kill a Cirra that was on the cursor.
 *		ESC			- Trigger an interesting ending sequence and quit :).
 *
 *	Controls (Wii):
 *		A			- Drop a Cirra into the field.
 *		B			- Kill a Cirra that was on the cursor.
 *		Home		- Trigger an interesting ending sequence and quit :).
 *
 *	In case you're wondering who Cirra is, it is one of Natalie Butler's Dayspring characters
 *	which you can find in missbutlerart.deviantart.com, I highly recommend checking her
 *	gallery out, you will not be disappointed! In fact, the Cirra sprites used in this example
 *	were actually recycled from a failed fan-game project some 2 years ago and this example game
 *	is actually a port of a test program that used an ill fated graphics engine written in QB64.
 *
 */


// Include library and necessary headers
#ifdef PLATFORM_PC
#include <mgdl-pc.h>
#else
#include <mgdl-wii.h>
#include <wiiuse/wpad.h>
#include "../../wii/common/wiimote.c"
#endif


// Include header of sprite index names
#include "names.h"


// Max sizes of Cirra and Effect entries
#define MAX_CIRRAS	2048
#define MAX_EFFECTS 14336

// Max sizes of sprite entry and ordering table entries
#define MAX_SPRITES 16384
#define OT_LENGTH   16384


// For graphics and sound, very self-explanatory
gdl::Image		pointer;	// Pointer for the Wiimote
gdl::SpriteSet	sprites;	// Sprite set
gdl::Sound      sound[4];	// Sounds

gdl::Image		fontImage;	// Font sheet image
gdl::FFont		font;		// Fixed-font object

bool			ending=false;


// For the 'Cirra' entities
typedef struct {
	float x;
	float y;
	float a;
	float aMove;
	short dir;
	float waitTime;
	float moveTime;
	float frameCount;
	short frameNum;
	short state;
} cirraType;

int			numCirras=0;
cirraType	cirra[MAX_CIRRAS];


// For sorting sprites relative to its Y axis
typedef struct {
	short spriteType;
	short x;
	short y;
	short a;
	float angle;
	float scale;
	float opacity;
	short index;
} SpriteEntryType;

int numSpriteEntries=0;
SpriteEntryType spriteEntry[MAX_SPRITES];

// The ordering table
void *spriteOT[OT_LENGTH];


// For effects
typedef struct {
	short effectNum;
	float x;
	float y;
	float a;
	float xMove;
	float yMove;
	float aMove;
	float angle;
	float angleMove;
	short index;
	float duration;
} effectType;

int         numEffects=0;
effectType  effect[MAX_EFFECTS];


// For the ending sequence :)
typedef struct {
	bool active;
	float x;
	float y;
	float a;
	float aMove;
	float duration;
} tntType;

tntType tnt;


// Function prototypes
void init();

void doPlayerInput();
bool onSprite(int x, int y, int sx, int sy, const gdl::Sprite *sprite);
bool inBox(int x, int y, int x1, int y1, int x2, int y2);

bool sortToOT(int tableIndex, SpriteEntryType *spriteEntry);
void addSprite(short x, short y, short a, short index);
void addSpriteX(short x, short y, short a, float angle, float scale, float op, short index);
void addFireParticle(short x, short y, short a, float opacity);
void drawSpriteOT();

void putGib(float x, float y, float a, short spriteIndex);
void putFireParticle(float x, float y, float a);
void doEffects();

void putCirra(short x, short y, short a);
void killCirra(short index);
void doCirras();


// Player input structure to keep things clean with different input configurations
typedef struct {
    short   pointerX;
    short   pointerY;
    bool    pointerOnScreen;
    bool    mouseLB;
    bool    mouseRB;
	bool	exitGame;
} playerInputType;

playerInputType playerInput={0};


// Main function
int main() {


	// Init and load stuff
	init();


	bool plotPressed=false;
    bool killPressed=false;
	bool exitPressed=false;
	bool doParticle=false;
	bool doneMusicPlay=false;

	float endCount=0;
	short explosion=0;


	// Put first cirra sprite
	putCirra(gdl::ScreenCenterX, gdl::ScreenCenterY, 100);


	// Main loop
	while(1) {


		// Get player input
        doPlayerInput();

		if (ending == false) {

			// Drop a Cirra into the field
			if (playerInput.mouseLB) {

				if (plotPressed==false) {

					putCirra(playerInput.pointerX, playerInput.pointerY, 100);
					//plotPressed=true;

				}

			} else {

				plotPressed=false;

			}

			// Kill a Cirra
			if (playerInput.mouseRB) {

				if (killPressed==false) {

					// Simple bounding box search, too lazy to add sorting but it works
					for (int s=0; s<numCirras; s++) {
						if (onSprite(playerInput.pointerX, playerInput.pointerY, cirra[s].x, cirra[s].y, sprites.SpriteInfo(CIRRA_DOWN))) {
							sound[1].Play2D(1.f + ((0.6f*((float)rand()/RAND_MAX))-0.2f), 100.f, cirra[s].x-gdl::ScreenCenterX, cirra[s].y-gdl::ScreenCenterY);
							killCirra(s);
							break;
						}
					}

					//killPressed=true;

				}

			} else {

				killPressed=false;

			}

		}


		// Initiate end sequence (blowing up all Cirras :) )
		#ifdef PLATFORM_PC
		if ((playerInput.exitGame) || gdl::pc::WindowShouldClose()) {
		#else
		if (playerInput.exitGame) {
		#endif

			if (exitPressed == false) {

				if (ending == false) {

					sound[2].Play(1.f, 100.f);

					tnt.active = true;
					tnt.x = gdl::ScreenCenterX;
					tnt.y = gdl::ScreenCenterY;
					tnt.a = 100;
					tnt.aMove = 0;
					tnt.duration = 0;
					ending = true;

					#ifdef PLATFORM_PC
                    glfwSetWindowShouldClose(gdl::pc::WindowContext, false);
					#endif

				} else {

					#ifdef PLATFORM_PC
					break;
					#else
					gdl::wii::Exit();
					#endif

				}

			}

			exitPressed = true;

		} else {

			exitPressed = false;

		}


		// Prepare to draw stuff
		gdl::PrepDisplay();


		// Reset sprite entry count and clear ordering table
        numSpriteEntries=0;
        memset(spriteOT, 0x00, sizeof(void*)*OT_LENGTH);


		// Do TNT animation
		if (tnt.active) {

			if (tnt.a > 0) {
				tnt.aMove += - (0.2f*gdl::Delta);
				tnt.a += (tnt.aMove*gdl::Delta);
				if (tnt.a <= 0) tnt.a = 0;
			} else {
				tnt.a = 0;
			}

			addSprite(tnt.x, tnt.y-1, 0, SHADOW);
			addSprite(tnt.x, tnt.y, tnt.a, TNT);

			doParticle ^= 1;
			if (doParticle) {
				putFireParticle(tnt.x, tnt.y, tnt.a+13);
				putFireParticle(tnt.x+1, tnt.y, tnt.a+13);
			}

			tnt.duration += (1.f*gdl::Delta);

			// Detonate!
			if (tnt.duration > 240) {

				// Kill all cirras when the TNT explodes
				while(numCirras) killCirra(0);

				sound[3].Play(1.f, 100.f);

				explosion = 2;
				endCount = 1;
				tnt.active = false;

			}

		}


		// Do Cirra entities
		doCirras();
        doEffects();


		// Draw sprites inside ordering table
        drawSpriteOT();


		// Draw pointer (only on Wii platform)
		#ifdef PLATFORM_WII
		if ((ending == false) && (playerInput.pointerOnScreen)) {
			pointer.Put(playerInput.pointerX, playerInput.pointerY, gdl::Color::White, 4, 0, 1, Wiimote[0].data.ir.angle);
		}
		#endif


		// Draw instruction text
		if (ending == false) {

			#ifdef GDL_PLATFORM_WII

			font.Printf(9, 17, 1.f, gdl::Color::Black, "Press A to plot, B to kill.");
			font.Printf(8, 16, 1.f, gdl::Color::White, "Press A to plot, B to kill.");
			font.Printf(9, 33, 1.f, gdl::Color::Black, "Press Home or Reset to exit...");
			font.Printf(8, 32, 1.f, gdl::Color::White, "Press Home or Reset to exit...");

			#else

			font.Printf(9, 7, 1.f, gdl::Color::Black, "Press LMB to plot, RMB to kill.");
			font.Printf(8, 6, 1.f, gdl::Color::White, "Press LMB to plot, RMB to kill.");
			font.Printf(9, 23, 1.f, gdl::Color::Black, "Press Escape or Close to exit...");
			font.Printf(8, 22, 1.f, gdl::Color::White, "Press Escape or Close to exit...");

			#endif // GDL_PLATFORM_WII

		}


		// Do TNT sequence
		if (endCount) {

			if ((endCount > 10) && (doneMusicPlay == false)) {

				// Play 'gameover' music
				gdl::PlayMusic("testdata/gameover.ogg", false);
				doneMusicPlay = true;

			}

			if (endCount > 60) {

				//sprites.PutX(gdl::ScreenCenterX, gdl::ScreenCenterY, GAMEOVER, gdl::Pivot, gdl::Pivot, 0, 4, gdl::Color::White);

				sprites.Put(gdl::ScreenCenterX, gdl::ScreenCenterY, GAMEOVER, gdl::Color::White, gdl::Pivot, gdl::Pivot, 4.f);

				#ifdef GDL_PLATFORM_WII
				font.Printf(gdl::Centered, gdl::ScreenYres-69, 1.f, gdl::Color::Black, "Press Home or Reset again to exit");
				font.Printf(gdl::Centered, gdl::ScreenYres-70, 1.f, gdl::Color::White, "Press Home or Reset again to exit");
				#else
				font.Printf(gdl::Centered, gdl::ScreenYres-69, 1.f, gdl::Color::Black, "Press Escape or Close again to exit");
				font.Printf(gdl::Centered, gdl::ScreenYres-70, 1.f, gdl::Color::White, "Press Escape or Close again to exit");
				#endif

			}

			if (endCount <= 60) endCount += 1.f*gdl::Delta;

		}


		// Simple flash effect for the TNT explosion
		if (explosion) {

			gdl::DrawBoxF(0, 0, gdl::ScreenXres-1, gdl::ScreenYres-1, gdl::Color::White);
			explosion--;

		}


		// Display stuff
		gdl::Display();


	}


}


// Init function
void init() {


	#ifdef GDL_PLATFORM_WII

	// Init code for Wii
	fatInitDefault();
	gdl::InitSystem(gdl::ModeAuto, gdl::AspectAuto, gdl::LowRes);

    WPAD_Init();
	InitWiimotes(1);

	#else

	// Init code for PC
	gdl::Init("Whack-a-Cirra", 427, 240, 32, 0, 0, 2);

	#endif


	// Set clear color
	gdl::SetClearColor(0, 63, 127, 255);


    #ifdef GDL_PLATFORM_WII

	// Set console mode and load pointer image
	gdl::ConsoleMode();
	if (!pointer.LoadImage("testdata/pointer.png", gdl::Nearest, gdl::RGB5A3))
		exit(0);

	#else

	// Step back a directory on PC
	chdir("../..");

	#endif


	// Load font
	if (!fontImage.LoadImage("testdata/font8x16.png", gdl::Nearest, gdl::RGB5A3))
		exit(0);

	font.BindSheet(fontImage, 8, 16);

	// Load sprites
	if (!sprites.LoadSprites("testdata/test_sprites.tsm", "testdata/", gdl::Nearest, gdl::RGB5A3))
		exit(0);

	// Load sound effects
    if (!sound[0].LoadSound("testdata/hitfloor.wav"))
		exit(0);
    if (!sound[1].LoadSound("testdata/smack.wav"))
		exit(0);
	if (!sound[2].LoadSound("testdata/fuse.wav"))
		exit(0);
	if (!sound[3].LoadSound("testdata/explode.wav"))
		exit(0);

}


// Reads player input
void doPlayerInput() {

    #ifdef PLATFORM_PC

    playerInput.exitGame = false;

	// Mouse clicks
	playerInput.mouseLB = gdl::Mouse::MouseInfo.left;
	playerInput.mouseRB = gdl::Mouse::MouseInfo.right;
	playerInput.pointerX = gdl::Mouse::MouseInfo.x;
	playerInput.pointerY = gdl::Mouse::MouseInfo.y;

	playerInput.exitGame = gdl::Keyboard::KeyDown(gdl::Keyboard::KEY_ESCAPE);

    #else

	CheckWiimotes();

    if (Wiimote[0].active) {
        if (Wiimote[0].status == WPAD_ERR_NONE) {

            playerInput.mouseLB = false;
            playerInput.mouseRB = false;
			playerInput.exitGame = false;
            if (Wiimote[0].data.btns_h & WPAD_BUTTON_A) playerInput.mouseLB = true;
            if (Wiimote[0].data.btns_h & WPAD_BUTTON_B) playerInput.mouseRB = true;
			if (Wiimote[0].data.btns_h & WPAD_BUTTON_HOME) playerInput.exitGame = true;

            if (Wiimote[0].data.ir.valid) {
                playerInput.pointerX = Wiimote[0].data.ir.x;
                playerInput.pointerY = Wiimote[0].data.ir.y;
                playerInput.pointerOnScreen = true;
            } else {
                playerInput.pointerOnScreen = false;
            }

        } else {

            playerInput.mouseLB = false;
            playerInput.mouseRB = false;

        }
    }

    #endif

}


// Simple bounding-box collision tester for sprites
bool onSprite(int x, int y, int sx, int sy, const gdl::Sprite *sprite) {

    int bx1,by1;
    int bx2,by2;

    bx1 = sx - sprite->cx;
    by1 = sy - sprite->cy;
    bx2 = sx + ((sprite->w-1) - sprite->cx);
    by2 = sy + ((sprite->h-1) - sprite->cy);

    if (inBox(x, y, bx1, by1, bx2, by2)) return(true);
    return(false);

}
bool inBox(int x, int y, int x1, int y1, int x2, int y2) {

    if ((x >= x1) && (x <= x2)) {
        if ((y >= y1) && (y <= y2)) {
            return(true);
        }
    }

    return(false);

}


// Sorts a sprite entry into the ordering table
bool sortToOT(int tableIndex, SpriteEntryType *spriteEntry) {

    int  index      =tableIndex;
    bool fwSearch   =true;

    while(1) {

        // If the OT entry is empty, register the sprite entry into it
        if (spriteOT[index] == NULL) {
            spriteOT[index] = (void*)spriteEntry;
            break;
        }

        // If not a single empty OT entry is found, simply quit (rejecting the sprite entry)
        if (index <= 0)
            return(false);

        // When the top of the list is reached, scan backwards
        if (index >= OT_LENGTH-1) {
            index   = tableIndex;
            fwSearch=false;
        }

        // Step up or down table index
        if (fwSearch) {
            index += 1;
        } else {
            index -= 1;
        }

    }

    return(true);

}
// Adds a sprite entry into the ordering table
void addSprite(short x, short y, short a, short index) {

    if (numSpriteEntries >= MAX_SPRITES) return;

    // Add sprite entry
    spriteEntry[numSpriteEntries].spriteType = 0; // Normal sprite
    spriteEntry[numSpriteEntries].x = x;
    spriteEntry[numSpriteEntries].y = y;
    spriteEntry[numSpriteEntries].a = a;
    spriteEntry[numSpriteEntries].index = index;

    // Now sort it into the ordering table
    int otIndex=(OT_LENGTH-1)*((float)y/gdl::ScreenYres);
    if (otIndex < 0) otIndex=0;
    if (otIndex > OT_LENGTH-2) otIndex=OT_LENGTH-2;

    if (sortToOT(otIndex, &spriteEntry[numSpriteEntries]))
        numSpriteEntries += 1;

}
// Adds an extended sprite entry into the ordering table
void addSpriteX(short x, short y, short a, float angle, float scale, float op, short index) {

	if (numSpriteEntries >= MAX_SPRITES) return;

	// Extended sprite
	spriteEntry[numSpriteEntries].spriteType = 1;
	spriteEntry[numSpriteEntries].x = x;
	spriteEntry[numSpriteEntries].y = y;
	spriteEntry[numSpriteEntries].a = a;
	spriteEntry[numSpriteEntries].angle = angle;
	spriteEntry[numSpriteEntries].scale = scale;
	spriteEntry[numSpriteEntries].opacity = op;
	spriteEntry[numSpriteEntries].index = index;

	// Now sort it into the ordering table
	int otIndex=(OT_LENGTH-1)*((float)y/gdl::ScreenYres);
	if (otIndex < 0) otIndex=0;
	if (otIndex > OT_LENGTH-2) otIndex=OT_LENGTH-2;

	if (sortToOT(otIndex, &spriteEntry[numSpriteEntries]))
		numSpriteEntries += 1;

}
// Adds a fire particle into the ordering table
void addFireParticle(short x, short y, short a, float opacity) {

	if (numSpriteEntries >= MAX_SPRITES) return;

	spriteEntry[numSpriteEntries].spriteType = 2; // Yellow pixel
	spriteEntry[numSpriteEntries].x = x;
	spriteEntry[numSpriteEntries].y = y;
	spriteEntry[numSpriteEntries].a = a;
	spriteEntry[numSpriteEntries].opacity = opacity;

	// Now sort it into the ordering table
    if (sortToOT((OT_LENGTH-1)*((float)y/gdl::ScreenYres), &spriteEntry[numSpriteEntries]))
        numSpriteEntries += 1;

}
// Draws all sprites that were registered into the ordering table
void drawSpriteOT() {

    SpriteEntryType *sprite;

    for(int i=0; i<OT_LENGTH; i++) {
        if (spriteOT[i] != NULL) {

            sprite = (SpriteEntryType*)spriteOT[i];
            int op=254*sprite->opacity;

            switch(sprite->spriteType) {
                case 0:
                    sprites.Put(sprite->x, sprite->y-sprite->a, sprite->index, gdl::Color::White, gdl::Pivot, gdl::Pivot, 1.f, 0);
                    break;
                case 1:
                    sprites.Put(sprite->x, sprite->y-sprite->a, sprite->index, RGBA(255, 255, 255, op), gdl::Pivot, gdl::Pivot, sprite->scale, sprite->angle);
                    break;
                case 2:
                    gdl::DrawBoxF(sprite->x-1, (sprite->y-sprite->a)-1, sprite->x+1, (sprite->y-sprite->a)+1, RGBA(255, 255, 0, op));
                    break;
            }

        }
    }

}


// Adds a bouncy sprite into the effects array
void putGib(float x, float y, float a, short spriteIndex) {

    if (numEffects >= MAX_EFFECTS) return;

    // Setup the effect entry
    effect[numEffects].effectNum = 0;
    effect[numEffects].x = x;
    effect[numEffects].y = y;
    effect[numEffects].a = a;

    float dirAngle = 360.f*((float)rand()/RAND_MAX);
    float dirSpeed = 4.f*((float)rand()/RAND_MAX);
    effect[numEffects].xMove = (dirSpeed*sin(dirAngle*ROTPI));
    effect[numEffects].yMove = (dirSpeed*cos(dirAngle*ROTPI));
    effect[numEffects].aMove = -4.f*((float)rand()/RAND_MAX);

    effect[numEffects].angle = 360.f*((float)rand()/RAND_MAX);
    effect[numEffects].angleMove = (16.f*((float)rand()/RAND_MAX))-8.f;
    effect[numEffects].duration = 0-(30*((float)rand()/RAND_MAX));
    effect[numEffects].index = spriteIndex;

    numEffects+=1;

}
// Adds a blood particle into the effects array
void putBlood(float x, float y, float a) {

    if (numEffects >= MAX_EFFECTS) return;

    // Setup the effect entry
    effect[numEffects].effectNum = 1;
    effect[numEffects].x = x;
    effect[numEffects].y = y;
    effect[numEffects].a = a;

    float dirAngle = (360.f*((float)rand()/RAND_MAX));
    float dirSpeed = 4.f*((float)rand()/RAND_MAX);
    effect[numEffects].xMove = (dirSpeed*sin(dirAngle*ROTPI));
    effect[numEffects].yMove = (dirSpeed*cos(dirAngle*ROTPI));
    effect[numEffects].aMove = -(4.f*((float)rand()/RAND_MAX));
    effect[numEffects].duration = 0-(30*((float)rand()/RAND_MAX));

    numEffects += 1;

}
// Adds a fire particle into the effects array
void putFireParticle(float x, float y, float a) {

	if (numEffects >= MAX_EFFECTS) return;

	// Setup the effect entry
	effect[numEffects].effectNum = 3;
	effect[numEffects].x = x;
	effect[numEffects].y = y;
	effect[numEffects].a = a;
	effect[numEffects].xMove = ((3.f*((float)rand()/RAND_MAX))-1.5f)/2.f;
	effect[numEffects].aMove = (1.f*((float)rand()/RAND_MAX));
	effect[numEffects].duration = 0;

	numEffects++;

}
// Process effect entries
void doEffects() {

    for(short i=0; i<numEffects; i++) {

        switch(effect[i].effectNum) {
        case 0: // Gib sprite
            if (effect[i].duration < 240) {

                effect[i].aMove += (0.2f*gdl::Delta);

                effect[i].x += (effect[i].xMove*gdl::Delta);
                effect[i].y += (effect[i].yMove*gdl::Delta);
                effect[i].a -= (effect[i].aMove*gdl::Delta);
                effect[i].angle += (effect[i].angleMove*gdl::Delta);

                if (effect[i].x < 0) {
                    effect[i].x = 0;
                    effect[i].xMove = -effect[i].xMove;
                }
                if (effect[i].x > gdl::ScreenXres-1) {
                    effect[i].x = gdl::ScreenXres-1;
                    effect[i].xMove = -effect[i].xMove;
                }
                if (effect[i].y < 0) {
                    effect[i].y = 0;
                    effect[i].yMove = -effect[i].yMove;
                }
                if (effect[i].y > gdl::ScreenYres-1) {
                    effect[i].y = gdl::ScreenYres-1;
                    effect[i].yMove = -effect[i].yMove;
                }

                if (effect[i].a < 0) {
                    effect[i].a = 0;
                    effect[i].angle = 0;
                    effect[i].angleMove = (effect[i].xMove*4.f);
                    effect[i].aMove = -(fabsf(effect[i].aMove)*0.8f);
                    effect[i].xMove = (effect[i].xMove*0.8f);
                    effect[i].yMove = (effect[i].yMove*0.8f);
                }

                if (ending == false)
					effect[i].duration += (1.f*gdl::Delta);

                addSpriteX(effect[i].x, effect[i].y, effect[i].a, effect[i].angle, 1, 1, effect[i].index);
                addSpriteX(effect[i].x, effect[i].y-1, 0, 0, (100-effect[i].a)/100, 1, SHADOW);

            } else {

                for(short d=i; d<numEffects-1; d++) {
                    effect[d] = effect[d+1];
                }
                numEffects-=1;
                i-=1;

            }

            break;

        case 1: // Blood

            if (effect[i].duration < 240) {

                effect[i].aMove += (0.2f*gdl::Delta);

                effect[i].x += (effect[i].xMove * gdl::Delta);
                effect[i].y += (effect[i].yMove * gdl::Delta);
                effect[i].a -= (effect[i].aMove * gdl::Delta);

                if (effect[i].x < 0) {
                    effect[i].x = 0;
                    effect[i].xMove = -effect[i].xMove;
                }
                if (effect[i].x > gdl::ScreenXres-1) {
                    effect[i].x = gdl::ScreenXres-1;
                    effect[i].xMove = -effect[i].xMove;
                }
                if (effect[i].y < 0) {
                    effect[i].y = 0;
                    effect[i].yMove = -effect[i].yMove;
                }
                if (effect[i].y > gdl::ScreenYres-1) {
                    effect[i].y = gdl::ScreenYres-1;
                    effect[i].yMove = -effect[i].yMove;
                }

                if (effect[i].a < 0) {
                    effect[i].a = 0;
                    effect[i].effectNum = 2;
                }


				if (ending == false)
					effect[i].duration += (1.f * gdl::Delta);

                addSprite(effect[i].x, effect[i].y, effect[i].a, BLOOD);

            } else {

                for(short d=i; d<numEffects-1; d++) {
                    effect[d] = effect[d+1];
                }
                numEffects-=1;
                i-=1;

            }

            break;

        case 2: // Small puddle of blood

            if (effect[i].duration < 240) {

                if (ending == false)
					effect[i].duration += (1.f*gdl::Delta);

                addSprite(effect[i].x, effect[i].y, 0, BLOOD+1);

            } else {

                //Effect(I).Active = FALSE
                for(short d=i; d<numEffects-1; d++) {
                    effect[d] = effect[d+1];
                }
                numEffects-=1;
                i-=1;

            }

            break;

		case 3: // Fire particle

			if (effect[i].duration < 30) {
				addFireParticle(effect[i].x, effect[i].y, effect[i].a, 1.f-(effect[i].duration/30));
				effect[i].x += (effect[i].xMove * gdl::Delta);
				effect[i].a += (effect[i].aMove * gdl::Delta);
				effect[i].aMove = effect[i].aMove + (0.1f * gdl::Delta);
				effect[i].xMove = ((effect[i].xMove*0.8f)*gdl::Delta);
				effect[i].duration += (1 * gdl::Delta);
			} else {

				for(short d=i; d<numEffects-1; d++) {
                    effect[d] = effect[d+1];
                }
                numEffects-=1;
                i-=1;

			}

			break;

        }

    }

}


// Adds a Cirra sprite into the Cirra array
void putCirra(short x, short y, short a) {

    if (numCirras >= MAX_CIRRAS) return;

    cirra[numCirras].x = x;
    cirra[numCirras].y = y;
    cirra[numCirras].a = a;
    cirra[numCirras].aMove = 0;
    cirra[numCirras].state = 0;
    cirra[numCirras].waitTime = 0;
    cirra[numCirras].moveTime = 0;

    numCirras += 1;

}
// Kills a Cirra sprite complete with gibbing effects
void killCirra(short index) {

    if (numCirras == 0) return;

    // Do gib effect
    putGib(cirra[index].x  , cirra[index].y, cirra[index].a+24  , CIRRA_BITS);
    putGib(cirra[index].x-4, cirra[index].y, cirra[index].a+18  , CIRRA_BITS+1);
    putGib(cirra[index].x+4, cirra[index].y, cirra[index].a+18  , CIRRA_BITS+2);
    putGib(cirra[index].x-4, cirra[index].y, cirra[index].a+6   , CIRRA_BITS+3);
    putGib(cirra[index].x+4, cirra[index].y, cirra[index].a+6   , CIRRA_BITS+4);
    putGib(cirra[index].x  , cirra[index].y, cirra[index].a+10  , CIRRA_BITS+5);

	// Add blood particles
    for(short b=0; b<30; b++) {
        putBlood(cirra[index].x+((8*((float)rand()/RAND_MAX))-8), cirra[index].y+((8*((float)rand()/RAND_MAX))-8), cirra[index].a+(32*((float)rand()/RAND_MAX)));
    }

    // Delete the killed cirra entry
    for(short i=index; i<numCirras-1; i++) {
        cirra[i] = cirra[i+1];
    }
    numCirras-=1;

}
// Do Cirra entries
void doCirras() {

    short spriteIndex=0;

	for(short i=0; i<numCirras; i++) {

		switch(cirra[i].state) {
        case 0: // Fall
            if (cirra[i].a <= 0) {
                cirra[i].aMove = 0;
                cirra[i].a = 0;
                cirra[i].state = 1;
            } else {
                cirra[i].aMove += (0.1f*gdl::Delta);
                cirra[i].a -= (cirra[i].aMove*gdl::Delta);
                if (cirra[i].a <= 0) cirra[i].a = 0;
            }

            addSprite(cirra[i].x, cirra[i].y, cirra[i].a, CIRRA_FALL);
            addSpriteX(cirra[i].x, cirra[i].y-1, 0, 0, (float)(100-cirra[i].a)/100, 1.f, SHADOW);

            break;


        case 1: // Hit floor
            if (cirra[i].waitTime < 2) {
                if (cirra[i].waitTime == 0) {
                    sound[0].Play2D(1.f + ((0.6f*((float)rand()/RAND_MAX))-0.2f), 100.f, cirra[i].x-gdl::ScreenCenterX, cirra[i].y-gdl::ScreenCenterY);
                }
                cirra[i].waitTime = cirra[i].waitTime+(1.f*gdl::Delta);
            } else {
                cirra[i].waitTime = 120*((float)rand()/RAND_MAX); //(rnd * 120)
                cirra[i].dir = 0;
                cirra[i].state = 2;
            }

            addSprite(cirra[i].x, cirra[i].y, 0, CIRRA_FALL+1);
            addSprite(cirra[i].x, cirra[i].y-1, 0, SHADOW);

            break;

        case 2: // Roaming

            // Draw the sprite
            if (cirra[i].moveTime > 0) {

                switch(cirra[i].dir) {
                case 0: // Down
                    spriteIndex = CIRRA_DOWN;
                    cirra[i].y += (1.f*gdl::Delta);
                    if (cirra[i].y > (gdl::ScreenYres-1)) {
                        cirra[i].y = gdl::ScreenYres-1;
                        cirra[i].moveTime = 0;
                    }
                    break;
                case 1: // Up
                    spriteIndex = CIRRA_UP;
                    cirra[i].y -= (1.f*gdl::Delta);
                    if (cirra[i].y < 0) {
                        cirra[i].y = 0;
                        cirra[i].moveTime = 0;
                    }
                    break;
                case 2: // Left
                    spriteIndex = CIRRA_LEFT;
                    cirra[i].x -= (1.f*gdl::Delta);
                    if (cirra[i].x < 0) {
                        cirra[i].x = 0;
                        cirra[i].moveTime = 0;
                    }
                    break;
                case 3: // Right
                    spriteIndex = CIRRA_RIGHT;
                    cirra[i].x += (1.f*gdl::Delta);
                    if (cirra[i].x > gdl::ScreenXres-1) {
                        cirra[i].x = gdl::ScreenXres-1;
                        cirra[i].moveTime = 0;
                    }
                    break;
                }

                spriteIndex += roundf(1.f*sin((90.f*cirra[i].frameNum)*ROTPI));

                cirra[i].moveTime -= (1.f*gdl::Delta);
                if (cirra[i].moveTime <= 0) {
                    cirra[i].moveTime = 0;
                    cirra[i].waitTime = (240*((float)rand()/RAND_MAX));
                }

            } else {

                if (cirra[i].waitTime > 0) {

                    cirra[i].waitTime = cirra[i].waitTime-(1.f*gdl::Delta);

                } else {

                    cirra[i].dir = 4*((float)rand()/RAND_MAX);
                    cirra[i].moveTime = 120*((float)rand()/RAND_MAX);

                    if (cirra[i].y >= gdl::ScreenYres-1)    cirra[i].dir=1;
                    if (cirra[i].y <= 0)                    cirra[i].dir=0;

                    if (cirra[i].x >= gdl::ScreenXres-1)    cirra[i].dir=2;
                    if (cirra[i].x <= 0)                    cirra[i].dir=3;

                }

                switch(cirra[i].dir) {
                    case 0: spriteIndex = CIRRA_DOWN;   break;
                    case 1: spriteIndex = CIRRA_UP;     break;
                    case 2: spriteIndex = CIRRA_LEFT;   break;
                    case 3: spriteIndex = CIRRA_RIGHT;  break;
                }

            }

            cirra[i].frameCount = cirra[i].frameCount + (1.f*gdl::Delta);
            while(cirra[i].frameCount >= 10) {
                cirra[i].frameNum = (cirra[i].frameNum+1)%4;
                cirra[i].frameCount = cirra[i].frameCount-10;
            }

            addSprite(cirra[i].x, cirra[i].y, 0, spriteIndex);
            addSprite(cirra[i].x, cirra[i].y-1, 0, SHADOW);

		}

	}

}
