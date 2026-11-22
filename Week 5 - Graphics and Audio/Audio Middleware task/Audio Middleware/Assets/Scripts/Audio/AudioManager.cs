using System.Collections.Generic;
using UnityEngine;
using FMODUnity;
using FMOD.Studio;

public class AudioManager : MonoBehaviour
{
    public static AudioManager instance { get; private set; }

    [Header("Background Music")]
    [SerializeField] private EventReference backgroundMusic;

    private EventInstance musicEventInstance;
    private readonly List<EventInstance> eventInstances = new List<EventInstance>();

    private void Awake()
    {
        if (instance != null)
        {
            Debug.LogError("More than one audio manager in scene.");
        }
        instance = this;
    }

    private void Start()
    {
        InitializeMusic(backgroundMusic);
    }

    public void PlayOneShot(EventReference sound, Vector3 worldPos)
    {
        if (sound.IsNull) return;
        RuntimeManager.PlayOneShot(sound, worldPos);
    }

    public EventInstance CreateInstance(EventReference eventReference)
    {
        EventInstance eventInstance = RuntimeManager.CreateInstance(eventReference);
        eventInstances.Add(eventInstance);
        return eventInstance;
    }

    private void InitializeMusic(EventReference musicEventReference)
    {
        if (musicEventReference.IsNull) return;

        musicEventInstance = CreateInstance(musicEventReference);
        musicEventInstance.start();
    }

    public void SetMusicParameter(string parameterName, float value)
    {
        if (musicEventInstance.isValid())
        {
            musicEventInstance.setParameterByName(parameterName, value);
        }
    }

    private void CleanUp()
    {
        foreach (EventInstance eventInstance in eventInstances)
        {
            eventInstance.stop(FMOD.Studio.STOP_MODE.IMMEDIATE);
            eventInstance.release();
        }
        eventInstances.Clear();
    }

    private void OnDestroy()
    {
        if (instance == this)
        {
            CleanUp();
            instance = null;
        }
    }
}