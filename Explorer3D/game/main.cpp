#include "Sprite.h"

#include "GameApp.h"
#include "MyGame.h"


int main(int argc, char* argv[])
{
	CGameApp app;
	
	app.OpenWindow(900, 700, "Explorer3D");

	app.Run(CMyGame::Game());
	return(0);
}
