#include "stdlib.h"
#include "bios.h"
#include "cpu.h"
#include "helper.h"
#include "graphics_window.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

int main(int argc, char** argv)
{
    Bios bios;
    initiate_bios(&bios);
    MemoryMapper memory_mapper = {.bios = bios};
    CPU cpu = {.registers = {0}, .memory_mapper = memory_mapper}; 
    initiate_cpu(&cpu);
    // dump_array(&bios, BIOS_SIZE);
    TextBox* buttons[2];
    TextBox* register_display_rects = malloc(sizeof(TextBox) * REGISTER_COUNT);

    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Texture *texture1, *texture2;
    SDL_Window *window;
    const char *font_path = "fonts/arial.ttf";
    bool quit;

    /* Inint TTF. */
    SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_WIDTH, 0, &window, &renderer);
    TTF_Init();
    TTF_Font *font = TTF_OpenFont(font_path, 24);
    if (font == NULL) {
        fprintf(stderr, "error: font not found\n");
        exit(EXIT_FAILURE);
    }

    buttons[0] = create_textbox(renderer, 239, WINDOW_HEIGHT - BOTTOM_PADDING, "Step CPU", font);
    buttons[1] = create_textbox(renderer, 239 + buttons[0]->rect_ptr->w + 10, WINDOW_HEIGHT - BOTTOM_PADDING, "Stop CPU", font);
    
    quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&event) == 1) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }   
            if (check_for_button_click(&event, buttons[0]))
            {
                printf("Clicked Step CPU\n");
                update_textbox_text(buttons[0], "Test CPU");
            }
            if (check_for_button_click(&event, buttons[1]))
            {
                printf("Clicked Stop\n");
            }
        }
        SDL_SetRenderDrawColor(renderer, 0x80, 0x80, 0x80, 0x80);
        SDL_RenderClear(renderer);

        /* Use TTF textures. */
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        for (int i = 0; i < 2; i++)
        {        
            SDL_RenderFillRect(renderer, buttons[i]->rect_ptr);
            render_textbox(buttons[i]);
        }
        

        SDL_RenderPresent(renderer);

        // Run PS1 CPU
        // read_and_execute(&cpu);
    }

    free(register_display_rects);

    for (int i = 0; i< 2; i++)
    {
        destroy_textbox(buttons[i]);
    }

    /* Deinit TTF. */
    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return EXIT_SUCCESS;

}