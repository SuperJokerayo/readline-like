#ifndef READLINE_H
#define READLINE_H

#include <string>
#include <vector>

class ReadLine
{

private:
    const int HISTORY_MAX_LINE = 1024;
    std::vector<std::string> history;

    int historyAdd(const std::string line);

public:
    ReadLine();
    ~ReadLine();

    int historyLoad(const std::string filename);


    // help function in development
    void printHistory();
};


#endif // READLINE_H