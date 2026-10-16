// Online C# compiler (editor) for free
// Write and run C# online using this editor.

using System;
using System.Collections.Generic;

 class Program
    {
     
        static void Main()
        {
            //need to store the lines to call up in a dictionary/list not write them straight up
            //Have a dictionary to store dialogue and a dictionary to say the dialogue and display the options.
            Dictionary<string, List<string>> dialogueStore = new();
            Dictionary<string, List<string>> dialogueOptions = new();
            dialogueStore["BorisAwake"] = new List<string> {"Boris: You're finally awake. It is time, are you ready for to do this?"};
            // Simple dialogue tree using adjacency list
            
            //Console.WriteLine(BorisAwake);
            dialogueOptions["Greeting"] = new List<string> {["BorisAwake"],"Yes", "No" };


            // Traverse dialogue
            foreach (var option in dialogue["Greeting"])
            {
                Console.WriteLine($"Option: {option}");                
            }
            string HeadOut = "";
            HeadOut = Console.ReadLine();
            if (HeadOut == "Yes")
            {
                Console.WriteLine("John: The timing is perfect to rob the jewelry store. Just need to decide the entrance. ");
            }
            else if (HeadOut == "No")
            {
                Console.WriteLine("You went back to sleep and the crew did the job without you. They were all arrested without your help. THE END.");
            }
        }
    }