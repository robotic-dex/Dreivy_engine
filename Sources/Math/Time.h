#pragma once
#include <windows.h>

/*
 * Namespace Time
 * Provides:
 *  - deltaTime : time between frames (seconds)
 *  - time      : time since application start (seconds)
 *
 * Use Time::deltaTime and Time::time in game logic to make it
 * frame-rate independent, so physics and animations behave
 * the same on machines with different performance.
 */

namespace Time {

    inline float deltaTime = 0.0f;   // seconds between frames
    inline float time = 0.0f;   // seconds since start

    inline LARGE_INTEGER frequency{};
    inline LARGE_INTEGER startCounter{};
    inline LARGE_INTEGER lastCounter{};

    // Call this ONCE at the beginning of loop (like in Core::Update the very first line)
    inline void Update()
    {
        // First call initialization
        if (frequency.QuadPart == 0) {
            QueryPerformanceFrequency(&frequency);
            QueryPerformanceCounter(&startCounter);
            lastCounter = startCounter;
            deltaTime = 0.0f;
            time = 0.0f;
            return;
        }

        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);

        deltaTime =
            float(now.QuadPart - lastCounter.QuadPart) /
            float(frequency.QuadPart);

        // If game was paused or something caused a big delay
        // we don't want to have a huge deltaTime...
        // which can cause physics to break or animations to jump.
        if (deltaTime > 0.1f)
            deltaTime = 0.1f;

        time =
            float(now.QuadPart - startCounter.QuadPart) /
            float(frequency.QuadPart);

        lastCounter = now;
    }

} // namespace Time
