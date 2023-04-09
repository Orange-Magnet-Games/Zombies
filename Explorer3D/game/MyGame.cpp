#include "MyGame.h"

using namespace std;

CMyGame::CMyGame(void) { score = 0; }

CMyGame::~CMyGame(void) {}


// --------  game initialisation --------
void CMyGame::OnInitialize()
{

	// Loading graphics and sound assets
	cout << "Loading assets" << endl;
	
	font.LoadDefault(); 
	
	CTexture::smooth=true;
	
	// enable lighting
	Light.Enable();

	// load abarlith model
	player.LoadModel("Abarlith/Abarlith.md2"); 
	player.SetScale( 3.5f);
	
	// todo: load grass model
	
	
	// todo: load box model
	
	
	// todo: load coin model
	
	
	// todo: load snake(cobra) model
	
	
	// todo: load python model
	
	
	// load screen sprite image
	screen.LoadImage("startScreen.bmp");
	screen.SetPosition(Width/2.0f, Height/2.0f);

	
	// load floor texture
	floor.LoadTexture("grass.bmp");
	floor.SetTiling(true);
	
	
	// player health bar
	hbar.SetSize(30,5);
	hbar.SetHealth(100);
	
}

	
void CMyGame::OnStartLevel(int level)
{
  // set size of the game world
  floor.SetSize(6000,6000);
  
  // todo: set box position, size and motion
  
  
  
  
  
  // player position and player health
  player.SetPosition(100,100,100); player.SetStatus(0);
  player.SetHealth(100);
  
  // todo: set python position and speed
  
  
  score=0; 
  
  // todo: clearing lists
  
    
  
  // todo: adding grass
  
 
  
  // todo: adding coins
  
  
  
  // todo: adding cobras
 
  
	
}

// Game Logic in the OnUpdate function called every frame @ 30 fps

void CMyGame::OnUpdate() 
{
	if (IsMenuMode() || IsGameOver()) return;
	
	long t =  GetTime();
	
	// --- updating models ----
	
	
	
	
	PlayerControl();
	
}

void CMyGame::PlayerControl()
{
	
	player.Update( GetTime());
	
	if (IsKeyDown(SDLK_w) || player.IsAutoMoving())
	{
      player.SetSpeed(500);
	}
	else player.SetSpeed(0);
	
	
	// collision between box and player
	if (player.HitTestFront(&box)) 
	{
		player.SetSpeed(0);
	}
	

	// todo: Add more player game logic 



		
	// play running sequence when UP key is pressed
	// otherwise play standing sequence
	
	if (IsKeyDown(SDLK_w) || player.IsAutoMoving() || player.GetSpeed() > 0)  player.PlayAnimation(40,45,7,true);
	else player.PlayAnimation(1,39,7,true);
	
    
    if (player.GetHealth() <= 0) GameOver();
	
}


//-----------------  Draw 2D overlay ----------------------
void CMyGame::OnDraw(CGraphics* g)
{
	if (IsMenuMode()) 
	{
		// draw screen here
		screen.Draw(g);
		
		 
		return;
	}
	
	// draw GTEC 3D text
	font.SetColor( CColor::Blue()); font.SetSize( 32); font.DrawText( Width-200,Height-50, "Explorer" );
	// draw score
	font.SetColor( CColor::Red()); font.DrawNumber(10,Height-50, score);
	
	// drawing the healthbar (which is a sprite object drawn in 2D)
	CVector pos=WorldToScreenCoordinate(player.GetPositionV());
	hbar.SetPosition(pos.x,pos.y+50); 
	hbar.SetHealth( player.GetHealth());
	hbar.Draw(g);
	
	
	
	// draw GAME OVER if game over
   	if (IsGameOver())
   	{
		font.SetSize(64); font.SetColor( CColor::Red()); font.DrawText( 250,300, "GAME OVER");	
	}
	
}

void CMyGame::CameraControl(CGraphics* g)
{
	// game world tilt
    float tilt=70; 
    float scale=1.0f;
	
	// ------ Global Transformation Functions -----
	
	//glTranslatef(0,-100,0);  // move game world down 
	glRotatef(tilt,1,0,0);			// tilt game world around x axis
    glScalef(scale,scale,scale);			// scaling the game world
	
	// ---- 3rd person camera setup -----
	//glRotatef( rotation,0,1,0);		// rotate game world around y axis
	glTranslatef(-player.GetX(), 0, -player.GetZ());    // position game world
		
	UpdateView();
	Light.Apply();
}


// ----------------   Draw 3D world -------------------------
void CMyGame::OnRender3D(CGraphics* g)
{
	CameraControl(g);
	
	// ------- Draw your 3D Models here ----------
	
	floor.Draw(g); // if true with grid, false without grid
	
	player.Draw(g);
	
	
	
	
	//ShowBoundingBoxes();
	ShowCoordinateSystem();
}





// called at the start of a new game - display menu here
void CMyGame::OnDisplayMenu()
{
 
}

// called when Game Mode entered
void CMyGame::OnStartGame()
{
     OnStartLevel(1);	
   
}


// called when Game is Over
void CMyGame::OnGameOver()
{
	
}

// one time termination code
void CMyGame::OnTerminate()
{
	
}

// -------    Keyboard Event Handling ------------

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_SPACE)
	{
		if (IsPaused()) ResumeGame();
		else PauseGame();

	}
	if (sym == SDLK_F2) NewGame();
  
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	
}

// -----  Mouse Events Handlers -------------

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
	CVector pos=ScreenToFloorCoordinate(x,y);
	
	player.SetDirectionAndRotationToPoint(pos.x,pos.z);
	
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{    
   if (IsMenuMode()) StartGame();
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
