# Dishonored Cheat menu
Simple Cheat menu for Dishonored 1 Steam version. Use at your own risk as this application might crash and corrupt your game/saves.

Uses ImGui and cazz's memory.h file. Big thanks to cazz (https://github.com/cazzwastaken) for memory.h and incredible ImGui tutorials. 

# Usage
This is an external cheat, meaning you do not need to tamper with any game files. All you need to do is click the "hook game" button in the cheat, while the game is running, then the cheat will attempt to find and hook onto the game. The cheat looks for "Dishonored.exe" specifically, so you might need to rename the game exe file. Once the game has been hooked, the cheat will start working.

# Features
Ability to bind buttons to cheats.

Player: invisibility, infinite health, infinite mana, unlimited elixirs, unlimited ammo, infinite clip (no reload), and teleport to specific coordinates ('~' will use current coordinates of that axis, just like minecraft).

Abilities: Remove Blink cooldown, increase Blink's height, setting the blink destination to current player position, infinite Dark Vision, infinite Possession, and infinite Bend Time.

Inventory: Add/Remove specific ammo, resources, and elixirs.

# Features in Development (these will be the last features added)

Set and reset character's stats, weapons, and abilities.

Although I'm driven to add the ability to reset character stats, weapons, etc. I do not believe it is possible, or if it is possible it is outside of my abilities and the scope of this project.

# Current Bugs/Unintended Features
When increasing runes, gold, or rewires, you must have atleast 1 to increase the amount. It works like this because the cheat only increases the amount of the items in the inventory but does not create new items in it.

The application taskbar name is a bunch of Chinese characters (when running, hover over the application on the taskbar).

When exiting out of the cheat while having infinite clip or unlimited health or leave Blink marker on, the cheat will break the game and those functionalities will stay present until a new save is loading or the game is restarted. To avoid this, just uncheck these options before closing the cheat.

# Build
If you'd like to build the project from the source, here is how to do it:

Download the project into its own folder

Use Visual Studio 2022 to open "DishonoredCheatImGui.sln"

At the top of Visual Studio, change the build from Debug to Release (it might already be on Release) and change x64 to x86 or Win32 (again it might already be on x86)

Now you are able to build the project- click on Build at the top-left of visual studio and click Build Solution.

The exe file should appear in a "build" file that wasn't in the folder before.

# Screenshots
![Dishonored](https://github.com/Crayfry/Dishonored_Cheat_menu/assets/52294803/0e4df8d6-5c9c-4f1e-9619-c13580af6eb4)
![Dishonored1](https://github.com/Crayfry/Dishonored_Cheat_menu/assets/52294803/b8b6a2e9-6fd8-498a-a69b-9c89937151b9)
