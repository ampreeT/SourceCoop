#if defined _srccoop_features_included
 #endinput
#endif
#define _srccoop_features_included

#pragma newdecls required
#pragma semicolon 1

enum SourceCoopFeature
{
	FT_INSTANCE_ITEMS			=	(1 << 0),
	FT_INSTANCE_ITEMS_NORESET 	=	(1 << 1),
	FT_FIRSTPERSON_DEATHCAM 	=	(1 << 2),
	FT_NO_TELEFRAGGING			=	(1 << 3),
	//Free						=	(1 << 4),
	FT_NOBLOCK					=	(1 << 5),
	FT_SHOW_WELCOME_MESSAGE		=	(1 << 6),
	FT_HEV_SOUNDS				=	(1 << 7),
	FT_STRIP_DEFAULT_EQUIPMENT	=	(1 << 8),
	FT_STRIP_DEFAULT_EQUIPMENT_KEEPSUIT	=	(1 << 9),
	FT_DISABLE_CANISTER_DROPS	=	(1 << 10),
	FT_AUTODETECT_MAP_END		=	(1 << 11),
	FT_CHANGELEVEL_FX			=	(1 << 12),
	FT_KEEP_EQUIPMENT			=	(1 << 13),
	FT_TRANSFER_PLAYER_STATE	=	(1 << 14),
}

methodmap FeatureMap < StringMap
{
	public FeatureMap()
	{
		StringMap pMap = new StringMap();
		
		pMap.SetValue("INSTANCE_ITEMS", FT_INSTANCE_ITEMS);
		pMap.SetValue("INSTANCE_ITEMS_NORESET", FT_INSTANCE_ITEMS_NORESET);
		pMap.SetValue("FIRSTPERSON_DEATHCAM", FT_FIRSTPERSON_DEATHCAM);
		pMap.SetValue("NO_TELEFRAGGING", FT_NO_TELEFRAGGING);
		pMap.SetValue("NOBLOCK", FT_NOBLOCK);
		pMap.SetValue("SHOW_WELCOME_MESSAGE", FT_SHOW_WELCOME_MESSAGE);
		pMap.SetValue("HEV_SOUNDS", FT_HEV_SOUNDS);
		pMap.SetValue("STRIP_DEFAULT_EQUIPMENT", FT_STRIP_DEFAULT_EQUIPMENT);
		pMap.SetValue("STRIP_DEFAULT_EQUIPMENT_KEEPSUIT", FT_STRIP_DEFAULT_EQUIPMENT_KEEPSUIT);
		pMap.SetValue("DISABLE_CANISTER_DROPS", FT_DISABLE_CANISTER_DROPS);
		pMap.SetValue("AUTODETECT_MAP_END", FT_AUTODETECT_MAP_END);
		pMap.SetValue("CHANGELEVEL_FX", FT_CHANGELEVEL_FX);
		pMap.SetValue("KEEP_EQUIPMENT", FT_KEEP_EQUIPMENT);
		pMap.SetValue("TRANSFER_PLAYER_STATE", FT_TRANSFER_PLAYER_STATE);
		
		return view_as<FeatureMap>(pMap);
	}
	
	public bool GetFeature(char[] szFeature, SourceCoopFeature &feature)
	{
		UpperCaseString(szFeature, szFeature, strlen(szFeature)+1);
		return this.GetValue(szFeature, feature);
	}
}