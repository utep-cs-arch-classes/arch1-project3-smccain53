
#include <state.h>

int master_state = 0;

int get_state() {
  return master_state;
}

void set_state(int state) {
  master_state = state;
}
