#pragma once
#include <string>
#include <vector>

// Reads a WAV or MP3 file into a mono 16-bit float PCM vector
bool read_audio_file(const std::string& path, std::vector<float>& pcmf32, int& sample_rate);
