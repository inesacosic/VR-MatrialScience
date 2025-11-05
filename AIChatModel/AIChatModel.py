import ollama
import json
from ollama import chat
from pathlib import Path


class AIChatModel:
    def __init__(self, template):
        self.instance = template
        self.model = self.instance['model']
        self.messages = self.instance['messages']

    def generateResponse(self, input):

        self.messages.append({"role": "user", "content": input})

        response = ollama.chat(model=self.model, messages=self.messages)

        reply = response["message"]["content"]

        self.messages.append({"role": "assistant", "content": reply})

        return reply


    def fetchMaterialScienceContent(self):
        pass

if __name__ == "__main__":
    template_file = 'AIChatModel/chat_template.json'
    with open(Path(template_file), 'r') as f:
            template = json.load(f)
    
    ai_chat_model = AIChatModel(template)

    response = ai_chat_model.generateResponse("What is material science?")

    print("AI:", response)