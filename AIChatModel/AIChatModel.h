#include <string>
#include <vector>
#include "ollama.hpp"

using namespace std;

class AIChatModel{
    private:
        string model_name; // ollama model we are using is llama3.2:3b
        string embedding_model_name; // embedding model we are using for RAG

        vector<pair<string, vector<float>>> RAG_DATABASE; // where the database will be stored as a pair of (chunk, embedding vector)
        ollama::messages chat_history; // initializes a vector where we will push all messages to build context

    public:
        AIChatModel(string);
        string generateResponse(string);
        void fetchMaterialScienceContent(string);
        void printChatHistory();

};