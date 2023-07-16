#ifndef READLINE_STATE_H_
#define READLINE_STATE_H_

#include <cstddef>
#include <string>

class ReadLineState
{

public:
    int in_completion;
    size_t completion_idx;
    int ifd;
    int ofd;
    std::string buf;
    const std::string prompt;
    size_t pos;
    int history_index;
    std::string text_color;

    ReadLineState();
    ~ReadLineState();
};

#endif // READLINE_STATE_H_