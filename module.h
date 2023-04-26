//
// Created by mister-rc on 25-04-2023.
//

#ifndef HACKASSEMBLER_CPP_INSMAP_H
#define HACKASSEMBLER_CPP_INSMAP_H

#endif //HACKASSEMBLER_CPP_INSMAP_H

#include <map>
#include <string>
#include <vector>
#include <bitset>


#define INSMAP std::map<std::string, std::string>
#define SYM_TABLE std::map<std::string, int>

#define FILE_NOT_FOUND -3
#define INVALID_INSTRUCTION -7
#define ASSEMBLY std::vector<std::string>
#define SRC 7
#define DEST 8
#define JMP 9

SYM_TABLE symTable;
INSMAP insMapDest;
INSMAP insMapSrc;
INSMAP insMapJmp;

int lineNum = 0;


void initInsMapDest(){
    insMapDest["null"] = "000";
    insMapDest["M"] = "001";
    insMapDest["D"] = "010";
    insMapDest["MD"] = "011";
    insMapDest["A"] = "100";
    insMapDest["AM"] = "101";
    insMapDest["AD"] = "110";
    insMapDest["AMD"] = "111";
}

void initInsMapSrc(){
    //  Source instructions
    insMapSrc["0"] = "0101010";
    insMapSrc["1"] = "0111111";
    insMapSrc["-1"] = "0111010";
    insMapSrc["D"] = "0001100";
    insMapSrc["A"] = "0110000";
    insMapSrc["!D"] = "0001101";
    insMapSrc["!A"] = "0110001";
    insMapSrc["-D"] = "0001111";
    insMapSrc["-A"] = "0110011";
    insMapSrc["D+1"] = "0011111";
    insMapSrc["A+1"] = "0110111";
    insMapSrc["D-1"] = "0001110";
    insMapSrc["A-1"] = "0110010";
    insMapSrc["D+A"] = "0000010";
    insMapSrc["A-D"] = "0000111";
    insMapSrc["D-A"] = "0010011";
    insMapSrc["D&A"] = "0000000";
    insMapSrc["D|A"] = "0010101";
    insMapSrc["M"] = "1110000";
    insMapSrc["!M"] = "1110001";
    insMapSrc["-M"] = "1110011";
    insMapSrc["M+1"] = "1110111";
    insMapSrc["M-1"] = "1110010";
    insMapSrc["D+M"] = "1000010";
    insMapSrc["D-M"] = "1010011";
    insMapSrc["M-D"] = "1000111";
    insMapSrc["D&M"] = "1000000";
    insMapSrc["D|M"] = "1010101";
};


void initInsMapJmp(){
    insMapJmp["JGT"] ="001";
    insMapJmp["JEQ"] ="010";
    insMapJmp["JGE"] ="011";
    insMapJmp["JLT"] ="100";
    insMapJmp["JNE"] ="101";
    insMapJmp["JLE"] ="110";
    insMapJmp["JMP"] ="111";
};


void initInsMaps(){
    initInsMapDest();
    initInsMapSrc();
    initInsMapJmp();
}

void setupSymTable(){
    symTable["SP"] = 0;
    symTable["LCL"] = 1;
    symTable["ARG"] = 2;
    symTable["THIS"] = 3;
    symTable["THAT"] = 4;
    symTable["SCREEN"] = 16384;
    symTable["KBD"] = 24576;
};

void initSymTable(ASSEMBLY& Assembly){
    setupSymTable();

    using namespace std;
    int symbolAddr = 16;

    string symbolName;
    string labelName;
    ASSEMBLY tokens;

    for (const auto& token: Assembly) {
        bool labelCondition = (token.starts_with("(") && token.ends_with(")"));
        if (labelCondition) {
            labelName = token.substr(1, token.length() - 2);
            symTable[labelName] = lineNum;
        }
        else{
            lineNum++;
        }
    }
    lineNum = 0;

    for (const auto& token: Assembly) {
        bool valueCondition = (token.starts_with("@") && !isdigit(token[1]));
        if (valueCondition) {
            symbolName = token.substr(1, token.length());

            if (symTable.find(symbolName) != symTable.end()) {
                tokens.push_back("@" + to_string(symTable[symbolName]));
                continue;
            }
            symTable[symbolName] = symbolAddr;
            tokens.push_back("@" + to_string(symTable[symbolName]));
            symbolAddr++;
        }
        else{
            tokens.push_back(token);
        }
        lineNum++;
    }

    Assembly = tokens;
}

std::string mapInsToBin(const std::string& token, int reason){
    using namespace std;
    INSMAP insMap;

    if (reason == SRC){
        insMap = insMapSrc;
    }
    else if (reason == DEST){
        insMap = insMapDest;
    }
    else if (reason == JMP){
        insMap = insMapJmp;
    }

    if (insMap.find(token) != insMap.end()){
        return insMap[token];
    }
    else {
        cout << "ERROR: Invalid instruction : " << token << endl;
        exit(INVALID_INSTRUCTION);
    }
}

void printAsm(ASSEMBLY& Assembly){
    using namespace std;
    for (const auto& token: Assembly){
        cout << token << endl;
    }
}

std::vector<std::string> parseAssembly(ASSEMBLY& Assembly){
    using namespace std;
    vector<string> binary;
    string tok;
    string insMapping;

    initSymTable(Assembly);

    for (const auto& token: Assembly){
        if (token.starts_with("@")){
            if (isdigit(token[1])){
                // A instruction; no symbols used
                tok = token.substr(1, token.length());
                // tok to binary
                bitset<16> bit(stoi(tok));
                binary.push_back(bit.to_string());
            }
        }
        else if (token.find("=") != string::npos){
//            Assignment
            insMapping.append("111");
            auto idx_assign = token.find("=");

            string dest = token.substr(0, idx_assign);
            string src = token.substr(idx_assign+1, token.length());

            auto src_mapping = mapInsToBin(src, SRC);
            auto dest_mapping = mapInsToBin(dest, DEST);

            insMapping.append(src_mapping);
            insMapping.append(dest_mapping);
            insMapping.append("000");

            binary.push_back(insMapping);
            insMapping = "";
        }
        else if (token.find(";") != string::npos){
            // conditional jump
            auto idx_sc = token.find(";");

            insMapping.append("111");

            string dest = "null";
            string src  = token.substr(0, idx_sc);
            string jmp = token.substr(idx_sc+1, token.length());

            auto src_mapping = mapInsToBin(src, SRC);
            auto dest_mapping = mapInsToBin(dest, DEST);
            auto jmp_mapping = mapInsToBin(jmp, JMP);

            insMapping.append(src_mapping);
            insMapping.append(dest_mapping);
            insMapping.append(jmp_mapping);
            binary.push_back(insMapping);
            insMapping = "";
        }
    }

    return binary;
}

ASSEMBLY cleanAssembly(ASSEMBLY& Assembly){
    using namespace std;

    ASSEMBLY tokens;
    string tok;

    for (const auto& token: Assembly){
        if (token.empty() || token.starts_with('/')) {continue;}

        auto idx_slash = token.find('/');
        auto idx_space = token.find(" ");


        if (idx_slash != string::npos){
            tok = token.substr(0, idx_slash);
        }
        else{
            tok = token;
        }
        if (idx_space != string::npos){
            tok.erase(remove(tok.begin(), tok.end(), ' '), tok.end());
            tokens.push_back(tok);
        }
        else{
            tokens.push_back(tok);
        }
    }

    return tokens;
}
