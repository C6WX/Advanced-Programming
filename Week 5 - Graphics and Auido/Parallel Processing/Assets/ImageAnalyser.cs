using System.IO;
using System.Diagnostics;
using Unity.Collections;
using Unity.Jobs;
using Unity.Burst;
using UnityEngine;

public class ImageAnalyser : MonoBehaviour
{
    void Start()
    {
        // Define path to the Textures folder in Assets
        string texturesFolderPath = Path.Combine(Application.dataPath, "Textures");

        if (!Directory.Exists(texturesFolderPath))
        {
            UnityEngine.Debug.LogError($"Textures folder not found at: {texturesFolderPath}");
            return;
        }

        // Scan and process every image file found in the folder
        string[] imageFiles = Directory.GetFiles(texturesFolderPath, "*.*");

        foreach (string filePath in imageFiles)
        {
            // Skip Unity meta files
            if (filePath.EndsWith(".meta")) continue;

            ProcessImageFromFile(filePath);
        }
    }

    void ProcessImageFromFile(string filePath)
    {
        // 1. Load image bytes from disk into a Texture2D
        byte[] fileData = File.ReadAllBytes(filePath);
        Texture2D tex = new Texture2D(2, 2, TextureFormat.RGBA32, false);
        
        if (!tex.LoadImage(fileData))
        {
            UnityEngine.Debug.LogWarning($"Failed to load image: {Path.GetFileName(filePath)}");
            Destroy(tex);
            return;
        }

        // 2. Get the native pixel memory buffer directly
        NativeArray<Color32> pixelColors = tex.GetPixelData<Color32>(0);
        int totalPixels = pixelColors.Length;

        // Allocate native array to store each pixel's red value in parallel
        NativeArray<long> redValues = new NativeArray<long>(totalPixels, Allocator.TempJob);

        // 3. Configure the parallel job struct
        ExtractRedJob redJob = new ExtractRedJob
        {
            Pixels = pixelColors,
            RedValues = redValues
        };

        // 4. Start timer, schedule job across CPU cores, and wait for completion
        Stopwatch stopwatch = Stopwatch.StartNew();

        JobHandle jobHandle = redJob.Schedule(totalPixels, 64);
        jobHandle.Complete(); // Blocks main thread until all worker threads finish

        stopwatch.Stop();

        // 5. Calculate final sum from the parallel results array
        long totalRedSum = 0;
        for (int i = 0; i < redValues.Length; i++)
        {
            totalRedSum += redValues[i];
        }

        // 6. Output log results and clean up unmanaged memory
        UnityEngine.Debug.Log($"Image: {Path.GetFileName(filePath)} | Total Red Sum: {totalRedSum} | Time: {stopwatch.ElapsedMilliseconds} ms");

        redValues.Dispose(); // Free native memory
        Destroy(tex);           // Clean up texture instance
    }
}

// Burst-compiled parallel job struct for high performance
[BurstCompile]
public struct ExtractRedJob : IJobParallelFor
{
    [ReadOnly] public NativeArray<Color32> Pixels;
    [WriteOnly] public NativeArray<long> RedValues;

    public void Execute(int index)
    {
        // Each thread writes to its own index, completely avoiding race conditions
        RedValues[index] = Pixels[index].r;
    }
}