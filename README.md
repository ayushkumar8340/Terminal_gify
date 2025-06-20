# 🖥️ Heavy Terminal

**Heavy Terminal** is a dynamic GTK-based terminal emulator with support for animated GIF overlays and intuitive GUI controls. Built using C++ and GTK3/VTE, it allows users to display an animated GIF inside or on top of a terminal window — ideal for personalization, streaming overlays, or just a bit of fun!

---

## ✨ Features

- 🧠 **Embedded Terminal**: Based on VTE for full bash/zsh shell support.
- 🎞️ **GIF Overlay**: Attach a looping GIF directly inside or atop the terminal window.
- 🖱️ **GUI Controls**: Configure the terminal theme, default shell, and overlay GIF via a GTK settings window.
- 🪟 **Resizable & Split Layouts**: Experimental support for terminal splitting using paned containers.
- 🔐 **Root-Privileged Copy**: Optionally authenticate to allow writing to restricted system directories.

---

## 📸 Preview

*(Add your own screenshot or GIF preview here)*

---

## 🛠️ Build Instructions

mkdir build 
cd build 
cmake ..
make -j4
sudo make install

### 🔧 Dependencies

- `gtk+-3.0`
- `vte-2.91`
- `cmake`
- `g++` (C++17)
- `libgif-dev` *(optional, if GIF decoding is handled manually)*

Install dependencies on Ubuntu:

```bash
sudo apt update
sudo apt install build-essential cmake libgtk-3-dev libvte-2.91-dev

## Running on ubuntu
search Heavy Terminal on searchbar


