#include "ChatSystem.h"


ChatSystem::ChatSystem()
{

	font_.reset(new hgeFont("font1.fnt"));
	font_->SetScale(0.5);
	ChatTime = false;
	StringtoRender = "This is the chat box. \n";

	for (int i = 0; i < 256; i++)
	{
		keys[i] = false;
	}
	BackSpace = false; Space = false;
	FirstRun = true;
	EndLineCounter = 0;
	x = 30; y = 270;
}


ChatSystem::~ChatSystem()
{
	font_.release();
}

void ChatSystem::Render()
{
	//std::cout << "I'm Rendering it" << std::endl;
	theBackground.Render(x,y);

	font_->printf(x + 5, y + 5, HGETEXT_LEFT, "%s",
		StringtoRender.c_str());

	font_->printf(50, 50, HGETEXT_LEFT, "%s",
		ToSendBuffer.c_str());
}

bool ChatSystem::KeyboardInput(bool &keydownenter)
{
	//std::cout << "CHAT MODE ON " << std::endl;
	HGE* hge = hgeCreate(HGE_VERSION);
	if (hge->Input_GetKeyState(HGEK_ENTER))
	{
		if (!keydownenter)
		{
			ChatTime = false;
			ReceiveString("You: " + ToSendBuffer + "\n");
			SendString = ToSendBuffer + "\n";
			ToSendBuffer.clear();
			std::cout << "CHAT MODE OFF" << std::endl;
			FirstRun = true;
			return false;
			//SEND THE STRING
		}
	}
	else
	{
		if (keydownenter)
		{
			keydownenter = false; 
		}
	}

	if (hge->Input_GetKeyState(HGEK_SPACE))
	{
		if (!Space)
		{
			ToSendBuffer += " ";
			std::cout << "Space" << std::endl;
			Space = true;
		}
	}
	else
	{
		if (Space)
		{
			Space = false;
		}
	}

	if (hge->Input_GetKeyState(HGEK_BACKSPACE))
	{
		if (!BackSpace)
		{
			if (ToSendBuffer.size() != 0)
			{
				ToSendBuffer.pop_back();
				std::cout << "BACKSPACE" << std::endl;
				BackSpace = true;
			}
		}
	}
	else
	{
		if (BackSpace)
		{
			BackSpace = false;
		}
	}


	for (int i = 0x41, a = 0; i < 0xC0; i++, a++)
	{
		if (hge->Input_GetKeyState(i))
		{
			if (!keys[a])
			{
				ToSendBuffer.push_back(i);
				keys[a] = true;	
				std::cout << "KEY INPUT " << ToSendBuffer.back() << std::endl;
			}
		}
		else
		{
			if (keys[a])
			{
				keys[a] = false;
			}
		}
	}


	hge->Release();	
	return true;
}

void ChatSystem::ReceiveString(std::string recv)
{
	EndLineCounter++;
	StringtoRender += recv;
}

std::string ChatSystem::StringtoSend()
{
	std::string BufferString = SendString;
	SendString.clear();
	return BufferString;
}