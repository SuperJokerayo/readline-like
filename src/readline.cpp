#include "readline.h"
#include <fstream>
#include <iostream>

ReadLine::ReadLine() {
    // std::cout << "initializing ReadLine" << std::endl;
    history = std::vector<std::string>();
}

ReadLine::~ReadLine() {

}

int ReadLine::historyLoad(const std::string filename)
{
    std::fstream fp;
    fp.open(filename, std::ios::in);
    if (!fp.is_open()) return -1;
    // std::cout << "start loading history" << std::endl;
    std::string line;
    while(getline(fp, line)) {
        int pos;
        pos = line.find_first_of('\r');
        if(pos == std::string::npos) pos = line.find_first_of('\n');
        if(pos != std::string::npos) line = line.substr(0, pos + 1);
        this -> historyAdd(line);
    }
    fp.close();
    return 0;
}

int ReadLine::historyAdd(const std::string line) {
    int n = this -> history.size();
    if(n and history.back() == line) return 0;
    if(n == this -> HISTORY_MAX_LINE) this -> history.erase(this -> history.begin());
    // std::cout << "adding history: " << line << std::endl;
    history.emplace_back(line);
    return 1;
}

void ReadLine::printHistory() {
    // std::cout << "start printing history" << std::endl;
    for(auto h : this -> history) {
        std::cout << h << std::endl;
    }
}