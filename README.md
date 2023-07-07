# Dishonored Cheat menu
Simple Cheat menu for Dishonored 1. Use at your own risk as this application might crash and corrupt your game/saves.

Uses ImGui and cazz's memory.h file. Big thanks to cazz (https://github.com/cazzwastaken) for memory.h and incredible ImGui tutorials. 

# Usage
This is an external cheat, meaning you do not need to tamper with any game files. All you need to do is click the "hook game" button in the cheat, while the game is running, then the cheat will attempt to find and hook onto the game. The cheat looks for "Dishonored.exe" specifically, so you might need to rename the game exe file. Once the game has been hooked, the cheat will start working.

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
Unlimited Clip currently does not work and will crash your game when loading into another save.

The application taskbar name is a bunch of Chinese characters (when running, hover over the application on the taskbar).

When displaying the memory addresses of client, player, and inventory, values are in decimal instead of hex.

# Screenshots
![Dishonored](https://github.com/Crayfry/Dishonored_Cheat_menu/assets/52294803/3e740032-5b71-4e1f-930a-8729178a71cd)
![Dishonored1](https://github.com/Crayfry/Dishonored_Cheat_menu/assets/52294803/2b297736-8482-4d6b-8376-ce0b2b48da50)
