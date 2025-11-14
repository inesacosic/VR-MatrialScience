#include "audio_utils.h"
#include <sndfile.h>   // for WAV/FLAC/OGG
#include <mpg123.h>    // for MP3 decoding
#include <iostream>

bool read_audio_file(const std::string& path, std::vector<float>& pcmf32, int& sample_rate) {
    pcmf32.clear();
    sample_rate = 0;

    // Check extension
    std::string ext = path.substr(path.find_last_of('.') + 1);
    for (auto& c : ext) c = tolower(c);

    if (ext == "wav" || ext == "flac" || ext == "ogg") {
        SF_INFO sfinfo;
        SNDFILE* sndfile = sf_open(path.c_str(), SFM_READ, &sfinfo);
        if (!sndfile) {
            std::cerr << "Failed to open audio file: " << path << "\n";
            return false;
        }
        std::vector<float> buf(sfinfo.frames * sfinfo.channels);
        sf_readf_float(sndfile, buf.data(), sfinfo.frames);
        sf_close(sndfile);

        // Convert to mono if needed
        if (sfinfo.channels > 1) {
            pcmf32.resize(sfinfo.frames);
            for (int i = 0; i < sfinfo.frames; ++i) {
                float sum = 0.0f;
                for (int c = 0; c < sfinfo.channels; ++c)
                    sum += buf[i * sfinfo.channels + c];
                pcmf32[i] = sum / sfinfo.channels;
            }
        } else {
            pcmf32 = std::move(buf);
        }
        sample_rate = sfinfo.samplerate;
        return true;
    }

    if (ext == "mp3") {
        mpg123_init();
        mpg123_handle* mh = mpg123_new(nullptr, nullptr);
        if (mpg123_open(mh, path.c_str()) != MPG123_OK) {
            std::cerr << "Failed to open MP3 file: " << path << "\n";
            mpg123_delete(mh);
            mpg123_exit();
            return false;
        }

        long rate;
        int channels, encoding;
        mpg123_getformat(mh, &rate, &channels, &encoding);
        sample_rate = static_cast<int>(rate);

        std::vector<unsigned char> buffer(8192);
        size_t bytes_read;
        std::vector<float> samples;

        do {
            int err = mpg123_read(mh, buffer.data(), buffer.size(), &bytes_read);
            if (bytes_read > 0) {
                size_t num_samples = bytes_read / sizeof(short);
                const short* samples_i16 = reinterpret_cast<short*>(buffer.data());
                for (size_t i = 0; i < num_samples; i++)
                    samples.push_back(samples_i16[i] / 32768.0f);
            }
            if (err == MPG123_DONE) break;
        } while (bytes_read > 0);

        // Convert to mono
        if (channels > 1) {
            pcmf32.resize(samples.size() / channels);
            for (size_t i = 0; i < pcmf32.size(); ++i) {
                float sum = 0.0f;
                for (int c = 0; c < channels; ++c)
                    sum += samples[i * channels + c];
                pcmf32[i] = sum / channels;
            }
        } else {
            pcmf32 = std::move(samples);
        }

        mpg123_close(mh);
        mpg123_delete(mh);
        mpg123_exit();
        return true;
    }

    std::cerr << "Unsupported file type: " << path << "\n";
    return false;
}
