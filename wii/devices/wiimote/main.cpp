/**
 *	Wiimote example with MTek-GDL by Lameguy64/TheCodingBrony
 *
 *	Demonstrates the standard capabilities of the Wiimote and the Nunchuk attachment.
 *	Support for other attachments be added soon.
 *
 *	The only feature I couldn't get to work is the speaker of the Wiimote. WPAD appears to have
 *	relevant functions to that but there's no documentation or examples on how to use it and when
 *	I tried to figure it out, I could barely get it to work before the console locks up.
 *
 *	Take note that 3rd party wiimotes and attachments (mostly non-officially licensed ones) may
 *	not work with wiiuse for some reason even though such peripherals work well with official games.
 *	Officially licensed third-party accessories however (like Hori controllers) will work.
 *
 *	Press A and B together on the Wiimote to trigger Rumble.
 *
 *	Changelog:
 *		04-15-2015: Added Classic Controller (Pro) suport.
 *
 */


// Include library and WPAD header
#include <mgdl-wii.h>
#include <wiiuse/wpad.h>

// Include my generic wiimote handler
#include "../../common/wiimote.c"


// Function prototypes
void init();


// Main function
int main() {


	// Init system and setup console
	init();
	gdl::ConsoleMode();


	// Load font sheet and bind it to a font handler
	gdl::Image fontImage;
	if (!fontImage.LoadImage("testdata/font8x16.png", gdl::Nearest, gdl::RGB5A3))
		exit(0);

	gdl::FFont font;
	font.BindSheet(fontImage, 8, 16);


	// Load pointer
	gdl::Image pointer;
	if (!pointer.LoadImage("testdata/pointer.png", gdl::Linear, gdl::RGB5A3))
		exit(0);


	char	textBuff[256]={0};
	int		ty=0;


	while(1) {


		// Get Wiimote data
		CheckWiimotes();


		// Prepare display
		gdl::PrepDisplay();

		ty = 32;
		Wiimote[0].rumble = false;
		if (Wiimote[0].status == WPAD_ERR_NONE) {

			// Print out battery level
			font.Printf(8, ty, 1, 0xffffffff, "Battery Level: %d/127", Wiimote[0].data.battery_level);
			ty += 32;

			// Print out buttons pressed
			textBuff[0]=0x00;
			if (Wiimote[0].data.btns_h & WPAD_BUTTON_HOME)	exit(0);
			if (Wiimote[0].data.btns_h & WPAD_BUTTON_A)		strcat(textBuff, "A ");
			if (Wiimote[0].data.btns_h & WPAD_BUTTON_B)		strcat(textBuff, "B ");
			if (Wiimote[0].data.btns_h & WPAD_BUTTON_PLUS)	strcat(textBuff, "+ ");
			if (Wiimote[0].data.btns_h & WPAD_BUTTON_MINUS)	strcat(textBuff, "- ");
			if (Wiimote[0].data.btns_h & WPAD_BUTTON_UP)	strcat(textBuff, "UP ");
			if (Wiimote[0].data.btns_h & WPAD_BUTTON_DOWN)	strcat(textBuff, "DOWN ");
			if (Wiimote[0].data.btns_h & WPAD_BUTTON_LEFT)	strcat(textBuff, "LEFT ");
			if (Wiimote[0].data.btns_h & WPAD_BUTTON_RIGHT)	strcat(textBuff, "RIGHT ");
			if (Wiimote[0].data.btns_h & WPAD_BUTTON_1)		strcat(textBuff, "1 ");
			if (Wiimote[0].data.btns_h & WPAD_BUTTON_2)		strcat(textBuff, "2 ");
			font.Printf(8, ty, 1, 0xffffffff, "Button(s) pressed: %s", textBuff);
			ty+=32;

			// Trigger rumble if A and B have been pressed together
			if ((Wiimote[0].data.btns_h & WPAD_BUTTON_A) && (Wiimote[0].data.btns_h & WPAD_BUTTON_B))
				Wiimote[0].rumble = true;

			// Print out gyro/accelerometer stats as well
			font.DrawText("Gyro/Accel Stats:", 8, ty, 1, 0xffffffff);
			ty+=16;
			font.Printf(8, ty, 1, 0xffffffff, "Roll: %3.2f  Yaw: %3.2f  Pitch: %3.2f", Wiimote[0].data.orient.roll, Wiimote[0].data.orient.yaw, Wiimote[0].data.orient.pitch);
			ty+=16;
			font.Printf(8, ty, 1, 0xffffffff, "X:%d Y:%d Z:%d", Wiimote[0].data.accel.x, Wiimote[0].data.accel.y, Wiimote[0].data.accel.z);
			ty+=32;


			// Print out pointer stats
			if (Wiimote[0].status == WPAD_ERR_NONE) {
				if (Wiimote[0].data.ir.valid) {
					font.DrawText("Pointer stats:", 8, ty, 1, 0xffffffff);
					ty+=16;
					font.Printf(8, ty, 1, 0xffffffff, "X:%3.2f Y:%3.2f A:%3.2f", Wiimote[0].data.ir.x, Wiimote[0].data.ir.y, Wiimote[0].data.ir.angle);
					ty+=32;
				}
			}


			// Print out nunchuk stats (if connected)
			if (Wiimote[0].data.exp.type == EXP_NUNCHUK) {

				// Print out analog stick stats
				font.DrawText("Nunchuk Stick Stats:", 8, ty, 1, 0xffffffff);
				ty+=16;
				font.Printf(8, ty, 1, 0xffffffff, "X:%d Y:%d", Wiimote[0].data.exp.nunchuk.js.pos.x-127, Wiimote[0].data.exp.nunchuk.js.pos.y-127);
				ty+=32;

				// Print out button stats
				textBuff[0]=0x00;
				if (Wiimote[0].data.exp.nunchuk.btns_held & NUNCHUK_BUTTON_Z) strcat(textBuff, "Z ");
				if (Wiimote[0].data.exp.nunchuk.btns_held & NUNCHUK_BUTTON_C) strcat(textBuff, "C ");
				font.Printf(8, ty, 1, 0xffffffff, "Nunchuk Button(s) Pressed: %s", textBuff);
				ty+=32;


				// Print out gyroscope and accelerometer stats
				font.DrawText("Nunchuk Gyro/Accel Stats:", 8, ty, 1, 0xffffffff);
				ty+=16;

				// Print out nunchuk's orientation
				font.Printf(8, ty, 1, 0xffffffff, "Roll: %3.2f Pitch: %3.2f",
					Wiimote[0].data.exp.nunchuk.orient.roll,
					Wiimote[0].data.exp.nunchuk.orient.pitch);
				ty+=16;


				// Print out nunchuk's accelerometer data
				font.Printf(8, ty, 1, 0xffffffff, "X:%3.2f Y:%3.2f, Z:%3.2f",
					100*Wiimote[0].data.exp.nunchuk.gforce.x,
					100*Wiimote[0].data.exp.nunchuk.gforce.y,
					100*Wiimote[0].data.exp.nunchuk.gforce.z);


			} else if (Wiimote[0].data.exp.type == EXP_CLASSIC) {

				font.DrawText("Classic Controller Stats:", 8, ty, 1, 0xffffffff);
				ty+=16;

				font.Printf(8, ty, 1, 0xffffffff, "LStick X:%d Y:%d", Wiimote[0].data.exp.classic.ljs.pos.x, Wiimote[0].data.exp.classic.ljs.pos.y);
				ty+=16;
				font.Printf(8, ty, 1, 0xffffffff, "RStick X:%d Y:%d", Wiimote[0].data.exp.classic.rjs.pos.x, Wiimote[0].data.exp.classic.rjs.pos.y);
				ty+=16;

				font.Printf(8, ty, 1, 0xffffffff, "LT:%1.2f", Wiimote[0].data.exp.classic.l_shoulder);
				ty+=16;
				font.Printf(8, ty, 1, 0xffffffff, "RT:%1.2f", Wiimote[0].data.exp.classic.r_shoulder);
				ty+=32;

				textBuff[0] = 0x00;
				if (Wiimote[0].data.exp.classic.btns_held & CLASSIC_CTRL_BUTTON_A)			strcat(textBuff, "A ");
				if (Wiimote[0].data.exp.classic.btns_held & CLASSIC_CTRL_BUTTON_B)			strcat(textBuff, "B ");
				if (Wiimote[0].data.exp.classic.btns_held & CLASSIC_CTRL_BUTTON_X)			strcat(textBuff, "X ");
				if (Wiimote[0].data.exp.classic.btns_held & CLASSIC_CTRL_BUTTON_Y)			strcat(textBuff, "Y ");
				if (Wiimote[0].data.exp.classic.btns_held & CLASSIC_CTRL_BUTTON_FULL_L)	strcat(textBuff, "L ");
				if (Wiimote[0].data.exp.classic.btns_held & CLASSIC_CTRL_BUTTON_FULL_R)	strcat(textBuff, "R ");
				if (Wiimote[0].data.exp.classic.btns_held & CLASSIC_CTRL_BUTTON_ZL)		strcat(textBuff, "ZL ");
				if (Wiimote[0].data.exp.classic.btns_held & CLASSIC_CTRL_BUTTON_ZR)		strcat(textBuff, "ZR ");
				if (Wiimote[0].data.exp.classic.btns_held & CLASSIC_CTRL_BUTTON_UP)		strcat(textBuff, "UP ");
				if (Wiimote[0].data.exp.classic.btns_held & CLASSIC_CTRL_BUTTON_DOWN)		strcat(textBuff, "DOWN ");
				if (Wiimote[0].data.exp.classic.btns_held & CLASSIC_CTRL_BUTTON_LEFT)		strcat(textBuff, "LEFT ");
				if (Wiimote[0].data.exp.classic.btns_held & CLASSIC_CTRL_BUTTON_RIGHT)		strcat(textBuff, "RIGHT ");
				if (Wiimote[0].data.exp.classic.btns_held & CLASSIC_CTRL_BUTTON_PLUS)		strcat(textBuff, "+ ");
				if (Wiimote[0].data.exp.classic.btns_held & CLASSIC_CTRL_BUTTON_MINUS)		strcat(textBuff, "- ");
				if (Wiimote[0].data.exp.classic.btns_held & CLASSIC_CTRL_BUTTON_HOME)		strcat(textBuff, "HOME ");

				font.Printf(8, ty, 1, 0xffffffff, "Button Stats: %s", textBuff);

			}

		} else {

			font.DrawText("At least one Wiimote must be connected.", 8, ty, 1, 0xffffffff);

		}


		// Draw pointer for all wiimotes
		for(short i=0; i<4; i++) {

			if (Wiimote[i].active) {
				if (Wiimote[i].status == WPAD_ERR_NONE) {
					if (Wiimote[i].data.ir.valid) {
						pointer.PutX(Wiimote[i].data.ir.x, Wiimote[i].data.ir.y, 4, 0, Wiimote[i].data.ir.angle, 2, 0xffffffff);
					}
				}
			}

		}


		// Display everything
		gdl::Display();


	}

}


// Init function
void init() {

	// Init file system library
	fatInitDefault();

	// Init system and set background color
	gdl::InitSystem(0);
	gdl::SetClearColor(0, 31, 63, 0);

	// Init wiimote subsystem
	WPAD_Init();
	InitWiimotes(4);	// Initialize for all 4 wiimotes (using my generic Wiimote handler)

}
