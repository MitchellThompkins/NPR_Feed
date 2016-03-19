#include "pebble.h"
#include "news_list.h"
#include "externs.h"

#define NUM_MENU_SECTIONS 1
#define MAX_NUM_MENU_ITEMS 20 // Max Number of menu items

static Window *s_news_window;
// static TextLayer *s_text_layer;
static SimpleMenuLayer *s_simple_news_menu_layer;
static SimpleMenuSection s_simple_news_menu_sections[NUM_MENU_SECTIONS];
static SimpleMenuItem s_simple_news_menu_items[MAX_NUM_MENU_ITEMS];

static void news_menu_select_callback(int index, void *ctx) {
  s_simple_news_menu_items[index].subtitle = "You've hit select here!";
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_news_menu_layer));
}

static void news_window_load(Window *window) {

  for (int i = 0; i < str_count; i++) {
    s_simple_news_menu_items[i+1] = (SimpleMenuItem) {
      .title = *(str_titles+i),
      .callback = news_menu_select_callback,
    };
  }

  s_simple_news_menu_sections[0] = (SimpleMenuSection) {
    .num_items = MAX_NUM_MENU_ITEMS,
    .items = s_simple_news_menu_items,
  };

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  s_simple_news_menu_layer = simple_menu_layer_create(bounds, window, s_simple_news_menu_sections, NUM_MENU_SECTIONS, NULL);

  layer_add_child(window_layer, simple_menu_layer_get_layer(s_simple_news_menu_layer));
}

// Unload loading window
static void news_window_unload(Window *window) {
  simple_menu_layer_destroy(s_simple_news_menu_layer);
}

// static void copy_strs(int story_count_arg, char **str_titles_arg, char **str_teasers_arg) {
//   str_count = story_count_arg;
//
//   char temp_buff[2];
//   snprintf(temp_buff, 2, "%d", str_count);
//   APP_LOG(APP_LOG_LEVEL_DEBUG, temp_buff);
//
//   // int len = 0; // Declare int len only once
//   str_titles = malloc(str_count * sizeof(int *));
//   for (int i = 0; i < str_count; i++) {
//     int len = strlen(*(str_titles_arg+i));
//     *(str_titles+i) = malloc(len+1);
//     strncpy(*(str_titles_arg+i), *(str_titles+i), strlen(*(str_titles_arg+i)));
//   }
//
//   for (int i = 0; i < str_count; i++) {
//     int len = strlen(*(str_titles_arg + i));
//     char *s_buffer = malloc(len+1);
//     snprintf(s_buffer, len+1, "%s", *(str_titles_arg + i));
//     APP_LOG(APP_LOG_LEVEL_DEBUG, s_buffer);
//     free(s_buffer);
//   }
//
//   for (int i = 0; i < str_count; i++) {
//     int len = strlen(*(str_titles + i));
//     char *s_buffer = malloc(len+1);
//     snprintf(s_buffer, len+1, "%s", *(str_titles + i));
//     APP_LOG(APP_LOG_LEVEL_DEBUG, s_buffer);
//     free(s_buffer);
//   }
//
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "Here1.");
//   str_teasers = malloc(str_count * sizeof(int *));
//   for (int i = 0; i < str_count; i++) {
//     int len = strlen(*(str_teasers_arg+i));
//     APP_LOG(APP_LOG_LEVEL_DEBUG, "Here2.");
//     *(str_teasers+i) = malloc(len+1);
//     strncpy(*(str_teasers_arg+i), *(str_teasers+i), strlen(*(str_teasers_arg+i)));
//   }
//
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "Here3.");
//
//   free(str_titles);
//   free(str_teasers);
// }

void news_list_init(void) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Here1.");
  s_news_window = window_create();

  window_set_window_handlers(s_news_window, (WindowHandlers) {
    .load = news_window_load,
    .unload = news_window_unload,
  });
  window_stack_push(s_news_window, true);
}

void news_list_deinit(void){
  window_destroy(s_news_window);
}
