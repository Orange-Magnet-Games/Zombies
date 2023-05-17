#include "MyGame.h"
using namespace std;

CMyGame* CMyGame::instance = nullptr;

CMyGame::CMyGame(void) { money = 0; }

CMyGame::~CMyGame(void) {}

// --------  game initialisation --------
void CMyGame::OnInitialize()
{
	srand(time(0));


	wavePause = 15 * 60;
	wavePauseCount = 0;
	waveEnemies = 10;
	waveEnemiesLeft = waveEnemies;
	timeBetweenSpawnMax = 30;
	timeBetweenSpawn = 0;
	currentWave = 1;
	waveStarted = true;

	currentBoxGun = rand() % 4 + 1;

	// ogro model
	zombo.LoadModel("Enemies/foap.obj");
	zombo.LoadTexture("Enemies/foap.png");
	zombo.SetScale(20);
	zombo.SetHealth(100);
	zombo.SetToFloor(0);

	box.LoadModel("box/box.obj");
	box.LoadTexture("box/box.bmp");
	box.SetScale(5.0f);
	box.SetToFloor(0);
	box.SetPosition(1700, 1700);

	Light.SetAmbientLight(0, 0, 0);
	//Light.SetDiffuseLight(0, 0, 0);

	// blood model
	blood[0].LoadModel("Particles/meat 1.obj");
	blood[0].SetScale(20.0f);

	blood[1].LoadModel("Particles/meat 2.obj");
	blood[1].SetScale(20.0f);

	blood[2].LoadModel("Particles/meat 3.obj");
	blood[2].SetScale(20.0f);

	blood[3].LoadModel("Particles/meat 4.obj");
	blood[3].SetScale(20.0f);

	blood[4].LoadModel("Particles/meat 5.obj");
	blood[4].SetScale(20.0f);

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
	
	CTexture::smooth=false;
	
	HideMouse();
	SDL_WM_GrabInput(SDL_GRAB_ON);

	camera.scale = CVector(100, 100, 100);

	// enable lighting
	Light.Enable();

	leftGun = new Pistol(CVector(-5, -10, -15));

	//rightGun = new Rifle(CVector(-5, -10, 15));


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
  
  
	money=0; 
  
	
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
	magSize = 2;
	bulletsInChamber = magSize;
	fireTimer = 0;
	fireRate = 60;
	bulletSpeed = 2000;
	automatic = false;
	held = false;
	bulletCount = 5;
	bulletSpread = 30;
	model.LoadModel("Guns/blunger.obj");
	model.LoadTexture("Guns/blunger.png");
	model.SetScale(3.5f);

	bullet.LoadModel("Guns/Bullets/smocke.obj");
	bullet.LoadTexture("Guns/Bullets/smocke.png");
	bullet.SetScale(25.f);
	bullet.SetHealth(20);
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
		//cout << bruh.x << " " << bruh.y << " " << bruh.z << endl;
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
	magSize = 30;
	bulletsInChamber = magSize;
	fireTimer = 0;
	fireRate = 10;
	bulletSpeed = 5000;
	automatic = true;
	held = false;
	bulletCount = 1;
	bulletSpread = 5;
	model.LoadModel("Guns/rifle.obj");
	model.LoadTexture("Guns/blunger.png");
	model.SetScale(3.5f);

	bullet.LoadModel("Guns/Bullets/Tracer.obj");
	bullet.LoadTexture("Guns/Bullets/yello.png");
	bullet.SetScale(2.f);
	bullet.SetHealth(20);
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
		//cout << bruh.x << " " << bruh.y << " " << bruh.z << endl;
		bullet->SetDirectionV(bruh);
		bullet->SetRotationV(bruh);
		bullet->SetSpeed(bulletSpeed);
		bullets.push_back(bullet);

	}
	return bullets;
}

// PISTOLIEEEEEEEEEEE ''''''''''''''''''''''''''''''''''''''''''''''''''

Pistol::Pistol(CVector offset) : Pistol() {
	this->gunOffset = offset;
}

Pistol::Pistol() {
	magSize = 10;
	bulletsInChamber = magSize;
	fireTimer = 0;
	fireRate = 10;
	bulletSpeed = 10000;
	automatic = false;
	held = false;
	bulletCount = 1;
	bulletSpread = 2;
	model.LoadModel("Guns/Pistolie.obj");
	model.LoadTexture("Guns/blunger.png");
	model.SetScale(3.5f);

	bullet.LoadModel("Guns/Bullets/Tracer.obj");
	bullet.LoadTexture("Guns/Bullets/yello.png");
	bullet.SetScale(15.f);
	bullet.SetHealth(20);
	gunOffset = CVector(0, 0, 0);
}

void Pistol::Update(float t) {
	model.SetRotation(CMyGame::Game()->camera.rotation.z, CMyGame::Game()->camera.rotation.y + 90, CMyGame::Game()->camera.rotation.x + fireTimer * 3);
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
		//cout << bruh.x << " " << bruh.y << " " << bruh.z << endl;
		bullet->SetDirectionV(bruh);
		bullet->SetRotationV(bruh);
		bullet->SetSpeed(bulletSpeed);
		bullets.push_back(bullet);

	}
	return bullets;
}

// CROBBOOOOOO '''''''''''''''''''''''''''''''''''''''''''''''''''''

Crossbow::Crossbow(CVector offset) : Crossbow() {
	this->gunOffset = offset;
}

Crossbow::Crossbow() {
	magSize = 1;
	bulletsInChamber = magSize;
	fireTimer = 0;
	fireRate = 60;
	bulletSpeed = 2000;
	automatic = false;
	held = false;
	bulletCount = 1;
	bulletSpread = 1;
	model.LoadModel("Guns/crobbo.obj");
	model.LoadTexture("Guns/blunger.png");
	model.SetScale(3.5f);

	bullet.LoadModel("Guns/Bullets/Arrow.obj");
	bullet.LoadTexture("Guns/blunger.png");
	bullet.SetScale(5.f);
	bullet.SetHealth(100);
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
		float y = DEG2RAD(rand() % (int)bulletSpread - bulletSpread / 2) + DEG2RAD(5);
		float z = DEG2RAD(rand() % (int)bulletSpread - bulletSpread / 2);
		bruh += CVector(x, y, z);
		//cout << bruh.x << " " << bruh.y << " " << bruh.z << endl;
		bullet->SetDirectionV(bruh);
		bullet->SetRotationV(bruh);
		bullet->SetSpeed(bulletSpeed);
		bullet->SetStatus(2);
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
	if (IsKeyDown(SDLK_LALT) && IsKeyDown(SDLK_F4)) ExitGame();

	if (IsMenuMode() || IsGameOver()) return;
	

	SpawnControl();

	long t =  GetTime();
	
	
	PlayerControl(t);

	GunControl(t);

	for (CModel* i : *walls) i->Update(t);
	for (CModel* i : *grasses) {
		i->SetRotationToPoint(player.GetPositionV().x, player.GetPositionV().z);
		i->SetRotation(i->GetRotation() + 90);
		i->Update(t);
	}
	
	

	zombos->delete_if(deleted);
	zomboBrainWave--;
	if (zomboBrainWave <= 0) { 
		ZomboControl(); 
		zomboBrainWave = 30;
	}
	invincibility--;
	if (invincibility <= -120) player.SetHealth(player.GetHealth() + 1);
	for (CModel* zombo : *zombos) {
		if (invincibility <= 0) {
			if (zombo->HitTest(&player)) {
				player.SetHealth(player.GetHealth() - 25);
				invincibility = 30;
			}
		}
		for (CModel* bullet : *bullets){
			if (bullet->HitTest(zombo)) {
				zombo->SetHealth(zombo->GetHealth() - bullet->GetHealth());
				bullet->Delete();
			}
		}
		if (zombo->GetHealth() <= 0) {
			for (int i = 0; i < 20; i++) {
				auto p = blood[rand() % 5].Clone();
				p->SetPositionV(zombo->GetPositionV());
				CVector pVel = CVector(0, 1, 0) * 1000;
				float x = DEG2RAD(rand() % 60 - 30);
				float y = DEG2RAD(rand() % 60 - 30);
				float z = DEG2RAD(rand() % 60 - 30);
				pVel = RotateDirection(pVel, x, y, z);
				p->SetVelocityV(pVel);
				p->SetOmegaV(CVector(x, y, z) * 1000);
				p->SetColor(CColor(rand() % 204 + 50, 0, 0));
				particles->push_back(p);
			}
			money++;
			zombo->Delete();

		}
		if (zombo->IsAutoMoving()) zombo->PlayAnimation(40, 45, 7, true);
		else zombo->PlayAnimation(1, 39, 7, true);
		zombo->Update(t);
	}

	for (CModel* p : *particles) {
		p->Update(t);
		p->SetYVelocity(p->GetYVelocity() - 20);
		if (p->GetY() < -100) p->Delete();
	}
	particles->delete_if(deleted);
}

void CMyGame::GunControl(long t) {


	bullets->delete_if(deleted);
	for (CModel* bullet : *bullets) {
		//if (IsInRange(CVector(-6000, -20, -6000), CVector(6000, 1000, 6000), bullet->GetPositionV())) bullet->Delete();
		if (bullet->GetY() > 1000 || bullet->GetY() < -100) bullet->Delete();
		else if (bullet->GetX() > 7000 || bullet->GetX() < -1000) bullet->Delete();
		else for (CModel* wall : *walls) if (bullet->HitTest(wall)) bullet->Delete();
		if (bullet->GetStatus() == 2) bullet->SetYVelocity(bullet->GetYVelocity() - 10);
		bullet->Update(t);
	}
	if (leftGun != nullptr) {
		if (leftGun->fireTimer > 0) leftGun->fireTimer--;
		else if (leftGun->reloading) {
			leftGun->bulletsInChamber = leftGun->magSize;
			leftGun->reloading = false;
		}
		else if (leftGun->automatic && leftGun->held) {
			leftGun->fireTimer = leftGun->fireRate;
			for (CModel* bullet : leftGun->Shoot()) {
				bullets->push_back(bullet);
			}
			if (leftGun->magSize > 1) {
				leftGun->bulletsInChamber--;
				if (leftGun->bulletsInChamber <= 0) {
					if (leftGun->automatic) leftGun->fireTimer = leftGun->fireRate * 10;
					else leftGun->fireTimer = leftGun->fireRate * 5;
					leftGun->reloading = true;
				}
			}
		}
		if (leftGun->automatic && leftGun->fireTimer <= 0) leftGun->Shoot();
		leftGun->Update(t);
	}
	if (rightGun != nullptr) {
		if (rightGun->fireTimer > 0) rightGun->fireTimer--;
		else if (rightGun->reloading) {
			rightGun->bulletsInChamber = rightGun->magSize;
			rightGun->reloading = false;
		}
		else if (rightGun->automatic && rightGun->held) {
			rightGun->fireTimer = rightGun->fireRate;
			for (CModel* bullet : rightGun->Shoot()) {
				bullets->push_back(bullet);
			}
			if (rightGun->magSize > 1) {
				rightGun->bulletsInChamber--;
				if (rightGun->bulletsInChamber <= 0) {
					if (rightGun->automatic) rightGun->fireTimer = rightGun->fireRate * 10;
					else rightGun->fireTimer = rightGun->fireRate * 5;
					rightGun->reloading = true;
				}
			}
		}
		if (rightGun->automatic && rightGun->fireTimer <= 0) rightGun->Shoot();
		rightGun->Update(t);
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
	font.SetColor(CColor::Red()); font.DrawNumber(10, Height - 100, money);

	// drawing the healthbar (which is a sprite object drawn in 2D)
	CVector pos = WorldToScreenCoordinate(player.GetPositionV());


	/*for (CModel* zombo : *zombos) {

		CVector pos = WorldToScreenCoordinate(zombo->GetPositionV() + CVector(0, 200, 0)); //THIS FUNCTION IS BROKEN AND SHOWS 2 COORDINATES
		CHealthBar* zombar = new CHealthBar();
		//zombar->SetHealth(zombo->GetHealth());
		zombar->SetSize(zombo->GetHealth() * 2, 10);
		zombar->SetPosition(pos.x, pos.y);
		zombar->Draw(g);
	}*/

	hbar.SetHealth(player.GetHealth());
	hbar.SetSize(maxHealth * 2, 10);
	hbar.SetPosition(maxHealth + 10, 690);
	hbar.Draw(g);

	if (leftGun != nullptr) if (leftGun->reloading) { font.SetSize(30); font.SetColor(CColor::White()); font.DrawText(50, 20, "Reloading!"); }
	if (rightGun != nullptr) if (rightGun->reloading) { font.SetSize(30); font.SetColor(CColor::White()); font.DrawText(750, 20, "Reloading!"); }

	string lmao;
	if (leftGun != nullptr) {
		lmao = to_string(leftGun->bulletsInChamber) + "\\" + to_string(leftGun->magSize);
		font.SetSize(64); if (leftGun->reloading) font.SetColor(CColor::Red()); else font.SetColor(CColor::White()); font.DrawText(25, 50, lmao);
	}
	if (rightGun != nullptr) {
		lmao = to_string(rightGun->bulletsInChamber) + "\\" + to_string(rightGun->magSize);
		font.SetSize(64); if (rightGun->reloading) font.SetColor(CColor::Red()); else font.SetColor(CColor::White()); font.DrawText(725, 50, lmao);
	}
	if (inProgress) {
		font.SetSize(64); font.SetColor(CColor::Red()); font.DrawText(10, Height - 200, "WAVE " + to_string(currentWave));
	}
	else {
		font.SetSize(64); font.SetColor(CColor::Red()); font.DrawText(10, Height - 200, "NEXT WAVE IN " + to_string((int)(wavePauseCount/60)) + " SECONDS");
	}
	if (player.GetPositionV().Distance(box.GetPositionV()) < 500) {
		switch (currentBoxGun) {
			case 1: lmao = "Pistol"; break;
			case 2: lmao = "Rifle"; break;
			case 3: lmao = "Crossbow"; break;
			case 4: lmao = "Blunderbuss"; break;
		}
		font.SetSize(32); font.SetColor(CColor::Red()); font.DrawText(0, 300, "CURRENT GUN: " + lmao); 
		font.DrawText(0, 250, "Price: 15");
		font.DrawText(0, 200, "Press Q or E to Buy");
	}
	if (IsGameOver()) {
		font.SetSize(128); font.SetColor(CColor::Red()); font.DrawText(100, 300, "GAME OVER");
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

void CMyGame::SpawnControl() {
	if (waveStarted) {
		if (timeBetweenSpawn > 0) timeBetweenSpawn--;
		else {
			if (waveEnemiesLeft > 0) {
				//SPAWN ENEMY
				inProgress = true;
				int i = rand() % 8;
				CModel* p = zombo.Clone();
				p->SetStatus(i);
				p->SetPositionV(path[i]);
				p->SetToFloor(0);
				zombos->push_back(p);

				timeBetweenSpawn = timeBetweenSpawnMax;
				waveEnemiesLeft--;
			}
			else if (zombos->size() <= 0) {
				waveStarted = false;

				inProgress = false;
				wavePauseCount = wavePause;
			}
		}
	}
	else if (wavePauseCount > 0) wavePauseCount--;
	else {
		cout << "WAVE STARTED" << endl;
		waveStarted = true;
		waveEnemies *= 1.3;
		waveEnemiesLeft = waveEnemies;
		currentWave++;
	}
}

void CMyGame::ZomboControl()
{


	for (CModel* zombo : *zombos) {

		/*CLine ray;
		ray.SetPositionV(zombo->GetPositionV(), player.GetPositionV());
		CVector d = player.GetPositionV() - zombo->GetPositionV();
		bool LOS = true;
		if (d.Dot(zombo->GetDirectionV()) > 0) {
			for (CModel* wall : *walls) {
				if (ray.HitTest(wall)) { LOS = false; break; } //WHY IS LINE HITTESTING SO SLOWWWWW
			}
		}

		if (LOS) {
			zombo->MoveTo(player.GetPositionV().x, player.GetPositionV().z, 500);
		}
		else {
			if (!zombo->IsAutoMoving() && zombo->GetStatus() < 8)
			{
				CVector v = path[zombo->GetStatus()];
				zombo->SetStatus(zombo->GetStatus() + 1);
				zombo->MoveTo(v.x, v.z, 500);
			}

			if (!zombo->IsAutoMoving() && zombo->GetStatus() >= 8)
			{
				zombo->SetStatus(0);
			}
		}
		*/
		// I HAD TO COMMENT ALL OF THIS OUT BECAUSE OF HOW FUCKING SLOW THIS ENGINE IS I HOPE YOU KNOW THAT
		zombo->MoveTo(player.GetPositionV().x, player.GetPositionV().z, 500);
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
	
	floor.Draw(g);
	box.Draw(g);
	
	//player.Draw(g);
	if (leftGun != nullptr) leftGun->Draw(g);
	if (rightGun != nullptr) rightGun->Draw(g);

	for (CModel* i : *walls) i->Draw(g);
	for (CModel* i : *grasses) i->Draw(g);
	for (CModel* i : *zombos) i->Draw(g);
	for (CModel* i : *bullets) i->Draw(g);
	for (CModel* p : *particles) p->Draw(g);
	
	
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

	 path[0] = CVector(-300, 0, -300);
	 path[1] = CVector(-300, 0, 4000);
	 path[2] = CVector(1700, 0, 4000);
	 path[3] = CVector(1700, 0, 1700);
	 path[4] = CVector(1700, 0, -300);
	 path[5] = CVector(4000, 0, -300);
	 path[6] = CVector(4000, 0, 4000);
	 path[7] = CVector(-300, 0, 4000);
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
	if (sym == SDLK_e && player.GetPositionV().Distance(box.GetPositionV()) < 500 && money >= 15) {
		switch (currentBoxGun) {
			case 1: rightGun = new Pistol(rightOffset); break;
			case 2: rightGun = new Rifle(rightOffset); break;
			case 3: rightGun = new Crossbow(rightOffset); break;
			case 4: rightGun = new BlunderBuss(rightOffset); break;
		}
		money -= 15;
		currentBoxGun = rand() % 4 + 1;
	}

	if (sym == SDLK_q && player.GetPositionV().Distance(box.GetPositionV()) < 500 && money >= 15) {
		switch (currentBoxGun) {
			case 1: leftGun = new Pistol(leftOffset); break;
			case 2: leftGun = new Rifle(leftOffset); break;
			case 3: leftGun = new Crossbow(leftOffset); break;
			case 4: leftGun = new BlunderBuss(leftOffset); break;
		}
		money -= 15;
		currentBoxGun = rand() % 4 + 1;
	}
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
	if (leftGun != nullptr) {
		if (!leftGun->automatic) {
			if (leftGun->fireTimer <= 0) {

				if (!leftGun->held) {
					leftGun->fireTimer = leftGun->fireRate;
					for (CModel* bullet : leftGun->Shoot()) {
						bullets->push_back(bullet);
					}
				}
				leftGun->held = true;
				if (leftGun->magSize > 1) {
					leftGun->bulletsInChamber--;
					if (leftGun->bulletsInChamber <= 0) {
						if (leftGun->automatic) leftGun->fireTimer = leftGun->fireRate * 10;
						else leftGun->fireTimer = leftGun->fireRate * 5;
						leftGun->reloading = true;
					}
				}
			}
		}
		else leftGun->held = true;
	}
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
	if (leftGun != nullptr) leftGun->held = false;
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
	if (rightGun != nullptr) {
		if (!rightGun->automatic) {
			if (rightGun->fireTimer <= 0) {
				if (!rightGun->held && !rightGun->automatic) {
					rightGun->fireTimer = rightGun->fireRate;
					for (CModel* bullet : rightGun->Shoot()) {
						bullets->push_back(bullet);
					}
				}
				rightGun->held = true;
				if (rightGun->magSize > 1) {
					rightGun->bulletsInChamber--;
					if (rightGun->bulletsInChamber <= 0) {
						if (rightGun->automatic) rightGun->fireTimer = rightGun->fireRate * 10;
						else rightGun->fireTimer = rightGun->fireRate * 5;
						rightGun->reloading = true;
					}
				}
			}
		}
		else rightGun->held = true;
	}
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
	if (rightGun != nullptr) rightGun->held = false;
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
