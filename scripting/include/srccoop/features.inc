#if defined _srccoop_features_included
 #endinput
#endif
#define _srccoop_features_included

enum SourceCoopFeature
{
	FT_ALLOW_FLASHLIGHT 		=	(1 << 0),
	FT_FIRSTPERSON_DEATHCAM 	=	(1 << 1),
	FT_INSTANCE_ITEMS			=	(1 << 2),
	FT_NO_TELEFRAGGING			=	(1 << 3),
	FT_NO_FRIENDLYFIRE			=	(1 << 4),
	FT_NOBLOCK					=	(1 << 5),
	FT_SHOW_WELCOME_MESSAGE		=	(1 << 6),
	FT_HEV_SOUNDS				=	(1 << 7),
	FT_STRIP_DEFAULT_EQUIPMENT	=	(1 << 8),
	FT_STRIP_DEFAULT_EQUIPMENT_KEEPSUIT	=	(1 << 9),
	FT_DISABLE_CANISTER_DROPS	=	(1 << 10)
}

methodmap FeatureMap < StringMap
{
	public FeatureMap()
	{
		StringMap pMap = new StringMap();
		
		pMap.SetValue("ALLOW_FLASHLIGHT", FT_ALLOW_FLASHLIGHT);
		pMap.SetValue("FIRSTPERSON_DEATHCAM", FT_FIRSTPERSON_DEATHCAM);
		pMap.SetValue("INSTANCE_ITEMS", FT_INSTANCE_ITEMS);
		pMap.SetValue("NO_TELEFRAGGING", FT_NO_TELEFRAGGING);
		pMap.SetValue("NO_FRIENDLYFIRE", FT_NO_FRIENDLYFIRE);
		pMap.SetValue("NOBLOCK", FT_NOBLOCK);
		pMap.SetValue("SHOW_WELCOME_MESSAGE", FT_SHOW_WELCOME_MESSAGE);
		pMap.SetValue("HEV_SOUNDS", FT_HEV_SOUNDS);
		pMap.SetValue("STRIP_DEFAULT_EQUIPMENT", FT_STRIP_DEFAULT_EQUIPMENT);
		pMap.SetValue("STRIP_DEFAULT_EQUIPMENT_KEEPSUIT", FT_STRIP_DEFAULT_EQUIPMENT_KEEPSUIT);
		pMap.SetValue("DISABLE_CANISTER_DROPS", FT_DISABLE_CANISTER_DROPS);
		
		return view_as<FeatureMap>(pMap);
	}
	
	public bool GetFeature(char[] szFeature, SourceCoopFeature &feature)
	{
		UpperCaseString(szFeature, szFeature, strlen(szFeature)+1);
		return this.GetValue(szFeature, feature);
	}
}