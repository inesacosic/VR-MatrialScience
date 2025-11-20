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


/*
# AIChatModel(string file_name)
#
# This is the constructor for the AIChatModel class. It initializes the model
# by parsing the provided JSON chat template file to initialize the model name, embedding model name,
# content files, and initial chat messages. It then calls the enbedContent() function
#
# Parameters:
#   1. file_name -> a string value which is the path to the JSON chat template file
#
# Return Type:
#   N/A - constructor
#
*/
AIChatModel::AIChatModel(string file_name){

    fstream f(file_name);

    if (!f.is_open()) {
    cerr << "Error: Could not open " << file_name << endl;
    exit(1);
    }

    json data = json::parse(f);

    this -> model_name = data["model"];
    this -> embedding_model_name = data["embed_model"];
    this -> content_files = data["content_files"];

    // for every initial message in the json file, add it to the chat history
    for (const auto& message : data["messages"]) {
        string role = message["role"];
        string content = message["content"];
        ollama::message msg(role, content);
        this -> chat_history.push_back(msg);
    }

    embedContent(); // call the embedContent function to load and embed the content files
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

    fetchMaterialScienceContent(input);
    
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


/*
# fetchMaterialScienceContent(string input)
#
# This function retrieves relevant information from the AIChatModels RAG_DATABASE
# which contains a vector embedding of the content_files. It uses the function RAG_retrieve()
# from the ollama::RAG namespace to get the most relevant chunks of information based on the 
# user's input. Afterwards, it appends the retrieved knowledge to th AIChatModel's chat_history
#
# Parameters:
#   1. input -> a string value which is the input given by the user 
#
# Return Type:
#   void
#
*/
void AIChatModel::fetchMaterialScienceContent(string input){

    auto retrieved_knowledge = RAG_retrieve(RAG_DATABASE, embedding_model_name, input, 2); // 2 = fetchCount (how many relevant chunks to fetch)

    string instruction_prompt = "You are a helpful chat bot that gives knowledge about material science.\nKnowledge:\n";

    // add each fetched chunk to the instruction_prompt
    for(const auto& [chunk, _] : retrieved_knowledge){
        instruction_prompt += "-" + chunk + "\n";
    }

    cout << instruction_prompt << endl;

    ollama:: message system_message("system", instruction_prompt);
    chat_history.push_back(system_message);
}


/*
# embedContent()
#
# This function loads and embeds the content files specified in the AIChatModel's content_files
# into the RAG_DATABASE using the RAG_loadDocument_ByLine() function from the ollama::RAG namespace.
# This function is called in the AIChatModel constructor to initialize the RAG_DATABASE upon creation.
#
# Parameters:
#   N/A
#
# Return Type:
#   void
#
*/
void AIChatModel::embedContent(){

    // for each content file path in this -> content_files, load the document into the RAG_DATABASE
    for (const auto& file_path : content_files){

        RAG_loadDocument_ByLine(
            RAG_DATABASE, 
            embedding_model_name, 
            file_path
        );
    }

    cout << "Loaded " << RAG_DATABASE.size() << " entries." << endl;
}


/* printChatHistory()
#
# This function prints the AIChatModel's chat_history
#
# Parameters:
#   N/A

# Return Type:
#   void
#
*/
void AIChatModel::printChatHistory(){

    cout << "-----------CHAT HISTORY--------------" << endl;
    for(const auto& msg: chat_history){
        // convert message to JSON
        cout << msg << endl << endl;
    }

}


int main(){

    
    // always give absolute path to the json file
	AIChatModel ai = AIChatModel(
        "C:\\Users\\Inesa Cosic\\OneDrive - The Pennsylvania State University\\VR-AI-Project\\VR-MatrialScience-1\\AIChatModel\\chat_template.json"
    );


    string input;
    getline (cin, input);

    while (input != "exit"){
        string response = ai.generateResponse(input);
        cout << "Model's response: " << response << endl;
        getline (cin, input);
    }

    ai.printChatHistory();
   

	return 0;

}




