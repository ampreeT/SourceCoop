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
	int FROZEN_FADE_R;
	int FROZEN_FADE_G;
	int FROZEN_FADE_B;
	int FROZEN_FADE_A;
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

	//------------------------------------------------------


	void Initialize(GameData pGameConfig)
	{
		#define STR(%1) this.GetString(pGameConfig, #%1, this.%1, sizeof(this.%1))
		#define FLT(%1) this.%1 = this.GetFloat(pGameConfig, #%1)
		#define INT(%1) this.%1 = this.GetInt(pGameConfig, #%1)

		STR(SND_BRINGALL);
		FLT(SND_BRINGALL_VOL);
		STR(SND_PORTAL);
		FLT(SND_PORTAL_VOL);
		STR(PORTAL_PARTICLE);
		FLT(PORTAL_TOUCH_SIZE);
		INT(FROZEN_FADE_R);
		INT(FROZEN_FADE_G);
		INT(FROZEN_FADE_B);
		INT(FROZEN_FADE_A);
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

		#undef STR
		#undef FLT
		#undef INT
	}

	void GetString(GameData pGameConfig, const char[] szKey, char[] szVal, int len)
	{
		if (!pGameConfig.GetKeyValue(szKey, szVal, len))
			SetFailState("Missing config key: \"%s\"", szKey);
	}

	float GetFloat(GameData pGameConfig, const char[] szKey)
	{
		char szVal[16]; float flVal;
		this.GetString(pGameConfig, szKey, szVal, sizeof(szVal));

		if (!StringToFloatEx(szVal, flVal))
			SetFailState("Config value for key: \"%s\" should be a float.", szKey);

		return flVal;
	}

	int GetInt(GameData pGameConfig, const char[] szKey)
	{
		char szVal[16]; int iVal;
		this.GetString(pGameConfig, szKey, szVal, sizeof(szVal));

		if (!StringToIntEx(szVal, iVal))
			SetFailState("Config value for key: \"%s\" should be an int.", szKey);

		return iVal;
	}

}

SourceCoopConfig Conf;