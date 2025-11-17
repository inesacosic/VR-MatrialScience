#include <gtest/gtest.h>
#include "AIChatModel.h"
#include <string>

TEST(AIChatModelTest, Initialization){

    AIChatModel model = AIChatModel(
    "C:\\Users\\Inesa Cosic\\OneDrive - The Pennsylvania State University\\VR-AI-Project\\VR-MatrialScience-1\\AIChatModel\\chat_template.json"
    );

    string input = "What is material science?";

    string response = model.generateResponse(input);

    EXPECT_TRUE(response.length() > 0);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}