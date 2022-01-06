#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

using std::string;
using std::map;
using std::multimap;
using std::vector;
using namespace std::string_literals;

class States {
    map<string, map<string, vector<string>>> instructions;
public:
    States() = default;

    vector<string> getNextState(const string &initial, const string &route) {
        return instructions[initial][route];
    }

    void instructor(const vector<string> &inst) {
        instructions[inst.at(0)][inst.at(1)] = {inst.at(2), inst.at(3), inst.at(4)};
    }

};

/**
 * Check if the machine is looping.
 *
 * This functions checks if the current snapshot occurred before.
 * If it has not occurred before, it saves the current snapshot to the `saved` vector
 *
 * @param snapshotsVec snapshot database vector.
 * @param snapshot current snapshot.
 * @return `saved` vector contains `str`.
 */
bool isLooping(vector<string> &snapshotsVec, const string &snapshot) {
    if (find(snapshotsVec.begin(), snapshotsVec.end(), snapshot) != snapshotsVec.end()) return true;
    else snapshotsVec.push_back(snapshot);
    return false;
}

vector<string> tokenizer(const string &str) {
    vector<string> tokenized;
    string token;
    auto delimiter = ' ';
    auto counter = 0;
    for (const char &ch: str) {
        if (ch == '(') break;  // if hints inside the parenthesis are included in the input.txt
        if (ch != delimiter) token.push_back(ch);  // append to string
        else {
            tokenized.push_back(token);  // append to vector
            token = "";
        }
        if (counter == str.length() - 1) {
            tokenized.push_back(token);  // append the remained string to vector
            break;
        }
        counter++;
    }
    return tokenized;
}

vector<string> stringToVector(const string &str) {
    vector<string> vec;
    for (const auto &ch: str)
        vec.emplace_back(1, ch);
    return vec;
}

string vectorToString(vector<string> &vec, const string &x = "") {
    auto s = ""s;
    for (const auto &str: vec)
        s += str + x;
    return s;
}


int main() {

    string myText;
    string startState;
    string acceptState;
    string rejectState;
    string blankSymbol;
    vector<string> tapes;

    vector<string> traversedVec;
    vector<string> acceptanceVec;

    string txtPath;
    if (std::filesystem::exists("input.txt")) txtPath = "input.txt";
    else if (std::filesystem::exists("../input.txt")) txtPath = "../input.txt";
    else {
        std::ofstream myFile("output.txt");
        myFile << "File Not Found" << std::endl;
        std::cout << "File Not Found" << std::endl;
        return 0;
    }

    States myState;
    std::ifstream myReadFile(txtPath);

    auto lineCounter = 0;
    while (getline(myReadFile, myText)) {
        if (lineCounter == 4) startState = myText;
        else if (lineCounter == 5) acceptState = myText;
        else if (lineCounter == 6) rejectState = myText;
        else if (lineCounter == 7) blankSymbol = myText;
        else if (lineCounter == 10) {
            if (myText.starts_with("q")) myState.instructor(tokenizer(myText));
            else tapes.push_back(myText);
            continue;
        }
        lineCounter++;
    }

    for (const auto &tape: tapes) {

        auto currentState = startState;
        auto currentTape = stringToVector(tape + blankSymbol);
        vector<string> traversedPath = {currentState};
        vector<string> snapshots = {};  // it holds snapshots from each iteration

        auto currentHead = 0;
        auto looping = false;
        while (!looping) {
            if (currentTape.back() != blankSymbol || currentHead >= currentTape.size() - 1)
                currentTape.push_back(blankSymbol);
            looping = isLooping(snapshots, currentState + std::to_string(currentHead) + vectorToString(currentTape));
            if (currentState == acceptState || currentState == rejectState) break;

            auto tempVec = myState.getNextState(currentState, currentTape[std::max(0, currentHead)]);
            traversedPath.push_back(tempVec.at(2));
            currentTape.at(currentHead) = tempVec.at(0);

            if (tempVec.at(1) == "R") ++currentHead;
            else if (tempVec.at(1) == "L" && currentHead > 0) --currentHead;

            currentState = tempVec.at(2);
        }
        traversedVec.push_back(vectorToString(traversedPath, " ") + "(route taken)");
        if (looping) acceptanceVec.emplace_back("Looped");
        else if (traversedPath.back() == acceptState) acceptanceVec.emplace_back("Accepted");
        else acceptanceVec.emplace_back("Rejected");
    }

    std::ofstream myOutFile("output.txt");
    for (auto i = 0; i < tapes.size(); ++i) {
        myOutFile << traversedVec.at(i) << std::endl;
        std::cout << traversedVec.at(i) << std::endl;
        myOutFile << acceptanceVec.at(i) << std::endl;
        std::cout << acceptanceVec.at(i) << std::endl;
    }
    myOutFile.close();

    return 0;
}
