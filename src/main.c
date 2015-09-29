#include <pebble.h>
#include "info-sessions.h"
#include "action-menu.h"
#include "keys.h"

static Window *s_category_window;
static MenuLayer *s_category_menu_layer;

static uint16_t category_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
  return 3;
}

static void category_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
  switch(cell_index->row) {
    case 0:
      // Info Sessions
      menu_cell_basic_draw(ctx, cell_layer, "Info Sessions", "CECA", NULL);
      break;
    case 1:
      // Lunch Menus
      menu_cell_basic_draw(ctx, cell_layer, "Lunch Menus", "UW Food Services", NULL);
      break;
    case 2:
      // Dinner Menus
      menu_cell_basic_draw(ctx, cell_layer, "Dinner Menus", "UW Food Services", NULL);
      break;
  }
}

static void category_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  switch (cell_index->row) {
    case 0:
      // Info Sessions
      info_sessions_window_push();

      // Send AppMessage to get info sessions
      DictionaryIterator *iter;
      app_message_outbox_begin(&iter);
      dict_write_uint8(iter, KEY_GET_DATA, INFO_SESSION);
      dict_write_end(iter);
      app_message_outbox_send();

      break;
    case 1:
      // Lunch Menus
      display_food_menus_action_menu(LUNCH_MENU);

      break;
    case 2:
      // Dinner Menus
      display_food_menus_action_menu(DINNER_MENU);

      break;
  }
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *key_returned_t = dict_find(iter, KEY_RECEIVED_DATA);
  uint8_t value_returned = key_returned_t->value->uint8;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "value_returned: %d", value_returned);

  if (value_returned == INFO_SESSION) {
    // Received info session data
    APP_LOG(APP_LOG_LEVEL_DEBUG, "info session data received");
    info_sessions_data_load(iter);
  }
  else if (value_returned == LUNCH_MENU) {
    // Received lunch menu data
  }
  else if (value_returned == DINNER_MENU) {
    // Received dinner menu data
  }
}


static void category_window_load(Window *window) {
  // Get the root layer
  Layer *category_window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(category_window_layer);

  // Create menu layer
  s_category_menu_layer = menu_layer_create(bounds);
  menu_layer_set_highlight_colors(s_category_menu_layer, GColorYellow, GColorBlack);
  menu_layer_set_callbacks(s_category_menu_layer, NULL, (MenuLayerCallbacks) {
    .get_num_rows = category_menu_get_num_rows_callback,
    .draw_row = category_menu_draw_row_callback,
    .select_click = category_menu_select_callback
  });

  // Sets up and down scrolling for the menu layer
  menu_layer_set_click_config_onto_window(s_category_menu_layer, window);

  layer_add_child(category_window_layer, menu_layer_get_layer(s_category_menu_layer));
}

static void category_window_unload(Window *window) {
  // Destroy menu layer
  menu_layer_destroy(s_category_menu_layer);
}

static void init() {
  // Create Window element and assign to pointer
  s_category_window = window_create();
  window_set_background_color(s_category_window, GColorYellow);

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_category_window, (WindowHandlers) {
    .load = category_window_load,
    .unload = category_window_unload
  });

  // Show the category Window on the watch
  window_stack_push(s_category_window, true);

  // Register with AppMessage
  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit() {
  // Destroy Windows
  window_destroy(s_category_window);
  window_destroy(s_info_sessions_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}