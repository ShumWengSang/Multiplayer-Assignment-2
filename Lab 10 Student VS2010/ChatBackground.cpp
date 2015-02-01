#include "ChatBackground.h"


ChatBackground::ChatBackground() : theColor(1,1,1,0.5)
{
	HGE* hge = hgeCreate(HGE_VERSION);
	tex_ = hge->Texture_Load("chatbackground.png");
	hge->Release();
	sprite_.reset(new hgeSprite(tex_, 0, 0, 350, 250));
	x = 0; y = 0;
	sprite_->SetColor(theColor.GetHWColor());
}


ChatBackground::~ChatBackground()
{
	sprite_.release();
	HGE* hge = hgeCreate(HGE_VERSION);
	hge->Texture_Free(tex_);
	hge->Release();

}

void ChatBackground::Render(int x, int y)
{
	sprite_->Render(x, y);
}