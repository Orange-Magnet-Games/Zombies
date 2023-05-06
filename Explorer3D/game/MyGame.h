#pragma once


#include "Game.h"
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

class Gun {
public:
	CModelMd2 model;
	float fireRate = 1, bulletSpeed = 1;
	CModelMd2 bullet;
	CVector gunOffset = CVector(0,0,0);
	virtual void Update(float t) = 0;
	virtual void Draw(CGraphics* g) = 0;
	virtual void Shoot() = 0;
};

class BlunderBuss : public virtual Gun {

	void Update(float t) {
		model.Update(t);
	}
	void Draw(CGraphics* g) {
		model.Draw(g);
	}
	void Shoot() {}

};

class CMyGame : public CGame
{
public:
	CMyGame();
	~CMyGame();

	// ----  Declare your game variables and objects here -------------
	Camera camera = Camera();
	// Variables
	int score;
	

	// Models and Model Lists
	CModelMd2 player;   // animated player model

	vector<Gun*> guns;
	Gun* leftGun, * rightGun;

	CModel wall;
	CModel zombo;
	CModelList* zombos = new CModelList();
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
	
	// Font
	CFont font;

	// -----   Add you member functions here ------

	void KeepInRange(float& in, float min, float max);

	CVector RotateDirection(CVector in, CVector rotation); // IN RADIANS 
	CVector RotateDirection(CVector in, float x, float y, float z); // IN RADIANS 
	CVector RotateInX(CVector in, float a);
	CVector RotateInY(CVector in, float a);
	CVector RotateInZ(CVector in, float a);
   
	void LoadLevelData(string filename);

	void PlayerControl(long t);

	void ZomboControl();
   
	void CameraControl(CGraphics* g);

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
