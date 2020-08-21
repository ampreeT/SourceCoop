#if defined _srccoop_features_included
 #endinput
#endif
#define _srccoop_features_included

#define FEATURES_DEFAULT (FT_ALLOW_FLASHLIGHT | FT_FIRSTPERSON_DEATHCAM | FT_NO_TELEFRAGGING | FT_NO_FRIENDLYFIRE | FT_NOBLOCK)

enum SourceCoopFeature
{
	FT_ALLOW_FLASHLIGHT 		=	(1 << 0),
	FT_FIRSTPERSON_DEATHCAM 	=	(1 << 1),
	FT_INSTANCE_ITEMS			=	(1 << 2),
	FT_NO_TELEFRAGGING			=	(1 << 3),
	FT_NO_FRIENDLYFIRE			=	(1 << 4),
	FT_NOBLOCK					=	(1 << 5)
}

methodmap FeatureMap < StringMap
{
	public FeatureMap()
	{
		StringMap pMap = new StringMap();
		
		pMap.SetValue("ALLOW_FLASHLIGHT", FT_ALLOW_FLASHLIGHT);
		pMap.SetValue("FIRSTPERSON_DEATHCAM", FT_FIRSTPERSON_DEATHCAM);
		pMap.SetValue("INSTANCE_ITEMS", FT_INSTANCE_ITEMS);
		pMap.SetValue("DISABLE_TELEFRAGGING", FT_NO_TELEFRAGGING);
		pMap.SetValue("NO_FRIENDLYFIRE", FT_NO_FRIENDLYFIRE);
		pMap.SetValue("NOBLOCK", FT_NOBLOCK);
		
		return view_as<FeatureMap>(pMap);
	}
	
	public bool GetFeature(char[] szFeature, SourceCoopFeature &feature)
	{
		UpperCaseString(szFeature, szFeature, strlen(szFeature)+1);
		return this.GetValue(szFeature, feature);
	}
}