#include <stdlib.h>
#include <stdbool.h>
#include "graphics_window.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/*
- x, y: upper left corner.
- texture, rect: outputs.
*/
void get_text_and_rect(SDL_Renderer *renderer, int x, int y, char *text,
        TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect) {
    int text_width;
    int text_height;
    SDL_Surface *surface;
    SDL_Color textColor = {0, 0, 0, 0};

    surface = TTF_RenderText_Solid(font, text, textColor);
    *texture = SDL_CreateTextureFromSurface(renderer, surface);
    text_width = surface->w;
    text_height = surface->h;
    SDL_FreeSurface(surface);
    rect->x = x;
    rect->y = y;
    rect->w = text_width;
    rect->h = text_height;
}

TextBox* create_textbox(SDL_Renderer* renderer, int x, int y, char *text, TTF_Font *font)
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
    get_text_and_rect(renderer, x, y, text, font, &texture, rect_ptr);
    TextBox box = {.renderer = renderer, .texture = texture, .rect_ptr = rect_ptr, .text = text, .font=font};
    *text_box = box;
    printf("Rect: (%i, %i)\n", rect_ptr->x, rect_ptr->y);
    return text_box;
}

void destroy_textbox(TextBox* text_box)
{
    SDL_DestroyTexture(text_box->texture);
    free(text_box->rect_ptr);
    free(text_box);
}

void update_textbox_text(TextBox* text_box, char* new_text)
{
    SDL_DestroyTexture(text_box->texture);
    get_text_and_rect(text_box->renderer, text_box->rect_ptr->x, text_box->rect_ptr->y, new_text, text_box->font, &text_box->texture, text_box->rect_ptr);
    text_box->text = new_text;
}

void render_textbox(TextBox* text_box)
{
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