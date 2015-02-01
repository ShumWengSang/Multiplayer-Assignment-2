#pragma once
#include <hge.h>
#include <hgerect.h>
#include <memory>
#include <vector>
#include "hgesprite.h"
#include "hgecolor.h"

class hgeSprite;
class hgeColor;

class ChatBackground
{
public:
	ChatBackground();
	~ChatBackground();
	hgeColor theColor;

	HTEXTURE tex_; //!< Handle to the sprite's texture
	std::auto_ptr<hgeSprite> sprite_; //!< The sprite used to display the ship

	float x;	//Position
	float y;	//Position

	void Render(int, int);

};

