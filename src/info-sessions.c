#include "info-sessions.h"
#include "action-menu.h"
#include "keys.h"

static Window *s_info_sessions_window;
static MenuLayer *s_info_sessions_menu_layer;
static TextLayer *s_loading_text_layer;
static uint8_t length = 15;
static char *employers, *dates;
static uint16_t employers_index[17], dates_index[17];

static uint16_t info_sessions_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
  return length;
}

static void info_sessions_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
  uint8_t row = cell_index->row;
  
  menu_cell_basic_draw(ctx, cell_layer, &employers[employers_index[row]], &dates[dates_index[row]], NULL);
}

static void info_sessions_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  display_info_session_action_menu(cell_index->row);
}

static void info_sessions_window_load(Window *window) {
  // Get the root layer
  Layer *info_session_window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(info_session_window_layer);
  uint8_t font_height = 24;
  int16_t window_width = bounds.size.w;
  int16_t window_height = bounds.size.h;

  // Create text layer
  s_loading_text_layer = text_layer_create(GRect(0, (window_height / 2) - font_height, window_width, font_height * 2));
  text_layer_set_background_color(s_loading_text_layer, GColorClear);
  text_layer_set_text_alignment(s_loading_text_layer, GTextAlignmentCenter);
  text_layer_set_font(s_loading_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text(s_loading_text_layer, "Loading Info Sessions...");
  layer_add_child(info_session_window_layer, text_layer_get_layer(s_loading_text_layer));
}

static void info_sessions_window_unload(Window *window) {
  // Destroy menu layer
  menu_layer_destroy(s_info_sessions_menu_layer);

  // Clears strings
  strcpy(employers, "");
  strcpy(dates, "");
}

static void info_sessions_menu_load() {
  Layer *info_session_window_layer = window_get_root_layer(s_info_sessions_window);
  GRect bounds = layer_get_bounds(info_session_window_layer);

  // Destroy text layer
  text_layer_destroy(s_loading_text_layer);

  // Create menu layer
  s_info_sessions_menu_layer = menu_layer_create(bounds);
  menu_layer_set_highlight_colors(s_info_sessions_menu_layer, GColorYellow, GColorBlack);
  menu_layer_set_callbacks(s_info_sessions_menu_layer, NULL, (MenuLayerCallbacks) {
    .get_num_rows = info_sessions_menu_get_num_rows_callback,
    .draw_row = info_sessions_menu_draw_row_callback,
    .select_click = info_sessions_menu_select_callback
  });

  // Sets up and down scrolling for the menu layer
  menu_layer_set_click_config_onto_window(s_info_sessions_menu_layer, s_info_sessions_window);

  layer_add_child(info_session_window_layer, menu_layer_get_layer(s_info_sessions_menu_layer));
}

void info_sessions_data_load(DictionaryIterator *iter) {
  Tuple *employer_t, *employer_length_t, *date_t, *date_length_t, *amount_t;

  amount_t = dict_find(iter, IS_AMOUNT);
  length = amount_t->value->uint8;

  employer_t = dict_find(iter, IS_EMPLOYER);
  employer_length_t = dict_find(iter, IS_EMPLOYER_INDEX);
  date_t = dict_find(iter, IS_DATE);
  date_length_t = dict_find(iter, IS_DATE_INDEX);

  employers = employer_t->value->cstring;
  uint8_t *employers_length = employer_length_t->value->data;
  dates = date_t->value->cstring;
  uint8_t *dates_length = date_length_t->value->data;

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "employers_t: %s", employer_t->value->cstring);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "employers: %s", employers);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "dates_t: %s", date_t->value->cstring);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "dates: %s", dates);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "employers length: %d", strlen(employers));
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "dates length: %d", strlen(dates));

  employers_index[0] = 0;
  dates_index[0] = 0;

  for (int i = 1; i <= length; i++) {
    employers_index[i] = employers_index[i - 1] + employers_length[i];
    employers[employers_index[i] - 1] = '\0';

    dates_index[i] = dates_index[i - 1] + dates_length[i];
    dates[dates_index[i] - 1] = '\0';

    //APP_LOG(APP_LOG_LEVEL_DEBUG, "employers_length[i]: %d", employers_length[i]);
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "employers_index[i]: %d", employers_index[i]);
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "dates_length[i]: %d", dates_length[i]);
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "dates_index[i]: %d", dates_index[i]);
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "employer: %s", &employers[employers_index[i - 1]]);
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "date: %s", &dates[dates_index[i - 1]]);
  }

  info_sessions_menu_load();
}

void info_sessions_window_push() {
  // Create Window element and assign to pointer
  s_info_sessions_window = window_create();
  window_set_background_color(s_info_sessions_window, GColorYellow);

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_info_sessions_window, (WindowHandlers) {
    .load = info_sessions_window_load,
    .unload = info_sessions_window_unload
  });

  // Show the info sessions Window on the watch
  window_stack_push(s_info_sessions_window, true);
}