#include "readline.h"

int main(int argc, const char** argv) {
    ReadLine rl;
    while (1) {
        auto text_color = "default";
        if(argc > 1) {
            text_color = argv[argc - 1];
        }
        rl.setTextColor(text_color);
        auto line = rl.editLine("Demo>>");
        if (-1 == line) break;
        else if(0 == line) 
            rl.responseLine(rl.getBuf());
    }
    return 0;
}