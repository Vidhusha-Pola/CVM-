#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "lexer.h"
#include "parser.h"
#include "compiler.h"
#include "vm.h"


void compileToFile(const std::string& sourceFile, const std::string& outFile, bool debugMode) {
    std::ifstream file(sourceFile);
    if (!file.is_open()) throw std::runtime_error("Could not open source file: " + sourceFile);
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    Lx lexer(source);
    Parser parser(lexer.tks());
    Compiler compiler;
    auto bytecode = compiler.compile(parser.parse());

    if (debugMode) compiler.disassemble();

  
    std::ofstream out(outFile, std::ios::binary);
    if (!out.is_open()) throw std::runtime_error("Could not open output file: " + outFile);
    out.write(reinterpret_cast<const char*>(bytecode.data()), bytecode.size());
    out.close();

    std::cout << "Successfully compiled '" << sourceFile << "' -> '" << outFile << "'\n";
}


void runFromFile(const std::string& binFile) {
    
    std::ifstream file(binFile, std::ios::binary | std::ios::ate);
    if (!file.is_open()) throw std::runtime_error("Could not open binary file: " + binFile);
    
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg); 
    
    std::vector<uint8_t> bytecode(size);
    if (file.read(reinterpret_cast<char*>(bytecode.data()), size)) {
        std::cout << "--- CVM++ VM Output ---\n";
        VM vm(bytecode);
        vm.run();
    } else {
        throw std::runtime_error("Failed to read binary data.");
    }
}


int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage:\n";
        std::cerr << "  Compile: cvm compile <source.cvm> <output.bin> [--debug]\n";
        std::cerr << "  Run:     cvm run <program.bin>\n";
        return 1;
    }

    std::string command = argv[1];

    try {
        if (command == "compile") {
            if (argc < 4) throw std::runtime_error("Missing output file name.");
            std::string sourceFile = argv[2];
            std::string outFile = argv[3];
            bool debugMode = (argc > 4 && std::string(argv[4]) == "--debug");
            
            compileToFile(sourceFile, outFile, debugMode);
        } 
        else if (command == "run") {
            std::string binFile = argv[2];
            runFromFile(binFile);
        } 
        else {
            std::cerr << "Unknown command: " << command << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}