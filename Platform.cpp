#include <spdlog/spdlog.h>

#include "Platform.h"

Platform::Platform(char const *title, int windowWidth, int windowHeight, int textureWidth, int textureHeight)
{
    spdlog::info(
        "Initializing SDL platform: window={}x{}, texture={}x{}",
        windowWidth,
        windowHeight,
        textureWidth,
        textureHeight);

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        spdlog::error("SDL video initialization failed: {}", SDL_GetError());
        return;
    }

    spdlog::info("SDL video subsystem initialized");

    if (!SDL_CreateWindowAndRenderer(
        title,
        windowWidth,
        windowHeight,
        0,
        &window,
        &renderer))
    {
        spdlog::error("Failed to create SDL window and renderer: {}", SDL_GetError());
        return;
    }

    spdlog::info("Created SDL window and renderer");

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        textureWidth,
        textureHeight);

    if (texture == nullptr)
    {
        spdlog::error("Failed to create SDL texture: {}", SDL_GetError());
        return;
    }

    if (!SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST))
    {
        spdlog::error("Failed to configure nearest-neighbor texture scaling: {}", SDL_GetError());
        return;
    }

    spdlog::info("Created streaming texture with nearest-neighbor scaling");
}

Platform::~Platform()
{
    spdlog::info("Shutting down SDL platform");
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Platform::Update(void *buffer, int pitch)
{
    if (!SDL_UpdateTexture(texture, nullptr, buffer, pitch))
    {
        spdlog::error("Failed to update SDL texture: {}", SDL_GetError());
        return;
    }

    if (!SDL_RenderClear(renderer))
    {
        spdlog::error("Failed to clear SDL renderer: {}", SDL_GetError());
        return;
    }

    if (!SDL_RenderTexture(renderer, texture, nullptr, nullptr))
    {
        spdlog::error("Failed to render SDL texture: {}", SDL_GetError());
        return;
    }

    SDL_RenderPresent(renderer);
}

bool Platform::ProcessInput(uint8_t *keys)
{
    bool quit = false;
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
        {
            spdlog::info("Received quit event");
            quit = true;
        }
        break;

        case SDL_EVENT_KEY_DOWN:
        {
            SDL_Keycode key = event.key.key;
            spdlog::info("Key pressed: {}", SDL_GetKeyName(key));
            switch (key)
            {
            case SDLK_X:
            {
                keys[0x0] = 1;
            }
            break;
            case SDLK_1:
            {
                keys[0x1] = 1;
            }
            break;
            case SDLK_2:
            {
                keys[0x2] = 1;
            }
            break;
            case SDLK_3:
            {
                keys[0x3] = 1;
            }
            break;
            case SDLK_Q:
            {
                keys[0x4] = 1;
            }
            break;
            case SDLK_W:
            {
                keys[0x5] = 1;
            }
            break;
            case SDLK_E:
            {
                keys[0x6] = 1;
            }
            break;
            case SDLK_A:
            {
                keys[0x7] = 1;
            }
            break;
            case SDLK_S:
            {
                keys[0x8] = 1;
            }
            break;
            case SDLK_D:
            {
                keys[0x9] = 1;
            }
            break;
            case SDLK_Z:
            {
                keys[0xA] = 1;
            }
            break;
            case SDLK_C:
            {
                keys[0xB] = 1;
            }
            break;
            case SDLK_4:
            {
                keys[0xC] = 1;
            }
            break;
            case SDLK_R:
            {
                keys[0xD] = 1;
            }
            break;
            case SDLK_F:
            {
                keys[0xE] = 1;
            }
            break;
            case SDLK_V:
            {
                keys[0xF] = 1;
            }
            break;

            default:
                break;
            }
        }
        break;

        case SDL_EVENT_KEY_UP:
        {
            SDL_Keycode key = event.key.key;
            spdlog::info("Key released: {}", SDL_GetKeyName(key));
            switch (key)
            {
            case SDLK_X:
            {
                keys[0x0] = 0;
            }
            break;
            case SDLK_1:
            {
                keys[0x1] = 0;
            }
            break;
            case SDLK_2:
            {
                keys[0x2] = 0;
            }
            break;
            case SDLK_3:
            {
                keys[0x3] = 0;
            }
            break;
            case SDLK_Q:
            {
                keys[0x4] = 0;
            }
            break;
            case SDLK_W:
            {
                keys[0x5] = 0;
            }
            break;
            case SDLK_E:
            {
                keys[0x6] = 0;
            }
            break;
            case SDLK_A:
            {
                keys[0x7] = 0;
            }
            break;
            case SDLK_S:
            {
                keys[0x8] = 0;
            }
            break;
            case SDLK_D:
            {
                keys[0x9] = 0;
            }
            break;
            case SDLK_Z:
            {
                keys[0xA] = 0;
            }
            break;
            case SDLK_C:
            {
                keys[0xB] = 0;
            }
            break;
            case SDLK_4:
            {
                keys[0xC] = 0;
            }
            break;
            case SDLK_R:
            {
                keys[0xD] = 0;
            }
            break;
            case SDLK_F:
            {
                keys[0xE] = 0;
            }
            break;
            case SDLK_V:
            {
                keys[0xF] = 0;
            }
            break;

            default:
                break;
            }
        }
        break;

        default:
            break;
        }
    }

    return quit;
}
