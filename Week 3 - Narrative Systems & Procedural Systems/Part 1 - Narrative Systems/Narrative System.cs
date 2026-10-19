using System;
using System.Collections.Generic;

class Program
{
    static void Main()
    {
       DialogueLine line0 = new DialogueLine("Boris: You're finally awake. It is time, are you ready for the job?");
        DialogueLine line1 = new DialogueLine("TO MAKE A CHOICE, YOU NEED TO INPUT THE NUMBER NEXT TO THE OPTION.");
        DialogueLine line2 = new DialogueLine("Are you ready to head out?\nOption 1: Yes\nOption 2: No", hasChoice: true);
        DialogueLine line3 = new DialogueLine("You went back to sleep and the crew did the job without you. They all died without your help. THE END.");
        DialogueLine line4 = new DialogueLine("John: The timing is perfect to rob the jewelry store. Just need to decide the entrance.");
        DialogueLine line5 = new DialogueLine("Which entrance did you want to use?\nOption 1: Main Entrance\nOption 2: Skylight", hasChoice: true);
        DialogueLine line6 = new DialogueLine("Trevor: The loud route is risky but might work");
        line6.AddEnding("You all enter the van and drive to the store");
        line6.AddEnding("The shop keeper sees you all out the front of the store holding guns and sets off the alarm.");
        line6.AddEnding("Everyone escapes in the van with no jewels. THE END");
        DialogueLine line7 = new DialogueLine("Trevor: The sneaky route, good choice. Ill grab the glass cutter and rappel gear.");
        DialogueLine line8 = new DialogueLine("You all enter the van and drive to the back of the store");
        DialogueLine line9 = new DialogueLine("Trevor: Boris, you stay and prepare to drive us away, the rest of you follow me.");
        DialogueLine line10 = new DialogueLine("You all see a guard watching over the ladder to the roof.");
        DialogueLine line11 = new DialogueLine("How do you deal with the guard?\nOption 1: Wait for the guard to leave\nOption 2: Have Boris get out and distract the guard", hasChoice: true);
        DialogueLine line12 = new DialogueLine("The guard left as the store locked up, the glass cutter isn't cut out to cut through metal shutters. THE END.");
        DialogueLine line13 = new DialogueLine("Boris ran over near the guard and tripped on the curb, drawing the guard over to check on him.");
        DialogueLine line14 = new DialogueLine("You all climb the ladder while boris shouts in pain, drowning out the noise of the ladder.");
        DialogueLine line15 = new DialogueLine("You all approach the glass roof. Trevor retrieves the glass cutter and places it on the glass, preparing to cut.");
        DialogueLine line16 = new DialogueLine("John prepares to cut the buildings power, deactivating the lights in the store. It turns out John has never seen a power system like this one, he asks you which wire he need to cut.");
        DialogueLine line17 = new DialogueLine("Which wire should he cut?\nOption 1: The wire with the bell symbol\nOption 2: The wire with the exclamation mark\nOption 3: The wire with the battery symbol", hasChoice: true);
        DialogueLine line18 = new DialogueLine("The alarm sounds and the police are called to the building, you are all arrested. THE END");
        DialogueLine line19 = new DialogueLine("The building goes onto lockdown, you all escape with no jewels. THE END");
        DialogueLine line20 = new DialogueLine("The lights in the shop go off and no alarm is sounded");
        DialogueLine line21 = new DialogueLine("Trevor gets the glass cut and you all rapel through the skylight");
        DialogueLine line22 = new DialogueLine("As you land in the store, you see a customer running for the exit");
        DialogueLine line23 = new DialogueLine("What do you do?\nOption 1: Shoot the customer\nOption 2: Let the customer escape\nOption 3: Tackle the customer and take them hostage", hasChoice: true);
        DialogueLine line24 = new DialogueLine("Everyone inside and outside the store hear the shot and start running as fast as they can to leave the area, with some on their phones to the police");
        DialogueLine line25 = new DialogueLine("The customer escapes and has likely called the police");
        DialogueLine line26 = new DialogueLine("How many jewels do you steal?\nOption 1: Grab them all\nOption 2: Grab a few and get out\nOption 3: Just leave", hasChoice: true);
        DialogueLine line27 = new DialogueLine("You all ignore the police being called and just go for the jewels, as you go to leave, you are met with the police. You all get arrested. THE END");
        DialogueLine line28 = new DialogueLine("You each grab a handful of jewels and get out. However Trevor gets greedy and stays back, causing him to leave as the cops arrive and gets arrested. You get away one member down and with only a small amount of jewels. THE END");
        DialogueLine line29 = new DialogueLine("You all escape before any police arrive, but with no jewels. THE END");
        DialogueLine line30 = new DialogueLine("As the customer didn't get away, no police have been called.");
        DialogueLine line31 = new DialogueLine("Everyone goes to take as many jewels as they can hold.");
        DialogueLine line32 = new DialogueLine("As you are stealing, you see the cashier shuffling around.");
        DialogueLine line33 = new DialogueLine("What do you do?\nOption 1: Send Trevor to check on her\nOption 2: Ignore her", hasChoice: true);
        DialogueLine line34 = new DialogueLine("She was about to hit the silent alarm but didn't make it.");
        DialogueLine line35 = new DialogueLine("All jewels have been stolen. The crew leaves the store and heads for the van.");
        DialogueLine line36 = new DialogueLine("You and the whole crew managed to get away with bags full of jewels. THE END");
        DialogueLine line37 = new DialogueLine("She sets off the silent alarm. As you leave, the police arrive at the door. Everyone is arrested. THE END.");
        
        List<DialogueLine> dialogueList = new List<DialogueLine> 
        { 
            line0, line1, line2, line3, line4, line5, line6, line7, line8, line9,
            line10, line11, line12, line13, line14, line15, line16, line17, line18, line19,
            line20, line21, line22, line23, line24, line25, line26, line27, line28, line29,
            line30, line31, line32, line33, line34, line35, line36, line37
        };

        for (int i = 0; i < dialogueList.Count; i++)
        {
            dialogueList[i].PrintSpeech();

            // End game automatically if the current line reaches a "THE END" scenario
            if (dialogueList[i].Speech.Contains("THE END"))
            {
                return;
            }

            if (dialogueList[i].HasChoice)
            {
                string choice = Console.ReadLine();

                if (choice == "1")
                {
                    if (i == 2)
                    {
                        i = 3; // Jump to line4
                    }
                    else if (i == 5)
                    {
                        // Main Entrance chosen -> Show line6 and terminate
                        return;
                    }
                    else if (i == 11)
                    {
                        dialogueList[12].PrintSpeech();
                        return;
                    }
                    else if (i == 17)
                    {
                        dialogueList[18].PrintSpeech();
                        return;
                    }
                    else if (i == 23)
                    {
                        // Shoot customer -> line24, then line26 choice
                        dialogueList[24].PrintSpeech();
                        i = 25; // Target line26
                    }
                    else if (i == 26)
                    {
                        dialogueList[27].PrintSpeech();
                        return;
                    }
                    else if (i == 33)
                    {
                        // Send Trevor -> line34 to line36
                        i = 33; // Target line34
                    }
                }
                else if (choice == "2")
                {
                    if (i == 2)
                    {
                        dialogueList[3].PrintSpeech();
                        return;
                    }
                    else if (i == 5)
                    {
                        i = 6; // Target line7 (Sneaky route)
                    }
                    else if (i == 11)
                    {
                        i = 12; // Target line13
                    }
                    else if (i == 17)
                    {
                        dialogueList[19].PrintSpeech();
                        return;
                    }
                    else if (i == 23)
                    {
                        // Let customer escape -> line25, then line26 choice
                        dialogueList[25].PrintSpeech();
                        i = 25; // Target line26
                    }
                    else if (i == 26)
                    {
                        dialogueList[28].PrintSpeech();
                        return;
                    }
                    else if (i == 33)
                    {
                        dialogueList[37].PrintSpeech();
                        return;
                    }
                }
                else if (choice == "3")
                {
                    if (i == 17)
                    {
                        i = 19; // Target line20 (Correct wire)
                    }
                    else if (i == 23)
                    {
                        // Hostage route -> line30 to line33
                        i = 29; // Target line30
                    }
                    else if (i == 26)
                    {
                        dialogueList[29].PrintSpeech();
                        return;
                    }
                }
            }
        }
    }
}

public class DialogueLine
{
    public string Speech;
    public bool HasChoice;
    public List<string> Endings = new List<string>();

    public DialogueLine(string speech, bool hasChoice = false)
    {
        Speech = speech;
        HasChoice = hasChoice;
    }

    public void AddEnding(string text)
    {
        Endings.Add(text);
    }

    public void PrintSpeech()
    {
        Console.WriteLine(Speech);

        foreach (string endingText in Endings)
        {
            Console.WriteLine(endingText);
        }
    }
}