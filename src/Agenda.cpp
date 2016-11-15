#include "AgendaUI.hpp"
#include "signal.h"

AgendaUI aui;
using std::cout;
// signal handlers

void sig_inter(int) {
    std::cout << "\nKeyboard interruption occured.\n"
              << "All changes are saved.\n";
    aui.quitAgenda(1);
}

void sig_kill(int) {
    aui.quitAgenda(2);
}

int main() {
    if (signal(SIGINT, sig_inter) == SIG_ERR) {
        cout << "Warning: signal handler for keyboard interruption isn't set "
                "properly.\n";
    }
    if (signal(SIGKILL, sig_kill) == SIG_ERR) {
        cout << "Warning: signal handler for killing process isn't set "
                "properly.\n";
    }
    aui.OperationLoop();
    return 0;
}
