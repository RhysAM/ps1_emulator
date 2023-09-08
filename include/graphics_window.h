#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define WINDOW_WIDTH 700
#define WINDOW_HEIGHT (WINDOW_WIDTH)
#define BOTTOM_PADDING 25

typedef struct TextBox {
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Rect* rect_ptr;
    char* text;
    TTF_Font* font;
} TextBox;

TextBox* create_textbox(SDL_Renderer* renderer, int x, int y, char *text, TTF_Font *font);
void destroy_textbox(TextBox* textbox);
void update_textbox_text(TextBox* text_box, char* new_text);
void render_textbox(TextBox* text_box);
void get_text_and_rect(SDL_Renderer *renderer, int x, int y, char *text, TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect);
bool check_for_button_click(SDL_Event* e, TextBox* rect);
