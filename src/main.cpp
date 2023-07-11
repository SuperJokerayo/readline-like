#include "readline.h"

int main() {
    ReadLine rl;
    while (1) {
        auto line = rl.editLine("Joker>>");
        if (-1 == line) break;
        else if(0 == line) rl.responseLine(rl.getBuf());
    }
    return 0;
}