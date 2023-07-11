#include "readline.h"

#include <sys/stat.h>
#include <unistd.h>

#include <fstream>
#include <iostream>

ReadLine::ReadLine() {
    history = std::vector<std::string>();
    historyLoad("../resource/history.txt");
}

ReadLine::~ReadLine() {

}

int ReadLine::editLine(const std::string prompt) {
    editLineStart(STDIN_FILENO, STDOUT_FILENO, "", prompt);
    int ret;
    while((ret = editLineFeed()) == 1);
    editLineEnd();
    return ret;
}

int ReadLine::editLineStart(int stdin_fd, int stdout_fd, std::string buf, 
                            const std::string prompt) {
    state.in_completion = 0;
    state.ifd = stdin_fd != -1 ? stdin_fd : STDIN_FILENO;
    state.ofd = stdout_fd != -1 ? stdout_fd : STDOUT_FILENO;
    state.buf = buf;
    std::string* prompt_ = (std::string*)&state.prompt;
    *prompt_ = prompt;
    state.pos = 0;
    state.history_index = history.size();

    if (enableRawMode(state.ifd) == -1) return -1;

    historyAdd(buf);
    auto prompt_with_color = setTextColor(prompt, state.text_color);
    if (write(state.ofd, prompt_with_color.c_str(), prompt_with_color.length()) == 
        -1) 
        return -1;
    return 0;
}

int ReadLine::editLineFeed() {
    char c;
    int nread;
    char seq[3];
    nread = read(state.ifd, &c, 1);
    if (nread <= 0) return -1;
    switch(c) {
        case ENTER:
            history.pop_back(); 
            return 0;
        case CTRL_C:
            return -1;
        case BACKSPACE:
        case CTRL_H:
            deleteLeft();
            break;
        case CTRL_D:
            if (state.buf.length() > 0) {
                deleteRight();
            } else {
                return -1;
            }
            break;
        case CTRL_T:
            swapLeftAndRight();
            break;
        case CTRL_B:
            moveLeft();
            break;
        case CTRL_F:
            moveRight();
            break;
        case CTRL_P:
            prevHistory();
            break;
        case CTRL_N:
            nextHistory();
            break;
        case ESC:
            if (read(state.ifd, seq, 1) == -1) break;
            if (read(state.ifd, seq + 1,1) == -1) break;

            if (seq[0] == '[') {
                if (seq[1] >= '0' && seq[1] <= '9') {
                    if (read(state.ifd, seq + 2, 1) == -1) break;
                    if (seq[2] == '~') {
                        switch(seq[1]) {
                        case '3': 
                            deleteRight();
                            break;
                        }
                    }
                } else {
                    switch(seq[1]) {
                    case 'A':
                        prevHistory();
                        break;
                    case 'B': 
                        nextHistory();
                        break;
                    case 'C': 
                        moveRight();
                        break;
                    case 'D': 
                        moveLeft();
                        break;
                    case 'H': 
                        moveHome();
                        break;
                    case 'F': 
                        moveEnd();
                        break;
                    }
                }
            }
            else if (seq[0] == 'O') {
                switch(seq[1]) {
                case 'H': 
                    moveHome();
                    break;
                case 'F': 
                    moveEnd();
                    break;
                }
            }
            break;
        default:
            insertChar(c);
            break;
        case CTRL_U:
            deleteLine();
            break;
        case CTRL_K:
            deleteTail();
            break;
        case CTRL_A:
            moveHome();
            break;
        case CTRL_E:
            moveEnd();
            break;
        case CTRL_L:
            clearScreen();
            break;
        case CTRL_W:
            deleteHead();
            break;
    }
    return 1;
}

void ReadLine::editLineEnd() {
    disableRawMode(state.ifd);
    std::cout << std::endl;
}


int ReadLine::refreshLine() {
    auto new_line = "\r"+ state.prompt + state.buf + "\x1b[0K" + "\r\x1b[" + 
                    std::to_string(state.pos + state.prompt.length()) + "C";
    history[state.history_index] = state.buf;
    new_line = setTextColor(new_line, state.text_color);
    if (write(state.ofd, new_line.c_str(), new_line.length()) == -1) return -1;
    return 0;
}

void ReadLine::deleteLeft() {
    if (state.pos > 0 and state.buf.length() > 0) {
        state.buf.erase(state.pos - 1, 1);
        state.pos--;
        refreshLine();
    }
}

void ReadLine::deleteRight() {
    if (state.pos < state.buf.length()) {
        state.buf.erase(state.pos, 1);
        refreshLine();
    }
}


void ReadLine::deleteLine() {
    if (state.buf.length() > 0) {
        state.buf = "";
        state.pos = 0;
        refreshLine();
    }
}


void ReadLine::deleteTail() {
    if (state.pos < state.buf.length()) {
        state.buf = state.buf.substr(0, state.pos);
        refreshLine();
    }
}


void ReadLine::deleteHead() {
    if (state.pos > 0) {
        state.buf = state.buf.substr(state.buf.length() - state.pos);
        refreshLine();
    }
}


void ReadLine::swapLeftAndRight() {
    if (state.pos > 0) {
        if (state.pos < state.buf.length()) {
            std::swap(state.buf[state.pos - 1], state.buf[state.pos]);
            state.pos++;
            refreshLine();
        } else if(state.buf.length() >= 2) {
            std::swap(state.buf[state.pos - 2], state.buf[state.pos - 1]);
            refreshLine();
        }
    }
}


void ReadLine::moveHome() {
    state.pos = 0;
    refreshLine();
}


void ReadLine::moveEnd() {
    state.pos = state.buf.length();
    refreshLine();
}


void ReadLine::moveLeft() {
    if (state.pos > 0) {
        state.pos--;
        refreshLine();
    }
}


void ReadLine::moveRight() {
    if (state.pos < state.buf.length()) {
        state.pos++;
        refreshLine();
    }
}

void ReadLine::prevHistory() {
    if (state.history_index > 0) {
        state.history_index--;
        state.buf = history[state.history_index];
        state.pos = state.buf.length();
        refreshLine();
    }
}


void ReadLine::nextHistory() {
    if (state.history_index < history.size() - 1) {
        state.history_index++;
        state.buf = history[state.history_index];
        state.pos = state.buf.length();
        refreshLine();
    }
}


void ReadLine::insertChar(const char c) {
    state.buf.insert(state.pos, 1, c);
    state.pos++;
    // history[state.history_index] = state.buf;
    refreshLine();
}


void ReadLine::clearScreen() {
    if (write(state.ofd, "\x1b[H\x1b[2J", 7) == -1) {}
    refreshLine();
}



int ReadLine::responseLine(const std::string line) {
    system(line.c_str());
    historyAdd(line);
    historySave("../resource/history.txt");
    state.history_index = history.size();
    return 0;
}


int ReadLine::historyLoad(const std::string filename)
{
    std::fstream fp;
    fp.open(filename, std::ios::in);
    if (!fp.is_open()) return -1;
    std::string line;
    while(getline(fp, line)) {
        int pos = line.find_first_of('\r');
        if(pos == std::string::npos) pos = line.find_first_of('\n');
        if(pos != std::string::npos) line = line.substr(0, pos + 1);
        historyAdd(line);
    }
    fp.close();
    return 0;
}

int ReadLine::historyAdd(const std::string line) {
    int n = history.size();
    if(n and history.back() == line) return 0;
    if(n == HISTORY_MAX_LINE) history.erase(history.begin());
    history.emplace_back(line);
    return 1;
}


int ReadLine::historySave(const std::string filename) {
    mode_t old_umask = umask(S_IXUSR|S_IRWXG|S_IRWXO);
    std::fstream fp;
    fp.open(filename, std::ios::out);
    umask(old_umask);
    if (!fp) return -1;
    chmod(filename.c_str(), S_IRUSR|S_IWUSR);
    for (auto h : history)
        fp << h << std::endl;
    fp.close();
    return 0;
}


static int enableRawMode(int ifd) {
    struct termios raw;

    if (!isatty(STDIN_FILENO)) return -1;
    if (!atexit_registered) {
        atexit(lineAtExit);
        atexit_registered = 1;
    }
    if (tcgetattr(ifd, &orig_termios) == -1) return -1;

    raw = orig_termios; 
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 1; raw.c_cc[VTIME] = 0; 
    if (tcsetattr(ifd, TCSAFLUSH, &raw) < 0) return -1;
    rawmode = 1;
    return 0;
}


static void lineAtExit(void) {
    disableRawMode(STDIN_FILENO);
}

static void disableRawMode(int ifd) {
    if (rawmode && tcsetattr(ifd, TCSAFLUSH, &orig_termios) != -1)
        rawmode = 0;
}

const std::string ReadLine::getBuf() {
    return state.buf;
}


const std::string ReadLine::setTextColor(const std::string line, 
                                         std::string text_color) {
    if (readline_color::color_map.find(text_color) != 
        readline_color::color_map.end())
        return "\x1b[" + std::to_string(readline_color::color_map[text_color]) + 
               "m" + line + "\x1b[0m";
    return line;
}

void ReadLine::setTextColor(const std::string text_color) {
    state.text_color = text_color;
}