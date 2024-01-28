<p align="center">
  <img src="https://github.com/CITM-UPC/EngineOfPower/blob/main/.github/images/TheOneEngine.png" />
  <br/>
  <br/>
  <br/>
  <a href="https://github.com/CITM-UPC/EngineOfPower/releases">Releases</a> |
  <a href="#screenshots">Screenshots</a> |
  <a href="#features">Features</a> |
  <a href="https://github.com/CITM-UPC/EngineOfPower/wiki">Wiki</a> |
  <a href="https://github.com/CITM-UPC/EngineOfPower/blob/main/CONTRIBUTING.md">Contributing</a>
  <br/>
  <br/>
  <br/>
  <a href="https://github.com/CITM-UPC/EngineOfPower/releases"><img alt="platforms" src="https://img.shields.io/badge/platforms-Windows-blue?style=flat-square"/></a>
  <a href="https://github.com/CITM-UPC/EngineOfPower/releases"><img alt="release" src="https://img.shields.io/github/v/release/CITM-UPC/TheOneEngine?style=flat-square"/></a>
  <br/>
  <a href="https://github.com/CITM-UPC/EngineOfPower/issues"><img alt="issues" src="https://img.shields.io/github/issues-raw/CITM-UPC/TheOneEnginesvg?color=yellow&style=flat-square"/></a>
  <a href="https://github.com/CITM-UPC/EngineOfPower/pulls"><img alt="pulls" src="https://img.shields.io/github/issues-pr-raw/CITM-UPC/TheOneEngine?color=yellow&style=flat-square"/></a>
  <br/>
  <a href="https://github.com/CITM-UPC/EngineOfPower/blob/main/LICENSE.md"><img alt="license" src="https://img.shields.io/github/license/CITM-UPC/TheOneEngine?color=green&style=flat-square"/></a>
  <a href="https://github.com/CITM-UPC/EngineOfPower/releases"><img alt="downloads" src="https://img.shields.io/github/downloads/CITM-UPC/TheOneEngine/total?color=green&style=flat-square"></a>
</p>

# 1. What Engine of Power
## 1.1. Description
Engine of Power is a game engine forked from [TheOne Engine](https://github.com/CITM-UPC/TheOneEngine) on its `Release_0.2` tag, crafted with passion by [Héctor Báscones](https://github.com/Hekbas), [Arnau Jiménez](https://github.com/Historn) and [Júlia Serra](https://github.com/softdrawss).

All modifications to the source code are by [Sergi Parra](https://github.com/t3m1X)

## 1.2. Modules
- `TheOneEngine`: Uses EngineCore, asset dependent, for any game built with TheOne
- `TheOneEditor`: Uses EngineCore, expose game development to the end-user

![Editor](https://github.com/CITM-UPC/EngineOfPower/blob/main/.github/images/Screenshot%202023-12-19%20222147.png)

# 2. History of EOP
Engine of Power is a project for the subject of Game Engines in our 3rd year at CITM (UPC).
It is a fork based on the second delivery of [TheOne Engine](https://github.com/CITM-UPC/TheOneEngine), created by the team ([Héctor Báscones](https://github.com/Hekbas), [Arnau Jiménez](https://github.com/Historn), [Júlia Serra](https://github.com/softdrawss) from scratch, because of their quality of code.

<span name="features"></span>
# 3. Features
## 3.1. Implemented
Here is a non-exhaustive list of EOP's main features:
- Model Import + Drag & Drop
- Texture Import
- GameObject hierarchy + Component Pattern
- Unity-like camera controls
- Editor tools (main menu and windows)
- Transform and Game Object hierarchy
- Custom file format
- Scene serialization
- Play / Stop
- Mouse picking
- Frustum culling
- Resource Management

## 3.2. To implement
Here is a list of EOP's coming features
- Scripting System
- Scripting Editor
  
# 4. Details
## 4.1. Software
- Visual Studio 2019 (v142)

## 4.2. Dependencies
- OpenGL + GLEW + GLM (Graphics API)
- SDL2 (OpenGL Binding + Input)
- Assimp (3D model loader)
- DeVil (Image library)
- rapidjson (JSON parser/generator)
- imGui (GUI)
- imPlot (2D plotting for ImGui)

## 4.3. Requirements
TheOne requires:
- RAM: 256MB
- OS: Windows 7
- GPU: Graphics card supporting OpenGL 3
- CPU: x64

## 4.4. Licence
TheOne is licenced under an MIT licence.
Engine of Power is licensed under an MIT license.

# 5. Contributing
Engine of Power is opened to any contribution. Do not hesitate to open issues (Features or bugs) or create pull requests.

If you want to help us, visit our [CONTRIBUTING.md](https://github.com/CITM-UPC/EngineOfPower/blob/main/CONTRIBUTING.md) guidelines document.
