<div align="center">

# 🎮 Game Specific Patches & DLL Wrappers  

***created and maintained by***

[![Chip-Biscuit Website](https://img.shields.io/badge/Chip--Biscuit-Website-blue?style=for-the-badge)](https://chip-biscuit.github.io/)

Reverse Engineering • Programming • Patching • Game Improvements • DLL Creation 

</div>

# Spider-Man-Shattered-Dimensions-PC-Fix

![shattereddimension](https://github.com/user-attachments/assets/d462980f-5290-4249-a518-e98806c0dcb6)

# Instructions
Go to releases and download the latest `SpidermanShatteredDimensions.zip`, extract the `d3d9.dll` and `d3d9.ini` files into your game folder next to the `Game.exe` file, and you are good to go! You can edit the settings you wish to use in the `d3d9.ini` file.

This fix is only designed to work in the Steam version of the game, as any other version has stability issues.

# Hobgoblin level freeze or crash issue
When playing the Hobgoblin level just after the 1st fight with Hobgoblin you will enter into a small building hallway where the game can either crash or freeze the image on you but still be playing in the background. There are a few different methods you can use to help try and fix this issue. It is also advised when getting to this part to run straight to the end of the corridor to help get past the guard who causes the freeze when coming out of the door.

Method 1 – If playing with an Nvidia graphics card go to Nvidia Control Panel and open 3D Manage Settings, go to Program Settings and select Spider-Man Shattered Dimensions exe file or if you can’t find it in the list add it by browsing to the game folder and selecting the exe file. Then change the setting Use Global Settings to High Performance NVIDIA Processor. This should then possibly stop the issue. On side note this will also help the games FPS be more stable.

Method 2 – If the game freezes press the pause button or esc and alt+tab out of the game and then go back into it once again and unpause and it should then resume like normal.

Method 3 – On both the Game.exe and Launcher.exe right click then and click properties, go to compatibility tab and change the compatibility mode to "Vista service pack 1" and tick the run as an administrator option and click apply.

Method 4 – If everything else fails then play this level with Windowed Mode and set the fps to 30.

# Resolution/Aspect Ratio
The default for resolution is set to `1920x1080` in the `d3d9.ini` file. Put the resolution that you wish to use in both the `ResolutionWidth` and `ResolutionHeight` field.

Resolution and aspect ratio edits are done together so you will only have to choose your resolution.

You can play the game in windowed mode by setting `WindowedMode` to `1`.

# FPS
The default FPS limit of the game is 60. You can set the game to play at the FPS you want in `d3d9.ini`.

Due to how the game is designed, problems will occur if the frame rate is much lower than the limit: **do not set the limit to a frame rate that isn't reached**.

# FOV
You can set `fov` to either `1` or `2` in the `d3d9.ini` file to increase the FOV in game.

The game will start with the original FOV by default. When in game, press the hotkey you chose in `[hotkey]keycodes.txt` and enter the virtual code in the ini to toggle between the original and the new FOV setting.

**General important note** – The game has bugs at higher FOVs, such as menus being misaligned, or enemies being able to see Spider-Man Noir when he is nowhere near them in the stealth sections. It is advised that the user uses the toggle feature during these moments to prevent these bugs.

# Vote to see the game return via GOG Dreamlist
If you are interested in potentially seeing this game easily available to purchase and use today then go and vote on the games GOG Dreamlist to help make this become a reality, you can vote for the game here and write a message about the game if you wish – https://www.gog.com/dreamlist/game/spider-man-shattered-dimensions 

# Issues/Problems
unfortunately this game has issues on AMD hardware, this fix has not been made with the intention of fixing issues on AMD as i do not have access to this hardware, this fix is only aimed at and tested on 
intel/nvidia hardware only. its possible in the future if i can get my hands on AMD it will be looked at so keep an eye for updates.
but be aware if you have AMD this fix probably wont work for you.

If you have any issues, with the fixes then please go to discord for help linked below. https://discord.gg/eVJ7sQH7Cc

# Credits

Credit to Elisha Riedlinger for the base wrapper and ThirteenAG.

Credit to GitHub user "SuperSamus" for pointing out that FPS limit should not be hardcoded to a high limit.

---

### Fix Enhancers  
https://fixenhancers.wixsite.com/fix-enhancers

“Creating compatibility fixes and enhancements for legacy PC games.”

# Chip
- founder
- reverse engineer
- programmer
- developer
- Game Preservationist
  
<img width="250" height="500" alt="my logoo" src="https://github.com/user-attachments/assets/9bb13d3f-0734-4f1d-b68f-14114b13744a" />


# JokerAlex21 
- founder
- admin
- tester 

<img width="250" height="250" alt="YouTube_Logo" src="https://github.com/user-attachments/assets/5c7204ca-4bca-4673-8117-965732e7ee6d" />
