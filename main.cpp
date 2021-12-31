#include <iostream>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include <string>
#include <fstream>

#ifdef __unix__
#ifndef SCORE_FILE_ADDR
#define SCORES_FILE_ADDR "./scores"
#ifndef __GFX__
#include <SDL2/SDL2_gfxPrimitives.h>
#define __GFX__
#endif
#endif
#endif

#ifdef __linux__
#ifndef SCORE_FILE_ADDR
#define SCORES_FILE_ADDR "./scores"
#ifndef __GFX__
#include <SDL2/SDL2_gfxPrimitives.h>
#define __GFX__
#endif
#endif
#endif

#define FONT_ADDR "./arial.ttf"
#define UME_SCORE_FONT_SIZE 35
#define UME_BTNS_FONT_SIZE 16
#define END_MENU_BTN_CNT 3
#define WINDOW_WIDTH 1080
#define WIDNOW_HEIGHT 720

#define DELAY 50

using namespace std;

//----==== OK! Tested ====-----
SDL_Rect render_text_center(SDL_Renderer *renderer, const char *text, SDL_Point *center_point, TTF_Font *font = NULL, SDL_Color color = {0, 0, 0, 255})
{
    string t = text;
    SDL_Texture *pervious_target = SDL_GetRenderTarget(renderer);
    if (font == NULL)
        font = TTF_OpenFont("./arial.ttf", 20);
    SDL_Surface *surf = TTF_RenderText_Blended(font, text, color);
    SDL_Texture *txt = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_Rect result = {center_point->x - surf->w / 2, center_point->y - surf->h / 2, surf->w, surf->h};

    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, txt, NULL, &result);

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(txt);
    SDL_SetRenderTarget(renderer, pervious_target);

    return result;
}

SDL_Rect render_text_right(SDL_Renderer *renderer, const char *text, SDL_Point *top_right_corner, TTF_Font *font = TTF_OpenFont(FONT_ADDR, 20), SDL_Color color = {0, 0, 0, 255})
{
    SDL_Texture *pervious_target = SDL_GetRenderTarget(renderer);
    SDL_Surface *surf = TTF_RenderText_Blended(font, text, color);
    SDL_Texture *txt = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_Rect result = {top_right_corner->x - surf->w, top_right_corner->y, surf->w, surf->h};

    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, txt, NULL, &result);

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(txt);
    SDL_SetRenderTarget(renderer, pervious_target);

    return result;
}

SDL_Rect render_text_left(SDL_Renderer *renderer, const char *text, SDL_Point *top_left_corner, TTF_Font *font = TTF_OpenFont(FONT_ADDR, 20), SDL_Color color = {0, 0, 0, 255})
{
    SDL_Texture *pervious_target = SDL_GetRenderTarget(renderer);
    SDL_Surface *surf = TTF_RenderText_Blended(font, text, color);
    SDL_Texture *txt = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_Rect result = {top_left_corner->x, top_left_corner->y, surf->w, surf->h};

    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, txt, NULL, &result);

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(txt);
    SDL_SetRenderTarget(renderer, pervious_target);

    return result;
}

typedef enum States
{
    STATE_START_MENU,
    STATE_PAUSE_MENU,
    STATE_END_MENU,
    STATE_GAMING,
    STATE_QUIT
} States;
States Game_State = STATE_END_MENU;

typedef struct Text
{
    std::string text = "";
    SDL_Color color = {0, 0, 0, 255};
    int ptsize = 20;
    std::string font_addr = FONT_ADDR;
    TTF_Font *font = TTF_OpenFont(font_addr.c_str(), ptsize);

    void update_font()
    {
        font = TTF_OpenFont(font_addr.c_str(), ptsize);
    }

    SDL_Rect render(SDL_Renderer *renderer, SDL_Point *top_center_coordinates)
    {
        return render_text_center(renderer, text.c_str(), top_center_coordinates, font, color);
    }

} Text;

struct Button
{
private:
    SDL_Rect bounds;
    Text text;
    SDL_Texture *back_texture = NULL;

    void init_text(){
        text.text = " ";
    }

    
public:
    Button(SDL_Renderer *p_renderer, SDL_Surface *back, SDL_Rect bounds)
    {
        init_text();
        this->bounds.x = bounds.x;
        this->bounds.y = bounds.y;
        this->bounds.w = bounds.w;
        this->bounds.h = bounds.h;
        back_texture = SDL_CreateTexture(p_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, bounds.w, bounds.h);
    }
    Button(SDL_Renderer *p_renderer, SDL_Color back_color, SDL_Rect bounds)
    {
        init_text();
        this->bounds.x = bounds.x;
        this->bounds.y = bounds.y;
        this->bounds.w = bounds.w;
        this->bounds.h = bounds.h;
        back_texture = SDL_CreateTexture(p_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, bounds.w, bounds.h);

        SDL_Texture *former_texture = SDL_GetRenderTarget(p_renderer);

        SDL_SetRenderTarget(p_renderer, back_texture);
        SDL_SetRenderDrawColor(p_renderer, back_color.r, back_color.g, back_color.b, back_color.a);
        SDL_RenderClear(p_renderer);
        SDL_SetRenderTarget(p_renderer, NULL);
        SDL_RenderCopy(p_renderer, back_texture, NULL, &bounds);
        if (former_texture != NULL)
            SDL_SetRenderTarget(p_renderer, former_texture);
        former_texture = NULL;
    }

    Button(SDL_Renderer *p_renderer, Uint8 back_r, Uint8 back_g, Uint8 back_b, Uint8 back_a, SDL_Rect bounds)
    {
        init_text();
        SDL_Color back_color{back_r, back_g, back_b, back_a};
        this->bounds.x = bounds.x;
        this->bounds.y = bounds.y;
        this->bounds.w = bounds.w;
        this->bounds.h = bounds.h;
        back_texture = SDL_CreateTexture(p_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, bounds.w, bounds.h);

        SDL_Texture *former_texture = SDL_GetRenderTarget(p_renderer);

        SDL_SetRenderTarget(p_renderer, back_texture);
        SDL_SetRenderDrawColor(p_renderer, back_color.r, back_color.g, back_color.b, back_color.a);
        SDL_RenderClear(p_renderer);
        SDL_SetRenderTarget(p_renderer, NULL);
        SDL_RenderCopy(p_renderer, back_texture, NULL, &bounds);
        if (former_texture != NULL)
            SDL_SetRenderTarget(p_renderer, former_texture);
        former_texture = NULL;
    }
    Button(SDL_Renderer *p_renderer, std::string back_addr, SDL_Rect bounds)
    {
        init_text();
        SDL_Surface *back_surf = IMG_Load(back_addr.c_str());
        back_texture = SDL_CreateTextureFromSurface(p_renderer, back_surf);
        SDL_FreeSurface(back_surf);
        this->bounds.x = bounds.x;
        this->bounds.y = bounds.y;
        this->bounds.w = bounds.w;
        this->bounds.h = bounds.h;
    }
    void set_font(const char *font_addr)
    {
        text.font_addr = font_addr;
        text.update_font();
    }
    void set_text_size(int size)
    {
        text.ptsize = size;
        text.update_font();
    }
    void set_font_and_size(const char *font_addr, int size)
    {
        text.font_addr = font_addr;
        text.ptsize = size;
        text.update_font();
    }
    bool is_clicked()
    {
        Uint32 state;
        int x = 0, y = 0;
        state = SDL_GetMouseState(&x, &y);
        SDL_PumpEvents();
        return (SDL_PointInRect(new SDL_Point{x, y}, &bounds) && (state & SDL_BUTTON_LMASK));
    }
    void set_text(Text text)
    {
        this->text = text;
    }
    void render(SDL_Renderer *renderer, SDL_Rect *srcrect = NULL)
    {
        int x = 0 , y = 0;
        SDL_GetMouseState(&x , &y);
        SDL_Texture *former_texture = SDL_GetRenderTarget(renderer);
        SDL_SetRenderTarget(renderer , NULL);
        SDL_RenderCopy(renderer , back_texture , srcrect , &bounds);
        
        if(SDL_PointInRect(new SDL_Point{x,y} , &bounds)){
            SDL_SetRenderDrawColor(renderer , 250 , 100 , 0 , 255);
            SDL_RenderDrawRect(renderer , &bounds);
        }
        
        render_text_center(renderer , text.text.c_str() , new SDL_Point{bounds.w/2 + bounds.x , bounds.h/2+bounds.y} , text.font , text.color);
        SDL_SetRenderTarget(renderer , former_texture);
        former_texture = NULL;
    }
    ~Button()
    {
        SDL_DestroyTexture(back_texture);
    }
};

typedef struct Button Button;

int end_mnu_btns[END_MENU_BTN_CNT][4];

bool read_best_score(long &score)
{
    std::ifstream file(SCORES_FILE_ADDR);
    if (!file.is_open())
    {
        std::ofstream m_file(SCORES_FILE_ADDR);
        m_file.close();
        file.open(SCORES_FILE_ADDR);
        if (!file.is_open())
        {
            return -1;
        }
    }
    if (file)
    {
        std::string data = "";
        file >> data;
        if (data != "")
        {
            score = std::stol(data);
        }
        else
        {
            score = 0;
        }
        file.close();
        return true;
    }
    return false;
}

bool set_best_score(const long score)
{
    std::ofstream file(SCORES_FILE_ADDR);
    if (file)
    {
        file << score;
        file.close();
        return true;
    }
    return false;
}

//-----===== OK! Tested ====-----
bool draw_image(SDL_Renderer *renderer, SDL_Point top_left_corner, const char *image_address , SDL_Rect * srcrect = NULL , SDL_Rect * dstrect = NULL)
{
    SDL_Surface *surf = IMG_Load(image_address);
    if (!surf)
    {
        return false;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
    if (!texture)
    {
        SDL_FreeSurface(surf);
        return false;
    }
    SDL_Texture *former_texture = SDL_GetRenderTarget(renderer);

    SDL_Rect rect = {top_left_corner.x, top_left_corner.y, surf->w, surf->h};
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, texture, srcrect, (dstrect == NULL?&rect : dstrect));

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(texture);
    SDL_SetRenderTarget(renderer, former_texture);
    former_texture = NULL;
    return true;
}

int end_menu(SDL_Renderer *renderer, SDL_Texture *back_texture, SDL_Rect *bounds, TTF_Font *text_font)
{

    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, back_texture, bounds, bounds);
    SDL_SetRenderTarget(renderer, back_texture);

    const int text_top_margin = 40;
    const int margin_between_buttons = 40;
    const int buttons_top_margin = 20;
    const int buttons_margins[] = {
        bounds->w / 3,  // Left margin
        bounds->w / 3,  // Right margin
        bounds->h / 20, // Top margin
        bounds->h / 20  // Bottom margin
    };
    SDL_Texture *texture_score = NULL;
    SDL_Rect rect_txt;

    std::string buttons_text[END_MENU_BTN_CNT] = {"start menu", "main menu", "quit"};

    //--------Best score rendering---------
    {
        std::string score_text("Best score: ");
        long int best_score = 0;

        if (!read_best_score(best_score))
        {
            return -1;
        }

        score_text += std::to_string(best_score);
        SDL_Color fg = {255, 255, 255, 255};
        SDL_Rect result;
        result = render_text_center(renderer, score_text.c_str(), new SDL_Point{bounds->w / 2 + bounds->x, bounds->y + text_top_margin}, TTF_OpenFont(FONT_ADDR, 30));
        rect_txt.x = bounds->w / 2 + bounds->x - result.w / 2;
        rect_txt.y = bounds->y + text_top_margin;
        rect_txt.w = result.w;
        rect_txt.h = result.h;
    }

    const int buttons_height = ((bounds->h - buttons_margins[3] - buttons_margins[2]) - rect_txt.h - text_top_margin - buttons_top_margin - (END_MENU_BTN_CNT - 1) * margin_between_buttons) / END_MENU_BTN_CNT;
    const int buttons_width = (bounds->w - buttons_margins[0] - buttons_margins[1]);

    const SDL_Rect buttons_rect = {bounds->x + buttons_margins[0],
                                   rect_txt.h + rect_txt.y + buttons_top_margin,
                                   buttons_width,
                                   (buttons_height * END_MENU_BTN_CNT) + (margin_between_buttons) * (END_MENU_BTN_CNT - 1)};

    SDL_SetRenderTarget(renderer, back_texture);

    // Buttons color
    SDL_SetRenderDrawColor(renderer, 200, 200, 180, 255);
    // Buttons font and color
    TTF_Font *buttons_font = TTF_OpenFont("./font.otf", 35);
    SDL_Color buttons_text_color{220, 100, 50, 255};

    for (int i = 0; i < END_MENU_BTN_CNT; i++)
    {
        end_mnu_btns[i][0] = buttons_rect.x;
        end_mnu_btns[i][1] = (buttons_height + (i <= 0 ? text_top_margin : margin_between_buttons)) * i + rect_txt.h + rect_txt.y + buttons_margins[2];
        end_mnu_btns[i][2] = buttons_rect.x + buttons_rect.w;
        end_mnu_btns[i][3] = end_mnu_btns[i][1] + buttons_height;

        SDL_RenderFillRect(renderer, new SDL_Rect{end_mnu_btns[i][0], end_mnu_btns[i][1], buttons_rect.w, buttons_height});
        SDL_RenderCopy(renderer, back_texture, &buttons_rect, &buttons_rect);
    }
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, back_texture, &buttons_rect, &buttons_rect);

    for (int i = 0; i < END_MENU_BTN_CNT; i++)
    {
        render_text_center(renderer, buttons_text[i].c_str(), new SDL_Point{buttons_rect.x + buttons_rect.w / 2, end_mnu_btns[i][1] + buttons_height / 2}, buttons_font, buttons_text_color);
    }

    SDL_DestroyTexture(texture_score);

    return 1;
}





int main(int argc, char *argv[])
{
    // Initialization of SDL window
    Uint32 SDL_flags = SDL_INIT_VIDEO | SDL_INIT_EVENTS;
    Uint32 WND_flags = SDL_WINDOW_HIDDEN;

    SDL_Window *m_window = NULL;
    SDL_Renderer *m_renderer = NULL;

    if (SDL_Init(SDL_flags) < 0 || TTF_Init() < 0 || IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP) == 0)
    {
        printf("Error with SDL_Init: %s", SDL_GetError());
        return -1;
    }

    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WIDNOW_HEIGHT, WND_flags, &m_window, &m_renderer);
    if (!m_renderer || !m_window)
    {
        printf("Error with renderer or window: %s", SDL_GetError());
        return -1;
    }

    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_ShowWindow(m_window);
    SDL_RenderClear(m_renderer);
    SDL_RenderPresent(m_renderer);

    SDL_Event *e = new SDL_Event;
    TTF_Font *font = TTF_OpenFont(FONT_ADDR, 30);

    while (Game_State != STATE_QUIT)
    {
        SDL_SetRenderTarget(m_renderer, NULL);
        SDL_SetRenderDrawColor(m_renderer, 30, 40, 50, 255);
        SDL_RenderClear(m_renderer);

        switch (Game_State)
        {
        case STATE_START_MENU:
            break;
        case STATE_GAMING:
            break;
        case STATE_PAUSE_MENU:
            break;
        case STATE_END_MENU:
        {
            draw_image(m_renderer , {0 , 0} , "./1.png" , NULL);
            break;
        }
        case STATE_QUIT:
            break;
        }

        while (SDL_PollEvent(e))
        {
            switch (e->type)
            {
            case SDL_KEYDOWN:
                if (e->key.keysym.sym == 'q')
                {
                    Game_State = STATE_QUIT;
                }
                break;
            case SDL_QUIT:
                Game_State = STATE_QUIT;
                break;
            }
        }

        SDL_RenderPresent(m_renderer);
        SDL_Delay(DELAY);
    }

    // Finalize and free resources
    SDL_DestroyWindow(m_window);
    SDL_DestroyRenderer(m_renderer);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}
