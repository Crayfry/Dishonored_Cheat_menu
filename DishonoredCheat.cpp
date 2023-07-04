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


	constexpr::std::ptrdiff_t resources_size = 0x000000CC;
	constexpr::std::ptrdiff_t resources_first = 0x00000004;
	constexpr::std::ptrdiff_t resources_gap = 0x0000000C;

	//player offsets
	constexpr::std::ptrdiff_t X = 0x000000C4;
	constexpr::std::ptrdiff_t Y = 0x000000C8;
	constexpr::std::ptrdiff_t Z = 0x000000CC;
	constexpr::std::ptrdiff_t health = 0x00000344;
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
}

DishonoredCheat::DishonoredCheat() : gameMemory(Memory{ "Dishonored.exe" }) {
	this->client = this->gameMemory.GetModuleAddress("Dishonored.exe");
	this->player = this->gameMemory.Read<uintptr_t>(this->client + offsets::player);
	this->inventory = this->gameMemory.Read<std::uintptr_t>(this->player + offsets::inventory);
}

DishonoredCheat::~DishonoredCheat() {
	delete & this->gameMemory;
	this->client = NULL;
	this->player = NULL;
	this->inventory = NULL;
}

int DishonoredCheat::RehookGame() {
	this->gameMemory = Memory{ "Dishonored.exe" };
	this->client = this->gameMemory.GetModuleAddress("Dishonored.exe");
	if (client == 0)
		return 0;
	this->player = this->gameMemory.Read<uintptr_t>(this->client + offsets::player);
	this->inventory = this->gameMemory.Read<std::uintptr_t>(this->player + offsets::inventory);
	return 1;
}

int DishonoredCheat::IsHooked() {
	if (this->client == this->gameMemory.GetModuleAddress("Dishonored.exe") && this->client != 0)
		return 1;
	return 0;
}

int DishonoredCheat::GetPlayer() {
	this->player = this->gameMemory.Read<uintptr_t>(this->client + offsets::player);
	this->inventory = this->gameMemory.Read<std::uintptr_t>(this->player + offsets::inventory);
	if (player == 0)
		return 0;
	return 1;
}

int DishonoredCheat::UpdatePlayer() {
	if (this->player == this->gameMemory.Read<uintptr_t>(this->client + offsets::player))
		return 1;
	if (!this->IsHooked())
		return 0;
	return this->GetPlayer();
}

int DishonoredCheat::InfiniteHealth() {
	if (!this->UpdatePlayer())
		return 0;
	gameMemory.Write(this->player + offsets::health, 100);
	return 1;
}

int DishonoredCheat::InfiniteMana() {
	if (!this->UpdatePlayer())
		return 0;
	gameMemory.Write(this->player + offsets::mana, 100);
	return 1;
}

int DishonoredCheat::InfiniteHealthElixir() {
	if (!this->UpdatePlayer())
		return 0;
	gameMemory.Write(this->inventory + offsets::healthElixir, 100);
	return 1;
}

int DishonoredCheat::InfiniteManaElixir() {
	if (!this->UpdatePlayer())
		return 0;
	gameMemory.Write(this->inventory + offsets::manaElixir, 100);
	return 1;
}

int DishonoredCheat::IncreaseElixir(const int type, const int amount) {
	if (!this->UpdatePlayer())
		return 0;
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
}

int DishonoredCheat::IncreaseAmmoCount(const int type, const int amount) {
	if (!this->UpdatePlayer())
		return 0;
	const uintptr_t ammo = gameMemory.Read<std::uintptr_t>(this->inventory + offsets::ammo);
	const uintptr_t typeAddress = ammo + (0x00000008 * type);
	gameMemory.Write<int>(typeAddress, gameMemory.Read<int>(typeAddress) + amount);
	return 1;
}

int DishonoredCheat::IncreaseResourceCount(const int amount) {
	if (!this->UpdatePlayer())
		return 0;
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
