/**
 *	MEM2 allocation example using valloc() with MTek-GDL by Lameguy64/TheCodingBrony
 *
 *	A very simple example for allocating, storing, and reading data to and from MEM2
 *	using valloc().
 *
 *	MEM2 is ideal for storing large amounts of data since the default memory area (MEM1)
 *	has only 19.2MB of usable memory while MEM2 has 51.8MB. However, MEM2 is a tiny bit
 *	slower (access speed is 3.9Gb/s as opposed to MEM1 which is 4.0Gb/s) but the
 *	speed difference is pretty much unnoticable anyway in most cases except data
 *	that is to be read by the GP (such as textures, geometry data, etc).
 *
 *	Take note that valloc() will always return a pointer that is alligned in 2048 byte
 *	increments so something like memalign() is not needed.
 *
 *	As of v0.65 and onwards of MTek-GDL, it is recommended to use the malloc2()/free2()
 *	macros for allocating/deallocating data blocks inside MEM2. They're basically macros
 *	to valloc()/vfree() functions.
 *
 */


// Include main library and valloc() library
#include <mgdl-wii.h>


// Main function
int main() {

	// Init stuff and then set console mode
	gdl::InitSystem(0);
	gdl::ConsoleMode();

	printf("malloc2/valloc function test...\n\n");


	// Create 1MB block
	void *testPtr = { NULL };
	testPtr = malloc2(1024*1024);
	printf("valloc was a success!\n");
	printf("PTR:%p\n", testPtr);


	// Create 32MB block
	void *bigTestPtr = { NULL };
	bigTestPtr = malloc2((1024*1024)*32);
	printf("BIG valloc was a success!\n");
	printf("PTR:%p\n", bigTestPtr);


	// Create 512KB block
	void *testPtr2 = { NULL };
	testPtr2 = malloc2(1024*512);
	printf("valloc was a success!\n");
	printf("PTR:%p\n", testPtr2);


	// Free 1MB block and then allocate it again as a 512KB block
	printf("%p is being freed...\n", testPtr);
	free2(testPtr);
	testPtr = malloc2(1024*512);
	printf("valloc was a success yet again!\n");
	printf("PTR:%p\n", testPtr);


	// Fill random data to the 32MB block
	printf("\nFilling %p with random data...", bigTestPtr);
	char *dataPtr = (char*)bigTestPtr;
	for(int i=0; i<(1024*1024)*32; i++) {
		dataPtr[i] = 255*((float)rand()/RAND_MAX);
	}
	printf("Ok!\n\n");


	// Store a simple string into 512KB block and print it
	printf("Now storing string to %p...", testPtr);
	strcpy((char*)testPtr, "This string is stored inside MEM2!");
	printf("Ok!\n");

	printf("Printing string in %p:\n", testPtr);
	printf("%s\n\n", (char*)testPtr);


	// Pause for 4 seconds and then exit
	printf("Exiting within 4 seconds...\n");
	for(short i=0; i<240; i++)
		VIDEO_WaitVSync();

}
