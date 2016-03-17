#include "pebble.h"
#include "strtok.h"
// #include "news_list.h"

static Window *s_loading_window;
static BitmapLayer *s_image_layer;
static GBitmap *s_image_bitmap;

const char delim[1] = "|";
char *token;

char *s_buffer;

typedef struct {
  int s_count;
  char* s_titles;
  char* s_teasers;
} newsInfo;
newsInfo cur_news;

// Keys
typedef enum {
   story_count = 0,
   story_titles,
   story_teasers,
   story_texts,
} newsKeys;

// Load loading window
static void loading_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  s_image_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_LOADING);

  s_image_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(s_image_layer, s_image_bitmap);
  bitmap_layer_set_alignment(s_image_layer, GAlignCenter);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_image_layer));
}

// Unload loading window
static void loading_window_unload(Window *window) {
  bitmap_layer_destroy(s_image_layer);
  gbitmap_destroy(s_image_bitmap);
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "News Info Received.");

  Tuple *count_tuple = dict_find(iter, story_count);
  Tuple *story_titles_tuple = dict_find(iter, story_titles);
  Tuple *story_teasers_tuple = dict_find(iter, story_teasers);

  int count_flag = 0;
  int titles_flag = 0;
  int teasers_flag = 0;

  if(count_tuple) {
    cur_news.s_count = count_tuple->value->int32;
    count_flag = 1;
    // TMT Temp
    char s_buffer[4];
    snprintf(s_buffer, sizeof(s_buffer), "%d", cur_news.s_count);
    APP_LOG(APP_LOG_LEVEL_DEBUG, s_buffer);
  }

  if(story_titles_tuple) {
    cur_news.s_titles = story_titles_tuple->value->cstring;
    titles_flag = 1;
  }

  if(story_teasers_tuple) {
    cur_news.s_teasers = story_teasers_tuple->value->cstring;
    teasers_flag = 1;
  }

  // If all data has been recieved
  if (count_flag && titles_flag && teasers_flag) {
    // Two Char arrays to store the string data
    char *str_titles[cur_news.s_count];
    char *str_teasers[cur_news.s_count];

    // This creates a copy of the entire string s_titles into s_buffer
    int len = strlen(cur_news.s_titles);
    s_buffer = (char *)malloc(len+1);
    strcpy(s_buffer, cur_news.s_titles);

    token = strtok(s_buffer, delim); // Get the first token for the titles
    // Walk through the other tokens
    int counter = 0;
    while(token != NULL) {
       *(str_titles + counter) = token;
       token = strtok(NULL, delim);
       counter++;
    }

    // This creates a copy of the entire string s_teasers into s_buffer
    len = strlen(cur_news.s_teasers);
    s_buffer = (char *)realloc(s_buffer, len+1);
    snprintf(s_buffer, len, "%s", cur_news.s_teasers);
    strcpy(s_buffer, cur_news.str_teasers);

    token = strtok(s_buffer, delim); // Get the first token for the teasers
    // Walk through the other tokens
    counter = 0;
    while(token != NULL) {
       *(str_teasers + counter) = token;
       token = strtok(NULL, delim);
       counter++;
    }
    free(s_buffer);

    // TMT This is just to visualize the data
    // for (int i = 0; i < cur_news.s_count; i++) {
    //   int len = strlen(*(str_titles + i));
    //   char *s_buffer = malloc(len+1);
    //   snprintf(s_buffer, len+1, "%s", *(str_titles + i));
    //   APP_LOG(APP_LOG_LEVEL_DEBUG, s_buffer);
    //   free(s_buffer);
    // }
    //
    // for (int i = 0; i < cur_news.s_count; i++) {
    //   int len = strlen(*(str_teasers + i));
    //   char *s_buffer = malloc(len+1);
    //   snprintf(s_buffer, len+1, "%s", *(str_teasers + i));
    //   APP_LOG(APP_LOG_LEVEL_DEBUG, s_buffer);
    //   free(s_buffer);
    // }

    // TMT here call news_list_init and pass it the two char arrays from above
  }

}

static void in_dropped_handler(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Dropped!");
}

// Init
static void init(void) {
  // Register message handlers
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);

  // app_message_open(64, 64); // TMT may need to increase size
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  s_loading_window = window_create();
  window_set_window_handlers(s_loading_window, (WindowHandlers) {
    .load = loading_window_load,
    .unload = loading_window_unload,
  });

  window_stack_push(s_loading_window, true);
}

// Deinit
static void deinit() {
  window_destroy(s_loading_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
