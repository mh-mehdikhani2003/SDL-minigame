#include <iostream>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include <string>
#include <fstream>
#include <vector>
#include <ctime>

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

#ifndef SCORE_FILE_ADDR
#define SCORE_FILE_ADDR ".\scores"
#endif

#ifndef __GFX__
#include <SDL2/SDL_gfx>
#endif

#define CHAR_RAW_ROOT "./raw/Char/"
#define FONT_ADDR "./arial.ttf"
#define UME_SCORE_FONT_SIZE 35
#define UME_BTNS_FONT_SIZE 16
#define END_MENU_BTN_CNT 3
#define WINDOW_WIDTH 1080
#define WIDNOW_HEIGHT 720

#define DELAY 50
#define GRAVITY 10

#define WIDTH 1200
#define HEIGHT 800
#define BOTTOM_MARGIN 100

using namespace std;

//-----===== OK! Tested ====-----
bool draw_image_on_point(SDL_Renderer *renderer, SDL_Point center_point, Uint16 height, const char *image_address, SDL_Rect *srcrect = NULL, SDL_Rect *dstrect = NULL)
{
    SDL_Surface *surf = IMG_Load(image_address);
    if (!surf)
    {
        return false;
    }

    float scale = (float)height / (float)surf->h;
    SDL_Rect rect = {center_point.x - (int)(surf->w / 2.0 * scale), center_point.y - height / 2, (int)(surf->w * scale), (int)(surf->h * scale)};

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
    if (!texture)
    {
        SDL_FreeSurface(surf);
        return false;
    }
    SDL_Texture *former_texture = SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, texture, srcrect, (dstrect == NULL ? &rect : dstrect));

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(texture);
    SDL_SetRenderTarget(renderer, former_texture);
    former_texture = NULL;
    return true;
}
// bool draw_image_on_point(SDL_Renderer *renderer, SDL_Point center_point, const char *image_address, SDL_Rect *dstrect, SDL_Rect *srcrect = NULL)
// {
//     SDL_Surface *surf = IMG_Load(image_address);
//     if (!surf)
//         return false;

//     // dstrect->x = bottom_center_point.x - surf->w/2;
//     // dstrect->y = bottom_center_point.y - surf->h;
//     SDL_Texture *img_texture = SDL_CreateTextureFromSurface(renderer, surf);
//     SDL_Texture *former_texture = SDL_GetRenderTarget(renderer);
//     SDL_SetRenderTarget(renderer, NULL);
//     SDL_RenderCopy(renderer, img_texture, srcrect, dstrect);
//     SDL_FreeSurface(surf);
//     SDL_DestroyTexture(img_texture);
//     if (former_texture != NULL)
//     {
//         SDL_SetRenderTarget(renderer, former_texture);
//     }
//     SDL_DestroyTexture(former_texture);
//     return true;
// }
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

SDL_Rect render_text_right(SDL_Renderer *renderer, const char *text, SDL_Point *right_center_point, TTF_Font *font = TTF_OpenFont(FONT_ADDR, 20), SDL_Color color = {0, 0, 0, 255})
{
    SDL_Texture *pervious_target = SDL_GetRenderTarget(renderer);
    SDL_Surface *surf = TTF_RenderText_Blended(font, text, color);
    SDL_Texture *txt = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_Rect result = {right_center_point->x - surf->w, right_center_point->y - surf->h / 2, surf->w, surf->h};

    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, txt, NULL, &result);

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(txt);
    SDL_SetRenderTarget(renderer, pervious_target);

    return result;
}

SDL_Rect render_text_left(SDL_Renderer *renderer, const char *text, SDL_Point *left_center_point, TTF_Font *font = TTF_OpenFont(FONT_ADDR, 20), SDL_Color color = {0, 0, 0, 255})
{
    SDL_Texture *pervious_target = SDL_GetRenderTarget(renderer);
    SDL_Surface *surf = TTF_RenderText_Blended(font, text, color);
    SDL_Texture *txt = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_Rect result = {left_center_point->x, left_center_point->y - surf->h / 2, surf->w, surf->h};

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
States Game_State = STATE_GAMING;

typedef enum Char_modes
{
    NORMAL,
    CONFUSED,
    FREEZED,
    TRIPLE,
    RUNNING_LEFT,
    RUNNING_RIGHT,
    JUMPING
} Char_modes;

typedef enum Char_types
{
    CHARACTER_RIGHT,
    CHARACTER_LEFT
} Char_types;

typedef struct Character
{
private:
    Char_types type = CHARACTER_RIGHT;
    Char_modes mode = NORMAL;
    int body_number = 0;
    int head_number = 0;

    int initial_y = 0;

    int shoes_current_number = 0;
    int shoes_model = 0;
    const static int shoes_cnt = 5;

    const char *head_root = "./raw/Char/heads/";
    const char *body_root = "./raw/Char/bodies/";
    const char *shoes_root = "./raw/Char/shoes/";

    float head_to_height_ratio = 0.4;
    float body_to_height_ratio = 0.45;
    float shoes_to_height_ratio = 0.15;

    const int triple_margin = 30;
    const int x_speed = 20;
    const int y_speed = 50;
    int dx = 0, dy = 0, dvy = 0;
    int keys[3] = {SDLK_RIGHT, SDLK_LEFT, SDLK_UP};

    SDL_Event *event = NULL;

    SDL_Rect bounds{0, 0, 0, 0};
    SDL_Texture *back_texture = NULL;
    SDL_Texture *head_texture = NULL;
    SDL_Texture *body_texture = NULL;
    SDL_Texture *shoes_textures[shoes_cnt] = {NULL};
    SDL_Rect body_rect{0, 0, 0, 0};
    SDL_Rect head_rect{0, 0, 0, 0};
    SDL_Rect shoes_rects[shoes_cnt];

    void set_scales()
    {
        int h = bounds.h;
        float head_ratio = (h * head_to_height_ratio) / head_rect.h;
        head_rect.h *= head_ratio;
        head_rect.w *= head_ratio;

        float body_ratio = (h * body_to_height_ratio) / body_rect.h;
        body_rect.h *= body_ratio;
        body_rect.w *= body_ratio;
        body_rect.y = bounds.y + head_rect.h;

        float shoes_ratio = (h * shoes_to_height_ratio) / shoes_rects[0].h;

        for (int i = 0; i < shoes_cnt; i++)
        {
            shoes_rects[i].h *= shoes_ratio;
            shoes_rects[i].w *= shoes_ratio;
            shoes_rects[i].y = body_rect.y + body_rect.h;
        }
    }
    void fill_textures(SDL_Renderer *renderer)
    {
        SDL_Surface *body_img = IMG_Load(create_body_and_head_address(body_root, body_number).c_str());
        body_texture = SDL_CreateTextureFromSurface(renderer, body_img);
        body_rect = body_img->clip_rect;
        SDL_FreeSurface(body_img);

        SDL_Surface *head_img = IMG_Load(create_body_and_head_address(head_root, head_number).c_str());
        head_texture = SDL_CreateTextureFromSurface(renderer, head_img);
        head_rect = head_img->clip_rect;
        SDL_FreeSurface(head_img);

        for (int i = 0; i < shoes_cnt; i++)
        {
            SDL_Surface *img = IMG_Load(create_shoes_address(shoes_model, i).c_str());
            shoes_textures[i] = SDL_CreateTextureFromSurface(renderer, img);
            shoes_rects[i] = img->clip_rect;
            // float scale = (float)(bounds.h) / (float)(img->h);
            // pics_bounds[i].w *= scale;
            // pics_bounds[i].h *= scale;
            SDL_FreeSurface(img);
        }
    }
    void destroy_pics_textures()
    {
        SDL_DestroyTexture(head_texture);
        head_rect = {0, 0, 0, 0};
        head_texture = NULL;
        SDL_DestroyTexture(body_texture);
        body_rect = {0, 0, 0, 0};
        body_texture = NULL;
        for (int i = 0; i < shoes_cnt; i++)
        {
            SDL_DestroyTexture(shoes_textures[i]);
            shoes_rects[i] = {0, 0, 0, 0};
            shoes_textures[i] = NULL;
        }
    }
    void render_head(SDL_Renderer *renderer)
    {
        SDL_Point dstpoint = {bounds.x + bounds.w / 2, bounds.y + head_rect.h / 2};
        draw_image_on_point(renderer, SDL_Point{dstpoint.x, dstpoint.y}, head_rect.h, create_body_and_head_address(head_root, head_number).c_str());
    }
    void render_body(SDL_Renderer *renderer)
    {
        SDL_Point dstpoint = {bounds.x + bounds.w / 2, bounds.y + head_rect.h + body_rect.h / 2};
        draw_image_on_point(renderer, SDL_Point{dstpoint.x, dstpoint.y}, body_rect.h, create_body_and_head_address(body_root, body_number).c_str());
    }
    void render_shoes(SDL_Renderer *renderer)
    {
        SDL_Point dstpoint_r{0, 0};
        SDL_Point dstpoint_l{0, 0};
        int right_index = shoes_current_number;
        int left_index = ((mode == RUNNING_LEFT || mode == RUNNING_RIGHT) ? shoes_cnt - 1 - shoes_current_number : shoes_current_number);

        if (type == CHARACTER_RIGHT)
        {
            dstpoint_l.x = bounds.x + bounds.w / 2 + body_rect.w / 2 - shoes_rects[shoes_current_number].w / 2 + body_rect.w / 3;
            dstpoint_r.x = bounds.x + bounds.w / 2 - shoes_rects[shoes_current_number].w / 2 + body_rect.w / 3;
        }
        else
        {
            dstpoint_r.x = bounds.x + bounds.w / 2 + body_rect.w / 2 - shoes_rects[shoes_current_number].w / 2 + body_rect.w / 3;
            dstpoint_l.x = bounds.x + bounds.w / 2 - shoes_rects[shoes_current_number].w / 2 + body_rect.w / 3;
        }

        dstpoint_l.y = bounds.y + head_rect.h + body_rect.h + shoes_rects[0].h / 2;
        dstpoint_r.y = bounds.y + head_rect.h + body_rect.h + shoes_rects[0].h / 2;
        draw_image_on_point(renderer, dstpoint_r, shoes_rects[right_index].h, create_shoes_address(shoes_model, right_index).c_str());
        draw_image_on_point(renderer, dstpoint_l, shoes_rects[left_index].h, create_shoes_address(shoes_model, left_index).c_str());
    }
    string create_shoes_address(int model, int number)
    {
        string result = shoes_root;
        result += to_string(model);
        if (number > 0)
        {
            result += "(";
            result += to_string(number);
            result += ")";
        }
        result += ".png";
        return result;
    }
    string create_body_and_head_address(string root, int pic_number)
    {
        root += to_string(pic_number);
        root += ".png";
        return root;
    }
    void set_mode()
    {
        const int right = keys[0];
        const int left = keys[1];
        const int up = keys[2];

        switch (event->type)
        {
        case SDL_KEYDOWN:
        {
            int key = event->key.keysym.sym;

            if (key == up)
            {
                mode = JUMPING;
            }
            if (key == left)
            {
                mode = RUNNING_LEFT;
            }
            if (key == right)
            {
                mode = RUNNING_RIGHT;
            }
        }
        break;

        case SDL_KEYUP:
        {
            int key = event->key.keysym.sym;

            if (key == up)
            {
                break;
            }
            if (key == left)
            {
                mode = NORMAL;
                break;
            }
            if (key == right)
            {
                mode = NORMAL;
                break;
            }
        }
        break;
        }
    }

public:
    Character(SDL_Renderer *renderer, SDL_Event *event, SDL_Rect bounds, Char_types type, int head_number = 0, int body_number = 0, int shoes_model = 0, int shoe_number = 0)
    {
        this->head_number = head_number;
        this->body_number = body_number;
        this->shoes_model = shoes_model;
        this->shoes_current_number = shoe_number;
        this->bounds = bounds;
        this->event = event;
        fill_textures(renderer);
        set_scales();
        initial_y = bounds.y;
        this->type = type;
    }
    Character(Character &character) = delete;
    void set_keys(int right, int left, int up) { keys[0] = right, keys[1] = left, keys[2] = up; }
    void set_mode(Char_modes mode) { this->mode = mode; }
    Char_modes get_mode() { return mode; }
    Char_types get_type() { return type; }
    void set_texture(SDL_Texture *new_texture) { back_texture = new_texture; }
    SDL_Texture *get_texture() { return back_texture; }
    void render(SDL_Renderer *renderer)
    {
        set_mode();
        render_body(renderer);
        render_head(renderer);
        if (mode == RUNNING_LEFT || mode == RUNNING_RIGHT)
        {
            dx = (mode == RUNNING_LEFT ? -x_speed : x_speed);
            shoes_current_number++;
            if (shoes_current_number >= shoes_cnt)
            {
                shoes_current_number = 0;
            }
        }
        if (mode == NORMAL)
        {
            shoes_current_number = 0;
            dx = 0;
        }
        if (mode == JUMPING)
        {
            if (dvy == 0)
            {
                dvy = GRAVITY;
                dy = -y_speed;
            }
        }
        if (bounds.y > initial_y)
        {
            bounds.y = initial_y;
            dy = 0;
            dvy = 0;
            mode = NORMAL;
        }
        // set_scales();
        render_shoes(renderer);
        bounds.x += dx;
        dy += dvy;
        bounds.y += dy;
    }
    ~Character()
    {
        SDL_DestroyTexture(back_texture);
        back_texture = NULL;
        destroy_pics_textures();
    }
} Character;

typedef struct ProgressBar
{
private:
    Uint16 total_value = 0;
    Uint16 current_value = 0;
    SDL_Color front_color;
    SDL_Color back_color;
    SDL_Rect bounds;
    SDL_Texture *back_texture = NULL;

public:
    ProgressBar(Uint16 total_value, Uint16 current_value, SDL_Color front_color, SDL_Color back_color, SDL_Rect bounds)
    {
        this->total_value = total_value;
        this->current_value = current_value;
        this->front_color = front_color;
        this->back_color = back_color;
        this->bounds = bounds;
    }

    void set_value(Uint16 value)
    {
        if (value > total_value)
        {
            value = total_value;
        }
        current_value = value;
    }

    Uint16 get_value()
    {
        return current_value;
    }

    void render(SDL_Renderer *renderer)
    {
        const float margin_ratio = 0.1;
        const int margin = 5;
        float ratio = (float)current_value / (float)total_value;
        SDL_Texture *former = SDL_GetRenderTarget(renderer);
        SDL_Rect front_rect{bounds.x + margin, bounds.y + margin, int(bounds.w * ratio * ((float)(bounds.w - 2 * margin) / (float)bounds.w)), bounds.h - 2 * margin};
        // SDL_Rect front_rect = {bounds.x, bounds.y, (int)(bounds.w * ratio), bounds.h};
        //  front_rect.x += (int)(margin_ratio * bounds.w);
        //  front_rect.y += (int)(margin_ratio * bounds.h);
        //  front_rect.w = (int)(front_rect.w - 2*bounds.w * margin_ratio);
        //  front_rect.h = (int)(front_rect.h - 2*bounds.h * margin_ratio);
        if (back_texture == NULL)
            back_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_RENDERER_TARGETTEXTURE, bounds.w, bounds.h);
        SDL_SetRenderTarget(renderer, back_texture);
        SDL_SetRenderDrawColor(renderer, back_color.r, back_color.g, back_color.b, back_color.a);
        SDL_RenderFillRect(renderer, &bounds);
        SDL_SetRenderDrawColor(renderer, front_color.r, front_color.g, front_color.b, front_color.a);
        SDL_RenderFillRect(renderer, &front_rect);
        SDL_SetRenderTarget(renderer, NULL);
        render_text_center(renderer, (to_string((int)(ratio * 100.0)) + string("%")).c_str(), new SDL_Point{bounds.x + bounds.w / 2, bounds.y + bounds.h / 2}, NULL, {255, 255, 255, 255});
        SDL_RenderCopy(renderer, back_texture, &bounds, &bounds);
        SDL_SetRenderTarget(renderer, former);
    }
    ~ProgressBar()
    {
        SDL_DestroyTexture(back_texture);
        back_texture = NULL;
    }
} ProgressBar;

typedef struct Timer
{
private:
    Uint64 start_time = 0;
    Uint64 total_time = 0;
    Uint16 alarm = 0;
    bool is_started = false;
    bool is_on = false;
    Timer(Timer &timer) = delete;

public:
    Timer()
    {
        start_time = time(nullptr);
        is_on = true;
        is_started = true;
    }
    void play()
    {
        is_on = true;
    }
    void pause()
    {
        if (is_on)
        {
            total_time = time(nullptr) - start_time;
        }
        start_time = time(nullptr) - total_time;
        is_on = false;
    }
    Uint64 get_time()
    {
        if (is_on)
        {
            return time(nullptr) - start_time;
        }
        return total_time;
    }
    void set_alarm(Uint16 time_in_seconds)
    {
        alarm = time_in_seconds;
    }

    bool get_status()
    {
        return is_on;
    }

    void clear()
    {
        total_time = 0;
        start_time = time(nullptr);
    }

    bool check_alarm()
    {
        return get_time() > alarm;
    }
} Timer;
typedef struct Text
{
    std::string text = " ";
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

typedef struct TextBox
{
private:
    Text text;
    SDL_Rect bounds;
    SDL_Texture *back_texture = NULL;
    SDL_Color back_color{255, 255, 200, 255};
    SDL_Color border_color{255, 200, 220, 255};
    bool enabled = false;
    SDL_Event* e=NULL;

    void read_keys_and_mouse(SDL_Event *event)
    {
        int x = 0, y = 0;
        if (SDL_GetMouseState(&x, &y) & SDL_BUTTON_LMASK)
        {
            if (SDL_PointInRect(new SDL_Point{x, y}, &bounds))
                enabled = true;
            else
                enabled = false;
        }

        if (SDL_PollEvent(event) && enabled)
        {
            if (event->type == SDL_KEYDOWN)
            {
                switch (event->key.keysym.sym)
                {
                case SDLK_BACKSPACE:
                {
                    if (text.text.size() > 0)
                        text.text.erase(text.text.end() - 1);
                }
                default:
                {
                    text.text += event->key.keysym.sym;
                }
                }
            }
        }
    }

public:
    TextBox(Text text, SDL_Color back_color, SDL_Rect bounds , SDL_Event* e)
    {
        this->text = text;
        text.text="";
        this->back_color = back_color;
        this->bounds = bounds;
        this->e = e;
    }
    TextBox(Text text, const char *back_surf_addr, SDL_Rect bounds)
    {
    }
    void render(SDL_Renderer *renderer)
    {
        read_keys_and_mouse(e);
        SDL_Texture *former_texture = SDL_GetRenderTarget(renderer);
        if(back_texture == NULL){
            back_texture = SDL_CreateTexture(renderer , SDL_PIXELFORMAT_RGBA8888 , SDL_TEXTUREACCESS_TARGET , bounds.w , bounds.h);
        }
        SDL_SetRenderTarget(renderer , back_texture);
        SDL_SetRenderDrawColor(renderer, back_color.r, back_color.g, back_color.b, back_color.a);
        SDL_RenderFillRect(renderer,new SDL_Rect{0,0,bounds.w , bounds.h});

        if (enabled)
        {
            SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g, border_color.b, border_color.a);
            SDL_RenderDrawRect(renderer, new SDL_Rect{0,0,bounds.w , bounds.h});
        }
        SDL_SetRenderTarget(renderer, NULL);
        SDL_RenderCopy(renderer , back_texture , NULL , &bounds);
        if (former_texture != NULL)
        {
            SDL_SetRenderTarget(renderer, former_texture);
        }
        render_text_center(renderer, text.text.c_str(), new SDL_Point{bounds.x + bounds.w / 2, bounds.y + bounds.h / 2});
    }
} TextBox;

struct Button
{
private:
    SDL_Rect bounds;
    Text text;
    SDL_Texture *back_texture = NULL;
    SDL_Color back_color{0, 0, 0, 0};

    void init_text()
    {
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
    void set_text(Text &text)
    {
        this->text = text;
    }
    void render(SDL_Renderer *renderer, SDL_Rect *srcrect = NULL)
    {
        int x = 0, y = 0;
        SDL_GetMouseState(&x, &y);
        SDL_Texture *former_texture = SDL_GetRenderTarget(renderer);
        SDL_SetRenderTarget(renderer, NULL);
        SDL_RenderCopy(renderer, back_texture, srcrect, &bounds);

        if (SDL_PointInRect(new SDL_Point{x, y}, &bounds))
        {
            SDL_SetRenderDrawColor(renderer, 250, 100, 0, 255);
            SDL_RenderDrawRect(renderer, &bounds);
        }

        render_text_center(renderer, text.text.c_str(), new SDL_Point{bounds.w / 2 + bounds.x, bounds.h / 2 + bounds.y}, text.font, text.color);
        SDL_SetRenderTarget(renderer, former_texture);
        former_texture = NULL;
    }
    SDL_Color get_color()
    {
        return back_color;
    }
    void set_back_color(SDL_Renderer *renderer, SDL_Color color)
    {
        back_color = color;
        SDL_Texture *former = SDL_GetRenderTarget(renderer);
        SDL_SetRenderTarget(renderer, back_texture);
        SDL_SetRenderDrawColor(renderer, back_color.r, back_color.g, back_color.b, back_color.a);
        SDL_RenderClear(renderer);
        SDL_SetRenderTarget(renderer, NULL);
        SDL_RenderCopy(renderer, back_texture, NULL, &bounds);
        SDL_SetRenderTarget(renderer, former);
    }
    ~Button()
    {
        SDL_DestroyTexture(back_texture);
    }
};

typedef struct Button Button;

// int end_mnu_btns[END_MENU_BTN_CNT][4];

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

/*int end_menu(SDL_Renderer *renderer, SDL_Texture *back_texture, SDL_Rect *bounds, TTF_Font *text_font)
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
}*/

int main(int argc, char *argv[])
{
    SDL_Window *m_window = NULL;
    SDL_Renderer *m_renderer = NULL;
    // Initialization of SDL window
    {
        Uint32 SDL_flags = SDL_INIT_VIDEO | SDL_INIT_EVENTS;
        Uint32 WND_flags = SDL_WINDOW_HIDDEN;

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
    }

    //-----====-Create main elements-====-----
    SDL_Event *e = new SDL_Event;
    Text text;
   // Timer timer;
    //ProgressBar prg1(100, 0, {240, 100, 80, 255}, {100, 120, 150, 255}, {100, 100, 300, 90});
    //Character l_char(m_renderer, e, {100, 400, 100, 300}, CHARACTER_LEFT, 0, 1, 2);
    //Character r_char(m_renderer, e, {500, 400, 100, 300}, CHARACTER_RIGHT, 0);
    //r_char.set_keys(SDLK_d, SDLK_a, SDLK_w);
    //Button btn1(m_renderer, SDL_Color{100, 200, 250, 255}, SDL_Rect{100, 100, 100, 40});

    TextBox tb(text , SDL_Color{150 , 190 , 220 , 255},{100 , 100 , 200 , 60} , e);
    //-----====-Main game loop start-====-----
    while (Game_State != STATE_QUIT)
    {
        SDL_PollEvent(e);

        SDL_SetRenderTarget(m_renderer, NULL);
        SDL_SetRenderDrawColor(m_renderer, 30, 40, 50, 255);
        SDL_RenderClear(m_renderer);

        switch (Game_State)
        {
        case STATE_START_MENU:
            break;
        case STATE_GAMING:
            tb.render(m_renderer);
            // prg1.render(m_renderer);
            // prg1.set_value(prg1.get_value() + 1);
            //r_char.render(m_renderer);
            //l_char.render(m_renderer);
            // btn1.render(m_renderer);
            break;
        case STATE_PAUSE_MENU:
            break;
        case STATE_END_MENU:
        {
            break;
        }
        case STATE_QUIT:
            break;
        }
        //-----====-Checking for quit and other events-====-----
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
