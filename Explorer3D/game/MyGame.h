#pragma once


#include "GameApp.h"
#include "Game.h"

class Gun {
public:
	CModel model;
	float fireTimer, fireRate, bulletSpeed;
	bool automatic, held, reloading = false;
	int bulletCount, magSize, bulletsInChamber;
	float bulletSpread;
	CModel bullet;
	CVector gunOffset;
	virtual void Update(float t) = 0;
	virtual void Draw(CGraphics* g) = 0;
	virtual vector<CModel*> Shoot() = 0;
};

class BlunderBuss : public virtual Gun {
public:
	BlunderBuss(CVector offset);
	BlunderBuss();

	void Update(float t);
	
	void Draw(CGraphics* g);
	vector<CModel*> Shoot();
};

class Rifle : public virtual Gun {
public:
	Rifle(CVector offset);
	Rifle();

	void Update(float t);

	void Draw(CGraphics* g);
	vector<CModel*> Shoot();
};

class Pistol : public virtual Gun {
public:
	Pistol(CVector offset);
	Pistol();

	void Update(float t);

	void Draw(CGraphics* g);
	vector<CModel*> Shoot();
};

class Crossbow : public virtual Gun {
public:
	Crossbow(CVector offset);
	Crossbow();

	void Update(float t);

	void Draw(CGraphics* g);
	vector<CModel*> Shoot();
};

class Camera {
public:
	CVector position;
	CVector rotation;
	CVector scale;

	Camera(CVector position, CVector rotation, CVector scale) :
		position(position),
		rotation(rotation),
		scale(scale)
	{}
	Camera(CVector position, CVector rotation) :
		position(position),
		rotation(rotation),
		scale(CVector(1, 1, 1))
	{}
	Camera(CVector position) :
		position(position),
		rotation(CVector(0, 0, 0)),
		scale(CVector(1, 1, 1))
	{}
	Camera() :
		position(CVector(0, 0, 0)),
		rotation(CVector(0, 0, 0)),
		scale(CVector(1, 1, 1))
	{}
};

class CMyGame : public CGame
{
private:
	static CMyGame* instance;
public:
	CMyGame();
	~CMyGame();

	// ----  Declare your game variables and objects here -------------
	Camera camera = Camera();

	int wavePause, wavePauseCount,
		waveEnemies, waveEnemiesLeft, 
		timeBetweenSpawnMax, timeBetweenSpawn,
		currentWave;
	bool waveStarted, inProgress;

	//INCREASE ENEMIES BY 1.3 EACH WAVE

	// Variables
	int money;
	static CMyGame* Game() {
		if (instance == nullptr) instance = new CMyGame();
		return instance;
	}
	int invincibility = 30;

	// Models and Model Lists
	CModelMd2 player;   // animated player model
	int zomboBrainWave;
	CModel wall;
	CModel zombo;
	CModel grass;
	CModel blood[5] = {};
	CModelList* grasses = new CModelList();
	CModelList* zombos = new CModelList();
	CModelList* bullets = new CModelList();
	CModelList* particles = new CModelList();
	CModel box;
	CModelList* walls = new CModelList();
	CVector path[8] = {};

	int currentBoxGun;
	CVector leftOffset = CVector(-5, -10, -15), 
		rightOffset = CVector(-5, -10, 15);

	float maxHealth = 100;

	float sensitivity = 0.25;
	// game world floor
	CFloor floor;
	
	// health indicator
	CHealthBar hbar;
	
	// Font
	CFont font;

	// -----   Add you member functions here ------

	static void KeepInRange(float& in, float min, float max);
	static CVector RotateInX(CVector in, float a);
	static CVector RotateInY(CVector in, float a);
	static CVector RotateInZ(CVector in, float a);
	static CVector RotateDirection(CVector in, float x, float y, float z);

	static CVector RotateDirection(CVector in, CVector rotation);

	

	Gun* leftGun = nullptr, *rightGun = nullptr;
	// Esssential Functions
   
	bool IsInRange(CVector min, CVector max, CVector orig);

	void LoadLevelData(string filename);

	void PlayerControl(long t);

	void ZomboControl();

	void GunControl(long t);

	void SpawnControl();
   
	void CameraControl(CGraphics* g);

	// Extension to Engine

	
	// ---------------------Event Handling --------------------------

	// Game Loop Funtions
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);
	virtual void OnRender3D(CGraphics* g);

	// Game Life Cycle
	virtual void OnInitialize();
	virtual void OnDisplayMenu();
	virtual void OnStartLevel(int level);
	virtual void OnStartGame();
	virtual void OnGameOver();
	virtual void OnTerminate();

	// Keyboard Event Handlers
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);

	// Mouse Events Handlers
	virtual void OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle);
	virtual void OnLButtonDown(Uint16 x,Uint16 y);
	virtual void OnLButtonUp(Uint16 x,Uint16 y);
	virtual void OnRButtonDown(Uint16 x,Uint16 y);
	virtual void OnRButtonUp(Uint16 x,Uint16 y);
	virtual void OnMButtonDown(Uint16 x,Uint16 y);
	virtual void OnMButtonUp(Uint16 x,Uint16 y);
};
