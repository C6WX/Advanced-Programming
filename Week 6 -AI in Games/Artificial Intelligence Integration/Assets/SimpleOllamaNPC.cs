using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;
using TMPro;
using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.UI;
using Debug = UnityEngine.Debug;

public class SimpleOllamaNPC : MonoBehaviour
{
    [Header("Ollama Settings")]
    public string ollamaUrl = "http://localhost:11434/api/chat";
    public string modelName = "llama3.2:3b";

    [Header("NPC Personalities")]
    public List<NPCPersonality> personalities = new List<NPCPersonality>()
    {
        new NPCPersonality
        {
            characterName = "Nervous Tavern Keeper",
            systemPrompt = "You are a nervous medieval tavern keeper worried about rats in the cellar. Stay in character and keep replies to 2 short sentences."
        },
        new NPCPersonality
        {
            characterName = "Grumpy Blacksmith",
            systemPrompt = "You are a blunt, impatient blacksmith who only cares about weapons, armor, and hot iron. Stay in character and keep replies to 2 short sentences."
        }
    };

    [Header("UI References")]
    public TMP_InputField inputField;
    public Button sendButton;
    public TMP_Text outputText;
    public ScrollRect scrollRect;
    public TMP_Dropdown personalityDropdown; // Optional

    [Header("Telemetry CSV")]
    public bool logTelemetryToCsv = true;
    public string csvFileName = "ollama_npc_telemetry.csv";

    private List<ChatMessage> chatHistory = new List<ChatMessage>();
    private int currentPersonalityIndex = 0;
    private bool isWaitingForResponse = false;
    private string csvFilePath;
    private string ollamaVersion = "unknown";

    void Start()
    {
        csvFilePath = Path.Combine(Application.persistentDataPath, csvFileName);

        if (logTelemetryToCsv)
        {
            InitializeCsv();
            Debug.Log($"[SimpleOllamaNPC] CSV Telemetry saved at: {csvFilePath}");
        }

        StartCoroutine(FetchOllamaVersion());

        if (sendButton != null)
            sendButton.onClick.AddListener(OnSendClicked);

        if (inputField != null)
            inputField.onSubmit.AddListener(_ => OnSendClicked());

        if (personalityDropdown != null)
        {
            personalityDropdown.ClearOptions();
            List<string> names = new List<string>();
            foreach (var p in personalities)
                names.Add(p.characterName);

            personalityDropdown.AddOptions(names);
            personalityDropdown.onValueChanged.AddListener(SwitchPersonality);
        }

        if (personalities.Count > 0)
            SwitchPersonality(0);
    }

    // Call this from UI Buttons (e.g., SwitchPersonality(0), SwitchPersonality(1)) or Dropdown
    public void SwitchPersonality(int index)
    {
        if (index < 0 || index >= personalities.Count) return;

        currentPersonalityIndex = index;
        NPCPersonality selected = personalities[index];

        chatHistory.Clear();
        chatHistory.Add(new ChatMessage
        {
            role = "system",
            content = selected.systemPrompt
        });

        if (outputText != null)
        {
            outputText.text = $"<i>[Speaking with: {selected.characterName}]</i>\n\n";
            RefreshScroll();
        }
    }

    public void OnSendClicked()
    {
        if (isWaitingForResponse || inputField == null) return;

        string userText = inputField.text.Trim();
        if (string.IsNullOrEmpty(userText)) return;

        inputField.text = "";
        inputField.ActivateInputField();

        AppendToDialogue($"You: {userText}\n");
        chatHistory.Add(new ChatMessage { role = "user", content = userText });

        StartCoroutine(SendChatRequest(userText));
    }

    private IEnumerator FetchOllamaVersion()
    {
        string versionUrl = ollamaUrl.Replace("/api/chat", "/api/version");
        using (UnityWebRequest req = UnityWebRequest.Get(versionUrl))
        {
            yield return req.SendWebRequest();
            if (req.result == UnityWebRequest.Result.Success && !string.IsNullOrEmpty(req.downloadHandler.text))
            {
                OllamaVersionResponse ver = JsonUtility.FromJson<OllamaVersionResponse>(req.downloadHandler.text);
                if (ver != null && !string.IsNullOrEmpty(ver.version))
                    ollamaVersion = ver.version;
            }
        }
    }

    private IEnumerator SendChatRequest(string userPrompt)
    {
        isWaitingForResponse = true;
        if (sendButton != null) sendButton.interactable = false;

        string npcName = personalities.Count > 0
            ? personalities[currentPersonalityIndex].characterName
            : "Character";

        string thinkingTag = $"{npcName}: <i>thinking...</i>\n\n";
        AppendToDialogue(thinkingTag);

        OllamaChatRequest payload = new OllamaChatRequest
        {
            model = modelName,
            messages = chatHistory,
            stream = false
        };

        byte[] bodyRaw = Encoding.UTF8.GetBytes(JsonUtility.ToJson(payload));

        Stopwatch stopwatch = new Stopwatch();
        stopwatch.Start();

        using (UnityWebRequest req = new UnityWebRequest(ollamaUrl, "POST"))
        {
            req.uploadHandler = new UploadHandlerRaw(bodyRaw);
            req.downloadHandler = new DownloadHandlerBuffer();
            req.SetRequestHeader("Content-Type", "application/json");

            yield return req.SendWebRequest();
            stopwatch.Stop();

            if (outputText != null && outputText.text.EndsWith(thinkingTag))
                outputText.text = outputText.text.Substring(0, outputText.text.Length - thinkingTag.Length);

            double unityRoundTripMs = stopwatch.Elapsed.TotalMilliseconds;

            if (req.result == UnityWebRequest.Result.Success)
            {
                OllamaChatResponse parsed = JsonUtility.FromJson<OllamaChatResponse>(req.downloadHandler.text);

                string reply = (parsed != null && parsed.message != null && !string.IsNullOrEmpty(parsed.message.content))
                    ? parsed.message.content.Trim()
                    : "[Empty response]";

                chatHistory.Add(new ChatMessage { role = "assistant", content = reply });
                AppendToDialogue($"{npcName}: {reply}\n\n");

                if (logTelemetryToCsv && parsed != null)
                    LogTelemetryRow(npcName, userPrompt, reply, unityRoundTripMs, parsed);
            }
            else
            {
                Debug.LogError($"[SimpleOllamaNPC] Error: {req.error}");
                AppendToDialogue($"[Error: {req.error}]\n\n");
            }
        }

        isWaitingForResponse = false;
        if (sendButton != null) sendButton.interactable = true;
    }

    private void AppendToDialogue(string line)
    {
        if (outputText == null) return;
        outputText.text += line;
        RefreshScroll();
    }

    private void RefreshScroll()
    {
        if (outputText == null) return;

        Canvas.ForceUpdateCanvases();
        RectTransform contentRect = outputText.transform.parent as RectTransform;
        if (contentRect != null)
            LayoutRebuilder.ForceRebuildLayoutImmediate(contentRect);

        if (scrollRect != null)
            scrollRect.verticalNormalizedPosition = 0f;
    }

    private void InitializeCsv()
    {
        if (File.Exists(csvFilePath)) return;

        string header = string.Join(",",
            "Timestamp",
            "ModelName",
            "OllamaVersion",
            "Personality",
            "PromptTokens",
            "GeneratedTokens",
            "TotalInferenceMs",
            "ModelLoadMs",
            "PromptEvalMs",
            "TokenGenMs",
            "TokensPerSec",
            "UnityResponseTimeMs",
            "Platform",
            "OperatingSystem",
            "CPU",
            "CPUCores",
            "SystemRAM_MB",
            "GPU",
            "GPUMemory_MB"
        );

        File.WriteAllText(csvFilePath, header + Environment.NewLine, Encoding.UTF8);
    }

    private void LogTelemetryRow(string personalityName, string prompt, string reply, double unityResponseMs, OllamaChatResponse res)
    {
        string usedModel = !string.IsNullOrEmpty(res.model) ? res.model : modelName;
        double totalMs = res.total_duration / 1_000_000.0;
        double loadMs = res.load_duration / 1_000_000.0;
        double promptEvalMs = res.prompt_eval_duration / 1_000_000.0;
        double genMs = res.eval_duration / 1_000_000.0;
        double tokensPerSec = (res.eval_duration > 0)
            ? res.eval_count / (res.eval_duration / 1_000_000_000.0)
            : 0.0;

        string row = string.Join(",",
            EscapeCsv(DateTime.UtcNow.ToString("o")),
            EscapeCsv(usedModel),
            EscapeCsv(ollamaVersion),
            EscapeCsv(personalityName),
            res.prompt_eval_count,
            res.eval_count,
            totalMs.ToString("F2"),
            loadMs.ToString("F2"),
            promptEvalMs.ToString("F2"),
            genMs.ToString("F2"),
            tokensPerSec.ToString("F2"),
            unityResponseMs.ToString("F2"),
            EscapeCsv(Application.platform.ToString()),
            EscapeCsv(SystemInfo.operatingSystem),
            EscapeCsv(SystemInfo.processorType),
            SystemInfo.processorCount,
            SystemInfo.systemMemorySize,
            EscapeCsv(SystemInfo.graphicsDeviceName),
            SystemInfo.graphicsMemorySize
        );

        File.AppendAllText(csvFilePath, row + Environment.NewLine, Encoding.UTF8);
    }

    private string EscapeCsv(string value)
    {
        if (string.IsNullOrEmpty(value)) return "\"\"";
        return "\"" + value.Replace("\"", "\"\"") + "\"";
    }

    // --- Helper Data Structures ---

    [Serializable]
    public class NPCPersonality
    {
        public string characterName;
        [TextArea(3, 8)]
        public string systemPrompt;
    }

    [Serializable]
    public class ChatMessage
    {
        public string role;
        public string content;
    }

    [Serializable]
    public class OllamaChatRequest
    {
        public string model;
        public List<ChatMessage> messages;
        public bool stream;
    }

    [Serializable]
    public class OllamaChatResponse
    {
        public string model;
        public ChatMessage message;
        public long total_duration;
        public long load_duration;
        public int prompt_eval_count;
        public long prompt_eval_duration;
        public int eval_count;
        public long eval_duration;
    }

    [Serializable]
    private class OllamaVersionResponse
    {
        public string version;
    }
}