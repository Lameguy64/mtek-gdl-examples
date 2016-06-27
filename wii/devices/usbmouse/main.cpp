#include <mgdl-wii.h>
#include <mgdl-wii/mgdl-input.h>

#include <ogc/usbmouse.h>

#define MOUSE_BUTTON_1  1
#define MOUSE_BUTTON_2  2
#define MOUSE_BUTTON_3  4

int main(int argc, char *argv[]) {

	gdl::InitSystem();
	gdl::ConsoleMode();

	printf("Hello world!\n");


    if (MOUSE_Init() < 0) {
        printf("ERROR: Could not initialize USB Mouse...");
        exit(EXIT_FAILURE);
    }

    printf("Initialized mouse.\n");


    if (MOUSE_IsConnected()) {
        printf("Mouse is not connected.\n");
        exit(EXIT_FAILURE);
    }
    MOUSE_FlushEvents();

    printf("Mouse is connected.\n");


	gdl::Keyboard::Init();


    int scrollY = 0;
    int bx=0, by=0;
    bool bDrag=false;

    struct MouseStruct {
        short oldButton;
        short button;
        short x,y;
        short scroll;
    } Mouse = {
        0,
        0,
        gdl::ScreenCenterX,
        gdl::ScreenCenterY,
        0
    };

    while(1) {

        // Parse mouse events
        {
            Mouse.oldButton = Mouse.button;
            mouse_event event;
            Mouse.scroll = 0;
            while(MOUSE_GetEvent(&event)) {
                Mouse.button = event.button;
                Mouse.x += event.rx;
                Mouse.y += event.ry;
                Mouse.scroll += event.rz;
            }
        }

        if (Mouse.x < 0)
            Mouse.x = 0;
        if (Mouse.y < 16)
            Mouse.y = 16;
        if (Mouse.x > gdl::ScreenXres-1)
            Mouse.x = gdl::ScreenXres-1;
        if (Mouse.y > gdl::ScreenYres-17)
            Mouse.y = gdl::ScreenYres-17;


        scrollY += Mouse.scroll;

        if (scrollY < 0)
            scrollY = 0;
        if (scrollY > 256)
            scrollY = 256;


        gdl::PrepDisplay();


        if (Mouse.button & MOUSE_BUTTON_1) {
            gdl::DrawBoxF(0, 16, 32, 16+32, gdl::Color::Red);
            if (!(Mouse.oldButton & MOUSE_BUTTON_1)) {
                bx = Mouse.x;
                by = Mouse.y;
                bDrag = true;
            }
        } else {
            bDrag = false;
        }
        if (Mouse.button & 2) gdl::DrawBoxF(32, 16, 32+32, 16+32, gdl::Color::Green);
        if (Mouse.button & 4) gdl::DrawBoxF(64, 16, 64+32, 16+32, gdl::Color::Blue);


        gdl::DrawBoxF(0, 64+scrollY, 32, (64+16)+scrollY, gdl::Color::Red);

        if (bDrag)
            gdl::DrawBoxF(bx, by, Mouse.x, Mouse.y, RGBA(0, 127, 255, 191));

        // Draw cursor
        gdl::DrawLine(Mouse.x-5, Mouse.y-1, Mouse.x+6, Mouse.y-1, gdl::Color::White);
        gdl::DrawLine(Mouse.x-5, Mouse.y+1, Mouse.x+6, Mouse.y+1, gdl::Color::White);
        gdl::DrawLine(Mouse.x-5, Mouse.y, Mouse.x+6, Mouse.y, gdl::Color::White);
        gdl::DrawLine(Mouse.x-1, Mouse.y-5, Mouse.x-1, Mouse.y+6, gdl::Color::White);
        gdl::DrawLine(Mouse.x+1, Mouse.y-5, Mouse.x+1, Mouse.y+6, gdl::Color::White);
        gdl::DrawLine(Mouse.x, Mouse.y-5, Mouse.x, Mouse.y+6, gdl::Color::White);

        gdl::Display();

    }


}
