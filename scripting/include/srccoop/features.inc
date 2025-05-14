#pragma newdecls required
#pragma semicolon 1

enum SourceCoopFeature
{
	FT_INSTANCE_ITEMS						=	(1 << 0),
	FT_INSTANCE_ITEMS_NORESET 				=	(1 << 1),
	FT_FIRSTPERSON_DEATHCAM 				=	(1 << 2),
	//Free									=	(1 << 3),
	//Free									=	(1 << 4),
	FT_NOBLOCK								=	(1 << 5),
	FT_SHOW_WELCOME_MESSAGE					=	(1 << 6),
	FT_HEV_SOUNDS							=	(1 << 7),
	//Free									=	(1 << 8),
	//Free									=	(1 << 9),
	FT_DISABLE_CANISTER_DROPS				=	(1 << 10),
	FT_AUTODETECT_MAP_END					=	(1 << 11),
	FT_CHANGELEVEL_FX						=	(1 << 12),
	FT_KEEP_EQUIPMENT						=	(1 << 13),
	FT_TRANSFER_PLAYER_STATE				=	(1 << 14),
	FT_SP_WEAPONS							=	(1 << 15),
	FT_ENABLE_TIMELIMIT						=	(1 << 16),
}

methodmap FeatureMap < StringMap
{
	public FeatureMap()
	{
		StringMap pMap = new StringMap();
		
		pMap.SetValue("INSTANCE_ITEMS", FT_INSTANCE_ITEMS);
		pMap.SetValue("INSTANCE_ITEMS_NORESET", FT_INSTANCE_ITEMS_NORESET);
		pMap.SetValue("FIRSTPERSON_DEATHCAM", FT_FIRSTPERSON_DEATHCAM);
		pMap.SetValue("NOBLOCK", FT_NOBLOCK);
		pMap.SetValue("SHOW_WELCOME_MESSAGE", FT_SHOW_WELCOME_MESSAGE);
		pMap.SetValue("HEV_SOUNDS", FT_HEV_SOUNDS);
		pMap.SetValue("DISABLE_CANISTER_DROPS", FT_DISABLE_CANISTER_DROPS);
		pMap.SetValue("AUTODETECT_MAP_END", FT_AUTODETECT_MAP_END);
		pMap.SetValue("CHANGELEVEL_FX", FT_CHANGELEVEL_FX);
		pMap.SetValue("KEEP_EQUIPMENT", FT_KEEP_EQUIPMENT);
		pMap.SetValue("TRANSFER_PLAYER_STATE", FT_TRANSFER_PLAYER_STATE);
		pMap.SetValue("SP_WEAPONS", FT_SP_WEAPONS);
		pMap.SetValue("ENABLE_TIMELIMIT", FT_ENABLE_TIMELIMIT);
		
		return view_as<FeatureMap>(pMap);
	}
	
	public bool GetFeature(const char[] szFeature, SourceCoopFeature &feature)
	{
		int len = strlen(szFeature) + 1;
		char[] szFeatureUpper = new char[len];
		UpperCaseString(szFeature, szFeatureUpper, len);
		return this.GetValue(szFeatureUpper, feature);
	}
}