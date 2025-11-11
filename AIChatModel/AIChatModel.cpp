#include "ollama.hpp"
#include <iostream>
#include <string.h>
#include <vector>
using namespace std;


class AIChatModel{
    private:
        string model_name = "llama3.2:3b"; // ollama model we are using is llama3.2:3b
        ollama::messages chat_history; // initializes a vector where we will push all messages to build context

    public:
        string generateResponse(string input);
        void printChatHistory();

};



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

int main(){

	AIChatModel ai;
    string input = "What is material science?";

    string response = ai.generateResponse(input);

    cout << "Model's response: " << response << endl;

	return 0;

}
