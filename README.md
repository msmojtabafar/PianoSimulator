# 🎹 Piano Simulator

A real-time piano simulator developed in **C++ for Linux**, designed to provide responsive and low-latency piano playback using a computer keyboard.

## ✨ Features

* 🎹 Real-time piano simulation
* ⌨️ Computer keyboard control
* 🔊 Real-time audio playback
* ⚡ Low-latency audio
* 🎵 Piano note playback
* 🖥️ Linux support
* 🔧 CMake-based build system
* 🔊 PipeWire audio support

## 🛠️ Requirements

The project is currently designed for Linux.

### Required

* Linux
* C++ compiler with C++17 support
* CMake
* Make
* PipeWire
* PipeWire development libraries

On Ubuntu/Debian:

```bash
sudo apt update
sudo apt install build-essential cmake pkg-config pipewire pipewire-audio
```

Make sure PipeWire is running:

```bash
systemctl --user status pipewire
```

You can also check the audio system with:

```bash
wpctl status
```

## 📥 Clone the Repository

Clone the project using SSH:

```bash
git clone git@github.com:msmojtabafar/PianoSimulator.git
cd PianoSimulator
```

## 🔨 Build

Create a build directory:

```bash
mkdir -p build
cd build
```

Configure the project:

```bash
cmake ..
```

Build the project:

```bash
make -j$(nproc)
```

After a successful build, the executable will be generated in the build directory.

## ▶️ Run

From the `build` directory:

```bash
./PianoSimulator
```

If the executable has a different name, check the generated files:

```bash
ls -lh
```

Then run the generated executable.

## 🎹 Controls

The computer keyboard is used to play the piano.

The keyboard mapping is defined by the application and can be expanded or modified as development continues.

## 🔊 Audio Configuration

The application uses the Linux audio subsystem and is designed to work with **PipeWire**.

Check the current audio configuration:

```bash
wpctl status
```

For low-latency audio, the PipeWire quantum can be configured when necessary:

```bash
pw-metadata -n settings 0 clock.force-quantum 1024
```

The exact value may depend on the system's audio hardware and configuration.

## 📁 Project Structure

```text
PianoSimulator/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── src/
│   └── main.cpp
└── piano-source/
    └── ...
```

## 🗺️ Roadmap

* [x] Basic piano sound playback
* [x] Keyboard input
* [x] Real-time audio playback
* [x] Linux audio support
* [x] Basic piano simulator
* [ ] Improve graphical interface
* [ ] Add visual piano keyboard
* [ ] Add volume control
* [ ] Add octave control
* [ ] Add sustain
* [ ] Add recording and playback
* [ ] Add MIDI support
* [ ] Further reduce audio latency
* [ ] Add configuration panel

## 🤝 Contributing

Contributions, bug reports, ideas, and improvements are welcome.

To contribute:

```bash
git clone git@github.com:msmojtabafar/PianoSimulator.git
cd PianoSimulator
```

Create a new branch:

```bash
git checkout -b feature/my-feature
```

Make your changes, commit them, and push the branch:

```bash
git add .
git commit -m "Add my feature"
git push -u origin feature/my-feature
```

Then open a Pull Request on GitHub.

