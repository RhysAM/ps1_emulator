#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#ifndef GRAPHICS_WINDOW_H
#define GRAPHICS_WINDOW_H

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT (WINDOW_WIDTH)
#define BOTTOM_PADDING 25
#define REGISTER_CHAR_BUFFER 15
#define REGISTER_EDGE 10
#define REGISTER_HEIGHT_START 10
#define REGISTER_WIDTH_SPACING 100
#define REGISTER_HEIGHT_SPACING 25

# define REGISTER_START 
typedef struct TextBox {
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Rect* rect_ptr;
    char* text;
    TTF_Font* font;
    SDL_Color text_color;
} TextBox;

typedef struct MainWindow {
    
} MainWindow;
TextBox* create_textbox(SDL_Renderer* renderer, int x, int y, char *text, TTF_Font *font, SDL_Color color);
void destroy_textbox(TextBox* textbox);
void update_textbox_text(TextBox* text_box, char* new_text);
void render_textbox(TextBox* text_box);
void get_text_and_rect(SDL_Renderer *renderer, int x, int y, char *text, TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect, SDL_Color color);
bool check_for_button_click(SDL_Event* e, TextBox* rect);
void create_register_display_buttons(TextBox** register_button_arr, SDL_Renderer* renderer, TTF_Font *font, SDL_Color color);
void update_register_buttons(TextBox** register_buttons, uint32_t* register_arr);
void create_window();
void cleanup();

#endif