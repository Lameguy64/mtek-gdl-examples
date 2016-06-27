/**	Mipmapped texture example with MTek-GDL by Lameguy64/TheCodingBrony
 *
 *	This example demonstrates mipmapped texture mapping which improves texture
 *	mapping quality while speeding up rendering performance a bit. Mipmapped textures
 *	are currently only supported through TPL files that contain mipmaps.
 *
 *	To create a TPL texture file with mipmaps, use the following parameters with gxtexconv:
 *		gxtexconv -i texture.png colfmt=14 minlod=0 maxlod=8 -o texture.tpl
 *
 *	Because mipmapped textures are roughly 35% larger than without mipmaps, it is best to
 *	use either RGB565 or DXT compressed texture format.
 *
 *	As of version 0.65 and onwards of MTek-GDL Wii, you can now generate mipmapped textures
 *	off of plain png images through gdl::Image::LoadImageMipmapped(). Converting textures to
 *	DXT1 compressed format however is still not supported.
 *
 *	Controls (requires Nunchuk):
 *		D-Pad			- Look.
 *		B				- Look using IR pointer.
 *		Home			- Exit.
 *		Nunchuk Stick	- Move around.
 *		Nunchuk Z		- Move downward (not relative to camera angle).
 *		Nunchuk	C		- Move upward (not relative to camera angle).
 *
 */


// Include library and neccessary headers
#include <mgdl-wii.h>
#include <wiiuse/wpad.h>
#include "../../common/wiimote.c"


// Main function
int main() {


	// Init system and Wiimote
	fatInitDefault();
	gdl::InitSystem(0);
	gdl::SetClearColor(0x00, 0x3f, 0x7f, 0xff);

	WPAD_Init();
	InitWiimotes(1);


	// Setup console mode
	gdl::ConsoleMode();


	// Load mipmapped texture
	gdl::Texture barbTex;
	barbTex.LoadTexture("testdata/barb_mm.tpl");

	// Setup level 4 anisotropic mipmap filtering to the texture (highest quality but can be GP intensive)
	GX_InitTexObjEdgeLOD(barbTex.TexObj(), GX_ENABLE);
	GX_InitTexObjMaxAniso(barbTex.TexObj(), GX_ANISO_4);


	// Setup vertex format for our 3D geomery
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_POS, GX_POS_XYZ,	GX_F32,	0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_CLR0, GX_CLR_RGB, GX_RGB8, 0);


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
	} Pos = {0};


	// Initial camera position
	Camera.z = 2000;
	Camera.y = 250;


	// Main loop
	while(1) {


		// Get Wiimote data
		CheckWiimotes();


		// Handle Wiimote input and camera controls
		if (Wiimote[0].status == WPAD_ERR_NONE) {

			// Camera controls

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

			if (Wiimote[0].data.btns_h & WPAD_BUTTON_UP)		Camera.vta -= 0.3f;
			if (Wiimote[0].data.btns_h & WPAD_BUTTON_DOWN)		Camera.vta += 0.3f;
			if (Wiimote[0].data.btns_h & WPAD_BUTTON_LEFT)		Camera.vpa -= 0.3f;
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

			// Exit if home is pressed
			if (Wiimote[0].data.btns_h & WPAD_BUTTON_HOME) exit(0);

		}


		// Calculate camera position and angle (with gdl::Delta so that speed will be consistent on PAL systems)
		Camera.x += Camera.vx*gdl::Delta;	Camera.vx *= 0.85f;
		Camera.y += Camera.vy*gdl::Delta;	Camera.vy *= 0.85f;
		Camera.z += Camera.vz*gdl::Delta;	Camera.vz *= 0.85f;

		Camera.pa += Camera.vpa*gdl::Delta;	Camera.vpa *= 0.85f;
		Camera.ta += Camera.vta*gdl::Delta;	Camera.vta *= 0.85f;


		// Prepare rendering and setup 3D projection mode
		gdl::PrepDisplay();
		gdl::Set3DMode(5000);


		// Stuff for matrix calculations
		guVector tempRot;
		Mtx tempMatrix;

		// Rotate matrix according to the camera
		tempRot.x = 1; tempRot.y = 0; tempRot.z = 0;
		guMtxRotAxisDeg(tempMatrix, &tempRot, Camera.ta);
		guMtxConcat(gdl::wii::ModelMtx, tempMatrix, gdl::wii::ModelMtx);

		tempRot.x = 0; tempRot.y = 1; tempRot.z = 0;
		guMtxRotAxisDeg(tempMatrix, &tempRot, Camera.pa);
		guMtxConcat(gdl::wii::ModelMtx, tempMatrix, gdl::wii::ModelMtx);

		// Translate matrix according to the camera and then set it as our position matrix
		guMtxApplyTrans(gdl::wii::ModelMtx, gdl::wii::ModelMtx, -Camera.x/8.f, -Camera.y/8.f, -Camera.z/8.f);
		GX_LoadPosMtxImm(gdl::wii::ModelMtx, GX_PNMTX0);

		// Load it to the GP and set the approiate TEV operator
		GX_LoadTexObj(barbTex.TexObj(), GX_TEXMAP0);
		GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);



		// Setup vertex descriptors for our geometry
		GX_ClearVtxDesc();
		GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
		GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
		GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);


		// Draw a quad with the textures tiled in a 4x4 grid
		GX_Begin(GX_QUADS, GX_VTXFMT1, 4);

			// Far-left
			GX_Position3f32(-400.0f, -30.0f, -400.0f);
			GX_Color3u8(255, 255, 255);
			GX_TexCoord2f32(0.f, 0.f);

			// Far-right
			GX_Position3f32(400.0f, -30.0f, -400.0f);
			GX_Color3u8(255, 255, 255);
			GX_TexCoord2f32(4.f, 0.f);

			// Near-right
			GX_Position3f32(400.0f, -30.0f, 400.0f);
			GX_Color3u8(255, 255, 255);
			GX_TexCoord2f32(4.f, 4.f);

			// Near-left
			GX_Position3f32(-400.0f, -30.0f, 400.0f);
			GX_Color3u8(255, 255, 255);
			GX_TexCoord2f32(0.f, 4.f);

		GX_End();


		// Set back to 2D projection mode
		gdl::Set2DMode(100);


		// Draw pointer
		if (Wiimote[0].status == WPAD_ERR_NONE) {
			if (Wiimote[0].data.ir.valid) {
				gdl::DrawBoxF(Wiimote[0].data.ir.x-4, Wiimote[0].data.ir.y-4, Wiimote[0].data.ir.x+4, Wiimote[0].data.ir.y+4, 0xffffffff);
			}
		}


		// Display the rendered frame
		gdl::Display();

	}

}
