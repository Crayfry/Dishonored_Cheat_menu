#pragma once

#include "memory.h"

class DishonoredCheat {
	Memory gameMemory;
	uintptr_t client;
	uintptr_t player;
	uintptr_t inventory;
	uintptr_t abilities;
	uintptr_t equipment;
	uintptr_t weaponry;
	bool maxHealthOpBroken;
	int prevMaxHealthOp[6];
	int prevHealthOp[6];

	public:
		DishonoredCheat();
		~DishonoredCheat();
		int RehookGame();
		int IsHooked();
		int GetPlayer();
		int UpdatePlayer();
		int InfiniteHealth();
		int BreakMaxHealthOp();
		int RestoreMaxHealthOp();
		int InfiniteMana();
		int InfiniteHealthElixir();
		int InfiniteManaElixir();
		int InfiniteClip();
		int infiniteAmmo();
		int NoBlinkCooldown();
		int IncreaseElixir(const int type, const int amount);
		int IncreaseAmmoCount(const int type, const int amount);
		int SetAmmoCount(const int type, const int amount);
		int IncreaseResourceCount(const int amount);
		uintptr_t getClientAddress();
		uintptr_t getPlayerAddress();
		uintptr_t getInventoryAddress();
		uintptr_t getAbilitiesAddress();
};