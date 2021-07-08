#include "judger_config.h"
#include "killerlib.h"


void monitor(pid_t pid, int time) {

    
    // if time % 1000 < 500 do ceil
    // if time % 1000 >= 500 do ceil then add 1
    time = ((time+500) / 1000 + 1);
    

    if (sleep(time) != 0) {
        kill(pid, SIGKILL);
        raise(KILLER_SLEEP_FAILED);
        return;
    }
    if (kill(pid, SIGKILL)) {
        raise(KILLER_KILL_FAILED);
    }
    return;
}

