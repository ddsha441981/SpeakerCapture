#include "SimplePA/SimplePA.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <atomic>
#include <csignal>
#include <chrono>
#include <ctime>
#include <cstring>

using namespace YukiWorkshop::SimplePA;

std::atomic<bool> g_running(true);

void signal_handler(int) {
    g_running = false;
}

void log(const std::string& msg) {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cerr << "[" << std::ctime(&now) << "] " << msg << std::flush;
}

// Simple WAV header writer
void write_wav_header(std::ofstream& file, uint32_t sampleRate, uint16_t bitsPerSample, uint16_t channels, uint32_t dataSize) {
    file.seekp(0);
    file.write("RIFF", 4);
    uint32_t chunkSize = 36 + dataSize;
    file.write(reinterpret_cast<const char*>(&chunkSize), 4);
    file.write("WAVE", 4);

    file.write("fmt ", 4);
    uint32_t subchunk1Size = 16;
    file.write(reinterpret_cast<const char*>(&subchunk1Size), 4);
    uint16_t audioFormat = 1; // PCM
    file.write(reinterpret_cast<const char*>(&audioFormat), 2);
    file.write(reinterpret_cast<const char*>(&channels), 2);
    file.write(reinterpret_cast<const char*>(&sampleRate), 4);
    uint32_t byteRate = sampleRate * channels * bitsPerSample / 8;
    file.write(reinterpret_cast<const char*>(&byteRate), 4);
    uint16_t blockAlign = channels * bitsPerSample / 8;
    file.write(reinterpret_cast<const char*>(&blockAlign), 2);
    file.write(reinterpret_cast<const char*>(&bitsPerSample), 2);

    file.write("data", 4);
    file.write(reinterpret_cast<const char*>(&dataSize), 4);
}

void latency_reporter(Recorder& recorder, std::atomic<bool>& running) {
    while (running.load()) {
        try {
            auto latency_usec = recorder.latency();
            std::cerr << "Latency: " << latency_usec / 1000 << " ms\n";
        } catch (...) {
            std::cerr << "Error getting latency\n";
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}


int main(int argc, char* argv[]) {
    std::signal(SIGINT, signal_handler);

    // Defaults
    std::string device = "alsa_output.pci-0000_00_1f.3.analog-stereo.monitor";
    uint32_t sampleRate = 16000;
    uint16_t channels = 1;
    uint16_t bitsPerSample = 16;
    size_t bufferSizeMs = 100; 
    bool enablePlayback = false;

    if (argc > 1) device = argv[1];
    if (argc > 2) {
        try {
            sampleRate = std::stoul(argv[2]);
        } catch (...) {
            std::cerr << "Invalid sample rate\n";
            return 1;
        }
    }
    if (argc > 3) {
        try {
            int ch = std::stoi(argv[3]);
            if (ch < 1 || ch > 8) {
                std::cerr << "Channels must be between 1 and 8\n";
                return 1;
            }
            channels = static_cast<uint16_t>(ch);
        } catch (...) {
            std::cerr << "Invalid channels\n";
            return 1;
        }
    }
    if (argc > 4) {
        try {
            bufferSizeMs = std::stoul(argv[4]);
        } catch (...) {
            std::cerr << "Invalid buffer size\n";
            return 1;
        }
    }
    if (argc > 5) enablePlayback = (std::string(argv[5]) == "1");

    log("Starting capture...\n");
    log("Device: " + device + "\n");
    log("Sample Rate: " + std::to_string(sampleRate) + "\n");
    log("Channels: " + std::to_string(channels) + "\n");
    log("Buffer Size (ms): " + std::to_string(bufferSizeMs) + "\n");
    log(std::string("Playback enabled: ") + (enablePlayback ? "yes" : "no") + "\n");

    Recorder recorder;
    recorder.set_device(device);
    recorder.set_sample_spec({
        PA_SAMPLE_S16LE,
        sampleRate,
        static_cast<uint8_t>(channels)
    });

    try {
        recorder.open();
    } catch (const std::exception& e) {
        std::cerr << "Failed to open recording stream: " << e.what() << "\n";
        return 1;
    }

    std::ofstream file("speaker_output.wav", std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open output file.\n";
        return 1;
    }

    file.write(std::string(44, '\0').c_str(), 44);

    size_t bytesPerSample = bitsPerSample / 8;
    size_t bufferSize = (sampleRate * channels * bytesPerSample * bufferSizeMs) / 1000;
    std::vector<uint8_t> buffer(bufferSize);

    Player player;
    if (enablePlayback) {
        player.set_device(device);
        player.set_sample_spec({
            PA_SAMPLE_S16LE,
            sampleRate,
            static_cast<uint8_t>(channels)
        });
        try {
            player.open();
        } catch (const std::exception& e) {
            std::cerr << "Failed to open playback stream: " << e.what() << "\n";
            enablePlayback = false;
        }
    }

    std::atomic<bool> latency_running(true);
    std::thread latency_thread(latency_reporter, std::ref(recorder), std::ref(latency_running));


    uint32_t totalDataBytes = 0;

    log("Recording... Press Ctrl+C to stop.\n");

    while (g_running.load()) {
        try {
            recorder.record(buffer.data(), buffer.size());
            file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
            file.flush(); 
            totalDataBytes += static_cast<uint32_t>(buffer.size());

            if (enablePlayback) {
                player.play(buffer.data(), buffer.size());
            }
        } catch (const std::exception& e) {
            std::cerr << "Error during recording: " << e.what() << "\n";
            break;
        }
    }

    log("Stopping recording...\n");

    latency_running = false;
    if (latency_thread.joinable())
        latency_thread.join();

    write_wav_header(file, sampleRate, bitsPerSample, channels, totalDataBytes);
    file.close();

    log("File saved as speaker_output.wav\n");

    return 0;
}

