# 🎹 Piano Simulator

A desktop piano simulator built with **C++ and SFML** that lets you play piano notes using your computer keyboard.

The project is designed to provide a simple and realistic piano-playing experience with real piano samples, visual keyboard feedback, and pedal simulation.

---

## ✨ Features

* 🎹 Full virtual piano keyboard
* 🔊 Real piano sound samples
* 🎵 Support for different piano notes and octaves
* ⌨️ Play piano using the computer keyboard
* 🎨 Visual feedback when keys are pressed
* 🦶 Three virtual piano pedals
* 🎛️ Keyboard controls for pedals
* 🔄 Pitch-based fallback for missing piano samples
* ⚡ Low-latency audio playback using SFML
* 🖥️ Native Linux desktop application

---

## 🎹 Keyboard Controls

### Piano Keys

The computer keyboard is mapped to the virtual piano keys.

Pressing a mapped key will:

1. Highlight the corresponding piano key.
2. Play the appropriate piano sound.
3. Release the sound when the key is released.

### 🦶 Pedals

| Keyboard Key  | Pedal        |
| ------------- | ------------ |
| `Left Shift`  | Left Pedal   |
| `Space`       | Middle Pedal |
| `Right Shift` | Right Pedal  |

The three pedals are displayed underneath the virtual piano.

---

## 🔊 Audio

The simulator uses real piano samples stored in the project's sound library.

When an exact sample is unavailable, the application can use a nearby available sample with pitch adjustment to reproduce the requested note.

Supported audio formats include:

* `.wav`
* `.flac`

---

## 🛠️ Technologies

* **C++**
* **SFML 2.6.x**

  * SFML Graphics
  * SFML Audio
* **CMake**
* **GNU C++ Compiler**
* **Linux**

---

## 📁 Project Structure

```text
PianoSimulator/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── src/
│   └── main.cpp
├── sounds/
│   └── *.wav
└── piano-source/
    └── Samples/
        └── *.flac
```

---

## 🚀 Build & Run

### 1. Clone the repository

```bash
git clone git@github.com:msmojtabafar/PianoSimulator.git
cd PianoSimulator
```

### 2. Create the build directory

```bash
mkdir -p build
cd build
```

### 3. Configure the project

```bash
cmake ..
```

### 4. Build

```bash
make -j$(nproc)
```

### 5. Run

```bash
./PianoSimulator
```

---

## 📦 Dependencies

Make sure the following are installed:

```bash
sudo apt update
sudo apt install build-essential cmake libsfml-dev
```

Then verify SFML is available:

```bash
pkg-config --modversion sfml-graphics
```

---

## 🔧 Audio Configuration

On some Linux systems using **PipeWire/PulseAudio**, audio playback may have crackling, distortion, or other sound-quality issues.

If you experience audio problems, try setting the PipeWire quantum value before running the piano:

```bash
pw-metadata -n settings 0 clock.force-quantum 1024
```

If `1024` does not provide good results, you can also try:

```bash
pw-metadata -n settings 0 clock.force-quantum 2048
```

or:

```bash
pw-metadata -n settings 0 clock.force-quantum 4096
```

After changing the value, run the application again:

```bash
./PianoSimulator
```

### Recommended order

Try the values in this order:

```text
1024 → 2048 → 4096
```

Use the value that provides the cleanest and most stable audio output on your system.

> **Note:** This setting affects the system's PipeWire audio configuration and may not be required on every Linux system.

---

## 🎼 How It Works

The application is built around three main components:

### Piano Rendering

SFML is used to draw the piano keyboard and its visual elements.

### Keyboard Input

Keyboard events are converted into piano-note actions. Pressing and releasing a keyboard key changes the visual state of the corresponding piano key.

### Audio Playback

Piano samples are loaded and played using SFML Audio. When an exact note sample is not available, the simulator can select a nearby sample and modify its pitch.

---

## 🎯 Project Goal

The goal of this project is to build a functional desktop piano simulator from scratch using C++ while learning and implementing:

* Audio processing
* Keyboard input handling
* 2D graphics
* Event-driven programming
* CMake-based project configuration
* Desktop application development

---

## 📌 Current Status

**Project:** Functional ✅

The current version includes:

* Virtual piano keyboard
* Real piano sounds
* Keyboard controls
* Visual key interaction
* Three virtual pedals
* Audio fallback with pitch adjustment
* Linux build system using CMake