#include "Sprite.h"

#include "GameApp.h"
#include "MyGame.h"


int main(int argc, char* argv[])
{
	CGameApp app;
	CMyGame game;
	
	app.OpenWindow(900, 700, "Explorer3D");

	app.Run(&game);
	return(0);
}
