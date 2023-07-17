# Dishonored Cheat menu
Simple Cheat menu for Dishonored 1 Steam version. Use at your own risk as this application might crash and corrupt your game/saves.

Uses ImGui and cazz's memory.h file. Big thanks to cazz (https://github.com/cazzwastaken) for memory.h and incredible ImGui tutorials. 

# Usage
This is an external cheat, meaning you do not need to tamper with any game files. All you need to do is click the "hook game" button in the cheat, while the game is running, then the cheat will attempt to find and hook onto the game. The cheat looks for "Dishonored.exe" specifically, so you might need to rename the game exe file. Once the game has been hooked, the cheat will start working.

# Features
Unlimited health, mana, elixirs, ammo, clip (no reload).

Remove Blink cooldown, Unlimited duration for Dark Vision and Possession, and setting blink marker on current position.

Add/Remove specific ammo, resources, and elixirs.

# Features in Development
Unlimited Time Bend.

Fast attack speed.

Remove all abilities.

Add/Remove all weapons.

Invisibility.

# Build
If you'd like to build the project from the source, here is how to do it:

Download the project into its own folder

Use Visual Studio 2022 to open "DishonoredCheatImGui.sln"

At the top of Visual Studio, change the build from Debug to Release (it might already be on Release) and change x64 to x86 or Win32 (again it might already be on x86)

Now you are able to build the project- click on Build at the top-left of visual studio and click Build Solution.

The exe file should appear in a "build" file that wasn't in the folder before.

# Current Bugs/Unintended Features
The application taskbar name is a bunch of Chinese characters (when running, hover over the application on the taskbar).

When exiting out of the cheat while having infinite clip or unlimited health or leave Blink marker on, the cheat will break the game and those functionalities will stay present until a new save is loading or the game is restarted. To avoid this, just uncheck these options before closing the cheat.

# Screenshots
![Dishonored](https://github.com/Crayfry/Dishonored_Cheat_menu/assets/52294803/ebc8f43b-8581-4486-85e2-9bada7de5704)
![Dishonored2](https://github.com/Crayfry/Dishonored_Cheat_menu/assets/52294803/1ed33fed-d3e0-4a94-8210-2994fa59f3db)

