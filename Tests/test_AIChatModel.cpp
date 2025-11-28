#include <gtest/gtest.h>

#include "AIChatModel.h"
#include "json.hpp"
#include "ollama.hpp"

#include <string>
#include <fstream>
#include <iostream>

using json = nlohmann::json;
using namespace std;

class AIChatModelFixture : public ::testing::Test {
    protected:
        AIChatModel model;

        AIChatModelFixture() 
        : model("C:\\Users\\Inesa Cosic\\OneDrive - The Pennsylvania State University\\VR-AI-Project\\VR-MatrialScience-1\\AIChatModel\\chat_template.json",
            {
                {"material", "steel"},
                {"parameter", "200 MPa"}
            }) 
        {}
};


TEST_F(AIChatModelFixture, Initialization){

    EXPECT_EQ(model.getModelName(), "llama3.2:3b");
    EXPECT_EQ(model.getEmbeddingModelName(), "nomic-embed-text:latest");
    EXPECT_EQ(model.getContentFiles().size(), 2);
    EXPECT_EQ(model.getChatHistory().size(), 4);

    string last_message_content = model.getChatHistory().back()["content"];

    EXPECT_EQ(
        last_message_content,
        "The student is currently looking at steel under 200 MPa force in a VR environment."
    );
}

TEST_F(AIChatModelFixture, fetchMaterialScienceContent){

    // before fetching content, RAG_DATABASE should be empty
    EXPECT_EQ(model.getChatHistory().size(), 4);

    model.fetchMaterialScienceContent("What is a material science engineer?");

    // after fetching content, RAG_DATABASE should have entries
    EXPECT_GT(model.getChatHistory().size(), 4);
}

TEST_F(AIChatModelFixture, GenerateResponse){

    string user_input = "What is a material science engineer?";
    string response = model.generateResponse(user_input);

    // Check that the response is not empty
    EXPECT_FALSE(response.empty());

    model.printChatHistory();

}

TEST(AIChatModelTest, GenerateResponseAccuracy){


    fstream f("C:\\Users\\Inesa Cosic\\OneDrive - The Pennsylvania State University\\VR-AI-Project\\VR-MatrialScience-1\\Tests\\amibiguous_prompts.json");

    json data = json::parse(f);

    for (const auto& msg : data["messages"]){

        AIChatModel model = AIChatModel(
        "C:\\Users\\Inesa Cosic\\OneDrive - The Pennsylvania State University\\VR-AI-Project\\VR-MatrialScience-1\\AIChatModel\\chat_template.json",
        {
            {"material", "steel"},
            {"parameter", "200 MPa"}
        }
        );

        string input = msg["content"];
        cout << "Testing input: " << input << endl;

        string result = model.generateResponse(input);
        cout << "Model response: " << result << endl;

        cout << "------------------------" << endl;

        EXPECT_FALSE(result.empty());

    }

    
}

TEST(AIChatModelTest, GenerateResponseAccuracy2){
    
     fstream f("C:\\Users\\Inesa Cosic\\OneDrive - The Pennsylvania State University\\VR-AI-Project\\VR-MatrialScience-1\\Tests\\expected_prompts.json");

    json data = json::parse(f);

    for (const auto& msg : data["messages"]){

        AIChatModel model = AIChatModel(
        "C:\\Users\\Inesa Cosic\\OneDrive - The Pennsylvania State University\\VR-AI-Project\\VR-MatrialScience-1\\AIChatModel\\chat_template.json",
        {
            {"material", "steel"},
            {"parameter", "200 MPa"}
        }
        );

        string input = msg["content"];
        cout << "Testing input: " << input << endl;

        string result = model.generateResponse(input);
        cout << "Model response: " << result << endl;

        cout << "------------------------" << endl;

        EXPECT_FALSE(result.empty());

    }

}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}