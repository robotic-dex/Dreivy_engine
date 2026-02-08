# Dreivy Engine

A small experimental game engine written in C++.

---

## Contents
- [What is this?](#what-is-this)
- [Design goals](#design-goals)
- [Architecture overview](#architecture-overview)
- [Project status](#project-status)
- [Who is this for?](#who-is-this-for)
- [Build & Run](#build--run)

---

## What is this?

Dreivy Engine is a small experimental game engine written in C++.

I started this project with a simple goal:  
to build a real game engine that is **easy to read, understand, and modify**
for developers with **junior or early middle C++ experience**.

There is no hidden magic here and no heavy abstractions.
Everything is explicit: data flow, ownership, and responsibilities.
The engine is under active development and constantly improving.

--- 

Some internal APIs may change as the codebase evolves.
The focus is on clarity, learning, and experimentation rather than
strict backward compatibility.


---

## Design goals

- Beginner-friendly codebase
- Clear and explicit data flow
- Explicit ECS → RenderQueue → Renderer pipeline
- Renderer is a *dumb executor*, not a decision maker
- No external dependencies (clone & build with Visual Studio)
- Clear ownership and lifetime (RAII)
- Minimal but real DirectX 11 rendering

---

## Architecture overview

The engine is split into clearly separated layers:

ECS
→ RenderQueueBuilder
→ RenderQueue
→ Renderer


- **ECS** knows nothing about rendering
- **RenderQueueBuilder** adapts ECS data for rendering
- **RenderQueue** is a pure data contract
- **Renderer** only consumes prepared render data

Each layer has a single responsibility and does not depend on higher-level systems.

---

## Project status

This is a **learning and experimental engine**.

The focus is on:
- clarity
- architecture
- correctness

Not on feature completeness or production readiness.

APIs may change.

---

## Who is this for?

- Developers learning how game engines work internally
- Junior C++ programmers who want a real, readable codebase
- Engine developers who appreciate simple, explicit architecture

If you want a huge feature set — this is not the project.  
If you want to understand how things work — it probably is.

---

## Build & Run

1. Clone the repository
2. Open the `.sln` file in Visual Studio (Windows only)
3. Build and run

No additional libraries or setup required.