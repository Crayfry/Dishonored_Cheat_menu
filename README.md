# Dishonored Cheat menu
Simple Cheat menu for Dishonored 1. Use at your own risk as this application might crash and corrupt your game/saves.

Uses ImGui and cazz's memory.h file. Big thanks to cazz (https://github.com/cazzwastaken) for memory.h and incredible ImGui tutorials. 

# Usage
This is an external cheat, meaning you do not need to tamper with any game files. All you need to do to get the cheat to work is click the "hook" button in the cheat, while the game is running. Then the cheat will attempt to find and hook onto the game. The cheat looks for "Dishonored.exe" specifically, so you might need to rename the game exe file. Once the game has been hooked, the cheat will start working.

# Features
Unlimited Health, Mana, Elixirs.

Add/Remove ammo and resources.

# Features in Development
Remove ability cooldowns.

Infinite ability timers.

Invisibility

# Build
If you'd like to build the project from the source, here is how to do it:

Download the project into its own folder

Use Visual Studio 2022 to open "DishonoredCheatImGui.sln"

At the top of Visual Studio, change the build from Debug to Release (it might already be on Release) and change x64 to x86 or Win32 (again it might already be on x86)

Now you are able to build the project- click on Build at the top-left of visual studio and click Build Solution.

The exe file should appear in a "build" file that wasn't in the folder before.

# Current Bugs/Unintended Features
Unlimited Health does not mean invincibility, since the program only executes cheats every 10 miliseconds.

The application taskbar name is a bunch of Chinese characters (when running, hover over the application on the taskbar).

When displaying the memory addresses of client, player, and inventory, values are in decimal instead of hex.

# Screenshots
![Dishonored1](https://github.com/Crayfry/Dishonored_Cheat_menu/assets/52294803/ea3b5db9-3d11-4e93-954f-5af661182dc2)
![Dishonored2](https://github.com/Crayfry/Dishonored_Cheat_menu/assets/52294803/8b680859-07e4-416c-83ad-b593cd76085e)
