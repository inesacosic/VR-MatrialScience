#pragma once
#include <string>
#include <vector>
#include "ollama.hpp"

using namespace std;

class AIChatModel{
    private:
        // fields
        string model_name; // ollama model we are using is llama3.2:3b
        string embedding_model_name; // embedding model we are using for RAG

        vector<string> content_files; // stores file paths for content documents
        vector<pair<string, vector<float>>> RAG_DATABASE; // where the database will be stored as a pair of (chunk, embedding vector) from the content documents
        ollama::messages chat_history; // initializes a "vector" where we will push all messages to build context

        // methods
        void embedContent();

    public:
        // methods
        AIChatModel(string, unordered_map<string, string>); // the unordered_map holds the values such as force amount and material type
        string generateResponse(string); 
        void printChatHistory();
        void fetchMaterialScienceContent(string);
        string insertParams(string, unordered_map<string, string>);

        // getters
        string getModelName();
        string getEmbeddingModelName();
        vector<string> getContentFiles();
        ollama::messages getChatHistory();
        

};