#include "DishonoredCheat.h"
#include "memory.h"
#include <cstring>
#include <cstddef>
#include <iostream>

namespace offsets {
	constexpr::std::ptrdiff_t player = 0x0105F628;
	constexpr::std::ptrdiff_t inventory = 0x0000059C;
	constexpr::std::ptrdiff_t ammo = 0x000000BC;
	constexpr::std::ptrdiff_t resources = 0x000000C8;
	constexpr::std::ptrdiff_t abilities = 0x00000AA8;

	constexpr::std::ptrdiff_t resources_size = 0x000000CC;
	constexpr::std::ptrdiff_t resources_first = 0x00000004;
	constexpr::std::ptrdiff_t resources_gap = 0x0000000C;

	//ability offsets
	constexpr::std::ptrdiff_t blinkCooldown = 0x00000138;

	//player offsets
	constexpr::std::ptrdiff_t X = 0x000000C4;
	constexpr::std::ptrdiff_t Y = 0x000000C8;
	constexpr::std::ptrdiff_t Z = 0x000000CC;
	constexpr::std::ptrdiff_t health = 0x00000344;
	constexpr::std::ptrdiff_t maxHealth = 0x00000348;
	constexpr::std::ptrdiff_t healthOp = 0x0074E030;
	constexpr::std::ptrdiff_t maxHealthOp = 0x0074E01A;
	constexpr::std::ptrdiff_t mana = 0x00000A60;

	//inventory offsets
	constexpr::std::ptrdiff_t healthElixir = 0x000000D4;
	constexpr::std::ptrdiff_t manaElixir = 0x000000D8;

	//ammo offsets
	constexpr::std::ptrdiff_t bullets = 0x00000000;
	constexpr::std::ptrdiff_t explosiveBullets = 0x00000008;
	constexpr::std::ptrdiff_t crossbowBolts = 0x00000010;
	constexpr::std::ptrdiff_t crossbowSleepDarts = 0x00000018;
	constexpr::std::ptrdiff_t crossbowIncendiaryBolts = 0x00000020;
	constexpr::std::ptrdiff_t springrazors = 0x00000028;
	constexpr::std::ptrdiff_t grenades = 0x00000030;
	constexpr::std::ptrdiff_t stickyGrenades = 0x00000038;
	constexpr::std::ptrdiff_t clipOp = 0x0081CA54;
}

DishonoredCheat::DishonoredCheat() : gameMemory(Memory{ "Dishonored.exe" }) {
	this->client = this->gameMemory.GetModuleAddress("Dishonored.exe");
	this->player = this->gameMemory.Read<uintptr_t>(this->client + offsets::player);
	this->abilities = this->gameMemory.Read<std::uintptr_t>(this->player + offsets::abilities);
	this->inventory = this->gameMemory.Read<std::uintptr_t>(this->player + offsets::inventory);
	this->ammo = gameMemory.Read<std::uintptr_t>(this->inventory + offsets::ammo);
	this->maxHealthOpBroken = false;
	this->clipOpBroken = false;
}

DishonoredCheat::~DishonoredCheat() {
	if (prevMaxHealthOp && prevHealthOp)
		this->RestoreMaxHealthOp();

	delete & this->gameMemory;
	this->client = NULL;
	this->player = NULL;
	this->abilities = NULL;
	this->inventory = NULL;
	this->ammo = NULL;
}

int DishonoredCheat::RehookGame() {
	this->gameMemory = Memory{ "Dishonored.exe" };
	this->client = this->gameMemory.GetModuleAddress("Dishonored.exe");
	if (client == 0)
		return 0;
	this->player = this->gameMemory.Read<uintptr_t>(this->client + offsets::player);
	this->abilities = this->gameMemory.Read<std::uintptr_t>(this->player + offsets::abilities);
	this->inventory = this->gameMemory.Read<std::uintptr_t>(this->player + offsets::inventory);
	this->ammo = gameMemory.Read<std::uintptr_t>(this->inventory + offsets::ammo);
	return 1;
}

int DishonoredCheat::IsHooked() {
	if (this->client == this->gameMemory.GetModuleAddress("Dishonored.exe") && this->client != 0)
		return 1;
	return 0;
}

int DishonoredCheat::UpdatePointers() {
	this->player = this->gameMemory.Read<uintptr_t>(this->client + offsets::player);
	this->abilities = this->gameMemory.Read<std::uintptr_t>(this->player + offsets::abilities);
	this->inventory = this->gameMemory.Read<std::uintptr_t>(this->player + offsets::inventory);
	this->ammo = gameMemory.Read<std::uintptr_t>(this->inventory + offsets::ammo);
	if (this->player == 0)
		return 0;
	return 1;
}

int DishonoredCheat::IsUpdated() {
	if (this->player == this->gameMemory.Read<uintptr_t>(this->client + offsets::player))
		return 1;
	return 0;
}

int DishonoredCheat::SyncPointers() {
	if (!this->IsUpdated())
		if (!this->UpdatePointers())
			return 0;
	return 1;
}

void DishonoredCheat::BreakMaxHealthOp() {
	for (int i = 0; i < 6; i++) {
		if (!prevMaxHealthOp[i])
			this->prevMaxHealthOp[i] = gameMemory.Read<BYTE>(client + offsets::maxHealthOp + (0x00000001 * i));
		if (!prevHealthOp[i])
			this->prevHealthOp[i] = gameMemory.Read<BYTE>(client + offsets::healthOp + (0x00000001 * i));
		this->gameMemory.Write<BYTE>(client + offsets::maxHealthOp + (0x00000001 * i), 144);
		this->gameMemory.Write<BYTE>(client + offsets::healthOp + (0x00000001 * i), 144);
	}
	this->maxHealthOpBroken = true;
}

void DishonoredCheat::RestoreMaxHealthOp() {
	for (int i = 0; i < 6; i++) {
		this->gameMemory.Write<BYTE>(client + offsets::maxHealthOp + (0x00000001 * i), this->prevMaxHealthOp[i]);
		this->gameMemory.Write<BYTE>(client + offsets::healthOp + (0x00000001 * i), this->prevHealthOp[i]);
	}
	this->maxHealthOpBroken = false;
}

int DishonoredCheat::InfiniteHealth() {
	if (!maxHealthOpBroken)
		this->BreakMaxHealthOp();
	gameMemory.Write<int>(this->player + offsets::maxHealth, INT_MAX);
	gameMemory.Write<int>(this->player + offsets::health, INT_MAX);
	return 1;
}

int DishonoredCheat::InfiniteMana() {
	gameMemory.Write(this->player + offsets::mana, 100);
	return 1;
}

int DishonoredCheat::InfiniteHealthElixir() {
	gameMemory.Write(this->inventory + offsets::healthElixir, 10);
	return 1;
}

int DishonoredCheat::InfiniteManaElixir() {
	gameMemory.Write(this->inventory + offsets::manaElixir, 10);
	return 1;
}

int DishonoredCheat::NoBlinkCooldown() {
	gameMemory.Write<float>(this->abilities + offsets::blinkCooldown, 1.0);
	return 1;
}

int DishonoredCheat::IncreaseElixir(const int type, const int amount) {
	switch (type) {
	case 0:
		gameMemory.Write(this->inventory + offsets::healthElixir, gameMemory.Read<int>(this->inventory + offsets::healthElixir) + amount);
		return 1;
		break;
	case 1:
		gameMemory.Write(this->inventory + offsets::manaElixir, gameMemory.Read<int>(this->inventory + offsets::manaElixir) + amount);
		return 1;
		break;
	default:
		break;
	}
	return 0;
}

int DishonoredCheat::IncreaseAmmoCount(const int type, const int amount) {
	const uintptr_t typeAddress = this->ammo + (0x00000008 * type);
	gameMemory.Write<int>(typeAddress, gameMemory.Read<int>(typeAddress) + amount);
	return 1;
}

int DishonoredCheat::SetAmmoCount(const int type, const int amount) {
	const uintptr_t typeAddress = this->ammo + (0x00000008 * type);
	gameMemory.Write<int>(typeAddress, amount);
	return 1;
}

int DishonoredCheat::infiniteAmmo() {
	for (int i = 0; i < 7; i++)
		if (this->SetAmmoCount(i, 10) == 0)
			return 0;
	return 1;
}

void DishonoredCheat::BreakClipOp() {
	for (int i = 0; i < 6; i++) {
		if (!prevClipOp[i])
			this->prevClipOp[i] = gameMemory.Read<BYTE>(client + offsets::clipOp + (0x00000001 * i));
		this->gameMemory.Write<BYTE>(client + offsets::clipOp + (0x00000001 * i), 144);
	}
	clipOpBroken = true;
}

void DishonoredCheat::RestoreClipOp() {
	for (int i = 0; i < 6; i++) {
		this->gameMemory.Write<BYTE>(client + offsets::clipOp + (0x00000001 * i), this->prevClipOp[i]);
	}
	clipOpBroken = false;
}

int DishonoredCheat::InfiniteClip() {
	if (!clipOpBroken)
		this->BreakClipOp();
	return 1;
}

int DishonoredCheat::IncreaseResourceCount(const int amount) {
	const uintptr_t resources = this->gameMemory.Read<std::uintptr_t>(this->inventory + offsets::resources);
	const int curResourcesSize = gameMemory.Read<int>(inventory + offsets::resources_size);
	auto curResource = resources + offsets::resources_first;
	for (int i = 0; i < curResourcesSize; i++) {
		gameMemory.Write<uintptr_t>(curResource, gameMemory.Read<int>(curResource) + amount);
		curResource = curResource + offsets::resources_gap;
	}
	return 1;
}

uintptr_t DishonoredCheat::getClientAddress() {
	return client;
}

uintptr_t DishonoredCheat::getPlayerAddress()
{
	return player;
}

uintptr_t DishonoredCheat::getInventoryAddress()
{
	return inventory;
}

uintptr_t DishonoredCheat::getAbilitiesAddress() {
	return abilities;
}