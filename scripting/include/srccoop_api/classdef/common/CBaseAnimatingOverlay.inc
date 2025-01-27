#pragma newdecls required
#pragma semicolon 1

static stock const int MAX_OVERLAYS = 15;

enum AnimLayerFlags
{
	ANIM_LAYER_NONE			= 0x0000,
	ANIM_LAYER_ACTIVE		= 0x0001,
	ANIM_LAYER_AUTOKILL		= 0x0002,
	ANIM_LAYER_KILLME		= 0x0004,
	ANIM_LAYER_DONTRESTORE	= 0x0008,
	ANIM_LAYER_CHECKACCESS	= 0x0010,
	ANIM_LAYER_DYING		= 0x0020,
};

static int offs_m_fFlags = 0;               //0x0000
static int offs_m_bSequenceFinished = 4;    //0x0004
static int offs_m_bLooping = 5;             //0x0005
static int offs_m_nSequence = 8;            //0x0008
static int offs_m_flCycle = 12;             //0x000C
static int offs_m_flPrevCycle = 16;         //0x0010
static int offs_m_flWeight = 20;            //0x0014
static int offs_m_flPlaybackRate = 24;      //0x0018
static int offs_m_flBlendIn = 28;           //0x001C
static int offs_m_flBlendOut = 32;          //0x0020
static int offs_m_flKillRate = 36;          //0x0024
static int offs_m_flKillDelay = 40;         //0x0028
static int offs_m_flLayerAnimtime = 44;     //0x002C
static int offs_m_flLayerFadeOuttime = 48;  //0x0030
static int offs_m_nActivity = 52;           //0x0034
static int offs_m_nPriority = 56;           //0x0038
static int offs_m_nOrder = 60;              //0x003C
static int offs_m_flLastEventCheck = 64;    //0x0040
static int offs_m_flLastAccess = 68;        //0x0044
static int offs_m_pOwnerEntity = 72;        //0x0048
static int sizeof_CAnimationLayer = 76;     //0x004C

methodmap CAnimationLayer
{
	public static void InitClassdef(const GameData hGameConfig)
	{
		// ToDo: Implement if necessary
	}
	public CAnimationLayer(Address pAddress)
	{
		return view_as<CAnimationLayer>(pAddress);
	}

	property Address address
	{
		public get() {return view_as<Address>(this);}
	}
	property AnimLayerFlags m_fFlags
	{
		public get() {return view_as<AnimLayerFlags>(LoadFromAddress(this.address + offs_m_fFlags, NumberType_Int32));}
		public set(const AnimLayerFlags value) {StoreToAddress(this.address + offs_m_fFlags, value, NumberType_Int32);}
	}
	property bool m_bSequenceFinished
	{
		public get() {return LoadFromAddress(this.address + offs_m_bSequenceFinished, NumberType_Int8);}
		public set(bool value) {StoreToAddress(this.address + offs_m_bSequenceFinished, value, NumberType_Int8);}
	}
	property bool m_bLooping
	{
		public get() {return LoadFromAddress(this.address + offs_m_bLooping, NumberType_Int8);}
		public set(bool value) {StoreToAddress(this.address + offs_m_bLooping, value, NumberType_Int8);}
	}
	property int m_nSequence
	{
		public get() {return LoadFromAddress(this.address + offs_m_nSequence, NumberType_Int32);}
		public set(int value)
		{
			if (this.m_nSequence != value)
			{
				StoreToAddress(this.address + offs_m_nSequence, value, NumberType_Int32);
				this.NetworkStateChanged();
			}
		}
	}
	property float m_flCycle
	{
		public get() {return LoadFromAddress(this.address + offs_m_flCycle, NumberType_Int32);}
		public set(float value)
		{
			if (this.m_flCycle != value)
			{
				StoreToAddress(this.address + offs_m_flCycle, value, NumberType_Int32);
				this.NetworkStateChanged();
			}
		}
	}
	property float m_flPrevCycle
	{
		public get() {return LoadFromAddress(this.address + offs_m_flPrevCycle, NumberType_Int32);}
		public set(float value)
		{
			if (this.m_flPrevCycle != value)
			{
				StoreToAddress(this.address + offs_m_flPrevCycle, value, NumberType_Int32);
				this.NetworkStateChanged();
			}
		}
	}
	property float m_flWeight
	{
		public get() {return LoadFromAddress(this.address + offs_m_flWeight, NumberType_Int32);}
		public set(float value)
		{
			if (this.m_flWeight != value)
			{
				StoreToAddress(this.address + offs_m_flWeight, value, NumberType_Int32);
				this.NetworkStateChanged();
			}
		}
	}
	property float m_flPlaybackRate
	{
		public get() {return LoadFromAddress(this.address + offs_m_flPlaybackRate, NumberType_Int32);}
		public set(float value) {StoreToAddress(this.address + offs_m_flPlaybackRate, value, NumberType_Int32);}
	}
	property float m_flBlendIn
	{
		public get() {return LoadFromAddress(this.address + offs_m_flBlendIn, NumberType_Int32);}
		public set(float value) {StoreToAddress(this.address + offs_m_flBlendIn, value, NumberType_Int32);}
	}
	property float m_flBlendOut
	{
		public get() {return LoadFromAddress(this.address + offs_m_flBlendOut, NumberType_Int32);}
		public set(float value) {StoreToAddress(this.address + offs_m_flBlendOut, value, NumberType_Int32);}
	}
	property float m_flKillRate
	{
		public get() {return LoadFromAddress(this.address + offs_m_flKillRate, NumberType_Int32);}
		public set(float value) {StoreToAddress(this.address + offs_m_flKillRate, value, NumberType_Int32);}
	}
	property float m_flKillDelay
	{
		public get() {return LoadFromAddress(this.address + offs_m_flKillDelay, NumberType_Int32);}
		public set(float value) {StoreToAddress(this.address + offs_m_flKillDelay, value, NumberType_Int32);}
	}
	property float m_flLayerAnimtime
	{
		public get() {return LoadFromAddress(this.address + offs_m_flLayerAnimtime, NumberType_Int32);}
		public set(float value) {StoreToAddress(this.address + offs_m_flLayerAnimtime, value, NumberType_Int32);}
	}
	property float m_flLayerFadeOuttime
	{
		public get() {return LoadFromAddress(this.address + offs_m_flLayerFadeOuttime, NumberType_Int32);}
		public set(float value) {StoreToAddress(this.address + offs_m_flLayerFadeOuttime, value, NumberType_Int32);}
	}
	property Activity m_nActivity
	{
		public get() {return LoadFromAddress(this.address + offs_m_nActivity, NumberType_Int32);}
		public set(Activity value) {StoreToAddress(this.address + offs_m_nActivity, value, NumberType_Int32);}
	}
	property int m_nPriority
	{
		public get() {return LoadFromAddress(this.address + offs_m_nPriority, NumberType_Int32);}
		public set(int value) {StoreToAddress(this.address + offs_m_nPriority, value, NumberType_Int32);}
	}
	property int m_nOrder
	{
		public get() {return LoadFromAddress(this.address + offs_m_nOrder, NumberType_Int32);}
		public set(int value)
		{
			if (this.m_nOrder != value)
			{
				StoreToAddress(this.address + offs_m_nOrder, value, NumberType_Int32);
				this.NetworkStateChanged();
			}
		}
	}
	property float m_flLastEventCheck
	{
		public get() {return LoadFromAddress(this.address + offs_m_flLastEventCheck, NumberType_Int32);}
		public set(float value) {StoreToAddress(this.address + offs_m_flLastEventCheck, value, NumberType_Int32);}
	}
	property float m_flLastAccess
	{
		public get() {return LoadFromAddress(this.address + offs_m_flLastAccess, NumberType_Int32);}
		public set(float value) {StoreToAddress(this.address + offs_m_flLastAccess, value, NumberType_Int32);}
	}
	property CBaseAnimatingOverlay m_pOwnerEntity
	{
		public get()
		{
			return CBaseAnimatingOverlay(GetEntityFromAddress(LoadFromAddress(this.address + offs_m_pOwnerEntity, NumberType_Int32)));
		}
		public set(CBaseAnimatingOverlay pOverlay)
		{
			StoreToAddress(this.address + offs_m_pOwnerEntity, pOverlay.address, NumberType_Int32);
		}
	}

	public void NetworkStateChanged()
	{
		CBaseAnimatingOverlay pEntity = this.m_pOwnerEntity;
		if (pEntity != NULL_CBASEENTITY)
		{
			ChangeEdictState(pEntity.entindex);
		}
	}

	public void Init(CBaseAnimatingOverlay pOverlay)
	{
		this.m_pOwnerEntity = pOverlay;
		this.m_fFlags = ANIM_LAYER_NONE;
		this.m_flWeight = 0.0;
		this.m_flCycle = 0.0;
		this.m_flPrevCycle = 0.0;
		this.m_bSequenceFinished = false;
		this.m_nActivity = ACT_INVALID;
		this.m_nSequence = 0;
		this.m_nPriority = 0;
		this.m_nOrder = MAX_OVERLAYS;
		this.m_flBlendIn = 0.0;
		this.m_flBlendOut = 0.0;
		this.m_flKillRate = 100.0;
		this.m_flKillDelay = 0.0;
		this.m_flPlaybackRate = 1.0;
		this.m_flLastEventCheck = 0.0;
		this.m_flLastAccess = GetGameTime();
		this.m_flLayerAnimtime = 0.0;
		this.m_flLayerFadeOuttime = 0.0;
	}

	// Inline methods per SSDK13
	public bool IsActive()    { return !!(this.m_fFlags & ANIM_LAYER_ACTIVE); }
	public bool IsAutokill()  { return !!(this.m_fFlags & ANIM_LAYER_AUTOKILL); }
	public bool IsKillMe()    { return !!(this.m_fFlags & ANIM_LAYER_KILLME); }
	public bool IsDying()     { return !!(this.m_fFlags & ANIM_LAYER_DYING); }
    public bool IsAutoramp()  { return (this.m_flBlendIn != 0.0 || this.m_flBlendOut != 0.0); }
	public void KillMe()      { this.m_fFlags |= ANIM_LAYER_KILLME; }
	public void Dying()       { this.m_fFlags |= ANIM_LAYER_DYING; }
	public void Dead()        { this.m_fFlags &= ~ANIM_LAYER_DYING; }
	public void MarkActive()  { this.m_flLastAccess = GetGameTime(); }
	public bool IsAbandoned()
	{
		return (this.IsActive() && !this.IsAutokill() && !this.IsKillMe() && this.m_flLastAccess > 0.0 && GetGameTime() - this.m_flLastAccess > 0.2);
	}
};

methodmap CBaseAnimatingOverlay < CBaseAnimating
{
	public static void InitClassdef(const GameData hGameConfig)
	{
		CAnimationLayer.InitClassdef(hGameConfig);
	}
	public CBaseAnimatingOverlay(const int iEntIndex = -1)
	{
		return view_as<CBaseAnimatingOverlay>(CBaseAnimating(iEntIndex));
	}

	property CUtlVector m_AnimOverlay
	{
		public get()
		{
			return CUtlVector(this.address + FindDataMapInfo(this.entindex, "m_AnimOverlay"));
		}
	}

	public int GetNumAnimOverlays()
	{
		return this.m_AnimOverlay.Count();
	}
	public CAnimationLayer GetAnimOverlay(const int iIndex)
	{
		CUtlVector pOverlayList = this.m_AnimOverlay;
		if (iIndex < 0 || iIndex >= pOverlayList.Count())
			ThrowError("Index %d out of bounds", iIndex);
		
		return CAnimationLayer(pOverlayList.Get(iIndex, sizeof_CAnimationLayer));
	}
	public bool IsValidLayer(int iLayer)
	{
		return (iLayer >= 0 && iLayer < this.GetNumAnimOverlays() && this.GetAnimOverlay(iLayer).IsActive());
	}
	public int FindGestureLayer(Activity activity)
	{
		CUtlVector pOverlayList = this.m_AnimOverlay;
		int iCount = pOverlayList.Count();

		for (int i = 0; i < iCount; i++)
		{
			CAnimationLayer pLayer = CAnimationLayer(pOverlayList.Get(i, sizeof_CAnimationLayer));

			if (!pLayer.IsActive())
				continue;

			if (pLayer.IsKillMe())
				continue;

			if (pLayer.m_nActivity == ACT_INVALID)
				continue;

			if (pLayer.m_nActivity == activity)
				return i;
		}

		return -1;
	}
	public bool IsPlayingGesture(Activity activity)
	{
		return this.FindGestureLayer(activity) != -1;
	}
	public void RemoveGesture(Activity activity)
	{
		int iLayer = this.FindGestureLayer(activity);
		if (iLayer == -1)
			return;
		
		this.RemoveLayer(iLayer);
	}
	public void RemoveAllGestures()
	{
		int iCount = this.m_AnimOverlay.Count();

		for (int i = 0; i < iCount; i++)
		{
			this.RemoveLayer(i);
		}
	}
	public void RemoveLayer(int iLayer, float flKillRate = 0.2, float flKillDelay = 0.0)
	{
		if (!this.IsValidLayer(iLayer))
			return;

		CAnimationLayer pLayer = this.GetAnimOverlay(iLayer);
		if (flKillRate > 0)
		{
			pLayer.m_flKillRate = pLayer.m_flWeight / flKillRate;
		}
		else
		{
			pLayer.m_flKillRate = 100.0;
		}

		pLayer.m_flKillDelay = flKillDelay;
		pLayer.KillMe();
	}
	public void FastRemoveLayer(int iLayer)
	{
		if (!this.IsValidLayer(iLayer))
			return;

		int iCount = this.GetNumAnimOverlays();
		CAnimationLayer pLayerToRemove = this.GetAnimOverlay(iLayer);
		
		// shift the other layers down in order
		for (int i = 0; i < iCount; i++)
		{
			CAnimationLayer pLayer = this.GetAnimOverlay(i);
			if (pLayer.IsActive() && pLayer.m_nOrder > pLayerToRemove.m_nOrder)
			{
				pLayer.m_nOrder--;
			}
		}

		pLayerToRemove.Init(this);
	}
}
