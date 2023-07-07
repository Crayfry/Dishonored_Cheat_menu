#include "gui.h"
#include <cstdio>
#include <string>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter);

extern long __stdcall WindowProcess(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter) {

	if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
		return true;

	switch (message)
	{
	case WM_SIZE: {
		if (gui::device && wideParameter != SIZE_MINIMIZED) {
			gui::presentParameters.BackBufferWidth = LOWORD(longParameter);
			gui::presentParameters.BackBufferHeight = HIWORD(longParameter);
			gui::ResetDevice();
		}
	}return 0;

	case WM_SYSCOMMAND: {
		if ((wideParameter & 0xfff0) == SC_KEYMENU)
			return 0;
	}break;

	case WM_DESTROY: {
		PostQuitMessage(0);
	}return 0;

	case WM_LBUTTONDOWN: {
		gui::position = MAKEPOINTS(longParameter);
	}return 0;

	case WM_MOUSEMOVE: {
		if (wideParameter == MK_LBUTTON) {
			const auto points = MAKEPOINTS(longParameter);
			auto rect = ::RECT{ };

			GetWindowRect(gui::window, &rect);

			rect.left += points.x - gui::position.x;
			rect.top += points.y - gui::position.y;

			if (gui::position.x >= 0 && gui::position.x <= gui::WIDTH && gui::position.y >= 0 && gui::position.y <= 19)
				SetWindowPos(gui::window, HWND_TOPMOST, rect.left, rect.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER);
		}
	}

	default:
		break;
	}

	return DefWindowProcW(window, message, wideParameter, longParameter);
}

void gui::CreateHWindow(const char* windowName, const char* className) noexcept {
	windowClass.cbSize = sizeof(WNDCLASSEXA);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = WindowProcess;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hIcon = 0;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = 0;
	windowClass.lpszMenuName = 0;
	windowClass.lpszClassName = className;
	windowClass.hIconSm = 0;

	RegisterClassExA(&windowClass);

	window = CreateWindowA(className, windowName, WS_POPUP, 100, 100, WIDTH, HEIGHT, 0, 0, windowClass.hInstance, 0);

	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);
}

void gui::DestroyHWindow() noexcept {
	DestroyWindow(window);
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

bool gui::CreateDevice() noexcept {
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d)
		return false;

	ZeroMemory(&presentParameters, sizeof(presentParameters));

	presentParameters.Windowed = TRUE;
	presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
	presentParameters.EnableAutoDepthStencil = TRUE;
	presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &presentParameters, &device) < 0)
		return false;

	return true;
}
void gui::ResetDevice() noexcept {
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = device->Reset(&presentParameters);
	if (result == D3DERR_INVALIDCALL)
		IM_ASSERT(0);

	ImGui_ImplDX9_CreateDeviceObjects();
}
void gui::DestroyDevice() noexcept {
	if (device) {
		device->Release();
		device = nullptr;
	}

	if (d3d) {
		d3d->Release();
		d3d = nullptr;
	}
}

void gui::CreateImGui() noexcept {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ::ImGui::GetIO();

	io.IniFilename = NULL;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
}
void gui::DestroyImGui() noexcept {
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void gui::BeginRender() noexcept {
	MSG message;
	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}
void gui::EndRender() noexcept {
	ImGui::EndFrame();

	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	if (device->BeginScene() >= 0) {
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	const auto result = device->Present(0, 0, 0, 0);

	if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		ResetDevice();
}
void gui::Render() noexcept {
	static DishonoredCheat* cheatInstance;
	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
	ImGui::Begin("Dishonored Cheat", &exit, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
	
	// GUI Begins here
	static bool gameHooked = false;
	static const char* successHook = "Successfully Hooked Game";
	static const char* failureHook = "Hook Failed";

	//	Hook button
	if (ImGui::Button("Hook Game")) {
		cheatInstance = new DishonoredCheat();
		if (cheatInstance->IsHooked())
			gameHooked = true;
		else
			gameHooked = false;
	}
	ImGui::SameLine();
	if (gameHooked)
		ImGui::TextColored(ImVec4(0, 185, 0, 100), successHook);
	else
		ImGui::TextColored(ImVec4(185, 0, 0, 100), failureHook);

	// This displays memory slots for the client, player, inventory
	if (cheatInstance) {
		ImGui::Text("Client:");
		ImGui::SameLine();
		ImGui::Text((std::to_string(cheatInstance->getClientAddress())).c_str());
		ImGui::Text("Player:");
		ImGui::SameLine();
		ImGui::Text((std::to_string(cheatInstance->getPlayerAddress())).c_str());
		ImGui::Text("Abilities:");
		ImGui::SameLine();
		ImGui::Text((std::to_string(cheatInstance->getAbilitiesAddress())).c_str());
		ImGui::Text("Inventory:");
		ImGui::SameLine();
		ImGui::Text((std::to_string(cheatInstance->getInventoryAddress())).c_str());
	}

	// Players GUI section
	ImGui::SeparatorText("Player");
	static bool infHealth = false;
	static bool MaxHealthOpBroken = false;
	ImGui::Checkbox("Unlimited Health", &infHealth);
	if (gameHooked && infHealth) {
		if (!cheatInstance->InfiniteHealth() && !cheatInstance->IsHooked()) {
			gameHooked = false;
		} else {
			MaxHealthOpBroken = true;
		}
	}
	if (gameHooked && !infHealth && MaxHealthOpBroken) {
		cheatInstance->RestoreMaxHealthOp();
		MaxHealthOpBroken = false;
	}

	static bool infMana = false;
	ImGui::Checkbox("Unlimited Mana", &infMana);
	if (gameHooked && infMana && !cheatInstance->InfiniteMana() && !cheatInstance->IsHooked())
		gameHooked = false;

	static bool infHealthElixir = false;
	ImGui::Checkbox("Unlimited Health Elixirs", &infHealthElixir);
	if (gameHooked && infHealthElixir && !cheatInstance->InfiniteHealthElixir() && !cheatInstance->IsHooked())
		gameHooked = false;

	static bool infManaElixir = false;
	ImGui::Checkbox("Unlimited Mana Elixirs", &infManaElixir);
	if (gameHooked && infManaElixir && !cheatInstance->InfiniteManaElixir() && !cheatInstance->IsHooked())
		gameHooked = false;

	// Abilities section
	ImGui::SeparatorText("Abilities");
	static bool noBlinkCooldown = false;
	ImGui::Checkbox("No Blink Cooldown", &noBlinkCooldown);
	if (gameHooked && noBlinkCooldown && !cheatInstance->NoBlinkCooldown() && !cheatInstance->IsHooked())
		gameHooked = false;

	// predefining button lists to simplify creation of buttons
	const char* buttonIds[] = { "-5", "-1", "+1", "+5" };
	const int buttonVals[] = { -5, -1, +1, +5 };

	// Resources GUI section
	ImGui::SeparatorText("Resources");
	const char* elixirs[] = { "Health Elixir", "Mana Elixir" };
	for (int i = 0; i < 2; i++) {
		ImGui::Text(elixirs[i]);
		for (int j = 0; j < 4; j++) {
			ImGui::SameLine();
			if (ImGui::Button(buttonIds[j]) && gameHooked) {
				cheatInstance->IncreaseElixir(i, buttonVals[j]);
			}
			ImGui::PushID(buttonIds[j]);
		}
	}

	ImGui::Text("Gold / Runes / Rewires");
	for (int j = 0; j < 4; j++) {
		ImGui::SameLine();
		if (ImGui::Button(buttonIds[j]) && gameHooked) {
			cheatInstance->IncreaseResourceCount(buttonVals[j]);
		}
		ImGui::PushID(buttonIds[j]);
	}
	/*
	const char* resources[] = { "Gold", "Runes", "Rewires" };
	for (int i = 0; i < 3; i++) {
		ImGui::Text(resources[i]);
		for (int j = 0; j < 4; j++) {
			ImGui::SameLine();
			if (ImGui::Button(buttonIds[j]) && gameHooked) {
				cheatInstance->IncreaseResourceCount(buttonVals[j]);
			}
			ImGui::PushID(buttonIds[j]);
		}
	}
	*/

	// Ammo GUI section
	ImGui::SeparatorText("Ammo");
	static bool infiniteClip = false;
	ImGui::Checkbox("Infinite Clip", &infiniteClip);
	if (gameHooked && infiniteClip && !cheatInstance->InfiniteClip() && cheatInstance->IsHooked())
		gameHooked = false;

	static bool infiniteAmmo = false;
	ImGui::Checkbox("Infinite Ammo", &infiniteAmmo);
	if (gameHooked && infiniteAmmo && !cheatInstance->infiniteAmmo() && !cheatInstance->IsHooked())
		gameHooked = false;

	const char* equipment[] = { "Bullets", "Explosive Bullets", "Crossbow Bolts", "CrossBow Sleep Darts", 
								"Crossbow Incendiary Bolts", "Spring Razors", "Grenades", "Sticky Grenades"};
	for (int i = 0; i < 7; i++) {
		ImGui::Text(equipment[i]);
		for (int j = 0; j < 4; j++) {
			ImGui::SameLine();
			if (ImGui::Button(buttonIds[j]) && gameHooked) {
				cheatInstance->IncreaseAmmoCount(i, buttonVals[j]);
			}
			ImGui::PushID(buttonIds[j]);
		}
	}
	ImGui::End();
}