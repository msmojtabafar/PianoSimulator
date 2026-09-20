# 🎹 Piano Simulator

A desktop piano simulator written in **C++** using **SFML 2.6.2**.

The project provides a playable virtual piano with real piano samples, computer keyboard controls, mouse interaction, three piano pedals, training mode, recording, and playback.

---

## ✨ Features

* 🎹 Virtual piano keyboard
* 🔊 Real piano audio samples
* 🎼 14 white keys
* ♯ 10 black keys
* 🖱️ Mouse support
* ⌨️ Computer keyboard support
* 🎛️ Three piano pedals
* 🧑‍🎓 Training mode
* 🔴 Recording mode
* ▶️ Recording playback
* ⏹️ Stop playback
* 🎵 Automatic pitch adjustment for missing samples
* 🖥️ Desktop graphical interface using SFML

---

## 🎹 Piano Keyboard

The piano currently contains two octaves:

**C4 → B5**

### White Keys

| Piano Note | Keyboard |
| ---------- | -------- |
| C4         | A        |
| D4         | S        |
| E4         | D        |
| F4         | F        |
| G4         | G        |
| A4         | H        |
| B4         | J        |
| C5         | K        |
| D5         | L        |
| E5         | Z        |
| F5         | X        |
| G5         | C        |
| A5         | V        |
| B5         | B        |

### Black Keys

| Piano Note | Keyboard |
| ---------- | -------- |
| C#4        | W        |
| D#4        | E        |
| F#4        | T        |
| G#4        | Y        |
| A#4        | U        |
| C#5        | O        |
| D#5        | P        |
| D#5        | 2        |
| F#5        | 3        |
| G#5        | 4        |

> The piano can also be played directly using the mouse.

---

## 🎛️ Piano Pedals

Three virtual pedals are available:

| Pedal     | Keyboard    |
| --------- | ----------- |
| Soft      | Left Shift  |
| Sostenuto | Space       |
| Sustain   | Right Shift |

The pedals can be activated while playing notes and are also included in the recording system.

---

## 🧑‍🎓 Training Mode

Training mode provides a simple interactive piano lesson.

Press:

```text
F8
```

to enable or disable training mode.

The program provides a sequence of notes that the player must play correctly.

### Example Training Sequence

```text
C4 C4 G4 G4 A4 A4 G4
F4 F4 E4 E4 D4 D4 C4
```

Features:

* Highlights the target note
* Detects correct notes
* Detects incorrect notes
* Shows feedback
* Tracks training progress
* Displays completion when the lesson is finished

---

## 🔴 Recording

Press:

```text
F9
```

to start recording.

The simulator records:

* Note press
* Note release
* Sustain pedal
* Soft pedal
* Sostenuto pedal
* Timing of events

Press `F9` again to stop recording.

---

## ▶️ Playback

Press:

```text
F10
```

to play the recorded performance.

Press:

```text
F11
```

to stop playback.

The recording system preserves the timing between events so the performance can be replayed.

---

## 🔊 Audio System

The simulator uses real piano samples stored in the:

```text
sounds/
```

directory.

Currently available samples include notes such as:

```text
A0.wav
A1.wav
A2.wav
...
C1.wav
C2.wav
...
D#1.wav
...
F#1.wav
...
```

When an exact sample is not available, the simulator selects a nearby sample and adjusts its playback pitch to produce the requested note.

This allows the keyboard to cover notes even when a dedicated sample does not exist.

---

## 🛠️ Technologies

* **C++**
* **SFML 2.6.2**
* **CMake**
* **GNU GCC**
* **SFML Graphics**
* **SFML Audio**
* **WAV piano samples**

---

## 📁 Project Structure

```text
PianoSimulator/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── sounds/
│   ├── A0.wav
│   ├── A1.wav
│   ├── ...
│   └── F#7.wav
└── src/
    └── main.cpp
```

---

## ⚙️ Requirements

Before building the project, make sure the following are installed:

* C++ compiler
* CMake
* SFML 2.6.x
* Make

On Ubuntu/Debian-based systems:

```bash
sudo apt update
sudo apt install cmake build-essential libsfml-dev
```

---

## 🔨 Build

Clone the repository:

```bash
git clone git@github.com:msmojtabafar/PianoSimulator.git
```

Enter the project directory:

```bash
cd PianoSimulator
```

Create the build directory:

```bash
mkdir build
cd build
```

Configure the project:

```bash
cmake ..
```

Build:

```bash
make -j$(nproc)
```

---

## ▶️ Run

From the `build` directory:

```bash
./PianoSimulator
```

---

## 🎮 Controls

| Key               | Function             |
| ----------------- | -------------------- |
| A–B / mapped keys | Play piano notes     |
| Left Shift        | Soft pedal           |
| Space             | Sostenuto pedal      |
| Right Shift       | Sustain pedal        |
| F8                | Training mode        |
| F9                | Start/Stop recording |
| F10               | Playback recording   |
| F11               | Stop playback        |
| Mouse             | Play piano keys      |

---

## 🖥️ Platform

The project is currently developed and tested on:

```text
Ubuntu Linux
x86_64
GCC 15.x
CMake
SFML 2.6.2
```

---

## 🚀 Project Status

The core piano simulator is currently functional.

Implemented:

* [x] Piano UI
* [x] White keys
* [x] Black keys
* [x] Real piano audio
* [x] Keyboard control
* [x] Mouse control
* [x] Sustain pedal
* [x] Soft pedal
* [x] Sostenuto pedal
* [x] Training mode
* [x] Recording
* [x] Playback
* [x] Stop playback
* [x] Pitch adjustment for missing samples

---

## 📌 Future Ideas

Possible future improvements include:

* 🎵 More piano octaves
* 🎼 Support for more instruments
* 🎹 Better pedal behavior
* 🎶 Automatic note-sequence playback
* 🤖 Note suggestions for the player
* 💾 Save and load recordings
* 🎚️ Volume controls
* 🎧 Improved audio processing
* 🎨 More advanced piano UI
* 🎼 MIDI support
