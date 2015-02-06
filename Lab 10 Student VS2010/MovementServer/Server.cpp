#include "ServerApp.h"
#include <Windows.h>

int main()
{
	time_t timer;
	timer = time(NULL);
	srand(time(&timer));

	ServerApp app;
	while (true)
	{
		Sleep(10);
		app.Loop();
	}
	return 0;
}