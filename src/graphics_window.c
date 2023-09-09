#include <stdlib.h>
#include <stdbool.h>
#include "graphics_window.h"
#include "cpu.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/*
- x, y: upper left corner.
- texture, rect: outputs.
*/
void get_text_and_rect(SDL_Renderer *renderer, int x, int y, char *text,
        TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect, SDL_Color text_color) {
    int text_width;
    int text_height;
    SDL_Surface *surface;

    surface = TTF_RenderUTF8_Blended(font, text, text_color);
    *texture = SDL_CreateTextureFromSurface(renderer, surface);
    text_width = surface->w;
    text_height = surface->h;
    SDL_FreeSurface(surface);
    rect->x = x;
    rect->y = y;
    rect->w = text_width;
    rect->h = text_height;
}

TextBox* create_textbox(SDL_Renderer* renderer, int x, int y, char *text, TTF_Font *font, SDL_Color text_color)
{
    SDL_Texture* texture;
    SDL_Rect new_rect = {0};
    TextBox* text_box = malloc(sizeof(TextBox));
    SDL_Rect* rect_ptr = malloc(sizeof(SDL_Rect));
    *rect_ptr = new_rect;
    if ((text_box == NULL) || (rect_ptr == NULL))
    {
        printf("failed to create textbox.\n");
        exit(1);
    }
    get_text_and_rect(renderer, x, y, text, font, &texture, rect_ptr, text_color);
    TextBox box = {.renderer = renderer, .texture = texture, .rect_ptr = rect_ptr, .text = text, .font = font, .text_color = text_color};
    *text_box = box;
    return text_box;
}

void destroy_textbox(TextBox* text_box)
{
    SDL_DestroyTexture(text_box->texture);
    free(text_box->rect_ptr);
    free(text_box->text);
    free(text_box);
}

void render_textbox(TextBox* text_box)
{
    SDL_RenderFillRect(text_box->renderer, text_box->rect_ptr);
    get_text_and_rect(text_box->renderer, text_box->rect_ptr->x, text_box->rect_ptr->y, text_box->text, text_box->font, &text_box->texture, text_box->rect_ptr, text_box->text_color);
    SDL_RenderCopy(text_box->renderer, text_box->texture, NULL, text_box->rect_ptr);
}

bool check_for_button_click(SDL_Event* e, TextBox* box)
{
    SDL_Rect* rect = box->rect_ptr;
    if (e->type != SDL_MOUSEBUTTONDOWN)
    {
        return false;
    }
    int x, y;
    SDL_GetMouseState(&x, &y);
     //Check if mouse is in button
    bool inside = true;

    //Mouse is left of the button
    if( x < rect->x )
    {
        inside = false;
    }
    //Mouse is right of the button
    else if( x > rect->x + rect->w )
    {
        inside = false;
    }
    //Mouse above the button
    else if( y < rect->y )
    {
        inside = false;
    }
    //Mouse below the button
    else if( y > rect->y + rect->h )
    {
        inside = false;
    }
    return inside;
}

void update_register_buttons(TextBox** register_buttons, uint32_t* register_arr)
{
    for (int i = 0; i < REGISTER_COUNT; i++)
    {
        snprintf(register_buttons[i]->text, REGISTER_CHAR_BUFFER, "%02d: %08x", i, register_arr[i]);
    }
}

void create_register_display_buttons(TextBox** register_button_arr, SDL_Renderer* renderer, TTF_Font *font, SDL_Color color)
{
    for (int i = 0; i < REGISTER_COUNT; i++)
    {
        char* text_buffer = malloc(REGISTER_CHAR_BUFFER);
        snprintf(text_buffer, REGISTER_CHAR_BUFFER, "%02d: 0x0000000", i);
        register_button_arr[i] = create_textbox(renderer, REGISTER_START + (i%8) * REGISTER_WIDTH_SPACING, REGISTER_HEIGHT_START + (i/8) * REGISTER_HEIGHT_SPACING, text_buffer, font, color);
    }
}