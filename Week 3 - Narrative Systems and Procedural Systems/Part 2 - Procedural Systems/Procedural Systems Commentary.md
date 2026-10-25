# Procedural Systems Mini-Project Commentary

## How your system works (189)
My system is a real-time procedural audio generator built entirely within Unreal Engine 5's MetaSound environment. The pipeline begins by using a trigger repeat and a BPM To Seconds node to convert musical tempo into precise time intervals, driving continuous 16th-note clock pulses into a Trigger Counter node to control sequence step length.<br>
Each trigger pulse then goes through a Random Get (Float:Array) node which randomly pulls MIDI pitch values from a dynamic note array to alter the pitch of each pulse. <br>
To generate the synth's tone,the sounds waves Sine and Saw is used. The Sine wave rapidly blends the Saw wave back and forth thousands of times per second, modifying the pitch of the Saw wave at audio speed. This is called Frequency Modulation synthesis and creates metallic, complex synth tones. <br>
After synthesis, the raw audio is shaped by and AD Envelope node to give each note a punchy start and quick fade. It then passes through a Ladder Filter that dynamically changes brightness, a WaveShaper to add aggressive distortion and a Stereo Delay to bounce echoes across the left and right channels.

## Tools, algorithms, or APIs used (166)
For this system, I used Unreal Engine 5's MetaSound Graph Environment, using built in digital signal processing nodes to generate and process everything in real time. <br>
For the sound engine, I used Frequency Modulation synthesis, linking a Sine wave oscillator as a modulator directly into the frequency pin of a Saw wave carrier to create dynamic metallic tones. A 4-pole Ladder Filter and a WaveShaper handle the subtractive synthesis side, controlling cutoff frequencies and adding distortion. <br>
For timing and pitch generation, the system relies on several key algorithmic nodes. These include: BPM To Seconds which calculates exact musical step durations from tempo values, Musical Scale To Note Array that converts musical scales and modes into usable MIDI arrays, Random Get (Float:Array) and Random (Int) which are used as random generators that iterate through array values to drive pitch variation and Map Range (Float) and Clamp (Int32) that are used as mathematical mapping tools that dynamically scale exposed gameplay parameters like DangerLevel into raw audio parameters.

## What is procedural or generative about your approach
What makes this system procedural is that it replaces pre-recorded audio files with real-time, non-linear audio generation. Instead of looping a static file, the melody is created dynamically on every beat. The procedural elements work across three sections. <br>
The first is the generative melodies. The pitch sequence is not hardcoded. Each clock step signals a random selection node to pick notes from a scale array, making sure the melodic pattern constantly changes without repeating itself. <br>
The second is the dynamic gameplay response. Using a variable within the blueprint, it is possible to change the tone based on the tone of the game. Adjusting the DangerLevel parameter increases the FM modulation depth, opens the filter cutoff and adds distortion, taking the tone from clean and ambient to aggressive and harsh. <br>
The final layer is the runtime parameter controls. Having exposed parameters such as the BPM, step count and scales allows the entire system to reshape its tempo, rhythm and mood instantly during gameplay if needed, without requiring any extra audio assets.
