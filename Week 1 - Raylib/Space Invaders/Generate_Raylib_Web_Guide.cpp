#include <iostream>
#include <fstream>

int main()
{
    std::ofstream outFile("Raylib_WebAssembly_Guide.md");

    if (!outFile.is_open())
    {
        std::cerr << "Error: Could not create output file!" << std::endl;
        return 1;
    }

    outFile << "# Raylib to WebAssembly (HTML5) Build & Deployment Guide\n\n";
    outFile << "This guide details the complete process for converting a Raylib C/C++ desktop game into a WebAssembly (WASM) build and publishing it on itch.io.\n\n";
    outFile << "---\n\n";

    outFile << "## 1. Prerequisites & Environment Setup\n\n";
    outFile << "### Install Emscripten (emsdk)\n";
    outFile << "1. Open Command Prompt and clone the Emscripten repository:\n";
    outFile << "   ```cmd\n";
    outFile << "   git clone [https://github.com/emscripten-core/emsdk.git](https://github.com/emscripten-core/emsdk.git) C:\\emsdk\n";
    outFile << "   cd C:\\emsdk\n";
    outFile << "   ```\n";
    outFile << "2. Install and activate the latest toolchain:\n";
    outFile << "   ```cmd\n";
    outFile << "   emsdk.bat install latest\n";
    outFile << "   emsdk.bat activate latest\n";
    outFile << "   ```\n";
    outFile << "3. Run the environment script:\n";
    outFile << "   ```cmd\n";
    outFile << "   emsdk_env.bat\n";
    outFile << "   ```\n\n";

    outFile << "### Resolve Windows Python Alias Conflicts\n";
    outFile << "If running `emsdk_env.bat` fails with a Python error:\n";
    outFile << "1. Open **Start Menu** -> Search for **App execution aliases**.\n";
    outFile << "2. Locate all toggles for **Python**, **Python3**, and **Python install manager** (`py.exe`, `pymanager.exe`).\n";
    outFile << "3. Toggle **OFF** all Python-related aliases to prevent Windows from redirecting calls to the Microsoft Store.\n\n";
    outFile << "---\n\n";

    outFile << "## 2. Source Code Preparation (Cross-Platform Guards)\n\n";
    outFile << "Web browsers run inside a sandboxed environment and cannot execute native Windows Desktop APIs (such as `winhttp.h` or `<process.h>`). Wrap desktop-only networking or threading logic inside `#ifndef PLATFORM_WEB` preprocessor guards.\n\n";
    outFile << "### C++ Code Structure (`Space_Invaders.cpp`)\n\n";
    outFile << "```cpp\n";
    outFile << "// 1. Desktop-only includes\n";
    outFile << "#ifndef PLATFORM_WEB\n";
    outFile << "#define WIN32_LEAN_AND_MEAN\n";
    outFile << "#define NOGDI\n";
    outFile << "#define NOUSER\n";
    outFile << "#include <windows.h>\n";
    outFile << "#include <winhttp.h>\n";
    outFile << "#include <process.h>\n";
    outFile << "#endif\n\n";
    outFile << "#include \"raylib.h\"\n";
    outFile << "#include <stdio.h>\n";
    outFile << "#include <stdlib.h>\n";
    outFile << "#include <string.h>\n";
    outFile << "#include <math.h>\n\n";
    outFile << "// Struct definitions\n";
    outFile << "typedef struct { Vector2 position; bool active; } Bullet;\n";
    outFile << "typedef struct { Vector2 position; Vector2 size; bool active; } Enemy;\n\n";
    outFile << "// Global state\n";
    outFile << "static float g_windSpeed = 12.0f;\n\n";
    outFile << "void FetchWeatherThread(void *param)\n";
    outFile << "{\n";
    outFile << "    (void)param;\n";
    outFile << "#ifndef PLATFORM_WEB\n";
    outFile << "    // Desktop HTTPS fetching logic here\n";
    outFile << "#endif\n";
    outFile << "}\n\n";
    outFile << "int main(void)\n";
    outFile << "{\n";
    outFile << "#ifndef PLATFORM_WEB\n";
    outFile << "    _beginthread(FetchWeatherThread, 0, NULL);\n";
    outFile << "#endif\n\n";
    outFile << "    InitWindow(800, 600, \"Space_Invaders\");\n";
    outFile << "    SetTargetFPS(60);\n\n";
    outFile << "    // Modern array initialization (prevents -Wmissing-braces)\n";
    outFile << "    Bullet bullets[5] = {};\n";
    outFile << "    Enemy enemies[40] = {};\n\n";
    outFile << "    while (!WindowShouldClose())\n";
    outFile << "    {\n";
    outFile << "        BeginDrawing();\n";
    outFile << "        ClearBackground((Color){ 15, 20, 35, 255 });\n";
    outFile << "        DrawText(\"SPACE INVADERS\", 20, 20, 20, WHITE);\n";
    outFile << "        EndDrawing();\n";
    outFile << "    }\n\n";
    outFile << "    CloseWindow();\n";
    outFile << "    return 0;\n";
    outFile << "}\n";
    outFile << "```\n\n";
    outFile << "---\n\n";

    outFile << "## 3. Compilation to WebAssembly\n\n";
    outFile << "### Step 1: Open Terminal & Activate Environment\n";
    outFile << "Open Command Prompt and configure environment paths:\n";
    outFile << "```cmd\n";
    outFile << "set PATH=C:\\raylib\\w64devkit\\bin;%PATH%\n";
    outFile << "C:\\emsdk\\emsdk_env.bat\n";
    outFile << "```\n\n";
    outFile << "### Step 2: Navigate to Project Directory & Create `/web`\n";
    outFile << "```cmd\n";
    outFile << "cd \"C:\\Github\\Advanced-Programming\\Week 1 - Raylib\\Space Invaders\"\n";
    outFile << "if not exist web mkdir web\n";
    outFile << "```\n\n";
    outFile << "### Step 3: Run the `emcc` Compiler\n";
    outFile << "Execute the Emscripten compiler command (Raylib 5.0+ incorporates utilities inside `rcore.c`):\n";
    outFile << "```cmd\n";
    outFile << "emcc -o web/index.html Space_Invaders.cpp C:/raylib/raylib/src/rcore.c C:/raylib/raylib/src/rshapes.c C:/raylib/raylib/src/rtextures.c C:/raylib/raylib/src/rtext.c C:/raylib/raylib/src/rmodels.c C:/raylib/raylib/src/raudio.c -Os -Wall -I C:/raylib/raylib/src -s USE_GLFW=3 -s ASYNCIFY -DPLATFORM_WEB --shell-file C:/raylib/raylib/src/shell.html\n";
    outFile << "```\n\n";
    outFile << "---\n\n";

    outFile << "## 4. Output Verification\n\n";
    outFile << "| File | Purpose |\n";
    outFile << "| :--- | :--- |\n";
    outFile << "| `index.html` | Web page shell containing canvas and WebGL renderer |\n";
    outFile << "| `index.js` | Emscripten JavaScript glue code loading WebAssembly |\n";
    outFile << "| `index.wasm` | Compiled WebAssembly binary bytecode |\n";
    outFile << "| `index.data` | *(Optional)* Embedded asset package (textures, audio, fonts) |\n\n";
    outFile << "---\n\n";

    outFile << "## 5. Local Testing\n\n";
    outFile << "Browsers block WebAssembly loaded directly via file paths (`file:///`). Test using a local HTTP server:\n";
    outFile << "1. Launch a local server in the `/web` directory:\n";
    outFile << "   ```cmd\n";
    outFile << "   python -m http.server 8000 --directory web\n";
    outFile << "   ```\n";
    outFile << "2. Open `http://localhost:8000` in Google Chrome or Microsoft Edge.\n\n";
    outFile << "---\n\n";

    outFile << "## 6. Deployment to itch.io\n\n";
    outFile << "1. **Package the Web Files:**\n";
    outFile << "   * Open the `/web` folder in File Explorer.\n";
    outFile << "   * Select `index.html`, `index.js`, and `index.wasm` (plus `index.data` if present).\n";
    outFile << "   * Right-click -> **Compress to ZIP file** (e.g., `web_build.zip`).\n";
    outFile << "   * *Ensure `index.html` is at the root level of the ZIP archive, not inside a subfolder.*\n\n";
    outFile << "2. **Configure itch.io Project Settings:**\n";
    outFile << "   * Log into [itch.io](https://itch.io) -> **Create New Project**.\n";
    outFile << "   * Set **Kind of project** to **HTML**.\n";
    outFile << "   * In **Uploads**, click **Upload files** and select `web_build.zip`.\n";
    outFile << "   * Check **\"This file will be played in the browser\"**.\n";
    outFile << "   * Under **Embed options**, select **Viewport dimensions** with Width `800` and Height `600`.\n";
    outFile << "   * Check **Enable fullscreen button**.\n";
    outFile << "   * Click **Save & view page** to publish.\n";

    outFile.close();
    std::cout << "Document successfully generated: Raylib_WebAssembly_Guide.md" << std::endl;

    return 0;
}