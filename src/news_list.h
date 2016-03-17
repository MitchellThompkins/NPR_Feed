#ifndef NEWS_LIST_H
#define NEWS_LIST_H
// Function list
void news_window_load(Window);
void news_list_init((int story_count, char *str_titles, char *str_teasers));
void news_list_deinit(void);

#endif
