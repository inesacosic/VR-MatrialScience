#include "AIChatModel.h"
#include <string>

using namespace std;

class NLPModule{

    private:
        // fields
        AIChatModel ai_model;
        string text_response;
        string audio_file_path;

    public:
        // methods
        void speechTotext(string);
        string processText();
        void textToSpeech(string);
};