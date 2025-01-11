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
	// COOPMANAGER
	//------------------------------------------------------

	// Freeze intro - screenfade properties
	RGBA FROZEN_FADE_COLOR;
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

	CONF_INIT(
		CONF_STR(SND_BRINGALL)
		CONF_FLT(SND_BRINGALL_VOL)
		CONF_STR(SND_PORTAL)
		CONF_FLT(SND_PORTAL_VOL)
		CONF_STR(PORTAL_PARTICLE)
		CONF_FLT(PORTAL_TOUCH_SIZE)
		CONF_CLR(FROZEN_FADE_COLOR)
		CONF_INT(FROZEN_FADE_DUR_IN)
		CONF_INT(FROZEN_FADE_DUR_OUT)
		CONF_STR(SND_TICK)
		CONF_FLT(SND_TICK_VOL)
		CONF_STR(SND_3)
		CONF_STR(SND_2)
		CONF_STR(SND_1)
		CONF_STR(SND_CHANGELEVEL)
		CONF_FLT(SND_CHANGELEVEL_VOL)
		CONF_STR(SND_CHANGELEVEL_TICK)
		CONF_FLT(SND_CHANGELEVEL_TICK_VOL)
		CONF_STR(CHANGELEVEL_EFFECT_MODEL)
		CONF_INT(CHANGELEVEL_EFFECT_AMOUNT)
		CONF_FLT(CHANGELEVEL_EFFECT_SPEED)
		CONF_FLT(CHANGELEVEL_EFFECT_PERIOD)
	)

}

SourceCoopConfig Conf;