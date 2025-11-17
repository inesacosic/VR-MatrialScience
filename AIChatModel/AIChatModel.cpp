#include "ollama.hpp"
#include "RAG.hpp"
#include "json.hpp"
#include "AIChatModel.h"

#include <iostream>
#include <string.h>
#include <vector>
#include <fstream>
#include <filesystem>

using namespace std;
using namespace ollama::RAG;
using json = nlohmann::json;


AIChatModel::AIChatModel(string file_name){

    fstream f(file_name);
    if (!f.is_open()) {
    cerr << "Error: Could not open " << file_name << endl;
    exit(1);
    }
    json data = json::parse(f);

    this -> model_name = data["model"];
    this -> embedding_model_name = data["embed_model"];

    // for every initial message in the json file, add it to the chat history
    for (const auto& message : data["messages"]) {
        string role = message["role"];
        string content = message["content"];
        ollama::message msg(role, content);
        this -> chat_history.push_back(msg);
    }
}

/*
# generateResponse(string input)
#
# This function adds the user's input into the AIChatModel's chat_history. It then generates a 
# response from the ollama model. Finally, it adds the model's response to the chat_history.
# Inputs and outputs are added to the AIChatModel's chat_history to build context or "memory".
#
# Parameters:
#   1. input -> a string value which is the input given by the user 
#
# Return Type:
#   string - > returns the response from the ollama model
#
*/
string AIChatModel::generateResponse(string input){
    
    // create a message from the user's input and add it to this -> chat_history
    ollama::message user_input("user", input);
    this -> chat_history.push_back(user_input);

    cout << "Connecting to ollama..." <<endl;

    // generate a response from the chat history and create a new message with the response
    string ollama_reply = ollama::chat(this->model_name, chat_history);
    ollama::message response("assistant", ollama_reply);

    // add the new message to this -> chat_history
    this -> chat_history.push_back(response);

    return ollama_reply;
}

void AIChatModel::fetchMaterialScienceContent(string input){

    RAG_loadDocument_ByLine(
        RAG_DATABASE, 
        embedding_model_name, 
        "C:\\Users\\Inesa Cosic\\OneDrive - The Pennsylvania State University\\VR-AI-Project\\VR-MatrialScience-1\\context.txt"
    );

    cout << "Loaded " << RAG_DATABASE.size() << " entries." << endl;

    auto retrieved_knowledge = RAG_retrieve(RAG_DATABASE, embedding_model_name, input, 1); // 1 = fetchCount

    string instruction_prompt = "You are a helpful chat bot that gives knowledge about material science.\nKnowledge:\n";

    for(const auto& [chunk, _] : retrieved_knowledge){
        instruction_prompt += "-" + chunk + "\n";
    }

    cout << instruction_prompt << endl;

    ollama:: message system_message("system", instruction_prompt);
    chat_history.push_back(system_message);
}




/* printChatHistory()
#
# This function prints the AIChatModel's chat_history
#
# Return Type:
#   void
#
*/
void AIChatModel::printChatHistory(){

    cout << "-----------CHAT HISTORY--------------" << endl;
    for(const auto& msg: chat_history){
        // convert message to JSON
        cout << msg << endl;
    }

}


// int main(){

    
//     // always give absolute path to the json file
// 	AIChatModel ai = AIChatModel(
//         "C:\\Users\\Inesa Cosic\\OneDrive - The Pennsylvania State University\\VR-AI-Project\\VR-MatrialScience-1\\AIChatModel\\chat_template.json"
//     );

//     string input;
//     getline (cin, input);

//     ai.fetchMaterialScienceContent(input);

//     string response = ai.generateResponse(input);

//     cout << "Model's response: " << response << endl;

// 	return 0;

// }




