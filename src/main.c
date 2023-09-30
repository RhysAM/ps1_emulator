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
    RAM* ram = malloc(sizeof(RAM));
    memset(ram->data, 0xdeadbeef, MAIN_RAM_SIZE);
    initiate_bios(&bios);
    MemoryMapper memory_mapper = {.bios = bios, .bad_state = false, .ram = ram};
    Coprocessor COP0 = {.registers = {0}, .halted = false};
    CPU cpu = {.registers = {0}, .memory_mapper = &memory_mapper, .COP0 = &COP0}; 
    initiate_cpu(&cpu);
    if (argc > 1)
    {
        // dump_array(&bios, BIOS_SIZE);
        TextBox* buttons[2];
        TextBox** register_textbox_arr = malloc(sizeof(TextBox*) * REGISTER_COUNT);
        SDL_Color black = {0,0,0,0};
        SDL_Color gray = {0x80,0x80,0x80,0x80};
        SDL_Color white = {0xff, 0xff, 0xff, 0xff};

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
        TTF_Font *big_font = TTF_OpenFont(font_path, 24);
        TTF_Font *small_font = TTF_OpenFont(font_path, 14);

        if ((big_font == NULL) || (small_font == NULL)) {
            fprintf(stderr, "error: font not found\n");
            exit(EXIT_FAILURE);
        }

        create_register_display_buttons(register_textbox_arr, renderer, small_font, white);
        char* button_0_text = malloc(256);
        char* button_1_text = malloc(256);
        strcpy(button_0_text, "Step CPU");
        strcpy(button_1_text, "Stop CPU");

        buttons[0] = create_textbox(renderer, 239, WINDOW_HEIGHT - BOTTOM_PADDING, button_0_text, big_font, white);
        buttons[1] = create_textbox(renderer, 239 + buttons[0]->rect_ptr->w + 10, WINDOW_HEIGHT - BOTTOM_PADDING, button_1_text, big_font, white);

        quit = 0;
        while (!quit) {
            while (SDL_PollEvent(&event) == 1) {
                if (event.type == SDL_QUIT) {
                    quit = true;
                }   
                if (check_for_button_click(&event, buttons[0]))
                {
                    read_and_execute(&cpu);
                }
                if (check_for_button_click(&event, buttons[1]))
                {
                }
            }

            update_register_buttons(register_textbox_arr, cpu.registers);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);

            /* Use TTF textures. */
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            for (int i = 0; i < 2; i++)
            {        
                render_textbox(buttons[i]);
            }

            for (int i = 0; i < REGISTER_COUNT; i++)
            {
                render_textbox(register_textbox_arr[i]);
            }
            

            SDL_RenderPresent(renderer);

            // Run PS1 CPU
        }
        
        for (int i = 0; i < REGISTER_COUNT; i++)
        {
            destroy_textbox(register_textbox_arr[i]);
        }

        free(register_textbox_arr);

        for (int i = 0; i< 2; i++)
        {
            destroy_textbox(buttons[i]);
        }

        /* Deinit TTF. */
        TTF_Quit();

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    else
    {
        while(!(cpu.halted) && !(memory_mapper.bad_state))
        {
            read_and_execute(&cpu);
            printf("Memory mapper state: %d\n", memory_mapper.bad_state);
            printf("CPU halted: %d\n", memory_mapper.bad_state);
            // printf("Value at bios: %08x\n", load_word((cpu.memory_mapper), 0xbfc0683c));
            // printf("Value at bios: %08x\n", load_word((cpu.memory_mapper), 0xbfc06840));
            // cpu.halted = true;
        }
    }
    free(ram);
    return EXIT_SUCCESS;
}