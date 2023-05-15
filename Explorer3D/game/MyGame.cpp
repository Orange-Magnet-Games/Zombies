#include "MyGame.h"

using namespace std;

CMyGame* CMyGame::instance = nullptr;

CMyGame::CMyGame(void) { score = 0; }

CMyGame::~CMyGame(void) {}

// --------  game initialisation --------
void CMyGame::OnInitialize()
{
	srand(time(0));
	// ogro model
	zombo.LoadModel("Enemies/placeholder.obj");
	zombo.SetScale(40.0f);
	zombo.SetHealth(100);
	zombo.SetToFloor(0);

	// coin model
	coin.LoadModel("coin/coin.obj");
	coin.SetY(50);
	coin.SetScale(10.f);

	// wall model
	wall.LoadModel("wal/wall2.obj");
	wall.SetScale(4.f);
	wall.SetToFloor(0); 

	grass.LoadModel("grass/grasse.obj");
	grass.LoadTexture("grass/grasse.png");
	grass.SetScale(.5f);
	grass.SetToFloor(0);

	// Loading graphics and sound assets
	cout << "Loading assets" << endl;
	
	font.LoadDefault(); 
	
	CTexture::smooth=true;
	
	HideMouse();
	SDL_WM_GrabInput(SDL_GRAB_ON);

	camera.scale = CVector(100, 100, 100);

	// enable lighting
	Light.Enable();

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

// BLUNDER BUSSSYYY '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

BlunderBuss::BlunderBuss(CVector offset) : BlunderBuss() {
	this->gunOffset = offset;
}

BlunderBuss::BlunderBuss() {
	fireTimer = 0;
	fireRate = 60;
	bulletSpeed = 2000;
	automatic = false;
	held = false;
	bulletCount = 5;
	bulletSpread = 30;
	model.LoadModel("Guns/blunger.md2");
	model.LoadTexture("Guns/blunger.png");
	model.SetScale(3.5f);

	bullet.LoadModel("Guns/Bullets/smocke.obj");
	bullet.LoadTexture("Guns/Bullets/smocke.png");
	bullet.SetScale(25.f);
	gunOffset = CVector(0, 0, 0);
}

void BlunderBuss::Update(float t) {
	model.SetRotation(CMyGame::Game()->camera.rotation.z, CMyGame::Game()->camera.rotation.y + 90, CMyGame::Game()->camera.rotation.x + fireTimer);
	model.SetPositionV(CMyGame::Game()->camera.position + CMyGame::RotateDirection(gunOffset, CVector(CMyGame::Game()->camera.rotation.z, -CMyGame::Game()->camera.rotation.y + 90, -CMyGame::Game()->camera.rotation.x)));
	model.Update(t);
}

void BlunderBuss::Draw(CGraphics* g) {
	model.Draw(g);
}

vector<CModel*> BlunderBuss::Shoot() {
	vector<CModel*> bullets = vector<CModel*>();
	for (int i = 0; i < bulletCount; i++) {
		CModel* bullet = this->bullet.Clone();
		CVector bruh = model.GetRotationV();
		bullet->SetPositionV(model.GetPositionV());
		float x = DEG2RAD(rand() % (int)bulletSpread - bulletSpread / 2);
		float y = DEG2RAD(rand() % (int)bulletSpread - bulletSpread / 2);
		float z = DEG2RAD(rand() % (int)bulletSpread - bulletSpread / 2);
		bruh += CVector(x, y, z);
		cout << bruh.x << " " << bruh.y << " " << bruh.z << endl;
		bullet->SetDirectionV(bruh);
		bullet->SetSpeed(bulletSpeed);
		bullet->SetOmega(x * 500, y * 500, z * 500);
		bullets.push_back(bullet);

	}
	return bullets;
}

// RIFLEEEEEEEEEEEEEE '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Rifle::Rifle(CVector offset) : Rifle() {
	this->gunOffset = offset;
}

Rifle::Rifle() {
	fireTimer = 0;
	fireRate = 10;
	bulletSpeed = 5000;
	automatic = false;
	held = false;
	bulletCount = 1;
	bulletSpread = 5;
	//model.LoadModel("Guns/blunger.md2");
	//model.LoadTexture("Guns/blunger.png");
	model.SetScale(3.5f);

	//bullet.LoadModel("Guns/Bullets/smocke.obj");
	//bullet.LoadTexture("Guns/Bullets/smocke.png");
	bullet.SetScale(25.f);
	gunOffset = CVector(0, 0, 0);
}

void Rifle::Update(float t) {
	model.SetRotation(CMyGame::Game()->camera.rotation.z, CMyGame::Game()->camera.rotation.y + 90, CMyGame::Game()->camera.rotation.x + fireTimer);
	model.SetPositionV(CMyGame::Game()->camera.position + CMyGame::RotateDirection(gunOffset, CVector(CMyGame::Game()->camera.rotation.z, -CMyGame::Game()->camera.rotation.y + 90, -CMyGame::Game()->camera.rotation.x)));
	model.Update(t);
}

void Rifle::Draw(CGraphics* g) {
	model.Draw(g);
}

vector<CModel*> Rifle::Shoot() {
	vector<CModel*> bullets = vector<CModel*>();
	for (int i = 0; i < bulletCount; i++) {
		CModel* bullet = this->bullet.Clone();
		CVector bruh = model.GetRotationV();
		bullet->SetPositionV(model.GetPositionV());
		float x = DEG2RAD(rand() % (int)bulletSpread - bulletSpread / 2);
		float y = DEG2RAD(rand() % (int)bulletSpread - bulletSpread / 2);
		float z = DEG2RAD(rand() % (int)bulletSpread - bulletSpread / 2);
		bruh += CVector(x, y, z);
		cout << bruh.x << " " << bruh.y << " " << bruh.z << endl;
		bullet->SetDirectionV(bruh);
		bullet->SetSpeed(bulletSpeed);
		bullet->SetOmega(x * 500, y * 500, z * 500);
		bullets.push_back(bullet);

	}
	return bullets;
}

// PISTOLIEEEEEEEEEEE ''''''''''''''''''''''''''''''''''''''''''''''''''

Pistol::Pistol(CVector offset) : Pistol() {
	this->gunOffset = offset;
}

Pistol::Pistol() {
	fireTimer = 0;
	fireRate = 30;
	bulletSpeed = 5000;
	automatic = false;
	held = false;
	bulletCount = 1;
	bulletSpread = 2;
	//model.LoadModel("Guns/blunger.md2");
	//model.LoadTexture("Guns/blunger.png");
	model.SetScale(3.5f);

	//bullet.LoadModel("Guns/Bullets/smocke.obj");
	//bullet.LoadTexture("Guns/Bullets/smocke.png");
	bullet.SetScale(25.f);
	gunOffset = CVector(0, 0, 0);
}

void Pistol::Update(float t) {
	model.SetRotation(CMyGame::Game()->camera.rotation.z, CMyGame::Game()->camera.rotation.y + 90, CMyGame::Game()->camera.rotation.x + fireTimer);
	model.SetPositionV(CMyGame::Game()->camera.position + CMyGame::RotateDirection(gunOffset, CVector(CMyGame::Game()->camera.rotation.z, -CMyGame::Game()->camera.rotation.y + 90, -CMyGame::Game()->camera.rotation.x)));
	model.Update(t);
}

void Pistol::Draw(CGraphics* g) {
	model.Draw(g);
}

vector<CModel*> Pistol::Shoot() {
	vector<CModel*> bullets = vector<CModel*>();
	for (int i = 0; i < bulletCount; i++) {
		CModel* bullet = this->bullet.Clone();
		CVector bruh = model.GetRotationV();
		bullet->SetPositionV(model.GetPositionV());
		float x = DEG2RAD(rand() % (int)bulletSpread - bulletSpread / 2);
		float y = DEG2RAD(rand() % (int)bulletSpread - bulletSpread / 2);
		float z = DEG2RAD(rand() % (int)bulletSpread - bulletSpread / 2);
		bruh += CVector(x, y, z);
		cout << bruh.x << " " << bruh.y << " " << bruh.z << endl;
		bullet->SetDirectionV(bruh);
		bullet->SetSpeed(bulletSpeed);
		bullet->SetOmega(x * 500, y * 500, z * 500);
		bullets.push_back(bullet);

	}
	return bullets;
}

// CROBBOOOOOO '''''''''''''''''''''''''''''''''''''''''''''''''''''

Crossbow::Crossbow(CVector offset) : Crossbow() {
	this->gunOffset = offset;
}

Crossbow::Crossbow() {
	fireTimer = 0;
	fireRate = 60;
	bulletSpeed = 4000;
	automatic = false;
	held = false;
	bulletCount = 1;
	bulletSpread = 1;
	//model.LoadModel("Guns/blunger.md2");
	//model.LoadTexture("Guns/blunger.png");
	model.SetScale(3.5f);

	//bullet.LoadModel("Guns/Bullets/smocke.obj");
	//bullet.LoadTexture("Guns/Bullets/smocke.png");
	bullet.SetScale(25.f);
	gunOffset = CVector(0, 0, 0);
}

void Crossbow::Update(float t) {
	model.SetRotation(CMyGame::Game()->camera.rotation.z, CMyGame::Game()->camera.rotation.y + 90, CMyGame::Game()->camera.rotation.x + fireTimer);
	model.SetPositionV(CMyGame::Game()->camera.position + CMyGame::RotateDirection(gunOffset, CVector(CMyGame::Game()->camera.rotation.z, -CMyGame::Game()->camera.rotation.y + 90, -CMyGame::Game()->camera.rotation.x)));
	model.Update(t);
}

void Crossbow::Draw(CGraphics* g) {
	model.Draw(g);
}

vector<CModel*> Crossbow::Shoot() {
	vector<CModel*> bullets = vector<CModel*>();
	for (int i = 0; i < bulletCount; i++) {
		CModel* bullet = this->bullet.Clone();
		CVector bruh = model.GetRotationV();
		bullet->SetPositionV(model.GetPositionV());
		float x = DEG2RAD(rand() % (int)bulletSpread - bulletSpread / 2);
		float y = DEG2RAD(rand() % (int)bulletSpread - bulletSpread / 2);
		float z = DEG2RAD(rand() % (int)bulletSpread - bulletSpread / 2);
		bruh += CVector(x, y, z);
		cout << bruh.x << " " << bruh.y << " " << bruh.z << endl;
		bullet->SetDirectionV(bruh);
		bullet->SetSpeed(bulletSpeed);
		bullet->SetOmega(x * 500, y * 500, z * 500);
		bullets.push_back(bullet);

	}
	return bullets;
}

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
	for (CModel* i : *grasses) {
		i->SetRotationToPoint(player.GetPositionV().x, player.GetPositionV().z);
		i->SetRotation(i->GetRotation() + 90);
		i->Update(t);
	}
	
	bullets->delete_if(deleted);
	for (CModel* bullet : *bullets) {
		//if (IsInRange(CVector(-6000, -20, -6000), CVector(6000, 1000, 6000), bullet->GetPositionV())) bullet->Delete();
		for (CModel* wall : *walls) if(bullet->HitTest(wall)) bullet->Delete();
		
		bullet->Update(t); 
	}

	if(leftGun->fireTimer > 0) leftGun->fireTimer--;
	if (rightGun->fireTimer > 0) rightGun->fireTimer--;

	if (leftGun->automatic && leftGun->fireTimer <= 0) leftGun->Shoot();
	if (rightGun->automatic && rightGun->fireTimer <= 0) rightGun->Shoot();


	leftGun->Update(t);
	rightGun->Update(t);

	zombos->delete_if(deleted);
	for (CModel* zombo : *zombos) {
		for (CModel* bullet : *bullets){
			if (bullet->HitTest(zombo)) {
				zombo->SetHealth(zombo->GetHealth() - 25);
				if (zombo->GetHealth() <= 0) zombo->Delete();
				bullet->Delete();

				cout << "HITTTTTTTTTTTT" << endl;
			}
		}
		zombo->Update(t);
	}
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
	
	for (auto zombar : zombars) delete zombar;
	zombars.clear();

	for (CModel* zombo : *zombos) {
		CHealthBar* zombar = new CHealthBar();
		//zombar->SetHealth(zombo->GetHealth());
		zombar->SetSize(zombo->GetHealth() * 2, 10);
		CVector pos = WorldToScreenCoordinate(zombo->GetPositionV() + CVector(0, 200, 0));
		zombar->SetPosition(pos.x, pos.y);
		zombar->Draw(g);
		zombars.push_back(zombar);
	}

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
	for (CModel* i : *grasses) i->Draw(g);
	for (CModel* i : *zombos) i->Draw(g);
	for (CModel* i : *bullets) i->Draw(g);
	
	
	//ShowBoundingBoxes();
	//ShowCoordinateSystem();
}





// called at the start of a new game - display menu here
void CMyGame::OnDisplayMenu()
{
 
}

// called when Game Mode entered
void CMyGame::OnStartGame()
{
     OnStartLevel(1);	
	 for (int i = 0; i < 1500; i++) {
		 grasses->push_back(grass.Clone());
		 grasses->back()->SetPosition(rand() % 8000 - 3000, rand() % 8000 - 3000);
	 }

	 auto p = zombo.Clone();
	 p->SetPosition(player.GetX() + 50, player.GetZ());
	 zombos->push_back(p);
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
