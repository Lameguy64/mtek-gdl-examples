typedef struct {
	
	bool	active;
	bool	rumble;
	int		status;
	u32		type;
	
	WPADData data;
	
} WiimoteType;

WiimoteType Wiimote[4]={0};


void InitWiimote(short numWiimotes);
void CheckWiimotes();


void InitWiimotes(short numWiimotes) {

	for(short i=0; i<numWiimotes; i++) {
		Wiimote[i].active = true;
		WPAD_SetDataFormat(i, WPAD_FMT_BTNS_ACC_IR);
		WPAD_SetVRes(i, gdl::ScreenXres+64, gdl::ScreenYres+64);
	}

}

void CheckWiimotes() {

	for(short i=0; i<4; i++) {
		
		if (Wiimote[i].active) {
			
			while(1) {
				
				Wiimote[i].status = WPAD_ReadEvent(i, &Wiimote[i].data);
				if (Wiimote[i].status != WPAD_ERR_NONE) break;
				
				WPAD_Rumble(i, Wiimote[i].rumble);
				
				Wiimote[i].data.ir.x -= 32;
				Wiimote[i].data.ir.y -= 32;
				
			}
			
			Wiimote[i].status = WPAD_Probe(i, &Wiimote[i].type);
			
		}
		
	}

}