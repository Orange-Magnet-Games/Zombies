#include "MyGame.h"

using namespace std;

CMyGame::CMyGame(void) { score = 0; }

CMyGame::~CMyGame(void) {}

#define RESTITUTION 0

// --------  game initialisation --------
void CMyGame::OnInitialize()
{
	srand(time(0));
	// ogro model
	zombo.LoadModel("Ogro/Ogro.md2");
	zombo.SetScale(3.5f);
	zombo.SetToFloor(0);

	// coin model
	coin.LoadModel("coin/coin.obj");
	coin.SetY(50);
	coin.SetScale(10.0f);

	// wall model
	wall.LoadModel("wal/wall2.obj");
	wall.SetScale(4.0f);
	wall.SetToFloor(0);

	// Loading graphics and sound assets
	cout << "Loading assets" << endl;
	
	font.LoadDefault(); 
	
	CTexture::smooth=true;
	
	HideMouse();
	SDL_WM_GrabInput(SDL_GRAB_ON);

	camera.scale = CVector(100, 100, 100);

	// enable lighting
	Light.Enable();

	// constructor this later
	leftGun = new BlunderBuss(CVector(-5, -10, -15));

	rightGun = new BlunderBuss(CVector(-5, -10, 15));


	// load abarlith model
	player.LoadModel("Abarlith/Abarlith.md2"); 
	player.SetScale( 3.5f);

	wall.LoadModel("wal/wall2.obj");
	wall.SetScale(12.f);

	
	// load floor texture
	floor.LoadTexture("grass.bmp");
	floor.SetTiling(true);
	floor.SetSize(10000, 10000);
	
	
	// player health bar
	hbar.SetSize(30,5);
	hbar.SetHealth(100);

	StartGame();
}


void CMyGame::OnStartLevel(int level)
{
    
	LoadLevelData("level.txt");

	player.SetPosition(0, 100, 0);
	player.SetStatus(0);
	player.SetHealth(100);
  
  
	score=0; 
  
	
}
void CMyGame::KeepInRange(float& in, float min, float max) {
	if (in > max) in = max;
	if (in < min) in = min;
}
CVector CMyGame::RotateInX(CVector in, float a) {
	return CVector(
		in.x,
		in.y * cos(a) - in.z * sin(a),
		in.y * sin(a) - in.z * cos(a)
	);
}
CVector CMyGame::RotateInY(CVector in, float a) {
	return CVector(
		in.x * cos(a) + in.z * sin(a),
		in.y,
		-in.x * sin(a) + in.z * cos(a)
	);

	/*

	From what I understand, what I applied here is a rotation matrix for the Y axis

		| cos(α)	0	sin(α) | |x|   | x cos(α) + 0 + z sin(α) |
		|	  0		1		0  | |y| = |		    y			 |
		|-sin(α)	0	cos(α) | |z|   |-x sin(α) + 0 + z cos(α) |

	credits to user "legends2k" on stackoverflow
	https://stackoverflow.com/questions/14607640/rotating-a-vector-in-3d-space

	wiki article for further explanation:
	https://en.wikipedia.org/wiki/Rotation_matrix

	further research: Gimbal Lock

	*/
}
CVector CMyGame::RotateInZ(CVector in, float a) {
	return CVector(
		in.x * cos(a) - in.y * sin(a),
		in.x * sin(a) + in.y * cos(a),
		in.z
	);
}
CVector CMyGame::RotateDirection(CVector in, float x, float y, float z) { // IN RADIANS 
	return RotateInX(RotateInY(RotateInZ(in, z), y), x);
	//until I figure out how to make one big matrix out of these this'll have to be it
} // IN RADIANS 

CVector CMyGame::RotateDirection(CVector in, CVector rotation) { // IN RADIANS 
	rotation = CVector(DEG2RAD(rotation.x), DEG2RAD(rotation.y), DEG2RAD(rotation.z));
	return RotateDirection(in, rotation.x, rotation.y, rotation.z);
} // IN RADIANS 

// Esssential Functions

bool CMyGame::IsInRange(CVector min, CVector max, CVector orig) {
	return (orig.x > min.x && orig.x < max.x) &&
		(orig.y > min.y && orig.y < max.y) &&
		(orig.z > min.z && orig.z < max.z);
}
void CMyGame::OnUpdate() 
{
	if (IsMenuMode() || IsGameOver()) return;
	
	long t =  GetTime();
	

	if (IsKeyDown(SDLK_LALT) && IsKeyDown(SDLK_F4)) ExitGame();
	
	PlayerControl(t);

	for (CModel* i : *walls) i->Update(t);
	
	for (CModel* bullet : *bullets) { 
		if (IsInRange(CVector(-6000, -20, -6000), CVector(6000, 1000, 6000), bullet->GetPositionV())) bullet->Delete();
		for (CModel* wall : *walls) if(bullet->HitTest(wall)) bullets->delete_if(deleted);
		
		bullet->Update(t); 
	}

	if(leftGun->fireTimer > 0) leftGun->fireTimer--;
	if (rightGun->fireTimer > 0) rightGun->fireTimer--;

	if (leftGun->automatic && leftGun->fireTimer <= 0) leftGun->Shoot();
	if (rightGun->automatic && rightGun->fireTimer <= 0) rightGun->Shoot();

	leftGun->model.SetRotation(camera.rotation.z, camera.rotation.y + 90, camera.rotation.x + leftGun->fireTimer);
	leftGun->model.SetPositionV(camera.position + RotateDirection(leftGun->gunOffset, CVector(camera.rotation.z, -camera.rotation.y + 90, -camera.rotation.x)));

	rightGun->model.SetRotation(camera.rotation.z, camera.rotation.y + 90, camera.rotation.x + rightGun->fireTimer);
	rightGun->model.SetPositionV(camera.position + RotateDirection(rightGun->gunOffset, CVector(camera.rotation.z, -camera.rotation.y + 90, -camera.rotation.x)));

	leftGun->Update(t);
	rightGun->Update(t);
}

void CMyGame::PlayerControl(long t)
{
	CVector dir = CVector(0, 0, 0);
	if (IsKeyDown(SDLK_w)) dir.x += 1;
	if (IsKeyDown(SDLK_s)) dir.x -= 1;
	if (IsKeyDown(SDLK_a)) dir.z -= 1;
	if (IsKeyDown(SDLK_d)) dir.z += 1;
	
	bool move = true;

	for (CModel* wall : *walls)
	{
		if (player.HitTest(wall)) {
			move = false;
			CVector amogus = player.GetPositionV() + (player.GetPositionV() - wall->GetPositionV()).Normalized();
			player.SetPosition(amogus.x, amogus.z);
		}

	}

	if (move && (IsKeyDown(SDLK_w) || IsKeyDown(SDLK_a) || IsKeyDown(SDLK_s) || IsKeyDown(SDLK_d)))
	{
		if (IsKeyDown(SDLK_LSHIFT)) player.SetSpeed(1000);
		else player.SetSpeed(500);
	}
	else player.SetSpeed(0);

	CVector point = player.GetPositionV() + RotateInY(dir, DEG2RAD(camera.rotation.y + 90));

	player.SetDirectionAndRotationToPoint(point.x, point.z);

	// play running sequence when UP key is pressed
	// otherwise play standing sequence

	if (IsKeyDown(SDLK_w) || player.IsAutoMoving() || player.GetSpeed() > 0)  player.PlayAnimation(40, 45, 7, true);
	else player.PlayAnimation(1, 39, 7, true);

	player.Update(t);

	
	camera.position = player.GetPositionV() + CVector(0, 60, 0);



    if (player.GetHealth() <= 0) GameOver();
	if (player.GetHealth() > maxHealth) player.SetHealth(maxHealth);
	
}


//-----------------  Draw 2D overlay ----------------------
void CMyGame::OnDraw(CGraphics* g)
{
	
	// draw GTEC 3D text
	// draw score
	font.SetColor( CColor::Red()); font.DrawNumber(10,Height-50, score);
	
	// drawing the healthbar (which is a sprite object drawn in 2D)
	CVector pos=WorldToScreenCoordinate(player.GetPositionV());
	
	

	hbar.SetHealth(player.GetHealth());
	hbar.SetSize(maxHealth * 2, 10);
	hbar.SetPosition(maxHealth + 10, 690);
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
	
	// ------ Global Transformation Functions -----
	//glTranslatef(0,-100,0);  // move game world down 
	glRotatef(-camera.rotation.x, 1, 0, 0);
	glRotatef(-camera.rotation.y, 0, 1, 0);
	glRotatef(-camera.rotation.z, 0, 0, 1);

    glScalef(camera.scale.x, camera.scale.y, camera.scale.z);			// scaling the game world
	
	// ---- 3rd person camera setup -----
	//glRotatef( rotation,0,1,0);		// rotate game world around y axis

	glTranslatef(-camera.position.x, -camera.position.y, -camera.position.z);  // position game world
	
	

	UpdateView();


	Light.Apply();
}

void CMyGame::ZomboControl()
{
	for (CModel* zombo : *zombos) {
		if (zombo->IsAutoMoving()) zombo->PlayAnimation(40, 45, 7, true);
		else zombo->PlayAnimation(1, 39, 7, true);

		// select a new waypoint if stopped unless we reached the last waypoint
		if (!zombo->IsAutoMoving() && zombo->GetStatus() < path.size())
		{
			// The status member variable is used to indicate which waypoint we aim for
			CVector v = path[zombo->GetStatus()];
			zombo->SetStatus(zombo->GetStatus() + 1);
			zombo->MoveTo(v.x, v.z, 100);
		}
		// we reached the last way point, re-position at the first waypoint
		if (!zombo->IsAutoMoving() && zombo->GetStatus() == path.size())
		{
			zombo->SetStatus(0);
			zombo->SetPositionV(path[0]);
		}
	}
}

void CMyGame::LoadLevelData(string filename)
{
	// Made to Be Compatible with GTEC Level Editor
	fstream myfile;
	myfile.open(filename, ios_base::in);
	int type, x, y, z, rot;
	string text;
	bool neof;
	do
	{
		myfile >> type >> x >> y >> z >> rot;

		x -= 400;
		z -= 400;

		x *= 3;
		y *= 3;
		z *= 3;

		neof = myfile.good();
		//cout << type << " " << x << " " << y << " " << z << " " << rot << endl;
		if (neof) 
		{
			switch (type) {
				case 1: //  Wall -> Wall 2.0 --- There was no change, I just made it sound fancier
				{
					CModel* wallInstance = wall.Clone(); // clone wall segment
					wallInstance->SetPosition((float)x, (float)y, (float)z);
					wallInstance->SetRotation(float(rot));
					wallInstance->SetToFloor(0);
					walls->push_back(wallInstance);
					break;
				}
  			    case 2: // Coin -> Waypoints 
				{
					path.push_back(CVector((float)x, (float)y, (float)z));
					break;
				}
				case 3: // Ogro -> Spawner
				{
					CModel* enemyInstance = zomboSpawner.Clone();
					enemyInstance->SetPosition((float)x, (float)y, (float)z); enemyInstance->SetToFloor(0);
					enemyInstance->SetRotation(float(rot)); enemyInstance->SetDirection((float)rot);
					enemyInstance->SetSpeed(100);
					zomboSpawners->push_back(enemyInstance);
					break;
				}

			}

		}
	} while (neof);
	myfile.close();

}

// ----------------   Draw 3D world -------------------------
void CMyGame::OnRender3D(CGraphics* g)
{
	CameraControl(g);
	
	// ------- Draw your 3D Models here ----------
	
	floor.Draw(g); // if true with grid, false without grid
	
	//player.Draw(g);
	leftGun->Draw(g);
	rightGun->Draw(g);

	for (CModel* i : *walls) i->Draw(g);
	
	
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
	
	if (sym == SDLK_F2) NewGame();
  
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	
}

// -----  Mouse Events Handlers -------------

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
	//CVector pos=ScreenToFloorCoordinate(x,y);
	camera.rotation += CVector(-rely, -relx, 0) * sensitivity;
	KeepInRange(camera.rotation.x, -90, 90);
	
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{    
	if (leftGun->fireTimer <= 0) {
		if (leftGun->automatic) {
			leftGun->fireTimer = leftGun->fireRate;
			for (CModel* bullet : leftGun->Shoot()) {
				bullets->push_back(bullet);
			}
		}
		else if(!leftGun->held) {
			leftGun->fireTimer = leftGun->fireRate;
			for (CModel* bullet : leftGun->Shoot()) {
				bullets->push_back(bullet);
			}
		}
		leftGun->held = true;
	}
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
	leftGun->held = false;
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
	if (rightGun->fireTimer <= 0) {
		if (rightGun->automatic) {
			rightGun->fireTimer = rightGun->fireRate;
			for (CModel* bullet : rightGun->Shoot()) {
				bullets->push_back(bullet);
			}
		}
		else if (!rightGun->held) {
			rightGun->fireTimer = rightGun->fireRate;
			for (CModel* bullet : rightGun->Shoot()) {
				bullets->push_back(bullet);
			}
		}
		rightGun->held = true;
	}
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
	rightGun->held = false;
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
