#if defined _srccoop_config_included
 #endinput
#endif
#define _srccoop_config_included

#pragma newdecls required
#pragma semicolon 1

enum struct SourceCoopConfig
{

	//------------------------------------------------------
	// SPAWN SYSTEM
	//------------------------------------------------------

	char SND_BRINGALL[PLATFORM_MAX_PATH];
	float SND_BRINGALL_VOL;
	char SND_PORTAL[PLATFORM_MAX_PATH];
	float SND_PORTAL_VOL;
	char PORTAL_PARTICLE[128];
	float PORTAL_TOUCH_SIZE;
	
	//------------------------------------------------------
	// MANAGER
	//------------------------------------------------------

	// Freeze intro - screenfade properties
	int FROZEN_FADE_COLOR[4];
	int FROZEN_FADE_DUR_IN;
	int FROZEN_FADE_DUR_OUT;

	// Intro timer sounds
	char SND_TICK[PLATFORM_MAX_PATH];
	float SND_TICK_VOL;
	char SND_3[PLATFORM_MAX_PATH];
	char SND_2[PLATFORM_MAX_PATH];
	char SND_1[PLATFORM_MAX_PATH];

	// Changelevel sounds
	char SND_CHANGELEVEL[PLATFORM_MAX_PATH];
	float SND_CHANGELEVEL_VOL;
	char SND_CHANGELEVEL_TICK[PLATFORM_MAX_PATH];
	float SND_CHANGELEVEL_TICK_VOL;

	//------------------------------------------------------
	// ENTITYPATCH
	//------------------------------------------------------
	
	char CHANGELEVEL_EFFECT_MODEL[PLATFORM_MAX_PATH];
	int CHANGELEVEL_EFFECT_AMOUNT;
	float CHANGELEVEL_EFFECT_SPEED;
	float CHANGELEVEL_EFFECT_PERIOD;

	//------------------------------------------------------

	void Initialize(GameData pGameConfig)
	{
		#define STR(%1) GetGamedataString(pGameConfig, #%1, this.%1, sizeof(this.%1))
		#define FLT(%1) this.%1 = GetGamedataFloat(pGameConfig, #%1)
		#define INT(%1) this.%1 = GetGamedataInt(pGameConfig, #%1)
		#define CLR(%1) GetGamedataColor(pGameConfig, #%1, this.%1)

		STR(SND_BRINGALL);
		FLT(SND_BRINGALL_VOL);
		STR(SND_PORTAL);
		FLT(SND_PORTAL_VOL);
		STR(PORTAL_PARTICLE);
		FLT(PORTAL_TOUCH_SIZE);
		CLR(FROZEN_FADE_COLOR);
		INT(FROZEN_FADE_DUR_IN);
		INT(FROZEN_FADE_DUR_OUT);
		STR(SND_TICK);
		FLT(SND_TICK_VOL);
		STR(SND_3);
		STR(SND_2);
		STR(SND_1);
		STR(SND_CHANGELEVEL);
		FLT(SND_CHANGELEVEL_VOL);
		STR(SND_CHANGELEVEL_TICK);
		FLT(SND_CHANGELEVEL_TICK_VOL);
		STR(CHANGELEVEL_EFFECT_MODEL);
		INT(CHANGELEVEL_EFFECT_AMOUNT);
		FLT(CHANGELEVEL_EFFECT_SPEED);
		FLT(CHANGELEVEL_EFFECT_PERIOD);

		#undef STR
		#undef FLT
		#undef INT
		#undef CLR
	}

}

SourceCoopConfig Conf;