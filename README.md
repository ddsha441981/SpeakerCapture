
# 🎵 SpeakerCapture

A lightweight, high-performance C++ application for capturing speaker output on Linux systems using PulseAudio. Perfect for recording system audio, creating audio samples, or monitoring audio streams in real-time.

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![Platform](https://img.shields.io/badge/platform-Linux-green.svg)
![PulseAudio](https://img.shields.io/badge/audio-PulseAudio-orange.svg)

## ✨ Features

- **🎯 Real-time Audio Capture**: Direct speaker output recording with minimal latency
- **📁 WAV Format Output**: High-quality uncompressed PCM audio files
- **⚙️ Configurable Parameters**: Adjustable sample rate, channels, buffer size, and audio device
- **📊 Latency Monitoring**: Real-time latency reporting during recording
- **🔄 Optional Playback**: Monitor captured audio in real-time
- **🛡️ Signal Handling**: Graceful shutdown with Ctrl+C
- **🚀 High Performance**: Optimized C++17 implementation with minimal overhead

## 🎛️ Audio Specifications

| Parameter | Range | Default | Description |
|-----------|--------|---------|-------------|
| **Sample Rate** | 8000-192000 Hz | 16000 Hz | Audio sampling frequency |
| **Channels** | 1-8 | 1 (Mono) | Number of audio channels |
| **Bit Depth** | 16-bit | 16-bit | PCM sample resolution |
| **Buffer Size** | 1-1000 ms | 100 ms | Internal buffer duration |

## 📋 System Requirements

### Dependencies
- **Linux** with PulseAudio server
- **PulseAudio Libraries**: `libpulse-dev`, `libpulse0`
- **Build Tools**: `gcc/g++`, `cmake`, `make`
- **C++17 Compiler** (GCC 7+ or Clang 5+)

### Supported Distributions
- Ubuntu 18.04+
- Debian 9+
- Fedora 30+
- Arch Linux
- openSUSE Leap 15+

## 🚀 Quick Start

### Pre-built Binary
```bash
wget https://github.com/yourusername/SpeakerCapture/releases/latest/download/speakercapture-linux-x86_64.tar.gz
tar -xzf speakercapture-linux-x86_64.tar.gz
./SpeakerCapture
```

### Build from Source
```bash
git clone --recursive https://github.com/yourusername/SpeakerCapture.git
cd SpeakerCapture
sudo apt update
sudo apt install build-essential cmake libpulse-dev pulseaudio-utils
mkdir build && cd build
cmake ..
make -j$(nproc)
./SpeakerCapture
```

## 📖 Usage Guide

### Basic Usage
```bash
./SpeakerCapture
timeout 30s ./SpeakerCapture
```

### Advanced Configuration
```bash
./SpeakerCapture [device] [sample_rate] [channels] [buffer_ms] [playback]
./SpeakerCapture default 44100 2 50 0
./SpeakerCapture default 16000 1 20 1
./SpeakerCapture alsa_output.pci-0000_00_1f.3.analog-stereo.monitor 48000 2 100 0
```

### Parameter Details

| Parameter | Description | Example Values |
|-----------|-------------|----------------|
| **device** | PulseAudio device name | `default`, `alsa_output.*.monitor` |
| **sample_rate** | Audio sample rate in Hz | `16000`, `44100`, `48000` |
| **channels** | Number of audio channels | `1`, `2` |
| **buffer_ms** | Buffer size in milliseconds | `20`, `100` |
| **playback** | Enable real-time playback | `0`, `1` |

## 🔧 Audio Device Discovery

```bash
pactl list short sources
pactl list short sources | grep monitor
pactl list sources
```

### Common Device Names
```bash
alsa_output.pci-0000_00_1f.3.analog-stereo.monitor
alsa_output.usb-*.analog-stereo.monitor
alsa_output.pci-0000_01_00.1.hdmi-stereo.monitor
```

## 🏗️ Build Options

### Manual Compilation
```bash
g++ main.cpp SimplePA/SimplePA.cpp     -I./SimplePA     -lpulse-simple -lpulse -pthread     -std=c++17 -O3     -o SpeakerCapture
```

### CMake Configuration
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
```

## 📊 Performance Tuning

```bash
./SpeakerCapture default 44100 2 10 0
./SpeakerCapture default 96000 2 100 0
./SpeakerCapture default 16000 1 200 0
```

## 🐛 Troubleshooting

**❌ "Failed to open recording stream"**
```bash
systemctl --user status pulseaudio
systemctl --user restart pulseaudio
pactl list short sources
```

**❌ "Permission denied"**
```bash
sudo usermod -a -G audio $USER
ls -la /tmp/pulse-*
```

**❌ "No such device"**
```bash
pactl list short sources | grep monitor
./SpeakerCapture default 16000 1 100 0
```

**❌ High latency**
```bash
./SpeakerCapture default 16000 1 200 0
pactl list short sources
cat /proc/asound/cards
```

### Debug Mode
```bash
PULSE_LOG=4 ./SpeakerCapture
pactl list sources | grep -A 10 "Name.*monitor"
```

## 📁 Output Files

- **Format**: WAV (PCM 16-bit, little-endian)
- **Default File**: `./speaker_output.wav`

Size (MB) ≈ (Sample Rate × Channels × 2 × Duration) / 1,048,576

Example: 44.1kHz stereo, 60s ≈ 10.1 MB

## 🔬 Technical Details

### Architecture
```
PulseAudio → SimplePA Wrapper → SpeakerApp Logic → WAV Output
```

### Dependencies
- **SimplePA** (submodule)
- **libpulse**
- **pthread**

## 🤝 Contributing

```bash
git clone --recursive https://github.com/yourusername/SpeakerCapture.git
cd SpeakerCapture
sudo apt install clang-format cppcheck valgrind
make test
clang-format -i *.cpp *.hpp
```

## 📄 License

MIT License - see [LICENSE](LICENSE)

## 🙏 Acknowledgments

- **SimplePA**: PulseAudio wrapper
- **PulseAudio Project**
- **STB Libraries**

## 📞 Support

- [GitHub Issues](https://github.com/yourusername/SpeakerCapture/issues)
- [Discussions](https://github.com/yourusername/SpeakerCapture/discussions)
- [Wiki](https://github.com/yourusername/SpeakerCapture/wiki)

---

**⭐ Star this project if you find it useful!**

Made with ❤️ for the Linux audio community
