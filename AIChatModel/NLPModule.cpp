// ----- FIX WINDOWS HEADER PROBLEMS -----
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _WIN32_WINNT 0x0A00     // Windows 10 API-level support

#include <Windows.h>
#undef byte
// ----------------------------------------

// ----- STANDARD HEADERS -----
#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <thread>

// ----- VOSK -----
#include "vosk_api.h"

// ----- SAPI (TTS / speech recognition) -----
#include <sapi.h>
#include <sphelper.h>

// ----- ATL (for CComPtr) -----
#include <atlbase.h>

// ----- YOUR MODULE -----
#include "NLPModule.h"
#include "AIChatModel.h"


#pragma comment(lib, "sapi.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "winmm.lib") // for waveIn functions

#define SAMPLE_RATE 16000
#define CHUNK_SIZE 4096
#define _SILENCE_CXX17_DEPRECATION_WARNINGS
#define _SILENCE_DEPRECATION_WARNINGS


string NLPModule::speechToText(){

    // Initialize COM for TTS
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr)) {
        std::cerr << "Failed to initialize COM\n";
        return "";
    }

    // Initialize TTS
    ISpVoice* voice = nullptr;
    if (FAILED(CoCreateInstance(CLSID_SpVoice, nullptr, CLSCTX_ALL, IID_ISpVoice, (void**)&voice))) {
        std::cerr << "Failed to initialize TTS\n";
        CoUninitialize();
        return "";
    }

    // Choose a more natural voice (Zira if available)
    CComPtr<IEnumSpObjectTokens> enumVoices;
    SpEnumTokens(SPCAT_VOICES, nullptr, nullptr, &enumVoices);
    CComPtr<ISpObjectToken> token;
    ULONG fetched = 0;
    bool ziraFound = false;
    while (enumVoices && enumVoices->Next(1, &token, &fetched) == S_OK) {
        LPWSTR desc = nullptr;
        if (SUCCEEDED(SpGetDescription(token, &desc))) {
            std::wstring ws(desc);
            CoTaskMemFree(desc);
            if (ws.find(L"Zira") != std::wstring::npos) {
                voice->SetVoice(token);
                ziraFound = true;
                break;
            }
        }
    }

    // Set default voice if Zira not found
    if (!ziraFound) {
        voice->SetRate(0);
        voice->SetVolume(100);
    }

    // Load Vosk model
    vosk_set_log_level(0);
    VoskModel* model = vosk_model_new("C:\\Users\\Inesa Cosic\\OneDrive - The Pennsylvania State University\\VR-AI-Project\\VR-MatrialScience-1\\build\\Debug\\models\\vosk-model-small-en-us-0.15");
    if (!model) {
        std::cerr << "ERROR: Failed to load model!\n";
        voice->Release();
        CoUninitialize();
        return "";
    }

    VoskRecognizer* recognizer = vosk_recognizer_new(model, SAMPLE_RATE);

    // Open microphone
    HWAVEIN mic;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, SAMPLE_RATE, SAMPLE_RATE * 2, 2, 16, 0 };
    if (waveInOpen(&mic, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
        std::cerr << "Failed to open microphone\n";
        vosk_recognizer_free(recognizer);
        vosk_model_free(model);
        voice->Release();
        CoUninitialize();
        return "";
    }

    char buffer[CHUNK_SIZE];
    WAVEHDR header = { 0 };
    header.lpData = buffer;
    header.dwBufferLength = CHUNK_SIZE;

    std::vector<char> recordingBuffer;
    bool isRecording = false;
    std::string spoken = "";

    std::cout << "Press Enter to start/stop recording. Say 'exit' to quit.\n";

    while (true) {
        // Toggle recording on Enter
        if (_kbhit()) {
            int ch = _getch();
            if (ch == 13) { // Enter key
                isRecording = !isRecording;

                if (isRecording) {
                    std::cout << "[Recording... Press Enter to stop]" << std::endl;
                    recordingBuffer.clear();

                    waveInPrepareHeader(mic, &header, sizeof(WAVEHDR));
                    waveInAddBuffer(mic, &header, sizeof(WAVEHDR));
                    waveInStart(mic);
                }
                else {
                    waveInStop(mic);

                    // Feed full recording to Vosk
                    vosk_recognizer_accept_waveform(recognizer, recordingBuffer.data(),
                        static_cast<int>(recordingBuffer.size()));
                    const char* result = vosk_recognizer_result(recognizer);

                    std::string resultStr(result);
                    size_t pos = resultStr.find("\"text\" : \"");
                    std::string spoken;
                    if (pos != std::string::npos) {
                        pos += 10;
                        size_t end = resultStr.find("\"", pos);
                        spoken = resultStr.substr(pos, end - pos);
                    }

                    if (!spoken.empty()) {
                        std::cout << "You said: " << spoken << std::endl;
                        return spoken;

                        // Convert to wide string and speak asynchronously
                        std::wstring ws(spoken.begin(), spoken.end());
                        voice->SetRate(0);      // normal speed
                        voice->SetVolume(100);  // full volume
                        voice->Speak(ws.c_str(), SPF_ASYNC | SPF_PURGEBEFORESPEAK, NULL);

                        if (spoken == "exit") break;
                    }

                    std::cout << "[Recording stopped]" << std::endl;
                }
            }
        }

        // During recording, keep appending mic chunks
        if (isRecording && (header.dwFlags & WHDR_DONE)) {
            recordingBuffer.insert(recordingBuffer.end(), buffer, buffer + header.dwBytesRecorded);

            header.dwFlags = 0;
            waveInPrepareHeader(mic, &header, sizeof(WAVEHDR));
            waveInAddBuffer(mic, &header, sizeof(WAVEHDR));
        }

        Sleep(10);
    }

    // Cleanup
    waveInClose(mic);
    vosk_recognizer_free(recognizer);
    vosk_model_free(model);
    voice->Release();
    CoUninitialize();

    return spoken;
}



int main() {
    cout << "Starting NLP Module Test..." << endl;
    NLPModule nlp;
    std::string result = nlp.speechToText();
    std::cout << result << std::endl;

    AIChatModel ai_model("C:\\Users\\Inesa Cosic\\OneDrive - The Pennsylvania State University\\VR-AI-Project\\VR-MatrialScience-1\\AIChatModel\\chat_template.json", {
                {"material", "steel"},
                {"parameter", "200 MPa"}
            });

    std::string response = ai_model.generateResponse(result);
    std::cout << "AI Response: " << response << std::endl;
    return 0;
}