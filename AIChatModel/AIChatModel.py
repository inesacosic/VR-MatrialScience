import ollama
import json
import os
import glob
from ollama import chat
from pathlib import Path
from typing import List, Dict, Any


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


    def fetchMaterialScienceContent(data_directory: str):
        pass
    
    """
    This function loads the .txt documents from the given directory (data_directory) and stores them into a dict
    with the structure {<< filename >>.txt: << content of file >>}
    """
    def loadDocuments(data_directory: str) -> Dictict[str, str]:

        documents = {}

        for file_path in glob.glob(os.path.join(data_directory, "*.txt")): # os.path.join(data_directory, "*.txt") creates a file pattern such as directory/*.txt
                                                                           # and glob.glob finds all files that match that pattern
             with open(file_path, 'r') as file:
                content = file.read()
                documents[os.path.basename(file_path)] = content           # os.path.basename(file_path) extracts just the filename from the path
        
        return documents
    
    def chunkDocuments(documents: Dict[str, str], chunk_size: int = 500, chunk_overlap: int = 50) -> List[Dict[str, Any]]:
        
        chunked_documents = []


         

if __name__ == "__main__":

    template_file = 'AIChatModel/chat_template.json'
    with open(Path(template_file), 'r') as f:
            template = json.load(f)
    
    ai_chat_model = AIChatModel(template)

    print("Student: What is material science?")

    response = ai_chat_model.generateResponse("What is material science?")

    print("AI:", response)

    # load documents
