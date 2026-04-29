#pragma once

#include "../initialize_listeners.h"

#include "../utils/draw_battery.h"
#include <stdbool.h>

struct connectivity_state {
  bool connected;
};

struct states {
  struct battery_state battery;
  struct connectivity_state connectivity;
};

extern struct states states;

void initialize_listeners();
