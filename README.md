# EPiano —— 简易合成器

![image](data/pianoDarker.png)

## 目录
- [项目背景](#项目背景)
- [项目介绍](#项目介绍)
- [项目结构](#项目结构)
- [项目依赖](#项目依赖)
- [项目编译](#项目编译)

## 项目背景
- 在当今数字音频领域，电子音乐合成器是广泛应用的工具之一。本人为了学习音色合成的基本原理，
满足音乐爱好者和专业音乐人简单的音色需求，我设计并计划开发一个名为EPiano的轻量化简易合成器。

## 项目介绍
- EPiano 是一款简易的合成器，具有直观的用户界面，包含一个可视化的88键钢琴键盘和一个设置界面，
旨在让用户通过调整包络（ADSR模型）和谐波强度（目前支持13谐波）来制作简单的的音色，
了解音色合成的基础原理。此外，EPiano支持鼠标点击、电脑键盘和MIDI键盘的演奏，
为用户提供多样化的演奏方式。

- EPiano使用MIT协议开源，可以用于学习交流分享和商业用途。

## 项目结构

```
// 非自动生成的文件
├── EPiano
│   ├── main.cpp
|   |	// UI界面
│   ├── MainWin.cpp		// 主界面
│   ├── MainWin.h
│   ├── MainWin.ui
│   ├── keysettingbutton.h	// 设置按键映射的按钮
│   ├── MyWidgets.h		// 自定义控件行为
|   |
│   |	// 钢琴键盘
│   ├── key.cpp			// 定义钢琴行为＆连接事件
│   ├── key.h
│   ├── PianoGLWin.cpp		// OpenGL
│   ├── PianoGLWin.h
│   ├── shader
|   |   ├── vertex.glsl
│   │   └── fragment.glsl
|   |
│   |	// 合成器
│   ├── Envelope.h		// 包络
│   ├── Sinwave.h		// 正弦波
│   ├── NotePlayer.cpp		// 合成器
│   ├── NotePlayer.h
|   |
│   |	// IO
│   ├── midiDeviceHandler.cpp	// MIDI设备
│   ├── midiDeviceHandler.h
│   ├── loadConfig.h		// 配置文件
|   |
│   |	// 音乐要素
│   ├── music.h			// 音符，和弦
|   └── music.cpp
```
```
// 依赖包
├── EPiano
│   ├── json.hpp		// json解析
│   ├── Skt.cpp			// 音频IO
│   ├── Skt.h
│   ├── RtAudio.cpp
│   ├── RtAudio.h
│   ├── RtMidi.cpp		// MIDI
|   └── RtMidi.h
```
### 项目依赖
	- Qt6.6.1
	- Skt
	- RtAudio
	- RtMidi
	- json

### 项目编译
	本项目使用VS2022编译，使用Qt6.6.1开发.

### 发行包

	仅支持 Windows x64 平台，在Release下载即可.
