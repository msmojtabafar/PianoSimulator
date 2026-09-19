# 🎹 Piano Simulator

A simple and lightweight **Piano Simulator** built with **C++**, **SFML 2.6**, and WAV piano samples.

The application provides a virtual piano that can be played using the computer keyboard or mouse, with support for three virtual piano pedals.

---

## ✨ Features

* 🎹 Virtual piano keyboard
* 🎵 Real WAV piano samples
* 🎚️ Automatic sample selection and pitch adjustment
* ⌨️ Computer keyboard support
* 🖱️ Mouse support
* 🎨 Visual key press feedback
* 🦶 Three virtual piano pedals
* ⚡ Low-latency audio playback using SFML
* 🐧 Linux support

---

## 🎹 Piano Keyboard Mapping

The piano keys are mapped to the computer keyboard as follows:

| Keyboard | Piano Note |
| -------- | ---------- |
| A        | C4         |
| S        | D4         |
| D        | E4         |
| F        | F4         |
| G        | G4         |
| H        | A4         |
| J        | B4         |
| K        | C5         |
| L        | D5         |
| Z        | E5         |
| X        | F5         |
| C        | G5         |
| V        | A5         |
| B        | B5         |

---

## 🦶 Pedal Mapping

The simulator includes three virtual piano pedals:

| Keyboard Key | Pedal        |
| ------------ | ------------ |
| Left Shift   | Left Pedal   |
| Space        | Middle Pedal |
| Right Shift  | Right Pedal  |

The pedals are displayed below the piano and visually move when pressed.

> **Note:** The pedals are currently visual controls. Their physical behavior does not yet modify the sound of the piano.

---

## 🖱️ Mouse Support

Piano keys can also be played using the mouse.

* **Left mouse button** → Play piano key
* Keys change appearance while pressed.

---

## 🔊 Audio Samples

The simulator uses WAV samples located in:

```text
sounds/
```

The application currently loads samples such as:

```text
A0.wav
A1.wav
A2.wav
...
C4.wav
C5.wav
...
D#4.wav
...
F#5.wav
...
```

If the exact sample for a requested note is unavailable, the simulator automatically selects the nearest available sample and adjusts its pitch.

For example:

```text
E4 -> D#4  pitch=1.05946
```

This means the `D#4` sample is being used to reproduce `E4` with pitch correction.

---

## 🛠️ Requirements

Before building the project, make sure the following are installed:

* C++ compiler
* CMake
* SFML 2.6.x
* SFML Audio
* SFML Graphics

On Ubuntu/Debian:

```bash
sudo apt update
sudo apt install build-essential cmake libsfml-dev
```

---

## 📁 Project Structure

```text
PianoSimulator/
├── CMakeLists.txt
├── README.md
├── sounds/
│   ├── A0.wav
│   ├── A1.wav
│   ├── ...
│   └── F#7.wav
│
├── piano-source/
│   └── main.cpp
│
└── build/
    └── PianoSimulator
```

---

## 🔨 Build

Go to the project root:

```bash
cd ~/PianoSimulator
```

Create a clean build directory:

```bash
rm -rf build
mkdir build
cd build
```

Configure the project:

```bash
cmake ..
```

Compile:

```bash
make -j$(nproc)
```

If everything is successful, you should see:

```text
[100%] Built target PianoSimulator
```

---

## ▶️ Run

From the `build` directory:

```bash
./PianoSimulator
```

You should see:

```text
Loaded samples: 30
```

Then the piano simulator window will open.

---

## 🎧 Audio Configuration

The project uses SFML for audio playback.

On Linux systems using PipeWire, audio latency can optionally be adjusted using:

```bash
pw-metadata -n settings 0 clock.force-quantum 1024
```

For example, another available setting is:

```bash
pw-metadata -n settings 0 clock.force-quantum 4096
```

Lower values can reduce latency but may increase the chance of audio glitches depending on the system.

---

## 💻 Tested Environment

The project has been tested on:

* Linux
* Ubuntu
* C++17
* SFML 2.6.2
* CMake
* PipeWire

---

## 🚀 Future Improvements

Possible future improvements include:

* 🎵 Real sustain pedal behavior
* 🎵 Soft pedal behavior
* 🎵 Sostenuto pedal behavior
* 🎼 More piano octaves
* 🎧 Better sample selection
* 🔊 Improved audio latency
* 🎚️ Volume control
* 🎛️ Pedal effects on audio
* 🎹 More realistic piano graphics
* 🎼 MIDI keyboard support
* 🎤 Recording and playback
* 💾 Save and load performances
