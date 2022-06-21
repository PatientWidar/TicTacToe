#include <iostream>
#include "main.h"
#include "SDL.h"
#include "SDL_image.h"

using namespace std;

Field winner = Field::empty;
SDL_Texture *cross_tex, *circle_tex, *background_tex;

void clearBoard(Field *board)
{
    for(int i = 0; i < 9; ++i)
    {
        board[i] = Field::empty;
    }
    winner = Field::empty;
}

bool isGameFinished(Field *board)
{
    for(int i = 0; i < 3; i++)
    {
        if(board[3*i] != Field::empty && board[3*i] == board[3*i + 1] && board[3*i + 1] ==  board[3*i + 2])
        {
            winner = board[3*i];
            return true;
        }
        if(board[i] != Field::empty && board[i] == board[i + 3] && board[i] == board[i + 6])
        {
            winner = board[i];
            return true;
        }
    }
    if(board[0] != Field::empty && board[0] == board[4] && board[4] == board[8])
    {
        winner = board[0];
        return true;
    }
    if(board[2] != Field::empty && board[2] == board[4] && board[4] == board[6])
    {
        winner = board[2];
        return true;
    }

    return false;
}

bool isTaken(int field, Field *board)
{
    if(field < 0)
        return true;

    if(field > 8)
        return true;

    return (board[field] != Field::empty);
}

bool running = true;

void renderBoard(Field *board, SDL_Renderer *renderer)
{
    for(int i = 0; i < 9; ++i)
    {
        SDL_Rect position;
        position.x = (i % 3) * 200;
        position.y = (i / 3) * 200;
        position.h = 200;
        position.w = 200;
        SDL_RenderCopy(renderer, background_tex, NULL, &position);
    }
    for(int i = 0; i < 9; ++i)
    {
        if(board[i] != Field::empty)
        {
            SDL_Rect position;
            position.x = (i % 3) * 200;
            position.y = (i / 3) * 200;
            position.h = 200;
            position.w = 200;
            SDL_RenderCopy(renderer, board[i] == Field::cross ? cross_tex : circle_tex, NULL, &position);
        }
    }
    SDL_RenderPresent(renderer);
}

void _handleEvents(int *move)
{
    SDL_Event event;
    SDL_PollEvent(&event);
    switch(event.type)
    {
        case SDL_QUIT:
            running = false;
            *move = 10;
            break;
        case SDL_MOUSEBUTTONUP:
            int x, y;
            SDL_GetMouseState(&x, &y);
            y /= 200;
            *move = x / 200 + 3 * y;
            break;

        case SDL_KEYDOWN:
            if(event.key.keysym.sym == SDLK_r)
            {
                *move = 9;
            }
            break;
    }
}

int main(int argc, char* argv[])
{
    Field playerTurn = Field::circle;
    Field board[9];

    SDL_Surface *circle, *cross, *bg_;

    bg_ = IMG_Load("res/background.png");
    circle = IMG_Load("res/circle.png");
    cross = IMG_Load("res/cross.png");
    

    clearBoard(board);
    SDL_Window *window;
    SDL_Renderer *renderer;
    if(SDL_Init(SDL_INIT_EVERYTHING) == 0)
    {
        window = SDL_CreateWindow("Ruch gracza: Kółko", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 600, 0);
        if(window)
            running = true;
        else
            running = false;

        renderer = SDL_CreateRenderer(window, -1, 0);
        if(!renderer)
            running = false;
    }

    circle_tex = SDL_CreateTextureFromSurface(renderer, circle);
    cross_tex = SDL_CreateTextureFromSurface(renderer, cross);
    background_tex = SDL_CreateTextureFromSurface(renderer, bg_);
    SDL_FreeSurface(circle);
    SDL_FreeSurface(cross);
    SDL_FreeSurface(bg_);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    //Generate board
    
    while(running)
    {
        clearBoard(board);
        SDL_RenderClear(renderer);
        renderBoard(board, renderer);
        SDL_RenderPresent(renderer);
        int move = -1;
        int counter = 0;
        while(!isGameFinished(board))
        {
            //Moves
            SDL_SetWindowTitle(window, playerTurn == Field::circle ? "Ruch gracza: Kółko" : "Ruch gracza: Krzyżyk");
            
            while(isTaken(move, board))
            {
                _handleEvents(&move);
                if(move > 8)
                    break;
            }
            ++counter;
            board[move] = playerTurn;
            renderBoard(board, renderer);
            if(move > 8 || counter == 9)
                break;

            playerTurn = playerTurn == Field::circle ? Field::cross : Field::circle;
        }
        if(move > 8)
            continue;

        SDL_SetWindowTitle(window, winner == Field::empty ? "Remis!" : winner == Field::circle ? "Wygrało Kółko!" : "Wygrał Krzyzyk!");
        while(true)
        {
            _handleEvents(&move);
            if(move > 8)
                break;
        }
    }


    SDL_DestroyTexture(cross_tex);
    SDL_DestroyTexture(circle_tex);
    SDL_DestroyTexture(background_tex);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    IMG_Quit();
    SDL_Quit();
    return 0;
}