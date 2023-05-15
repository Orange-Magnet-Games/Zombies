#pragma once


#include "Game.h"

class Gun {
public:
	CModelMd2 model;
	float fireTimer, fireRate, bulletSpeed;
	bool automatic, held;
	int bulletCount;
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
	// Variables
	int score;
	static CMyGame* Game() {
		if (instance == nullptr) instance = new CMyGame();
		return instance;
	}

	// Models and Model Lists
	CModelMd2 player;   // animated player model

	CModel wall;
	CModel zombo;
	CModel grass;
	CModelList* grasses = new CModelList();
	CModelList* zombos = new CModelList();
	CModelList* bullets = new CModelList();
	CModel zomboSpawner;
	CModel coin;
	CModel box;
	CModelList* zomboSpawners = new CModelList();;
	CModelList* walls = new CModelList();;
	vector<CVector> path;
	
	float maxHealth = 100;

	float sensitivity = 0.25;
	// game world floor
	CFloor floor;
	
	// health indicator
	CHealthBar hbar;
	vector<CHealthBar*> zombars;
	
	// Font
	CFont font;

	// -----   Add you member functions here ------

	static void KeepInRange(float& in, float min, float max);
	static CVector RotateInX(CVector in, float a);
	static CVector RotateInY(CVector in, float a);
	static CVector RotateInZ(CVector in, float a);
	static CVector RotateDirection(CVector in, float x, float y, float z);

	static CVector RotateDirection(CVector in, CVector rotation);

	

	vector<Gun*> guns;
	Gun* leftGun, * rightGun;
	// Esssential Functions
   
	bool IsInRange(CVector min, CVector max, CVector orig);

	void LoadLevelData(string filename);

	void PlayerControl(long t);

	void ZomboControl();
   
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
