#include <unistd.h>
#include <mgdl-wii.h>
#include <mgdl-wii/mgdl-input.h>

typedef struct {
    USBKeyboard_event keyEvent;
    bool keyHit;
} KeyStatStrict;


KeyStatStrict KeyStat;
int highKey = 0;


bool keyHit = false;
int  keyCode;
int  keyScancode;
int  keyAction;
int	 keyMods;
int	 keyCount;

//extern char _input_Default_KeyMap[];

void KeyCallback(int key, int scancode, int action, int mods) {

	keyHit		= true;
	keyCode		= key;
	keyScancode	= scancode;
	keyAction	= action;
	keyMods		= mods;

	keyCount++;

	if (key == gdl::Keyboard::KEY_ESCAPE)
		gdl::wii::Exit();

}

void KeyboardConnected(bool connected) {

    if (connected) {
        printf("Keyboard was connected.\n");
    } else {
        printf("Keyboard was disconnected.\n");
    }

}

int main(int argc, char *argv[]) {

	fatInitDefault();
	gdl::InitSystem();
	gdl::ConsoleMode();


	gdl::Image fontImage;
	gdl::FFont font;

    if (!fontImage.LoadImage("testdata/font8x16.png", gdl::Nearest, gdl::I4)) {
		exit(0);
    }
    font.BindSheet(fontImage, 8, 16);




	printf("Initializing USB mouse subsystem...\n");
	gdl::Mouse::Init();
	printf("Ok.\n");

    printf("Initializing keyboard driver...\n");
    if (!gdl::Keyboard::Init()) {
        printf("Could not initialize driver.\n");
        exit(EXIT_FAILURE);
    }


	/*
	printf("\n_");

	while(1) {

		char c = getchar();

		if (c == '\e') {
			printf("Exiting...");
			exit(0);
		}

		printf("\b \b%c_", c);
		VIDEO_WaitVSync();

	}
	*/


    float x=gdl::ScreenCenterX,y=gdl::ScreenCenterY;
	float xMove=0,yMove=0;
	char charBuff[32] = { 0 };

	gdl::Keyboard::SetKeyCallback(KeyCallback);

    while(1) {

		if (gdl::Keyboard::KeyDown(gdl::Keyboard::KEY_W))
			yMove -= 0.5f;
		if (gdl::Keyboard::KeyDown(gdl::Keyboard::KEY_S))
			yMove += 0.5f;
		if (gdl::Keyboard::KeyDown(gdl::Keyboard::KEY_A))
			xMove -= 0.5f;
		if (gdl::Keyboard::KeyDown(gdl::Keyboard::KEY_D))
			xMove += 0.5f;

		x += xMove;
		y += yMove;
		xMove *= 0.94f;
		yMove *= 0.94f;

		if (floor(x) < 8) {
			x = 8;
			xMove = 0;
		}
		if (floor(x) > gdl::ScreenXres-9) {
			x = gdl::ScreenXres-9;
			xMove = 0;
		}
		if (floor(y) < 40) {
			y = 40;
			yMove = 0;
		}
		if (floor(y) > gdl::ScreenYres-41) {
			y = gdl::ScreenYres-41;
			yMove = 0;
		}


		/*
		{

			char c = gdl::Keyboard::GetChar();

			if (c == 8) {

				if (strlen(charBuff) > 0)
					charBuff[strlen(charBuff)-1] = 0x00;

			} else if (c > 0) {

				if (strlen(charBuff) < 31)
					strncat(charBuff, &c, 1);

			}

		}
		*/



		gdl::PrepDisplay();


		font.Printf(8, 32, 1.f, gdl::Color::White, "Hello World!");
		//font.Printf(8, 48, 1.f, gdl::Color::White, "%s<", charBuff);



        gdl::DrawBoxF(floor(x)-4, floor(y)-4, floor(x)+4, floor(y)+4, gdl::Color::LightBlue);


		if (keyHit) {
			font.Printf(8, 48, 1.f, gdl::Color::White, "KEY HIT!", charBuff);
			keyHit = false;
		}

		font.Printf(8, 64, 1.f, gdl::Color::White, "K:%d S:%d A:%d M:%d", keyCode, keyScancode, keyAction, keyMods);
		font.Printf(8, 80, 1.f, gdl::Color::White, "C:%d", keyCount);


        // Draw cursor
        if (gdl::Mouse::GetStats().inWindow) {

			gdl::DrawLine(gdl::Mouse::GetStats().x-5, gdl::Mouse::GetStats().y-1, gdl::Mouse::GetStats().x+6, gdl::Mouse::GetStats().y-1, gdl::Color::White);
			gdl::DrawLine(gdl::Mouse::GetStats().x-5, gdl::Mouse::GetStats().y+1, gdl::Mouse::GetStats().x+6, gdl::Mouse::GetStats().y+1, gdl::Color::White);
			gdl::DrawLine(gdl::Mouse::GetStats().x-5, gdl::Mouse::GetStats().y, gdl::Mouse::GetStats().x+6, gdl::Mouse::GetStats().y, gdl::Color::White);
			gdl::DrawLine(gdl::Mouse::GetStats().x-1, gdl::Mouse::GetStats().y-5, gdl::Mouse::GetStats().x-1, gdl::Mouse::GetStats().y+6, gdl::Color::White);
			gdl::DrawLine(gdl::Mouse::GetStats().x+1, gdl::Mouse::GetStats().y-5, gdl::Mouse::GetStats().x+1, gdl::Mouse::GetStats().y+6, gdl::Color::White);
			gdl::DrawLine(gdl::Mouse::GetStats().x, gdl::Mouse::GetStats().y-5, gdl::Mouse::GetStats().x, gdl::Mouse::GetStats().y+6, gdl::Color::White);

        }

		gdl::Display();


    }

    printf("Exiting...");


}
