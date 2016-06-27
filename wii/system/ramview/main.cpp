/**
 *	Memory viewer utility function example with MTek-GDL by Lameguy64/TheCodingBrony
 *
 *	This is just a simple memory viewer utility function example which you can easily
 *	incorporate into whatever project you're working on and you need to visually look
 *	at a certain memory region to analyze its data such as getting the exact size of
 *	each GX FIFO command when creating a display list so you can accurately estimate
 *	the required size of the display list buffer.
 *
 *	Everything in this example is pretty self explanatory so there's really not much
 *	to talk much about this.
 *
 */

 // Some includes, the typical stuff
#include <mgdl-wii.h>
#include <wiiuse/wpad.h>
#include "../../common/wiimote.c"


// A prototype
void ViewMemory(void *addr);


// Main function
int main() {


	// Init stuff and then set console mode
	gdl::InitSystem(0);
	gdl::ConsoleMode();

	// Init Wiimote for simple input
	WPAD_Init();


	// Display whatever was stored in myVar and whatever else will be visible
	ViewMemory((void*)0x80000000);


}


// Memory viewer routine
void ViewMemory(void *addr) {

	int		input;
	u_char	*curAddr=(u_char*)addr;
	u_char	*dispAddr[2];

	while(1) {

		printf("\x1b[2J");
		printf("\n\n");


		// Draw a hex editor style display of the specified memory area
		printf("  ADDRESS: %p\n\n", curAddr);

		dispAddr[0]=(u_char*)curAddr;
		for(int cy=0; cy<24; cy++) {

			printf("%p \xba ", dispAddr[0]);

			// Draw hex values
			dispAddr[1] = dispAddr[0];
			for(int cx=0; cx<16; cx++) {

				printf("%02x ", *dispAddr[0]);

				dispAddr[0]++;

			}

			printf("\xba ");

			// Draw valid characters
			for(int cx=0; cx<16; cx++) {

				// Display only characters 32 and onwards, everything else cannot be printed without screwing up the terminal
				if (*dispAddr[1] >= 32) {
					printf("%c", *dispAddr[1]);
				} else {
					printf(".");
				}

				dispAddr[1]++;

			}

			printf("\n");

		}


		// Wait for user input
		while(1) {

			WPAD_ScanPads();
			input = WPAD_ButtonsDown(0);

			// Scroll up
			if (input & WPAD_BUTTON_UP) {
				curAddr-=16;
				break;
			}

			// Scroll down
			if (input & WPAD_BUTTON_DOWN) {
				curAddr+=16;
				break;
			}

			// Exit
			if (input & WPAD_BUTTON_HOME)
				break;

			VIDEO_WaitVSync();

		}

		if (input & WPAD_BUTTON_HOME)	break;

	}

}
