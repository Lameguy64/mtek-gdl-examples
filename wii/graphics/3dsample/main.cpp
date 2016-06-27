/**	3D model viewer example with MTek-GDL by Lameguy64/TheCodingBrony
 *
 *	This example demonstrates rendering a series of 3D models in Stanford PLY format
 *	complete with simple lighting and texture mapping. It makes use of vertex arrays
 *	and display lists to render models at full speed.
 *
 *	Remember that GX_VTXFMT0 is reserved to MTek-GDL's 2D graphics functions so keep
 *	that in mind when setting up your own vertex descriptor format.
 *
 *	PLY models are handled by plylib.cpp which can be very useful for 3D prototyping.
 *
 *	Controls (requires Nunchuk):
 *		1				- Toggle between automatic circle-cam or manual FPS camera.
 *						  (requires Nunchuk to be able to move around)
 *		D-Pad			- Look.
 *		B				- Look using IR pointer.
 *		Home			- Exit.
 *		Nunchuk Stick	- Move around.
 *		Nunchuk Z		- Move downward (not relative to camera angle).
 *		Nunchuk	C		- Move upward (not relative to camera angle).
 *
 */


// Include necessary headers
#include <mgdl-wii.h>
#include <wiiuse/wpad.h>

#include "../../common/wiimote.c"
#include <plylib.cpp>



// Function prototypes
void init();


// Main function
int main() {


	// Init and set console mode
	init();
	gdl::ConsoleMode();


	// Load an image as our floor
	gdl::Image	texImage;
	texImage.LoadImageMipmapped("testdata/barb.png", gdl::LN_MM_LN, gdl::Linear, 9, gdl::RGB565);


	// Allocate objects for our model and its textures
	ply::Model	bulb;
	ply::Model	model[8];
	gdl::Image	modelTex[6];


	// Load models
	if (bulb.LoadModel("testdata/bulb.ply")) {
		printf("Could not load testdata/bulb.ply.\n");
		exit(0);
	}
	if (model[0].LoadModel("testdata/rarity_model/rarity_body.ply")) {
		printf("Could not load testdata/rarity_body.ply.\n");
		exit(0);
	}
	if (model[1].LoadModel("testdata/rarity_model/rarity_eyes.ply")) {
		printf("Could not load testdata/rarity_eyes.ply.\n");
		exit(0);
	}
	if (model[2].LoadModel("testdata/rarity_model/rarity_horn.ply")) {
		printf("Could not load testdata/rarity_horn.ply.\n");
		exit(0);
	}
	if (model[3].LoadModel("testdata/rarity_model/rarity_hair_front.ply")) {
		printf("Could not load testdata/rarity_hair_front.ply.\n");
		exit(0);
	}
	if (model[4].LoadModel("testdata/rarity_model/rarity_hair_back.ply")) {
		printf("Could not load testdata/rarity_hair_back.ply.\n");
		exit(0);
	}
	if (model[5].LoadModel("testdata/rarity_model/rarity_tail.ply")) {
		printf("Could not load testdata/rarity_tail.ply.\n");
		exit(0);
	}
	if (model[6].LoadModel("testdata/rarity_model/rarity_eyelash_lower.ply")) {
		printf("Could not load testdata/rarity_eyelash_lower.ply.\n");
		exit(0);
	}
	if (model[7].LoadModel("testdata/rarity_model/rarity_eyelash_upper.ply")) {
		printf("Could not load testdata/rarity_eyelash_upper.ply.\n");
		exit(0);
	}

	// Load textures for the models
	if (!modelTex[0].LoadImage("testdata/rarity_model/rarity_body.png", gdl::Linear, gdl::RGB565))
		exit(0);
	if (!modelTex[1].LoadImage("testdata/rarity_model/rarity_eyes.png", gdl::Linear, gdl::RGB565))
		exit(0);
	if (!modelTex[2].LoadImage("testdata/rarity_model/rarity_horn.png", gdl::Linear, gdl::RGB565))
		exit(0);
	if (!modelTex[3].LoadImage("testdata/rarity_model/rarity_hair_front.png", gdl::Linear, gdl::RGB565))
		exit(0);
	if (!modelTex[4].LoadImage("testdata/rarity_model/rarity_hair_back.png", gdl::Linear, gdl::RGB565))
		exit(0);
	if (!modelTex[5].LoadImage("testdata/rarity_model/rarity_tail.png", gdl::Linear, gdl::RGB565))
		exit(0);


	// Simple camera stuff
	struct {
		float accel;		// Acceleration
		float x,y,z;		// Position
		float vx,vy,vz;		// Movement velocities
		float pa,ta;		// Pan and tilt angles
		float vpa,vta,vra;	// Pan and tilt velocities
	} Camera = { 0 };

	struct {
		float x,y,z;
	} Pos={0};


	// For automatic camera and light source rotation
	float	a=0;
	bool	autoCam=true;
	bool	b2Pressed=false;


	#define J 255.f/360*3
	#define K 255.f/360*2
	#define L 255.f/360*1


	// Main loop
	int maxRarities = 1;
	bool plusPressed = 0;
	bool minusPressed = 0;

	while(1) {

		CheckWiimotes();


		// Handle Wiimote input and camera controls
		if (Wiimote[0].status == WPAD_ERR_NONE) {

			if ((Wiimote[0].data.btns_l & WPAD_BUTTON_PLUS) && (maxRarities < 81)) {

				if (plusPressed == false)
					maxRarities++;

				plusPressed = true;

			} else {

				plusPressed = false;

			}

			if ((Wiimote[0].data.btns_l & WPAD_BUTTON_MINUS) && (maxRarities > 0)) {

				if (minusPressed == false)
					maxRarities--;

				minusPressed = true;

			} else {

				minusPressed = false;

			}

			// Camera controls
			if (autoCam == false) {

				// Acceleration (with the analog stick)
				Pos.x += (Wiimote[0].data.accel.x-512.f)/1024.f;
				Pos.y += (Wiimote[0].data.accel.y-512.f)/1024.f;

				// Looking
				if (Wiimote[0].data.btns_h & WPAD_BUTTON_B) {
					if (Wiimote[0].data.ir.valid) {
						Camera.vpa +=  ((Wiimote[0].data.ir.x-gdl::ScreenCenterX)/gdl::ScreenCenterX)/1.5f;
						Camera.vta +=  ((Wiimote[0].data.ir.y-gdl::ScreenCenterY)/gdl::ScreenCenterY)/1.5f;
					}
				}

				if (Wiimote[0].data.btns_h & WPAD_BUTTON_UP)	Camera.vta -= 0.3f;
				if (Wiimote[0].data.btns_h & WPAD_BUTTON_DOWN)	Camera.vta += 0.3f;
				if (Wiimote[0].data.btns_h & WPAD_BUTTON_LEFT)	Camera.vpa -= 0.3f;
				if (Wiimote[0].data.btns_h & WPAD_BUTTON_RIGHT)	Camera.vpa += 0.3f;

				// Movement
				if (Wiimote[0].data.exp.type == EXP_NUNCHUK) {

					if (abs(Wiimote[0].data.exp.nunchuk.js.pos.x-127) > 8) {

						Camera.accel = ((f32)(Wiimote[0].data.exp.nunchuk.js.pos.x-127)/127);
						Camera.vx += (Camera.accel*cos(Camera.pa*ROTPI));
						Camera.vz += (Camera.accel*sin(Camera.pa*ROTPI));

					}

					if (abs(Wiimote[0].data.exp.nunchuk.js.pos.y-127) > 8) {

						Camera.accel = ((f32)(Wiimote[0].data.exp.nunchuk.js.pos.y-127)/127);
						Camera.vz -= ((Camera.accel*cos(Camera.pa*ROTPI))*cos(Camera.ta*ROTPI));
						Camera.vx += ((Camera.accel*sin(Camera.pa*ROTPI))*cos(Camera.ta*ROTPI));
						Camera.vy -= (Camera.accel*sin(Camera.ta*ROTPI));

					}

					if (Wiimote[0].data.exp.nunchuk.btns_held & NUNCHUK_BUTTON_C)
						Camera.vy += 0.5f;

					if (Wiimote[0].data.exp.nunchuk.btns_held & NUNCHUK_BUTTON_Z)
						Camera.vy -= 0.5f;

				}

			}

			// Toggle between 'automatic' (or circle-cam) and 'manual' camera control
			if (Wiimote[0].data.btns_h & WPAD_BUTTON_1) {

				if (b2Pressed==false)
					autoCam ^= 1;

				b2Pressed=true;

			} else {

				b2Pressed=false;

			}

			// Exit if home is pressed
			if (Wiimote[0].data.btns_h & WPAD_BUTTON_HOME)
				gdl::wii::Exit();

			if (Wiimote[0].data.btns_h & WPAD_BUTTON_2)
				gdl::SaveScreen("/screencap.png");

		}


		// Calculate camera position and angle (with gdl::Delta so that speed will be consistent on PAL systems)
		Camera.x += Camera.vx*gdl::Delta;	Camera.vx *= 0.85f;
		Camera.y += Camera.vy*gdl::Delta;	Camera.vy *= 0.85f;
		Camera.z += Camera.vz*gdl::Delta;	Camera.vz *= 0.85f;

		Camera.pa += Camera.vpa*gdl::Delta;	Camera.vpa *= 0.85f;
		Camera.ta += Camera.vta*gdl::Delta;	Camera.vta *= 0.85f;


		// Circle-cam stuff (with gdl::Delta so that speed will be consistent on PAL systems)
		a+=0.3f*gdl::Delta;

		if (autoCam) {
			Camera.pa = -a;
			Camera.ta = 20;
			Camera.x = 1000*sin(a*ROTPI);
			Camera.y = 200;
			Camera.z = 1000*cos(a*ROTPI);
		}


		// Prepare display and set 3D projection mode
		gdl::PrepDisplay();
		gdl::Set3DMode(5000);

		gdl::SetClipBox(20, 100, gdl::ScreenXres-21, 300);

		// Matrix stuff
		guVector tempRot;
		Mtx tempMatrix;


		// Rotate matrix according to the camera
		tempRot.x = 1; tempRot.y = 0; tempRot.z = 0;
		guMtxRotAxisDeg(tempMatrix, &tempRot, Camera.ta);
		guMtxConcat(gdl::wii::ModelMtx, tempMatrix, gdl::wii::ModelMtx);

		tempRot.x = 0; tempRot.y = 1; tempRot.z = 0;
		guMtxRotAxisDeg(tempMatrix, &tempRot, Camera.pa);
		guMtxConcat(gdl::wii::ModelMtx, tempMatrix, gdl::wii::ModelMtx);


		// Set it as our normal matrix (required to get lighting working properly)
		GX_LoadNrmMtxImm(gdl::wii::ModelMtx, GX_PNMTX0);


		// Translate matrix according to the camera and then set it as our position matrix
		guMtxApplyTrans(gdl::wii::ModelMtx, gdl::wii::ModelMtx, -Camera.x/8.f, -Camera.y/8.f, -Camera.z/8.f);
		GX_LoadPosMtxImm(gdl::wii::ModelMtx, GX_PNMTX0);


		// Set texture for the bottom planes
		GX_LoadTexObj(texImage.Texture.TexObj(), GX_TEXMAP0);
		GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);


		// Set vertex descriptors and then draw a simple quad
		GX_ClearVtxDesc();
		GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
		GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
		GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

		GX_Begin(GX_QUADS, GX_VTXFMT1, 4);

			// Far-left
			GX_Position3f32(-200.0f, -50.0f, -200.0f);
			GX_Color3u8(255, 255, 255);
			GX_TexCoord2f32(0.f, 0.f);

			// Far-right
			GX_Position3f32(200.0f, -50.0f, -200.0f);
			GX_Color3u8(255, 255, 255);
			GX_TexCoord2f32(1.f, 0.f);

			// Near-right
			GX_Position3f32(200.0f, -50.0f, 200.0f);
			GX_Color3u8(255, 255, 255);
			GX_TexCoord2f32(1.f, 1.f);

			// Near-left
			GX_Position3f32(-200.0f, -50.0f, 200.0f);
			GX_Color3u8(255, 255, 255);
			GX_TexCoord2f32(0.f, 1.f);

		GX_End();


		// Calculate position for the light source
		float lx=100.f*(float)cos((a*4)*ROTPI), ly=50.f, lz=100.f*(float)sin((a*4)*ROTPI);


		// Enable back-face culling (recommended when rendering models)
		GX_SetCullMode(GX_CULL_FRONT);


		// Render bulb
		guMtxApplyTrans(gdl::wii::ModelMtx, tempMatrix, lx, ly, lz);
		guMtxApplyScale(tempMatrix, tempMatrix, 4.f, 4.f, 4.f);
		GX_LoadPosMtxImm(tempMatrix, GX_PNMTX0);

		GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
		bulb.RenderModel();


		// Set light source
		GXLightObj light;
		guVector lpos = { lx, ly, lz };

		guVecMultiply(gdl::wii::ModelMtx, &lpos, &lpos);
		GX_InitLightPos(&light, lpos.x, lpos.y, lpos.z);
		GX_InitLightColor(&light, (GXColor) { 255, 255, 255, 0xFF });
		GX_InitLightSpot(&light, 0.0f, GX_SP_OFF);
		GX_InitLightDistAttn(&light, 20.0f, 1.0f, GX_DA_MEDIUM); // DistAttn = 20.0  &  Brightness=1.0f (full)
		GX_LoadLightObj(&light, GX_LIGHT0);

		// Prepare color channel for rendering the models
		GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT0, GX_DF_CLAMP, GX_AF_SPOT);
		GX_SetChanAmbColor(GX_COLOR0A0, (GXColor){ 191, 191, 191, 0xFF});

		{

			int drawCount = 0;

			for (int my=-2; my<=2; my++) {
				for (int mx=-2; mx<=2; mx++) {

					// Scale things up for the model
					guMtxApplyTrans(gdl::wii::ModelMtx, tempMatrix, 30*mx, 0.f, 60*my);
					guMtxApplyScale(tempMatrix, tempMatrix, 8.f, 8.f, 8.f);
					GX_LoadPosMtxImm(tempMatrix, GX_PNMTX0);

					// Render the models
					for(short i=0; i<6; i++) {
						GX_LoadTexObj(modelTex[i].Texture.TexObj(), GX_TEXMAP0);	// Set appropriate texture
						GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);						// Set proper tevstage
						model[i].RenderModel();									// Render model
					}

					// Render the untextured parts of the model
					GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);		// Disable texture mapping
					model[6].RenderModel();
					model[7].RenderModel();

					drawCount++;
					if (drawCount >= maxRarities)
						break;

				}

				if (drawCount >= maxRarities)
					break;

			}

		}


		// Set to 2D matrix mode and reset color channel
		gdl::Set2DMode(100);
		GX_SetCullMode(GX_CULL_NONE);
		GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_VTX, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);


		//gdl::DrawBoxF(16, 40, 16+(400*gdl::Delta), 59, gdl::Color::Yellow);
		gdl::DrawBoxF(16, 40, 16+(400*gdl::wii::CPUdelta), 59, gdl::Color::Blue);
		gdl::DrawBoxF(16, 60, 16+(400*gdl::wii::GPUdelta), 79, gdl::Color::Red);
		//gdl::DrawBoxF(16, 80, 16+(400*(gdl::wii::CPUdelta+gdl::wii::GPUdelta)/2), 99, gdl::Color::Yellow);
		gdl::DrawLine(16, 40, 16, 79, gdl::Color::White);
		gdl::DrawLine(216, 40, 216, 79, gdl::Color::White);
		gdl::DrawLine(416, 40, 416, 79, gdl::Color::White);
		gdl::DrawLine(616, 40, 616, 79, gdl::Color::White);


		// Draw pointer
		if (Wiimote[0].status == WPAD_ERR_NONE) {
			if (Wiimote[0].data.ir.valid) {

				gdl::DrawBoxF(Wiimote[0].data.ir.x-4, Wiimote[0].data.ir.y-4, Wiimote[0].data.ir.x+4, Wiimote[0].data.ir.y+4, 0xffffffff);

			}
		}


		/*
		{

			void *tempBuff =malloc(1024*1024*3);
            char *tempPtr = (char*)tempBuff;

			for(int py=0; py<104; py++) {
				for(int px=0; px<104; px++) {

					short col = (sin(px*PI/270)*255)+(sin(py*PI/45)*31)+(sin(((py+px)+(4*a))*PI/90)*63);

					int r = 255*fabs(sin((J*col)*ROTPI));
					*tempPtr = r;
					tempPtr++;

					int g = 255*fabs(sin((K*col)*ROTPI));
					*tempPtr = g;
					tempPtr++;

					int b = 255*fabs(sin((L*col)*ROTPI));
					*tempPtr = b;
					tempPtr++;

				}
			}

			free(tempBuff);

		}
		*/


		// Display
		gdl::Display();


	}


}


// Init stuff
void init() {


	// Initialize filesystem library
	fatInitDefault();

	// Initialize system
	gdl::InitSystem(gdl::ModeAuto, gdl::AspectAuto, gdl::HiRes, 0);


	// Initialize wiimote
	WPAD_Init();
	InitWiimotes(1);


	// Set clear color
	gdl::SetClearColor(0, 63, 127, 255);


	// Set vertex format for 3D vertexes required for plylib (remember that GX_VTXFMT0 is reserved to MTek-GDL for drawing 2D graphics)
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_POS, GX_POS_XYZ,	GX_F32,	0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_NRM, GX_NRM_XYZ,	GX_F32,	0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_CLR0, GX_CLR_RGB, GX_RGB8, 0);


}
