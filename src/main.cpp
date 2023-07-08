#include "readline.h"

int main() {
    ReadLine rl;
    rl.historyLoad("../resource/history.txt");
    rl.printHistory();
    return 0;
}