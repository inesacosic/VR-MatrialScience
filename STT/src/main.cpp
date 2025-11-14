#include <iostream>
#include <vector>
#include <string>

#include "audio_utils.h"
#include "whisper.h"

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: stt <model.gguf> <audiofile.wav/mp3/ogg/flac>\n";
        return 1;
    }

    std::string model_path = argv[1];
    std::string audio_path = argv[2];

    // -----------------------------
    // Load audio
    // -----------------------------
    std::vector<float> pcmf32;
    int sample_rate = 0;

    std::cout << "Loading audio file: " << audio_path << "\n";

    if (!read_audio_file(audio_path, pcmf32, sample_rate)) {
        std::cerr << "Error: Failed to read audio.\n";
        return 1;
    }

    std::cout << "Loaded audio. Samples: " << pcmf32.size()
              << "  Sample rate: " << sample_rate << "\n";

    // -----------------------------
    // Load Whisper model
    // -----------------------------
    whisper_context *ctx = whisper_init_from_file(model_path.c_str());
    if (!ctx) {
        std::cerr << "Error: could not load whisper model: " << model_path << "\n";
        return 1;
    }

    whisper_full_params params = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
    params.print_realtime   = false;
    params.print_progress   = false;
    params.print_timestamps = true;

    std::cout << "Transcribing...\n";

    // -----------------------------
    // Run Whisper
    // -----------------------------
    if (whisper_full(ctx, params, pcmf32.data(), pcmf32.size()) != 0) {
        std::cerr << "Error: Whisper failed.\n";
        whisper_free(ctx);
        return 1;
    }

    // -----------------------------
    // Print result
    // -----------------------------
    std::cout << "\n--- TRANSCRIPTION ---\n";

    int segments = whisper_full_n_segments(ctx);
    for (int i = 0; i < segments; i++) {
        const char* text = whisper_full_get_segment_text(ctx, i);
        std::cout << text;
    }

    std::cout << "\n----------------------\n";

    whisper_free(ctx);
    return 0;
}
