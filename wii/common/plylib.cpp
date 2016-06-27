/*
 *	My simple little PLY model parser and renderer for this 3dsample example.
 *
 *	It only draws PLY models properly if the coordinate system is set to -Z forward Y up when exporting
 *	and make sure that all polygons are 3 point triangles.
 *
 */

#include "plylib.h"

void ply::Init() {

	// Setup vertex format for 3D coordinates in slot 1
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_POS, GX_POS_XYZ,	GX_F32,	0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_NRM, GX_NRM_XYZ,	GX_F32,	0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_CLR0, GX_CLR_RGB, GX_RGB8, 0);

}

// Constructor and destructors
ply::Model::Model() {

	vList = NULL;
	nList = NULL;
	tList = NULL;
	cList = NULL;
	iList = NULL;
	dList = NULL;

	vertCount = 0;
	faceCount = 0;

}
ply::Model::~Model() {

	if (vList != NULL) free(vList);
	if (tList != NULL) free(tList);
	if (cList != NULL) free(cList);
	if (iList != NULL) free(iList);
	if (dList != NULL) free(dList);

}


int ply::Model::LoadModel(const char* fileName) {

	#define VT_X    1
    #define VT_Y    2
    #define VT_Z    3
    #define VT_NX   4
    #define VT_NY   5
    #define VT_NZ   6
    #define VT_S    7   // Also known as U (texcoord)
    #define VT_T    8   // Also known as V (texcoord)
    #define VT_R    9
    #define VT_G    10
    #define VT_B    11


    FILE *file;
    if (!(file = fopen(fileName, "rb"))) return(PLY_ERR_NOTFOUND);


	int		temp;
	int		v0,v1,v2;

	short	vertexNum=0;
	short	vertexFmt[14];

	bool	normPresent		=false;
	bool	tcoordPresent	=false;
	bool	colPresent		=false;

	char	lineBuff[256];
	char	strBuff[4][64];


	// Check header
	fgets(lineBuff, 256, file); lineBuff[strlen(lineBuff)-1] = 0x00;
    if (strcmp(lineBuff, "ply") != 0) return(PLY_ERR_NOTPLY);


	// Now parse the header area
    while(1) {

        if (feof(file))
            return(PLY_ERR_INCOMPLETE);

        fgets(lineBuff, 256, file); lineBuff[strlen(lineBuff)-1] = 0x00;
        if (strcmp(lineBuff, "end_header") == 0) break;

        sscanf(lineBuff, "%s %s %s", (char*)&strBuff[0], (char*)&strBuff[1], (char*)&strBuff[2]);

        if (strcmp(strBuff[0], "property") == 0) {

            if (strcmp(strBuff[2], "x") == 0) {
                if (vertexNum >= 11) { fclose(file); return(PLY_ERR_VERTEXPROP); }
                vertexFmt[vertexNum]=VT_X;  vertexNum++;
            } else if (strcmp(strBuff[2], "y") == 0) {
                if (vertexNum >= 11) { fclose(file); return(PLY_ERR_VERTEXPROP); }
                vertexFmt[vertexNum]=VT_Y;  vertexNum++;
            } else if (strcmp(strBuff[2], "z") == 0) {
                if (vertexNum >= 11) { fclose(file); return(PLY_ERR_VERTEXPROP); }
                vertexFmt[vertexNum]=VT_Z;  vertexNum++;
            } else if (strcmp(strBuff[2], "nx") == 0) {
                if (vertexNum >= 11) { fclose(file); return(PLY_ERR_VERTEXPROP); }
                vertexFmt[vertexNum]=VT_NX;  vertexNum++;	normPresent=true;
            } else if (strcmp(strBuff[2], "ny") == 0) {
                if (vertexNum >= 11) { fclose(file); return(PLY_ERR_VERTEXPROP); }
                vertexFmt[vertexNum]=VT_NY;  vertexNum++;	normPresent=true;
            } else if (strcmp(strBuff[2], "nz") == 0) {
                if (vertexNum >= 11) { fclose(file); return(PLY_ERR_VERTEXPROP); }
                vertexFmt[vertexNum]=VT_NZ;  vertexNum++;	normPresent=true;
            } else if (strcmp(strBuff[2], "s") == 0) {
                if (vertexNum >= 11) { fclose(file); return(PLY_ERR_VERTEXPROP); }
                vertexFmt[vertexNum]=VT_S;  vertexNum++;	tcoordPresent=true;
            } else if (strcmp(strBuff[2], "t") == 0) {
                if (vertexNum >= 11) { fclose(file); return(PLY_ERR_VERTEXPROP); }
                vertexFmt[vertexNum]=VT_T;  vertexNum++;	tcoordPresent=true;
            } else if (strcmp(strBuff[2], "red") == 0) {
                if (vertexNum >= 11) { fclose(file); return(PLY_ERR_VERTEXPROP); }
                vertexFmt[vertexNum]=VT_R;  vertexNum++;	colPresent=true;
            } else if (strcmp(strBuff[2], "green") == 0) {
                if (vertexNum >= 11) { fclose(file); return(PLY_ERR_VERTEXPROP); }
                vertexFmt[vertexNum]=VT_G;  vertexNum++;	colPresent=true;
            } else if (strcmp(strBuff[2], "blue") == 0) {
                if (vertexNum >= 11) { fclose(file); return(PLY_ERR_VERTEXPROP); }
                vertexFmt[vertexNum]=VT_B;  vertexNum++;	colPresent=true;
            }

        } else if (strcmp(strBuff[0], "element") == 0) {

            if (strcmp(strBuff[1], "vertex") == 0) {
                vertCount = atoi(strBuff[2]);
            } else if (strcmp(strBuff[1], "face") == 0) {
                faceCount = atoi(strBuff[2]);
            }

        }

    }


	// Deallocate when already allocated
	if (vList != NULL) free(vList); vList=NULL;
	if (tList != NULL) free(tList); tList=NULL;
	if (cList != NULL) free(cList); cList=NULL;
	if (iList != NULL) free(iList); iList=NULL;
	if (dList != NULL) free(dList); dList=NULL;


	// Allocate for our vertex data
	vList = memalign(32, sizeof(VERTEX3f)*vertCount);
	if (normPresent)	nList = memalign(32, sizeof(VERTEX3f)*vertCount);
	if (tcoordPresent)	tList = memalign(32, sizeof(TCOORD2f)*vertCount);
	if (colPresent)		cList = memalign(32, sizeof(COLOR3b)*vertCount);


	// Now parse the vertices
	VERTEX3f	*vertex	=(VERTEX3f*)vList;
	VERTEX3f	*normal	=(VERTEX3f*)nList;
	TCOORD2f	*tcoord	=(TCOORD2f*)tList;
	COLOR3b		*color	=(COLOR3b*)cList;


	for(short i=0; i<vertCount; i++) {

        fgets(lineBuff, 256, file); lineBuff[strlen(lineBuff)-1] = 0x00;

        short  spi=0;
        for(u_short s=0; s<strlen(lineBuff);) {

            sscanf(&lineBuff[s], "%s", (char*)&strBuff[0]);

            switch(vertexFmt[spi]) {
                case VT_X:  sscanf(strBuff[0], "%f", &vertex[i].x);	break;
                case VT_Y:  sscanf(strBuff[0], "%f", &vertex[i].y);	break;
                case VT_Z:  sscanf(strBuff[0], "%f", &vertex[i].z);	break;
                case VT_NX: sscanf(strBuff[0], "%f", &normal[i].x);	break;
                case VT_NY: sscanf(strBuff[0], "%f", &normal[i].y);	break;
                case VT_NZ: sscanf(strBuff[0], "%f", &normal[i].z);	break;
                case VT_S:  sscanf(strBuff[0], "%f", &tcoord[i].u);	break;
                case VT_T:  sscanf(strBuff[0], "%f", &tcoord[i].v);	break;
                case VT_R:  sscanf(strBuff[0], "%d", (int*)&temp);	color[i].r = temp;	break;
                case VT_G:  sscanf(strBuff[0], "%d", (int*)&temp);	color[i].g = temp;	break;
                case VT_B:  sscanf(strBuff[0], "%d", (int*)&temp);	color[i].b = temp;	break;
            }

            for(u_short c=s; c<=strlen(lineBuff); c++) {
                if ((lineBuff[c] == ' ') || (lineBuff[c] == 0x00)) {
                    s+=(c-s)+1;
                    break;
                }
            }

            spi++;

        }

		if (tcoordPresent) tcoord[i].v = 1-tcoord[i].v;

    }


	// Parse polygons
	iList = (INDEX*)malloc(sizeof(INDEX)*faceCount);

	for(short i=0; i<faceCount; i++) {

		fgets(lineBuff, 256, file); lineBuff[strlen(lineBuff)-1] = 0x00;
		sscanf(lineBuff, "%s %d %d %d", (char*)&strBuff[0], &v0, &v1, &v2);
		iList[i].v0 = v0;
		iList[i].v1 = v1;
		iList[i].v2 = v2;

        if (atoi(strBuff[0]) != 3) {
            return(PLY_ERR_INVALIDPOLY);
        }

	}


	// Flush vertexes then close file
	FlushVertexes();
	fclose(file);


	// Now generate the display list for the model

	// Estimate base size of display list
	{
        short packetSize = 4;
        if (nList != NULL) packetSize += 2;
		if (tList != NULL) packetSize += 2;
        listSize=32*(((3+(packetSize*(faceCount*3)))+63)/32);
	}

	// Allocate and then invalidate
	dList=memalign(32, listSize);
	DCInvalidateRange(dList, listSize);

	// Print load stats
	if (gdl::ConsoleActive) {
		printf("%s\n", fileName);		// File name
		printf("DLS:%d/", listSize);	// Initial display list size
	}


	// Begin building the display list
	GX_BeginDispList(dList, listSize);
	GX_Begin(GX_TRIANGLES, GX_VTXFMT1, 3*faceCount);

	// Since we're using vertex arrays, we'll just use index primitives
	for(u_short i=0; i<faceCount; i++) {

		GX_Position1x16(iList[i].v0);
		if (nList != NULL) GX_Normal1x16(iList[i].v0);
		if (cList != NULL) GX_Color1x16(iList[i].v0); else GX_Color1x16(0);
		if (tList != NULL) GX_TexCoord1x16(iList[i].v0);

		GX_Position1x16(iList[i].v1);
		if (nList != NULL) GX_Normal1x16(iList[i].v1);
		if (cList != NULL) GX_Color1x16(iList[i].v1); else GX_Color1x16(0);
		if (tList != NULL) GX_TexCoord1x16(iList[i].v1);

		GX_Position1x16(iList[i].v2);
		if (nList != NULL) GX_Normal1x16(iList[i].v2);
		if (cList != NULL) GX_Color1x16(iList[i].v2); else GX_Color1x16(0);
		if (tList != NULL) GX_TexCoord1x16(iList[i].v2);

	}

	// Finish creating the display list and then trim down the list block
	GX_End();
	listSize = GX_EndDispList();
	realloc(dList, listSize);


	// Print more stats (if console is active)
	if (gdl::ConsoleActive) {
		printf("%d\n", listSize);	// New display list size
		printf("V:%p N:%p T:%p C:%p\n", vList, nList, tList, cList);	// Array pointers
	}


	return(0);

}


void ply::Model::FlushVertexes() {

	if (vList != NULL) DCFlushRange(vList, sizeof(VERTEX3f)*vertCount);
	if (nList != NULL) DCFlushRange(nList, sizeof(VERTEX3f)*vertCount);
	if (tList != NULL) DCFlushRange(tList, sizeof(TCOORD2f)*vertCount);
	if (cList != NULL) DCFlushRange(cList, sizeof(COLOR3b)*vertCount);

}


void ply::Model::RenderModel() {


	// Set vertex descriptors for our display list
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_INDEX16);
	if (nList != NULL) GX_SetVtxDesc(GX_VA_NRM, GX_INDEX16);
	GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX16);
	if (tList != NULL) GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX16);


	// Bind to our vertex arrays
	GX_SetArray(GX_VA_POS, vList, 3*sizeof(f32));
	if (nList != NULL) GX_SetArray(GX_VA_NRM, nList, 3*sizeof(f32));
	if (tList != NULL) GX_SetArray(GX_VA_TEX0, tList, 2*sizeof(f32));

	if (cList != NULL) {
		GX_SetArray(GX_VA_CLR0,	cList, 3*sizeof(u8));
	} else {
		GX_SetArray(GX_VA_CLR0,	&ply::DefaultCol, 3*sizeof(u8));
	}


	// Call display list (which draws the model)
	GX_CallDispList(dList, listSize);


}
