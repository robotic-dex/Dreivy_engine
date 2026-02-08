# Math

This folder contains small math-related utilities used by the engine.
They are simple, explicit, and designed to be easy to understand and replace.

## Contents

- [Time](#time)
- [TransformUtils](#transformutils)

---

## Time

Provides frame-rate independent timing values.

- `Time::deltaTime` — time between frames (seconds)
- `Time::time` — time since application start (seconds)

Use `deltaTime` for simulation (movement, physics, animation).
Use `time` for mathematical expressions (sine waves, oscillations).

See: `Time.h`

---

## TransformUtils

Helpers for building transformation matrices from ECS components.

Currently provides:
- `BuildWorldMatrix(const Transform&)`

This converts position, rotation, and scale into a single world matrix
using a standard TRS order:

```cpp
World = Scale * Rotation * Translation
Used by rendering systems to convert ECS data into render-ready form.

See: TransformUtils.h