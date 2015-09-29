#include "action-menu.h"
#include "notice-window.h"
#include "keys.h"

#define ROOT_LEVEL_ITEMS 1

typedef enum {
  AddPin,
  SubscribeLunch,
  UnsubscribeLunch,
  SubscribeDinner,
  UnsubscribeDinner
} ActionType;

static ActionMenu *s_action_menu;
static ActionMenuLevel *s_root_level;
static uint8_t s_action_type;
static uint8_t s_selected_row;

static void action_performed_callback (ActionMenu *action_menu, const ActionMenuItem *action, void *context) {
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "selected_row: %d", s_selected_row);
  DictionaryIterator *iter;

	switch(s_action_type) {
    case AddPin:
      // Add pin to timeline
      APP_LOG(APP_LOG_LEVEL_DEBUG, "add pin selected");
    
	    // Send AppMessage to get info sessions
	    app_message_outbox_begin(&iter);
      dict_write_uint8(iter, KEY_ADD_PIN, INFO_SESSION);
      dict_write_uint8(iter, KEY_PIN_DATA, s_selected_row);
	    dict_write_end(iter);
	    app_message_outbox_send();

      notice_window_push(s_action_type);

      break;
    case SubscribeLunch:
      // Subscribe to lunch menu topic
      APP_LOG(APP_LOG_LEVEL_DEBUG, "subscribe lunch selected");
    
      // Send AppMessage to subscribe
      app_message_outbox_begin(&iter);
      dict_write_uint8(iter, KEY_SUBSCRIBE_TOPIC, LUNCH_MENU);
      dict_write_end(iter);
      app_message_outbox_send();

      notice_window_push(s_action_type);
      persist_write_int(LUNCH_SUBSCRIBED, 1);

      break;
    case UnsubscribeLunch:
      // Unsubscribe to lunch menu topic
      APP_LOG(APP_LOG_LEVEL_DEBUG, "unsubscribe lunch selected");
    
      // Send AppMessage to unsubscribe
      app_message_outbox_begin(&iter);
      dict_write_uint8(iter, KEY_UNSUBSCRIBE_TOPIC, LUNCH_MENU);
      dict_write_end(iter);
      app_message_outbox_send();

      notice_window_push(s_action_type);
      persist_write_int(LUNCH_SUBSCRIBED, 0);

      break;
    case SubscribeDinner:
      // Subscribe to dinner menu topic
      APP_LOG(APP_LOG_LEVEL_DEBUG, "subscribe dinner selected");
    
      // Send AppMessage to subscribe
      app_message_outbox_begin(&iter);
      dict_write_uint8(iter, KEY_SUBSCRIBE_TOPIC, DINNER_MENU);
      dict_write_end(iter);
      app_message_outbox_send();

      notice_window_push(s_action_type);
      persist_write_int(DINNER_SUBSCRIBED, 1);

      break;
    case UnsubscribeDinner:
      // Unsubscribe to dinner menu topic
      APP_LOG(APP_LOG_LEVEL_DEBUG, "unsubscribe dinner selected");
    
      // Send AppMessage to unsubscribe
      app_message_outbox_begin(&iter);
      dict_write_uint8(iter, KEY_UNSUBSCRIBE_TOPIC, DINNER_MENU);
      dict_write_end(iter);
      app_message_outbox_send();

      notice_window_push(s_action_type);
      persist_write_int(DINNER_SUBSCRIBED, 0);

      break;
	}
}

void display_info_session_action_menu (uint8_t row) {
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

void display_food_menus_action_menu (uint8_t menu_type) {
  // Create root level of the action menu
  s_root_level = action_menu_level_create(ROOT_LEVEL_ITEMS);

  s_action_type = AddPin;

  if (menu_type == LUNCH_MENU && persist_exists(LUNCH_SUBSCRIBED)) {
    if (persist_read_int(LUNCH_SUBSCRIBED) == 0) {
      // User not subscribed
      s_action_type = SubscribeLunch;

      // Subscribe to topic action
      action_menu_level_add_action(s_root_level, "Subscribe to Pins", action_performed_callback, NULL);
    }
    else {
      // User already subscribed
      s_action_type = UnsubscribeLunch;

      // Unsubscribe to topic action
      action_menu_level_add_action(s_root_level, "Unsubscribe to Pins", action_performed_callback, NULL);
    }
  }
  else if (menu_type == LUNCH_MENU) {
    // User not subscribed
    s_action_type = SubscribeLunch;

    // Subscribe to topic action
    action_menu_level_add_action(s_root_level, "Subscribe to Pins", action_performed_callback, NULL);
  }
  else if (persist_exists(DINNER_SUBSCRIBED)) {
    if (persist_read_int(DINNER_SUBSCRIBED) == 0) {
      // User not subscribed
      s_action_type = SubscribeDinner;

      // Subscribe to topic action
      action_menu_level_add_action(s_root_level, "Subscribe to Pins", action_performed_callback, NULL);
    }
    else {
      // User already subscribed
      s_action_type = UnsubscribeDinner;

      // Unsubscribe to topic action
      action_menu_level_add_action(s_root_level, "Unsubscribe to Pins", action_performed_callback, NULL);
    }
  }
  else {
    // User not subscribed
    s_action_type = SubscribeDinner;

    // Subscribe to topic action
    action_menu_level_add_action(s_root_level, "Subscribe to Pins", action_performed_callback, NULL);
  }

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