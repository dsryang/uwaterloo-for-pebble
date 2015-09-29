#include "action-menu.h"
#include "notice-window.h"
#include "keys.h"

#define ROOT_LEVEL_ITEMS 1

typedef enum {
  AddPin,
  RemovePin
} ActionType;

static ActionMenu *s_action_menu;
static ActionMenuLevel *s_root_level;
static uint8_t s_action_type;
static uint8_t s_selected_row;

static void action_performed_callback (ActionMenu *action_menu, const ActionMenuItem *action, void *context) {
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "selected_row: %d", s_selected_row);

	switch(s_action_type) {
    case AddPin:
      // Add pin to timeline
      APP_LOG(APP_LOG_LEVEL_DEBUG, "add pin selected");
    
	    // Send AppMessage to get info sessions
	    DictionaryIterator *iter;
	    app_message_outbox_begin(&iter);
      dict_write_uint8(iter, KEY_ADD_PIN, INFO_SESSION);
      dict_write_uint8(iter, KEY_PIN_DATA, s_selected_row);
	    dict_write_end(iter);
	    app_message_outbox_send();

      notice_window_push(s_action_type);

      break;

    case RemovePin:
      // Remove pin to timeline
      APP_LOG(APP_LOG_LEVEL_DEBUG, "remove pin selected");

      break;
	}
}

void display_action_menu (uint8_t row) {
  // Create root level of the action menu
  s_root_level = action_menu_level_create(ROOT_LEVEL_ITEMS);

  s_action_type = AddPin;
  s_selected_row = row;

  // Add pin action
  action_menu_level_add_action(s_root_level, "Add Pin", action_performed_callback, NULL);

	// Configure the action menu
	ActionMenuConfig config = (ActionMenuConfig) {
		.root_level = s_root_level,
		.colors = {
			.background = GColorYellow,
			.foreground = GColorBlack
		},
		.align = ActionMenuAlignCenter
	};

	s_action_menu = action_menu_open(&config);
}