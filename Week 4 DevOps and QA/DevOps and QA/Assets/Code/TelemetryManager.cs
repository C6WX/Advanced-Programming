using System.Collections;
using System.Text;
using UnityEngine;
using UnityEngine.Networking;

public class TelemetryManager : MonoBehaviour
{
    [Header("Supabase Configuration")]
    [SerializeField] private string supabaseUrl = "https://YOUR_PROJECT_REF.supabase.co";
    [SerializeField] private string supabaseAnonKey = "YOUR_ANON_KEY_HERE";

    [System.Serializable]
    private class TelemetryPayload
    {
        public float completion_time;
        public int death_count;
        public int jump_count;
    }

    public void SendData(Player player)
    {
        if (player == null) return;

        // Read variables directly from the Player script
        float finalTime = player.GetElapsedTime();
        int deaths = player.deathCount;
        int jumps = player.jumpCount;

        StartCoroutine(PostTelemetry(finalTime, deaths, jumps));
    }

    private IEnumerator PostTelemetry(float finalTime, int deaths, int jumps)
    {
        string endpoint = $"{supabaseUrl}/rest/v1/telemetry";

        // Create payload object matching database table columns
        TelemetryPayload payload = new TelemetryPayload
        {
            completion_time = finalTime,
            death_count = deaths,
            jump_count = jumps
        };

        string jsonPayload = JsonUtility.ToJson(payload);

        using (UnityWebRequest request = new UnityWebRequest(endpoint, "POST"))
        {
            byte[] bodyRaw = Encoding.UTF8.GetBytes(jsonPayload);
            request.uploadHandler = new UploadHandlerRaw(bodyRaw);
            request.downloadHandler = new DownloadHandlerBuffer();

            // HTTP Headers required by Supabase API
            request.SetRequestHeader("Content-Type", "application/json");
            request.SetRequestHeader("apikey", supabaseAnonKey);
            request.SetRequestHeader("Authorization", $"Bearer {supabaseAnonKey}");

            yield return request.SendWebRequest();

            if (request.result == UnityWebRequest.Result.Success)
            {
                Debug.Log("Telemetry successfully saved to online database!");
            }
            else
            {
                Debug.LogError($"Failed to send telemetry: {request.error}\n{request.downloadHandler.text}");
            }
        }
    }
}