#include "Sprite.h"

#include "MyGame.h"

CGameApp* CGameApp::instance = nullptr;

int main(int argc, char* argv[])
{
	
	
	CGameApp::App()->OpenWindow(900, 700, "Explorer3D");

	CGameApp::App()->SetFPS(60);
	CGameApp::App()->SetClearColor(CColor::Red());

	CGameApp::App()->Run(CMyGame::Game());
	return(0);
}
