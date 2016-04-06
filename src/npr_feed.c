#include "pebble.h"
#include "inttypes.h"
#include "strtok.h"
#include "news_list.h"
#include "externs.h"

// Function list
static void in_received_handler(DictionaryIterator *iter, void *context);
static void in_dropped_handler(AppMessageResult reason, void *context);
static void loading_window_load(Window *window);
static void loading_window_unload(Window *window);
static void loading_init(void);
static void loading_deinit(void);

// Windows
static Window *loading_window;

// Layers
static BitmapLayer *image_layer;
static GBitmap *image_bitmap;

// Delcare Variables
int str_count;
char **str_titles;
char **str_teasers;

// Variables for string parsing
const char delim[1] = "|";
char *token;

// Temp Buffer to hold various string data
char *s_buffer;

// Struct to temporarly hold recieved data
typedef struct {
        int s_count;
        char* s_titles;
        char* s_teasers;
} s_newsInfo;
s_newsInfo s_cur_news;

// Keys
typedef enum {
        story_count = 0,
        story_titles,
        story_teasers,
        story_texts,
} newsKeys;

// Initalize Variables
int count_flag = 0;
int titles_flag = 0;
int teasers_flag = 0;

static void in_received_handler(DictionaryIterator *iter, void *context) {
        // TMT Include timeout
        APP_LOG(APP_LOG_LEVEL_DEBUG, "News Info Received.");

        Tuple *count_tuple = dict_find(iter, story_count);
        Tuple *story_titles_tuple = dict_find(iter, story_titles);
        Tuple *story_teasers_tuple = dict_find(iter, story_teasers);

        if(count_tuple) {
                s_cur_news.s_count = count_tuple->value->int32;
                count_flag = 1;
        }

        if(story_titles_tuple) {
                s_cur_news.s_titles = story_titles_tuple->value->cstring;
                titles_flag = 1;
        }

        if(story_teasers_tuple) {
                s_cur_news.s_teasers = story_teasers_tuple->value->cstring;
                teasers_flag = 1;
        }

        // If all data has been recieved
        if (count_flag && titles_flag && teasers_flag) {
                // Two Char arrays to store the string data
                str_count = s_cur_news.s_count;
                str_titles = malloc(s_cur_news.s_count * sizeof(char*));
                str_teasers = malloc(s_cur_news.s_count * sizeof(char*));

                // This creates a copy of the entire string s_titles into s_buffer
                int len = strlen(s_cur_news.s_titles) + 1;
                s_buffer = (char *)malloc(len);
                strcpy(s_buffer, s_cur_news.s_titles);

                token = strtok(s_buffer, delim); // Get the first token for the titles
                // Walk through the other tokens. Check to ensure str_count is not greater than counter (should be impossible)
                int counter = 0;
                while(token != NULL && counter < str_count) {
                        *(str_titles + counter) = malloc((strlen(token) + 1) * sizeof(char));
                        strcpy(*(str_titles + counter), token);
                        token = strtok(NULL, delim);
                        counter++;
                }

                // This creates a copy of the entire string s_teasers into s_buffer
                len = strlen(s_cur_news.s_teasers) + 1;
                s_buffer = (char *)realloc(s_buffer, len);
                strcpy(s_buffer, s_cur_news.s_teasers);

                token = strtok(s_buffer, delim); // Get the first token for the teasers
                // Walk through the other tokens
                counter = 0;
                while(token != NULL && counter < str_count) {
                        *(str_teasers + counter) = malloc((strlen(token) + 1) * sizeof(char));
                        strcpy(*(str_teasers + counter), token);
                        token = strtok(NULL, delim);
                        counter++;
                }
                free(s_buffer);

                news_list_window_init(loading_window);
        }
}

// Called if failed to properly recieve data
static void in_dropped_handler(AppMessageResult reason, void *context) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Dropped!");
}

// Load loading window
static void loading_window_load(Window *window) {
        Layer *window_layer = window_get_root_layer(window);
        GRect bounds = layer_get_frame(window_layer);

        image_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_LOADING);

        image_layer = bitmap_layer_create(bounds);
        bitmap_layer_set_bitmap(image_layer, image_bitmap);
        bitmap_layer_set_alignment(image_layer, GAlignCenter);
        layer_add_child(window_layer, bitmap_layer_get_layer(image_layer));
}

// Unload loading window
static void loading_window_unload(Window *window) {
        bitmap_layer_destroy(image_layer);
        gbitmap_destroy(image_bitmap);
}

// loading Init
static void loading_init(void) {
        // Register message handlers
        app_message_register_inbox_received(in_received_handler);
        app_message_register_inbox_dropped(in_dropped_handler);

        // Open message app
        app_message_open(3024,3024);

        loading_window = window_create();
        window_set_window_handlers(loading_window, (WindowHandlers) {
                .load = loading_window_load,
                .unload = loading_window_unload,
        });

        window_stack_push(loading_window, true);
}

// Loading Window Deinit
static void loading_deinit(void) {
        window_destroy(loading_window);
}

// Main function
int main(void) {
        loading_init();
        app_event_loop();
        loading_deinit();
}
