#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "module.h"
#include <cstring>

int main(int argc, const char** argv) {
    using namespace std;

    string fData;
    ASSEMBLY Assembly;
    vector<string> binary;
    string outputFileName;

    if (argc > 1){
        if (argc > 3 && strcmp(argv[2],"-o") == 0){
            outputFileName = argv[3];
        }
        else{
            outputFileName = argv[1];
            outputFileName.append(".hack");
        }
    }
    else{
        cout << "Usage: " << argv[0] << " <filename> <optional args>\n";
        cout << "Optional arguments:" << endl;
        cout << "\t-o output.hack" << endl;
        exit(-1);
    }

    ifstream fStream(argv[1]);
    ofstream output(outputFileName);

    if (!fStream.good()){
        cout << "ERROR: Unable to open file!" << endl;
        exit(FILE_NOT_FOUND);
    }

    initInsMaps();

    while (getline(fStream, fData)) {
        Assembly.push_back(fData);
    }

    Assembly = cleanAssembly(Assembly);
    binary = parseAssembly(Assembly);

   for (const auto& bin: binary){
       output << bin << endl;
   }

    cout << "[SUCCESS] Output file saved to "  << outputFileName << endl;

    fStream.close();
    output.close();
    return 0;
}