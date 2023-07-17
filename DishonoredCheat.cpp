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
	constexpr::std::ptrdiff_t darkVisionInfo = 0x00000AB0;
	constexpr::std::ptrdiff_t darkVisionDuration = 0x00000094;
	constexpr::std::ptrdiff_t blinkDistOp = 0x007F5E1F;
	constexpr::std::ptrdiff_t blinkDistOpX = 0x007F5E29;
	constexpr::std::ptrdiff_t blinkDistOpY = 0x007F5E44;
	constexpr::std::ptrdiff_t blinkDistOpZ = 0x007F5E4C;
	constexpr::std::ptrdiff_t blinkDistX = 0x00000060;
	constexpr::std::ptrdiff_t blinkDistY = 0x00000064;
	constexpr::std::ptrdiff_t blinkDistZ = 0x00000068;
	constexpr::std::ptrdiff_t blinkCooldown = 0x00000138;
	constexpr::std::ptrdiff_t possessionInfo = 0x00000AAC;
	constexpr::std::ptrdiff_t possessionDuration = 0x00000094;

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
	this->blinkMarkerLeft = false;
	this->blinkDistOpBroken = false;
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

void DishonoredCheat::BreakBlinkDistOp() {
	for (int i = 0; i < 3; i++) {
		if (!prevBlinkDistOpX[i])
			this->prevBlinkDistOpX[i] = gameMemory.Read<BYTE>(client + offsets::blinkDistOpX + (0x00000001 * i));
		if (!prevBlinkDistOpY[i])
			this->prevBlinkDistOpY[i] = gameMemory.Read<BYTE>(client + offsets::blinkDistOpY + (0x00000001 * i));
		if (!prevBlinkDistOpZ[i])
			this->prevBlinkDistOpZ[i] = gameMemory.Read<BYTE>(client + offsets::blinkDistOpZ + (0x00000001 * i));
		this->gameMemory.Write<BYTE>(client + offsets::blinkDistOpX + (0x00000001 * i), 144);
		this->gameMemory.Write<BYTE>(client + offsets::blinkDistOpY + (0x00000001 * i), 144);
		this->gameMemory.Write<BYTE>(client + offsets::blinkDistOpZ + (0x00000001 * i), 144);
	}
	this->blinkDistOpBroken = true;
}

void DishonoredCheat::RestoreBlinkDistOp() {
	for (int i = 0; i < 3; i++) {
		this->gameMemory.Write<BYTE>(client + offsets::blinkDistOpX + (0x00000001 * i), this->prevBlinkDistOpX[i]);
		this->gameMemory.Write<BYTE>(client + offsets::blinkDistOpY + (0x00000001 * i), this->prevBlinkDistOpY[i]);
		this->gameMemory.Write<BYTE>(client + offsets::blinkDistOpZ + (0x00000001 * i), this->prevBlinkDistOpZ[i]);
	}
	this->blinkDistOpBroken = false;
}

float DishonoredCheat::GetX() {
	return gameMemory.Read<float>(this->player + offsets::X);
}

float DishonoredCheat::GetY() {
	return gameMemory.Read<float>(this->player + offsets::Y);
}

float DishonoredCheat::GetZ() {
	return gameMemory.Read<float>(this->player + offsets::Z);
}

int DishonoredCheat::TeleportToCoords(float x, float y, float z) {
	gameMemory.Write<float>(this->player + offsets::X, x);
	gameMemory.Write<float>(this->player + offsets::Y, y);
	gameMemory.Write<float>(this->player + offsets::Z, z);
	return 1;
}

int DishonoredCheat::GetFov() {
	return 1;//gameMemory.Read<float>(client + offsets::fov);
}

int DishonoredCheat::SetFov(float fovCount) {
	//gameMemory.Write<float>(client + offsets::fov, (float)fovCount);
	return 1;
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

int DishonoredCheat::UnlimitedDarkVision() {
	gameMemory.Write<float>(gameMemory.Read<uintptr_t>(this->player + offsets::darkVisionInfo) + offsets::darkVisionDuration, 1);
	return 1;
}

int DishonoredCheat::SetBlinkDist(float x, float y, float z) {
	gameMemory.Write<float>(this->abilities + offsets::blinkDistX, x);
	gameMemory.Write<float>(this->abilities + offsets::blinkDistY, y);
	gameMemory.Write<float>(this->abilities + offsets::blinkDistZ, z);
	return 1;
}

int DishonoredCheat::LeaveBlinkMarker() {
	if (!this->blinkDistOpBroken)
		this->BreakBlinkDistOp();
	if (!blinkMarkerLeft) {
		this->blinkMarkerX = gameMemory.Read<float>(this->player + offsets::X);
		this->blinkMarkerY = gameMemory.Read<float>(this->player + offsets::Y);
		this->blinkMarkerZ = gameMemory.Read<float>(this->player + offsets::Z);
		blinkMarkerLeft = true;
	}
	this->SetBlinkDist(blinkMarkerX, blinkMarkerY, blinkMarkerZ);
	return 1;
}

int DishonoredCheat::RemoveBlinkMarker() {
	blinkMarkerLeft = false;
	RestoreBlinkDistOp();
	blinkDistOpBroken = false;
	return 1;
}

int DishonoredCheat::NoBlinkCooldown() {
	gameMemory.Write<float>(this->abilities + offsets::blinkCooldown, 1.0);
	return 1;
}

int DishonoredCheat::UnlimitedPossession() {
	gameMemory.Write<float>(gameMemory.Read<uintptr_t>(this->player + offsets::possessionInfo) + offsets::possessionDuration, 14.0);
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