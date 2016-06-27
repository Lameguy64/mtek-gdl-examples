#include <mgdl-wii.h>

#include <plylib.h>
#include "plylib.cpp"


int main(int argc, char *argv[]) {

	fatInitDefault();
	gdl::InitSystem();
	gdl::ConsoleMode();


	ply::Init();


	ply::Model	potion_glass;
	ply::Model	potion_cork;
	ply::Model	potion_liquid;

	gdl::Image	celshade_tex;
	gdl::Image	potion_glass_tex;
	gdl::Image	glass_spec_tex;

	potion_glass.LoadModel("testdata/potion/potion_glass.ply");
	potion_cork.LoadModel("testdata/potion/potion_cork.ply");
	potion_liquid.LoadModel("testdata/potion/potion_liquid.ply");

	//potion_glass.LoadModel("testdata/bottle/bottle_glass.ply");
	//potion_glass.LoadModel("testdata/roundcube.ply");

	celshade_tex.LoadImage("testdata/celshades.png", gdl::Linear, gdl::IA4);
	potion_glass_tex.LoadImage("testdata/glass_map.png", gdl::Linear, gdl::I8);
	glass_spec_tex.LoadImage("testdata/glass_shade_map.png", gdl::Linear, gdl::RGB565);


	for(int i=0; i<60; i++) {
		VIDEO_WaitVSync();
	}

	printf("If you see this... You've done fucked up as usual.");


	gdl::SetClearColor(0, 0, 0, 0);

	float	rot = 0.f;
	Mtx		rotMatrix;
	bool	capDone = false;

	while(1) {

		gdl::PrepDisplay();


		gdl::DrawBoxF(0, gdl::ScreenCenterY-8, gdl::ScreenXres, gdl::ScreenCenterY, gdl::Color::Yellow);

		gdl::Set3DMode();


		// Set light source
		{

            GXLightObj light;
            GXLightObj lightSpec;
            guVector lpos = { -80, 80, 72 };
            guVector ldir = { 1.f, -1.f, -1.f };

            guVecMultiply(gdl::wii::ModelMtx, &lpos, &lpos);

			// For regular diffuse light
            GX_InitLightPos(&light, lpos.x, lpos.y, lpos.z);
            GX_InitLightColor(&light, (GXColor) { 255, 255, 255, 255 });
            GX_InitLightSpot(&light, 0.0f, GX_SP_OFF);
            GX_InitLightDistAttn(&light, 15.0f, 1.f, GX_DA_GENTLE); // DistAttn = 20.0  &  Brightness=1.0f (full)

			// For specular lighting
			GX_InitLightPos(&lightSpec, lpos.x, lpos.y, lpos.z);
			GX_InitLightColor(&lightSpec, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
			GX_InitSpecularDirv(&lightSpec, &ldir);
			GX_InitLightShininess(&lightSpec, 16.f);


            GX_LoadLightObj(&light, GX_LIGHT0);
            GX_LoadLightObj(&lightSpec, GX_LIGHT1);

        }


		guMtxIdentity(rotMatrix);

		{

			Mtx tmpMatrix;
            guVector rotAxis;

			rotAxis.x = 0;	rotAxis.y = 1;	rotAxis.z = 0;
			guMtxRotAxisDeg(tmpMatrix, &rotAxis, rot);
            guMtxConcat(rotMatrix, tmpMatrix, rotMatrix);

            rotAxis.x = 0;	rotAxis.y = 0;	rotAxis.z = 1;
			guMtxRotAxisDeg(tmpMatrix, &rotAxis, 25.f);
            guMtxConcat(rotMatrix, tmpMatrix, rotMatrix);

		}

		rot += 1.f;

		GX_LoadNrmMtxImm(rotMatrix, GX_PNMTX0);

		{
			Mtx texMatrix;

			guMtxIdentity(texMatrix);
			guMtxScaleApply(texMatrix, texMatrix, 0.55f, 0.55f, 1.f);
			guMtxTransApply(texMatrix, texMatrix, 0.5f, 0.5f, 0.f);
			guMtxConcat(texMatrix, rotMatrix, texMatrix);

			GX_LoadTexMtxImm(texMatrix, GX_TEXMTX0, GX_MTX2x4);
		}


		// Apply rotation matrix to the model matrix and load it to the GX
		guMtxApplyTrans(gdl::wii::ModelMtx, gdl::wii::ModelMtx, 0, -0.5f, -4);
		guMtxConcat(gdl::wii::ModelMtx, rotMatrix, gdl::wii::ModelMtx);
        GX_LoadPosMtxImm(gdl::wii::ModelMtx, GX_PNMTX0);


		GX_SetCullMode(GX_CULL_FRONT);


		// Draw cork of potion bottle
		// db6f00
		GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);

		GX_SetNumChans(2);
		GX_SetChanAmbColor(GX_COLOR0A0, (GXColor){ 15, 15, 15, 255 });
		GX_SetChanMatColor(GX_COLOR0A0, (GXColor){ 191, 191, 191, 255 });
		GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT0, GX_DF_CLAMP, GX_AF_SPOT);

		GX_SetChanAmbColor(GX_COLOR1A1, (GXColor){ 0xdb, 0x6f, 0x00, 255 });
		GX_SetChanMatColor(GX_COLOR1A1, (GXColor){ 0xdb, 0x6f, 0x00, 255 });
		GX_SetChanCtrl(GX_COLOR1A1, GX_ENABLE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);



		GX_SetNumTexGens(1);
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_SRTG, GX_TG_COLOR0, GX_IDENTITY);
		GX_LoadTexObj(celshade_tex.Texture.TexObj(), GX_TEXMAP0);


        GX_SetNumTevStages(1);
        GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR1A1);
        GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);

        potion_cork.RenderModel();


		// Draw potion liquid
		GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);

		GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_VTX, GX_SRC_VTX, 0, GX_DF_CLAMP, GX_AF_SPOT);

		GX_SetNumTexGens(0);

		GX_SetNumTevStages(1);
        GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
        GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

		potion_liquid.RenderModel();


		// Draw glass of potion bottle
        GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);

		GX_SetNumChans(1);
        GX_SetChanCtrl(GX_COLOR0A0, GX_ENABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT0, GX_DF_CLAMP, GX_AF_SPOT);
        GX_SetChanAmbColor(GX_COLOR0A0, (GXColor){ 15, 15, 15, 255 });
		GX_SetChanMatColor(GX_COLOR0A0, (GXColor){ 191, 191, 191, 255 });

		GX_SetNumTexGens(2);
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_NRM, GX_TEXMTX0);
		GX_SetTexCoordGen(GX_TEXCOORD1, GX_TG_SRTG, GX_TG_COLOR0, GX_IDENTITY);

		// Load the texture objects
		GX_LoadTexObj(potion_glass_tex.Texture.TexObj(), GX_TEXMAP0);
		GX_LoadTexObj(glass_spec_tex.Texture.TexObj(), GX_TEXMAP1);

		GX_SetNumTevStages(2);
        GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLORNULL);
        GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);
        GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_TEXA, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
        GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

        GX_SetTevColor(GX_TEVREG0, (GXColor){ 255, 255, 255, 114 });

		GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLORNULL);
        GX_SetTevOp(GX_TEVSTAGE1, GX_BLEND);
        GX_SetTevAlphaIn(GX_TEVSTAGE1, GX_CA_A0, GX_CA_TEXA, GX_CA_APREV, GX_CA_ZERO);
        GX_SetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

		potion_glass.RenderModel();


		// Reset color channels and TEV stages for the simpler, 2D stuff
		GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);

		GX_SetNumChans(1);
		GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_VTX, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);

		GX_SetNumTexGens(1);
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

		GX_SetNumTevStages(1);
		GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);

		GX_SetCullMode(GX_CULL_NONE);



		gdl::Set2DMode();

		//potion_glass_tex.Put(16, 32);
		//glass_spec_tex.Put(16, 114);

		if ((SYS_ResetButtonDown()) && (!capDone)) {
			gdl::SaveScreen("celpotion.png");
			capDone = true;
		}

		gdl::Display();

	}

}
