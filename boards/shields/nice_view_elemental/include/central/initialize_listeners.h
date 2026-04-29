#pragma once

#include "../initialize_listeners.h"

#include "../utils/draw_battery.h"
#include <stdbool.h>
#include <zmk/endpoints.h>
#include <zmk/keymap.h>

struct connectivity_state {
  struct zmk_endpoint_instance selected_endpoint;
  int active_profile_index;
  bool active_profile_connected;
  bool active_profile_bonded;
};

struct layer_state {
  zmk_keymap_layer_index_t index;
  const char *name;
};

struct modifiers_state {
  bool is_shift_active;
  bool is_ctrl_active;
  bool is_alt_active;
  bool is_gui_active;
};

struct states {
  struct battery_state battery;
  struct connectivity_state connectivity;
  struct layer_state layer;
  struct modifiers_state modifiers;
};

extern struct states states;

void initialize_listeners();
