#include "notice-window.h"

static Window *s_notice_window;
static TextLayer *s_notice_text_layer;
static AppTimer *s_notice_timer;
static uint8_t s_notice_type;

static void notice_window_load(Window *window) {
  // Get the root layer
  Layer *notice_window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(notice_window_layer);
  uint8_t font_height = 24;
  int16_t window_width = bounds.size.w;
  int16_t window_height = bounds.size.h;

  // Create text layer
  s_notice_text_layer = text_layer_create(GRect(0, (window_height / 2) - (font_height / 2), window_width, font_height));
  text_layer_set_background_color(s_notice_text_layer, GColorClear);
  text_layer_set_text_alignment(s_notice_text_layer, GTextAlignmentCenter);
  text_layer_set_font(s_notice_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));

  if (s_notice_type == 0) {
    text_layer_set_text(s_notice_text_layer, "Pin Added!");
  }
  else if (s_notice_type == 1) {
    text_layer_set_text(s_notice_text_layer, "Pin Removed!");
  }

  layer_add_child(notice_window_layer, text_layer_get_layer(s_notice_text_layer));
}

static void notice_window_unload(Window *window) {
  // Destroy text layer
  text_layer_destroy(s_notice_text_layer);
}

static void notice_timer_callback(void *data) {
  window_stack_remove(s_notice_window, true);
}

void notice_window_push(uint8_t type) {
	s_notice_type = type;

  // Create Window element and assign to pointer
  s_notice_window = window_create();
  window_set_background_color(s_notice_window, GColorYellow);

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_notice_window, (WindowHandlers) {
    .load = notice_window_load,
    .unload = notice_window_unload
  });

  // Show the notice Window on the watch
  window_stack_push(s_notice_window, true);
  s_notice_timer = app_timer_register(2000, notice_timer_callback, NULL);
}