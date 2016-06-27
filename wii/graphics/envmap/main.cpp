#include <mgdl-wii.h>
#include <wiiuse/wpad.h>
#include "..\..\common\plylib.cpp"


void ViewMemory(void *addr);


int main(int argc, char *argv[]) {

    fatInitDefault();
	gdl::InitSystem();
    WPAD_Init();


    GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_POS, GX_POS_XYZ,	GX_F32,	0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_NRM, GX_NRM_XYZ,	GX_F32,	0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_CLR0, GX_CLR_RGB, GX_RGB8, 0);


    /*
    3 - GX_Begin
    2 - GX_Position1x16
    2 - GX_Normal1x16
    2 - GX_TexCoord1x16
    2 - GX_Color1x16
    */

    /*
    void *dispList = memalign(32, 2048);
    memset(dispList, 0xff, 2048);

    DCFlushRange(dispList, 2048);

    GX_BeginDispList(dispList, 2048);

    GX_Begin(GX_POINTS, GX_VTXFMT1, 2);

    GX_Position1x16(0xf0f0);
    GX_Normal1x16(0xdead);
    GX_TexCoord1x16(0xbeef);
    GX_Color1x16(0xffff);

    GX_Position1x16(0xf0f0);
    GX_Normal1x16(0xdead);
    GX_TexCoord1x16(0xbeef);
    GX_Color1x16(0xffff);

    GX_End();

    int dispListSize = GX_EndDispList();
    */

    gdl::ConsoleMode();

    gdl::Image ChromeTex;
    ChromeTex.LoadImageMipmapped("/testdata/chrome.png", gdl::LN_MM_LN, gdl::Linear, 8, gdl::RGB565);
    ChromeTex.Texture.SetWrapMode(gdl::Repeat, gdl::Repeat);

    gdl::Image CelTex;
    CelTex.LoadImage("/testdata/celshades.png", gdl::Nearest, gdl::IA4);
    CelTex.Texture.SetWrapMode(gdl::Clamp, gdl::Clamp);

    gdl::Image BarbTex;
    BarbTex.LoadImageMipmapped("/testdata/Barb.png", gdl::LN_MM_LN, gdl::Linear, 8, gdl::RGBA8);

    ply::Model object[3];
    object[0].LoadModel("/testdata/TestModel.ply");
    object[1].LoadModel("/testdata/Suzanne.ply");
    object[2].LoadModel("/testdata/Cube.ply");


    //ViewMemory(dispList);


	gdl::SetClearColor(0, 127, 255, 0);

    float rot = 0;

	while(1) {

        gdl::PrepDisplay();


        gdl::Set3DMode(1000);

        {

            GXLightObj light;
            guVector lpos = { -80, 80, 72 };

            guVecMultiply(gdl::wii::ModelMtx, &lpos, &lpos);
            GX_InitLightPos(&light, lpos.x, lpos.y, lpos.z);
            GX_InitLightColor(&light, (GXColor) { 240, 240, 240, 255 });
            GX_InitLightSpot(&light, 0.0f, GX_SP_OFF);
            GX_InitLightDistAttn(&light, 10.0f, 1.f, GX_DA_GENTLE); // DistAttn = 20.0  &  Brightness=1.0f (full)
            GX_LoadLightObj(&light, GX_LIGHT0);

        }

        // Calculate model rotation matrix (for all the rendered objects)
        Mtx rotMatrix;
        Mtx texMatrix;
        guMtxIdentity(rotMatrix);

        {

            Mtx tempMatrix;
            guVector rotAxis;

            guMtxIdentity(tempMatrix);

            rotAxis.x = 1;
            rotAxis.y = 0;
            rotAxis.z = 0;
            guMtxRotAxisDeg(tempMatrix, &rotAxis, rot);
            guMtxConcat(rotMatrix, tempMatrix, rotMatrix);

            rotAxis.x = 0;
            rotAxis.y = 1;
            rotAxis.z = 0;
            guMtxRotAxisDeg(tempMatrix, &rotAxis, rot);
            guMtxConcat(rotMatrix, tempMatrix, rotMatrix);

            rotAxis.x = 0;
            rotAxis.y = 0;
            rotAxis.z = 1;
            guMtxRotAxisDeg(tempMatrix, &rotAxis, rot);
            guMtxConcat(rotMatrix, tempMatrix, rotMatrix);

        }

        rot += 0.5;

        GX_LoadNrmMtxImm(rotMatrix, GX_PNMTX0);


        guMtxIdentity(texMatrix);
        guMtxScaleApply(texMatrix, texMatrix, 0.44f, -0.44f, 1.f);
        guMtxTransApply(texMatrix, texMatrix, 0.5f, 0.5f, 0.f);
        guMtxConcat(texMatrix, rotMatrix, texMatrix);

        GX_LoadTexMtxImm(texMatrix, GX_TEXMTX0, GX_MTX2x4);



        // Chrome/reflection mapping (by using normals as texcoords)
        GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_NRM, GX_TEXMTX0);
        GX_SetCullMode(GX_CULL_FRONT);

        GX_LoadTexObj(ChromeTex.Texture.TexObj(), GX_TEXMAP0);
        GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);

        {

            Mtx modelMatrix;

            guMtxTransApply(gdl::wii::ModelMtx, modelMatrix, -7, 4, -12);
            guMtxConcat(modelMatrix, rotMatrix, modelMatrix);

            GX_LoadPosMtxImm(modelMatrix, GX_PNMTX0);

        }
        object[0].RenderModel();


        // Cel-shaded rendering
        GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT0, GX_DF_CLAMP, GX_AF_SPOT);
		GX_SetChanAmbColor(GX_COLOR0A0, (GXColor){ 0, 0, 0, 255 });

        GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_SRTG, GX_TG_COLOR0, GX_IDENTITY);
        GX_SetCullMode(GX_CULL_FRONT);

        GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

        GX_LoadTexObj(CelTex.Texture.TexObj(), GX_TEXMAP0);
        GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);

        {

            Mtx modelMatrix;

            guMtxTransApply(gdl::wii::ModelMtx, modelMatrix, 7, 4, -12);
            guMtxConcat(modelMatrix, rotMatrix, modelMatrix);

            GX_LoadPosMtxImm(modelMatrix, GX_PNMTX0);

        }
        object[1].RenderModel();


        // Texture mapped with Cel-shading (using the TEV)
        GX_LoadTexObj(BarbTex.Texture.TexObj(), GX_TEXMAP0); // Load model texture to GX_TEXMAP1
        GX_LoadTexObj(CelTex.Texture.TexObj(), GX_TEXMAP1);  // Load cel shader texture to GX_TEXMAP0

        // Prepare TexGens for cel-shading and texture mapping
        GX_SetNumTexGens(2);
        GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY); // Model texture coords
        GX_SetTexCoordGen(GX_TEXCOORD1, GX_TG_SRTG, GX_TG_COLOR0, GX_IDENTITY); // For cel-shading


        // Set TEV stages for cel shading with texture mapping
        GX_SetNumTevStages(2);


        // Prepare TEV stage 0 (cel texture)
        GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD1, GX_TEXMAP1, GX_COLORNULL);
        /*
        // Get TEV color input to texture color
        GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_TEXC, GX_CC_C0, GX_CC_ZERO, GX_CC_ZERO);
        // We don't need the alpha channel of the cel texture as we'll be modulating its intensity to the model's texture
        GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);

        // Set TEV operators to ADD and use default bias and scale values
        GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, GX_TRUE, GX_TEVPREV);
        GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, GX_TRUE, GX_TEVPREV);
        */

        // We'll be ignoring vertex colors since we're going for cel shading so we'll just use GX_REPLACE
        GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);


        // Prepare TEV stage 1 (model texture)
        GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
        /*
        // Take color and alpha values from texture
        GX_SetTevColorIn(GX_TEVSTAGE1, GX_CC_TEXC, GX_CC_C0, GX_CC_ZERO, GX_CC_ZERO);
        GX_SetTevAlphaIn(GX_TEVSTAGE1, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO, GX_CA_ZERO);

        // Same operator values as TEV stage 0
        GX_SetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GX_SetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        */
        // Modulate the final pixel color relative to the cel shader texture
        GX_SetTevOp(GX_TEVSTAGE1, GX_MODULATE);

        {

            Mtx modelMatrix;

            guMtxTransApply(gdl::wii::ModelMtx, modelMatrix, 0, -4, -12);
            guMtxConcat(modelMatrix, rotMatrix, modelMatrix);

            GX_LoadPosMtxImm(modelMatrix, GX_PNMTX0);

        }

        object[2].RenderModel();


        // Reset TexGens and TEV order back to MTek-GDL's default parameters
        GX_SetNumTexGens(1);
        GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

        GX_SetNumTevStages(1);
        GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
        GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_VTX, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);

        GX_SetCullMode(GX_CULL_NONE);

        /*
        GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT0, GX_DF_CLAMP, GX_AF_SPOT);
		GX_SetChanAmbColor(GX_COLOR0A0, (GXColor){ 0, 0, 0, 255 });

        GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_SRTG, GX_TG_COLOR0, GX_IDENTITY);
        GX_SetCullMode(GX_CULL_FRONT);

        GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

        GX_LoadTexObj(CelTex.Texture.TexObj(), GX_TEXMAP0);
        GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);
        object.RenderModel();

        GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
        GX_SetCullMode(GX_CULL_NONE);
        */


		/*
        if (SYS_ResetButtonDown()) {

            gdl::SaveScreen("shaders.png");

        }
        */


        gdl::Set2DMode(100);


        gdl::Display();

	}


}


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
