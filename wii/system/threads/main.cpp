/**
 *	Simple threads example with MTek-GDL by Lameguy64/TheCodingBrony
 *
 *	This example demonstrates the use of threads by creating one that increments a
 *	global variable named 'counter' constantly.
 *
 *	Threads are useful when you want to play a short loading animation while loading
 *	a large amount of data (images, sounds, sprite sets, etc.) in the background by
 *	creating a thread that runs a function which loads your data.
 *
 *	The maximum number of possible threads to create is unclear for now.
 *
 */

// Include library and necessary headers
#include <mgdl-wii.h>
#include <unistd.h>


// Counter variable and reset flag
u_int	counter;
bool	resetPressed=false;


// Function prototypes
void *threadCode(void *arg);

void init();
void resetCallback();


// Main function
int main() {


	// Init stuff
	init();

	// Set console mode
	gdl::ConsoleMode();


	// Create thread handle for the thread to be created
	lwp_t myThread = LWP_THREAD_NULL;

	// Create thread
	LWP_CreateThread(	&myThread,			/* thread handle */
						threadCode,			/* code (pointer to a function) */
						NULL,				/* pointer for arguments of function*/
						NULL,				/* stack base */
						16*1024,			/* stack size */
						31					/* priority level (0 is idle, 127 is highest priority) */
	);


	// Clear terminal screen
	printf("\e[1;1H\e[2J");


	// Main loop
	while(1) {

		// Exit if reset was pressed
		if (resetPressed) exit(0);


		// Reset cursor position
		printf("\x1b[1;0H");

		// Print stuff
		printf("Basic Thread Example\n");
		printf("Press RESET to quit...\n\n");

		printf("This value is incremented by the thread we created:\n%d\n", counter);
		printf("\nYet, there is no counter++ inside this loop.");


		// Wait VSync
		VIDEO_WaitVSync();

	}

}


// Thread function for the thread we'll be creating
void *threadCode(void *arg) {

	// Thread loop
	while(1) {

		counter++;	// Add up counter variable

		usleep(10);	// Wait for a few msecs to give way for other processes
					// (this is important when making looped threads)

	}

	// If the thread leaves the function, it is automatically terminated.
	return(NULL);

}


// Init function
void init() {

	// Init system
	gdl::InitSystem(0);

	// Set reset callback
	SYS_SetResetCallback(resetCallback);

}


// Reset callback
void resetCallback() {

	resetPressed=true;

}
