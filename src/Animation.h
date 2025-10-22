#pragma once

#include <vector>
#include "SDL3/SDL_rect.h"

class Animation
{
public:
    float speed = 1.0f;
    bool loop = true;
    std::vector<SDL_Rect> frames;

private:
    float currentFrame = 0.0f;

public:
    SDL_Rect& GetCurrentFrame()
    {
        currentFrame += speed;
        if (currentFrame >= frames.size())
        {
            currentFrame = (loop) ? 0.0f : static_cast<float>(frames.size() - 1);
        }
        return frames[(int)currentFrame];
    }

    void Reset()
    {
        currentFrame = 0.0f;
    }

    bool HasFinished()
    {
        return !loop && currentFrame >= frames.size() - 1;
    }
};