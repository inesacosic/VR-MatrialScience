#include "ollama.hpp"
#include <iostream>
#include <string.h>
#include <vector>
using namespace std;


class AIChatModel{
    private:
        string model_name = "llama3.2:3b";
        ollama::messages chat_history; // initializes a vector where we will push all messages to build context

    public:
        string generateResponse(string input);

};

string AIChatModel::generateResponse(string input){
    
    cout << "Connecting to ollama..." <<endl;
    string reply = ollama::generate(this->model_name, input);
    ollama::message response("assistant", reply);
    this -> chat_history.push_back(response);
    return reply;
}

int main(){

	AIChatModel ai;
    string input = "What is material science?";

    string response = ai.generateResponse(input);

    cout << "Model's response: " << response << endl;
	return 0;

}
