#pragma once

#include <pebble.h>

static Window *s_info_sessions_window;

void info_sessions_data_load(DictionaryIterator *iter);

void info_sessions_window_push();