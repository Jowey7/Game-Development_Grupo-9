#pragma once

#include "Module.h"
#include "Vector2D.h"
#include "SDL3/SDL.h"

class Render : public Module
{
public:

	Render();
	virtual ~Render();

	bool Awake();
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void SetViewPort(const SDL_Rect& rect);
	void ResetViewPort();
	void StartFrame();

	// Drawing
	// CORREGIDO: Se usa SDL_FlipMode en lugar de SDL_RendererFlip y se ajusta el orden de par�metros
	bool DrawTexture(SDL_Texture* texture, int x, int y, const SDL_Rect* section = NULL, float speed = 1.0f, double angle = 0, int pivotX = INT_MAX, int pivotY = INT_MAX, int width = 0, int height = 0, SDL_FlipMode flip = SDL_FLIP_NONE) const;
	bool DrawRectangle(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool filled = true, bool useCamera = true) const;
	bool DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool useCamera = true) const;
	bool DrawCircle(int x1, int y1, int redius, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool useCamera = true) const;

	void SetBackgroundColor(SDL_Color color);

public:
	SDL_Renderer* renderer;
	SDL_Rect camera;
	SDL_Rect viewport;
	SDL_Color background;

private:
	bool vsync = false;
	SDL_Texture* renderTarget = nullptr;
	int nativeWidth = 0;
	int nativeHeight = 0;
};