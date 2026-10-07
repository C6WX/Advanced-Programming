# Raylib Space Invaders GDD

## Approach 
I chose to build Space Invaders for my first Raylib project because it's simple core mechanics left ample room for new features to fulfill all the task goals. <br>
My primary focus was determining how to implement a web request feature and integrate the retrieved data effectively into the gameplay experience. My original plan was to build a dedicated leaderboard website that displayed top scores and updated automatically whenever players set new high scores. The game itself would pull these top scores from the database and display them directly on the game over screen to fully satisfy the requirement. However, during the early development phase, I explored an alternative idea that I ultimately decided to use instead: fetching a live weather report online and using real-world wind speeds to physically drift the player's bullets sideways in real time, simulating environmental weather within the game.

## Result 
I am extremely proud of the final result, which includes far more features and mechanics than I had originally planned to implement. Throughout the development process, I continuously expanded the project scope by adding elements such as a scoring system, which I kept even after changing my initial API plans. I also built a round system that continuously spawns new waves of enemies after the previous group is defeated, while gradually increasing enemy difficulty by stepping up their movement speed after every wave. I did much more than initially planned simply because I thoroughly enjoyed programming in C again. <br>
After finishing the main gameplay loop, I realised the project lacked audio, which meant I needed to find suitable audio files, write implementation code, and re-package the final build for Itch.io. I added sound effects for player shots, enemy explosions and losing, alongside continuous background music. Integrating audio was relatively straightforward but the trickiest part was ensuring every audio asset was properly preloaded and unloaded in memory to ensure a stable final build.

## Reflection
This task proved to be a valuable learning experience, especially since I had not written code in a very long time and had never worked with Raylib or Notepad++ before. My biggest takeaway was learning how to construct a complete game without relying on a full game engine, proving that simple games can remain highly entertaining and full of features. <br>
One major challenge I encountered was getting the packaged game to run properly in a web browser before uploading it. When hosted, the browser build would become permanently stuck on downloading without ever launching. After extensive trial and error, I traced the issue back to the build command line. To resolve this, I iteratively refined the packaging command using Gemini until it successfully generated a clean, error free web build. <br>
Another significant challenge was fine tuning the weather based wind mechanic. Initially, the raw wind values pulled from the weather data were far too strong, causing bullets to fly off screen and making enemies impossible to hit. After tweaking the underlying formulas and adjustment variable, I successfully balanced the system so that bullets curve based on real time wind conditions whilst still allowing players to reliably hit the enemies. <br>
Overall, completing the task offers long term benefits for my professional career. It introduced me to a practical, lightweight approach to game development that expands my overall programming capabilities and strengthens my project portfolio.

## Gameplay Video
https://youtu.be/KsvIUCqcStg?si=YQ2XMPAYP3Wjm0Wo

## Itch.io Link
https://c6wx.itch.io/raylib-space-invaders