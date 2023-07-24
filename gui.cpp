#include "gui.h"
#include <cstdio>
#include <string>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include <map>
#include <unordered_map>

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

	// optionsBool
	enum {
		invis, infHealth, infMana, infOxy, infHealthElixir, infManaElixir, infDarkVision, 
		blinkMarker, noBlinkCooldown, blinkHeightIncrease, infTimeBend, infPossession, infClip, infAmmo
	};

	// otherBool
	enum {
		gameHooked, showBinds, lastState
	};

	// predefining some stuff to simplify creation of gui
	const char* buttonIds[] = { "-5", "-1", "+1", "+5" };
	const int buttonVals[] = { -5, -1, +1, +5 };

	const char* elixirs[] = { "Health Elixir", "Mana Elixir" };
	const char* equipment[] = { "Bullets", "Explosive Bullets", "Crossbow Bolts", "CrossBow Sleep Darts",
								"Crossbow Incendiary Bolts", "Spring Razors", "Grenades", "Sticky Grenades" };

	static const char* successHook = "Successfully Hooked Game";
	static const char* failureHook = "Hook Failed";

	static DishonoredCheat* cheatInstance;

	const char* options[14] = { "Invisibility", "Infinite Health", "Infinite Mana", "Infinite Oxygen", "Unlimited Health Elixirs", 
								"Unlimited Mana Elixirs", "Infinite Dark Vision", "Leave Blink Destination", "No Blink Cooldown", "Increase Blink Height", 
								"Infinite Bend Time", "Infinite Possession", "Infinite Clip", "Unlimited Ammo" };
	const int optionsSize = 14;

	static bool optionsBool[14] = { false, false, false, false, false , false , false,
				false, false, false, false, false, false };

	static bool otherBool[3] = { false, false, false };

	static std::map<const char*, int> keyBinds;
	static std::map<const char*, char*> showKeyBinds;
	static bool once = false;
	if (!once) {
		for (auto option : options) {
			keyBinds[option] = '\0';
			showKeyBinds[option] = (char*) "Not Bound";
		}
	}
	once = true;

	static std::unordered_map<int, ImGuiKey> imGuiIntToKey = {
		{513,ImGuiKey_LeftArrow},
		{514,ImGuiKey_RightArrow},
		{515,ImGuiKey_UpArrow},
		{516,ImGuiKey_DownArrow},
		{517,ImGuiKey_PageUp},
		{518,ImGuiKey_PageDown},
		{519,ImGuiKey_Home},
		{520,ImGuiKey_End},
		{521,ImGuiKey_Insert},
		{522,ImGuiKey_Delete},
		{523,ImGuiKey_Backspace},
		{524,ImGuiKey_Space},
		{525,ImGuiKey_Enter},
		{526,ImGuiKey_Escape},
		{527,ImGuiKey_LeftCtrl},
		{528,ImGuiKey_LeftShift},
		{529,ImGuiKey_LeftAlt},
		{530,ImGuiKey_LeftSuper},
		{531,ImGuiKey_RightCtrl},
		{532,ImGuiKey_RightShift},
		{533,ImGuiKey_RightAlt},
		{534,ImGuiKey_RightSuper},
		{535,ImGuiKey_Menu},
		{536,ImGuiKey_0},
		{537,ImGuiKey_1},
		{538,ImGuiKey_2},
		{539,ImGuiKey_3},
		{540,ImGuiKey_4},
		{541,ImGuiKey_5},
		{542,ImGuiKey_6},
		{543,ImGuiKey_7},
		{544,ImGuiKey_8},
		{545,ImGuiKey_9},
		{546,ImGuiKey_A},
		{547,ImGuiKey_B},
		{548,ImGuiKey_C},
		{549,ImGuiKey_D},
		{550,ImGuiKey_E},
		{551,ImGuiKey_F},
		{552,ImGuiKey_G},
		{553,ImGuiKey_H},
		{554,ImGuiKey_I},
		{555,ImGuiKey_J},
		{556,ImGuiKey_K},
		{557,ImGuiKey_L},
		{558,ImGuiKey_M},
		{559,ImGuiKey_N},
		{560,ImGuiKey_O},
		{561,ImGuiKey_P},
		{562,ImGuiKey_Q},
		{563,ImGuiKey_R},
		{564,ImGuiKey_S},
		{565,ImGuiKey_T},
		{566,ImGuiKey_U},
		{567,ImGuiKey_V},
		{568,ImGuiKey_W},
		{569,ImGuiKey_X},
		{570,ImGuiKey_Y},
		{571,ImGuiKey_Z},
		{572,ImGuiKey_F1},
		{573,ImGuiKey_F2},
		{574,ImGuiKey_F3},
		{575,ImGuiKey_F4},
		{576,ImGuiKey_F5},
		{577,ImGuiKey_F6},
		{578,ImGuiKey_F7},
		{579,ImGuiKey_F8},
		{580,ImGuiKey_F9},
		{581,ImGuiKey_F10},
		{582,ImGuiKey_F11},
		{583,ImGuiKey_F12},
		{584,ImGuiKey_Apostrophe},
		{585,ImGuiKey_Comma},
		{586,ImGuiKey_Minus},
		{587,ImGuiKey_Period},
		{588,ImGuiKey_Slash},
		{589,ImGuiKey_Semicolon},
		{590,ImGuiKey_Equal},
		{591,ImGuiKey_LeftBracket},
		{592,ImGuiKey_Backslash},
		{593,ImGuiKey_RightBracket},
		{594,ImGuiKey_GraveAccent},
		{595,ImGuiKey_CapsLock},
		{596,ImGuiKey_ScrollLock},
		{597,ImGuiKey_NumLock},
		{598,ImGuiKey_PrintScreen},
		{599,ImGuiKey_Pause},
		{600,ImGuiKey_Keypad0},
		{601,ImGuiKey_Keypad1},
		{602,ImGuiKey_Keypad2},
		{603,ImGuiKey_Keypad3},
		{604,ImGuiKey_Keypad4},
		{605,ImGuiKey_Keypad5},
		{606,ImGuiKey_Keypad6},
		{607,ImGuiKey_Keypad7},
		{608,ImGuiKey_Keypad8},
		{609,ImGuiKey_Keypad9},
		{610,ImGuiKey_KeypadDecimal},
		{611,ImGuiKey_KeypadDivide},
		{612,ImGuiKey_KeypadMultiply},
		{613,ImGuiKey_KeypadSubtract},
		{614,ImGuiKey_KeypadAdd},
		{615,ImGuiKey_KeypadEnter},
		{616,ImGuiKey_KeypadEqual}
	};

	static std::unordered_map<ImGuiKey, int> imGuiToVK = {
		// predefining misc keys
		{ImGuiKey_Tab, VK_TAB},
		{ImGuiKey_LeftArrow, VK_LEFT},
		{ImGuiKey_RightArrow, VK_RIGHT},
		{ImGuiKey_UpArrow, VK_UP},
		{ImGuiKey_DownArrow, VK_DOWN},
		{ImGuiKey_PageUp, VK_PRIOR},
		{ImGuiKey_PageDown, VK_NEXT},
		{ImGuiKey_Home, VK_HOME},
		{ImGuiKey_End, VK_END},
		{ImGuiKey_Insert, VK_INSERT},
		{ImGuiKey_Delete, VK_DELETE},
		{ImGuiKey_Backspace, VK_BACK},
		{ImGuiKey_Space, VK_SPACE},
		{ImGuiKey_Enter, VK_RETURN},
		{ImGuiKey_Escape, VK_ESCAPE},
		{ImGuiKey_LeftCtrl, VK_CONTROL},
		{ImGuiKey_LeftShift, VK_LSHIFT},
		{ImGuiKey_LeftAlt, VK_LMENU},
		{ImGuiKey_RightCtrl, VK_CONTROL},
		{ImGuiKey_RightShift, VK_RSHIFT},
		{ImGuiKey_RightAlt, VK_RMENU},
		{ImGuiKey_Menu, VK_MENU},
		{ImGuiKey_Apostrophe, VK_OEM_7},
		{ImGuiKey_Comma, VK_OEM_COMMA},
		{ImGuiKey_Minus, VK_OEM_MINUS},
		{ImGuiKey_Period, VK_OEM_PERIOD},
		{ImGuiKey_Slash, VK_OEM_2},
		{ImGuiKey_Semicolon, VK_OEM_1},
		{ImGuiKey_Equal, VK_OEM_PLUS},
		{ImGuiKey_LeftBracket, VK_OEM_4},
		{ImGuiKey_Backslash, VK_OEM_5},
		{ImGuiKey_RightBracket, VK_OEM_6},
		{ImGuiKey_GraveAccent, VK_OEM_3},
		{ImGuiKey_CapsLock, VK_CAPITAL},
		{ImGuiKey_ScrollLock, VK_SCROLL},
		{ImGuiKey_NumLock, VK_NUMLOCK},
		{ImGuiKey_PrintScreen, VK_SNAPSHOT},
		{ImGuiKey_Pause, VK_PAUSE},
		{ImGuiKey_Keypad0, VK_NUMPAD0},
		{ImGuiKey_Keypad1, VK_NUMPAD1},
		{ImGuiKey_Keypad2, VK_NUMPAD2},
		{ImGuiKey_Keypad3, VK_NUMPAD3},
		{ImGuiKey_Keypad4, VK_NUMPAD4},
		{ImGuiKey_Keypad5, VK_NUMPAD5},
		{ImGuiKey_Keypad6, VK_NUMPAD6},
		{ImGuiKey_Keypad7, VK_NUMPAD7},
		{ImGuiKey_Keypad8, VK_NUMPAD8},
		{ImGuiKey_Keypad9, VK_NUMPAD9},
		{ImGuiKey_KeypadDecimal, VK_DECIMAL},
		{ImGuiKey_KeypadDivide, VK_DIVIDE},
		{ImGuiKey_KeypadMultiply, VK_MULTIPLY},
		{ImGuiKey_KeypadSubtract, VK_SUBTRACT},
		{ImGuiKey_KeypadAdd, VK_ADD},
		{ImGuiKey_KeypadEnter, VK_RETURN}
	};

	// adding the alphabet
	for (int i = 0; i < 26; i++) {
		imGuiToVK[imGuiIntToKey[i+546]] = 65 + i;
	}
	// adding function keys
	for (int i = 0; i < 12; i++) {
		imGuiToVK[imGuiIntToKey[i+572]] = 112 + i;
	}
	// adding number keys
	for (int i = 0; i < 10; i++) {
		imGuiToVK[imGuiIntToKey[i+536]] = 48 + i;
	}

	// Button Binds
	if (!otherBool[showBinds])
		for (int i = 0; i < optionsSize; i++)
			if (!otherBool[lastState] && GetAsyncKeyState(keyBinds[options[i]]))
				optionsBool[i] = !optionsBool[i];

	static float curX, curY, curZ;
	static char posXInput[128];
	static char posYInput[128];
	static char posZInput[128];
	static float posX, posY, posZ;

	if (otherBool[showBinds]) {
		ImGui::SetNextWindowPos({ 0, 0 });
		ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
		ImGui::Begin("Binds Window", &exit, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
		static bool bindingPage = false;
		static int type = NULL;
		for (int i = 0; i < optionsSize; i++) {
			ImGui::Text(showKeyBinds[options[i]]);
			ImGui::SameLine();
			if (ImGui::Button("Bind")) {
				bindingPage = true;
				type = i;
			}
			ImGui::PushID("Bind");
			ImGui::SameLine();
			if (ImGui::Button("Unbind")) {
				keyBinds[options[i]] = '\0';
				showKeyBinds[options[i]] = (char*)"Not Bound";
			}
			ImGui::PushID("Unbind");
			ImGui::SameLine();
			ImGui::Text(": ");
			ImGui::SameLine();
			ImGui::Text(options[i]);
			
		}
		if (bindingPage) {
			ImGui::SetNextWindowPos({ 0, 0 });
			ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
			ImGui::Begin("Awaiting Keyboard Input", &exit, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
			ImGui::Text("Awaiting Keyboard Input");
			if (ImGui::Button("Cancel")) {
				bindingPage = false;
				type = NULL;
			}
			for (int j = 512; j <= 616; j++) {
				if (ImGui::IsKeyPressed(imGuiIntToKey[j])) {
					showKeyBinds[options[type]] = (char *) ImGui::GetKeyName(imGuiIntToKey[j]);
					keyBinds[options[type]] = imGuiToVK.at(imGuiIntToKey[j]);
					bindingPage = false;
					type = NULL;
				}
			}
			ImGui::End();
		}
		if (ImGui::Button("Close")) {
			otherBool[showBinds] = false;
			otherBool[lastState] = true;
		}
		ImGui::End();
	} else {
		ImGui::SetNextWindowPos({ 0, 0 });
		ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
		ImGui::Begin("Dishonored Cheat", &exit, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

		// This makes sure the pointers are up-to date
		if (otherBool[gameHooked])
			cheatInstance->SyncPointers();

		//	Hook button
		if (ImGui::Button("Hook Game")) {
			cheatInstance = new DishonoredCheat();
			if (cheatInstance->IsHooked())
				otherBool[gameHooked] = true;
			else
				otherBool[gameHooked] = false;
		}
		ImGui::SameLine();
		if (otherBool[gameHooked])
			ImGui::TextColored(ImVec4(0, 185, 0, 100), successHook);
		else
			ImGui::TextColored(ImVec4(185, 0, 0, 100), failureHook);

		if (ImGui::Button("Bind Buttons")) {
			otherBool[showBinds] = true;
			otherBool[lastState] = true;
		}

		// This displays memory slots for the client, player, inventory
		if (cheatInstance) {
			ImGui::Text("Client: %x", cheatInstance->getClientAddress());
			ImGui::Text("Player: %x", cheatInstance->getPlayerAddress());
			ImGui::Text("Abilities: %x", cheatInstance->getAbilitiesAddress());
			ImGui::Text("Inventory: %x", cheatInstance->getInventoryAddress());
			ImGui::Text("Assets: %x", cheatInstance->getAssets());
		}

		// Players GUI section
		ImGui::SeparatorText("Player");
		if (cheatInstance) {
			ImGui::Text("Current Coords");
			ImGui::Text("X: %f", cheatInstance->GetX());
			ImGui::Text("Y: %f", cheatInstance->GetY());
			ImGui::Text("Z: %f", cheatInstance->GetZ());
		}

		// teleportation
		ImGui::Text("Teleport to Coords");
		ImGui::InputText("X Coord", posXInput, IM_ARRAYSIZE(posXInput));
		ImGui::InputText("Y Coord", posYInput, IM_ARRAYSIZE(posYInput));
		ImGui::InputText("Z Coord", posZInput, IM_ARRAYSIZE(posZInput));
		if (posXInput[0] == '~')
			posX = cheatInstance->GetX();
		else
			posX = atof(posXInput);
		if (posYInput[0] == '~')
			posY = cheatInstance->GetY();
		else
			posY = atof(posYInput);
		if (posZInput[0] == '~')
			posZ = cheatInstance->GetZ();
		else
			posZ = atof(posZInput);

		if (ImGui::Button("Teleport") && otherBool[gameHooked]) {
			if (!cheatInstance->TeleportToCoords(posX, posY, posZ) && !cheatInstance->IsHooked()) {
				otherBool[gameHooked] = false;
			}
		}
		
		// Checkmark look for all checkmarks
		for (int i = 0; i < optionsSize; i++) {
			if (i == 0)
				ImGui::SeparatorText("Player");
			if (i == infDarkVision)
				ImGui::SeparatorText("Abilities");
			if (i == infClip)
				ImGui::SeparatorText("Ammo");
			ImGui::Checkbox(options[i], &optionsBool[i]);
			if (optionsBool[i] && otherBool[gameHooked])
				cheatInstance->ActivateCheat(i);
			if (i == invis || i == infHealth || i == blinkMarker || i == infClip)
				if (!optionsBool[i] && otherBool[gameHooked])
					cheatInstance->RepairCheat(i);
		}
		// Ammo GUI section
		for (int i = 0; i < 7; i++) {
			ImGui::Text(equipment[i]);
			for (int j = 0; j < 4; j++) {
				ImGui::SameLine();
				if (ImGui::Button(buttonIds[j]) && otherBool[gameHooked]) {
					cheatInstance->IncreaseAmmoCount(i, buttonVals[j]);
				}
				ImGui::PushID(buttonIds[j]);
			}
		}

		// Resources GUI section
		ImGui::SeparatorText("Resources");
		for (int i = 0; i < 2; i++) {
			ImGui::Text(elixirs[i]);
			for (int j = 0; j < 4; j++) {
				ImGui::SameLine();
				if (ImGui::Button(buttonIds[j]) && otherBool[gameHooked]) {
					cheatInstance->IncreaseElixir(i, buttonVals[j]);
				}
				ImGui::PushID(buttonIds[j]);
			}
		}

		ImGui::Text("Gold / Runes / Rewires");
		for (int j = 0; j < 4; j++) {
			ImGui::SameLine();
			if (ImGui::Button(buttonIds[j]) && otherBool[gameHooked]) {
				cheatInstance->IncreaseResourceCount(buttonVals[j]);
			}
			ImGui::PushID(buttonIds[j]);
		}

		otherBool[lastState] = false;

		for (int i = 0; i < optionsSize; i++)
			if (GetAsyncKeyState(keyBinds[options[i]])) {
				otherBool[lastState] = true;
				break;
			}

		ImGui::End();
	}
}