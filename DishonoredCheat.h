#pragma once

#include "memory.h"

class DishonoredCheat {
	Memory gameMemory;
	uintptr_t client;
	uintptr_t player;
	uintptr_t inventory;

	public:
		DishonoredCheat();
		~DishonoredCheat();
		int RehookGame();
		int IsHooked();
		int GetPlayer();
		int UpdatePlayer();
		int InfiniteHealth();
		int InfiniteMana();
		int InfiniteHealthElixir();
		int InfiniteManaElixir();
		int NoBlinkCooldown();
		int IncreaseElixir(const int type, const int amount);
		int IncreaseAmmoCount(const int type, const int amount);
		int IncreaseResourceCount(const int amount);
		uintptr_t getClientAddress();
		uintptr_t getPlayerAddress();
		uintptr_t getInventoryAddress();
};