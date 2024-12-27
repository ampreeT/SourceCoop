#pragma newdecls required
#pragma semicolon 1

methodmap CSpriteOriented < CSprite
{
	public CSpriteOriented(const int iEntIndex = -1)
	{
		return view_as<CSpriteOriented>(CSprite(iEntIndex));
	}
	public static CSpriteOriented Create()
	{
		return CSpriteOriented(CreateEntityByName("env_sprite_oriented"));
	}
}
