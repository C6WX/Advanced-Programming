# Multiplayer and Networking Commentary

## How the networking element was implemented (protocol/library/engine API used, and what data is synced)
The game uses WebRTC and PeerJS over 4 letter room codes to connect two browsers, with Canvas 2D and Web Audio for the game. Player 1 runs the ball physics and sends balls physics and sends ball, score, status and player 1 paddle data every 25ms, whilst player 2 sends their own paddle position every 25ms.


## Clear steps on how to run and test it on other devices to verify that it works
1. Open `index.html` (or the hosted GitHub Pages URL) on **Device 1 (PC)** and click **Host** to generate a 4-letter room code.
2. Open the same URL on **Device 2 (Phone or second PC)**, enter the room code, and click **Join**.
3. Move paddles via touch drag, on-screen buttons, mouse, or `W/S` / `↑/↓` keys (first to 7 points wins).

## Personal Contribution
Before the game was created on HTML, I started making the first iteration of the project in Unity, this version was working until I hit a road block with implementing the multiplayer. Due to the time limit on the task, the best method to implement this into Unity is using cloud saves or IP addresses. The cloud saves were not working on this PC and I did not like the idea of having players share their IP addresses with each other so we moved onto looking into making the project with HTML and created a new repository to start again.

## Declarations 
- Gemini was used to create the code 