#ifndef READLINE_H_
#define READLINE_H_

#include <string>
#include <vector>
#include <termios.h>
#include "readline_state.h"


enum KEY_ACTION{
    KEY_NULL = 0,	    /* NULL */
	CTRL_A = 1,         /* Ctrl+a */
	CTRL_B = 2,         /* Ctrl-b */
	CTRL_C = 3,         /* Ctrl-c */
	CTRL_D = 4,         /* Ctrl-d */
	CTRL_E = 5,         /* Ctrl-e */
	CTRL_F = 6,         /* Ctrl-f */
	CTRL_H = 8,         /* Ctrl-h */
	TAB = 9,            /* Tab */
	CTRL_K = 11,        /* Ctrl+k */
	CTRL_L = 12,        /* Ctrl+l */
	ENTER = 13,         /* Enter */
	CTRL_N = 14,        /* Ctrl-n */
	CTRL_P = 16,        /* Ctrl-p */
	CTRL_T = 20,        /* Ctrl-t */
	CTRL_U = 21,        /* Ctrl+u */
	CTRL_W = 23,        /* Ctrl+w */
	ESC = 27,           /* Escape */
	BACKSPACE =  127    /* Backspace */
};
static struct termios orig_termios;
static int rawmode = 0;
static int atexit_registered = 0;


static int enableRawMode(int ifd);
static void disableRawMode(int ifd);
static void lineAtExit(void);
class ReadLine
{

private:
    const int HISTORY_MAX_LINE = 1024;
    std::vector<std::string> history;
    
    ReadLineState state;

    int editLineStart(int stdin_fd, int stdout_fd, std::string buf, const std::string prompt);
    int editLineFeed();
    void editLineEnd();

    // int responseLine(const std::string line);

    int refreshLine();

    void deleteLeft();
    void deleteRight();
    void deleteLine();
    void deleteTail();
    void deleteHead();
    void swapLeftAndRight();
    void moveHome();
    void moveEnd();
    void moveLeft();
    void moveRight();
    void prevHistory();
    void nextHistory();
    void insertChar(const char c);
    void clearScreen();


    int historyLoad(const std::string filename);
    int historyAdd(const std::string line);
    int historySave(const std::string filename);

    // help function in development
public:
    ReadLine();
    ~ReadLine();
    int editLine(const std::string prompt);
    int responseLine(const std::string line);
    const std::string getBuf();
    void printHistory();
};


#endif // READLINE_H_