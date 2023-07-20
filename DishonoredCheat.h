#pragma once

#include "memory.h"

class DishonoredCheat {
	Memory gameMemory;
	uintptr_t client;
	uintptr_t player;
	uintptr_t abilities;
	uintptr_t inventory;
	uintptr_t ammo;
	uintptr_t assets;
	bool maxHealthOpBroken;
	bool clipOpBroken;
	int prevMaxHealthOp[6];
	int prevHealthOp[6];
	int prevClipOp[6];
	bool blinkMarkerLeft;
	float blinkMarkerX;
	float blinkMarkerY;
	float blinkMarkerZ;
	bool blinkDistOpBroken;
	int prevBlinkDistOpX[3];
	int prevBlinkDistOpY[3];
	int prevBlinkDistOpZ[3];
	bool bendTimerExtended;

	public:
		DishonoredCheat();
		~DishonoredCheat();
		int RehookGame();
		int IsHooked();
		int UpdatePointers();
		int IsUpdated();
		int SyncPointers();
		int InfiniteHealth();
		void BreakMaxHealthOp();
		void RestoreMaxHealthOp();
		float GetX();
		float GetY();
		float GetZ();
		int TeleportToCoords(float x, float y, float z);
		int GetFov();
		int SetFov(float fovCount);
		int InfiniteMana();
		int InfiniteOxygen();
		int UnlimitedHealthElixir();
		int UnlimitedManaElixir();
		int InfiniteDarkVision();
		void BreakClipOp();
		void RestoreClipOp();
		int UnlimitedClip();
		int UnlimitedAmmo();
		int SetBlinkDist(float x, float y, float z);
		void BreakBlinkDistOp();
		void RestoreBlinkDistOp();
		int LeaveBlinkMarker();
		int RemoveBlinkMarker();
		int NoBlinkCooldown();
		int SetBlinkHeight(float height);
		int InfinitePossession();
		int InfiniteBendTime();
		int InfiniteBendTimeOff();
		int IncreaseElixir(const int type, const int amount);
		int IncreaseAmmoCount(const int type, const int amount);
		int SetAmmoCount(const int type, const int amount);
		int IncreaseResourceCount(const int amount);
		uintptr_t getClientAddress();
		uintptr_t getPlayerAddress();
		uintptr_t getInventoryAddress();
		uintptr_t getAbilitiesAddress();
		uintptr_t getAssets();
};