//=============================================================================
// Color Cube App.cpp by Frank Luna (C) 2008 All Rights Reserved.
//
// Demonstrates coloring.
//
// Controls:
//		'A'/'D'/'W'/'S' - Rotate 
//
//=============================================================================


#include "d3dApp.h"
#include "Box.h"
#include "GameObject.h"
#include "Line.h"
#include "Quad.h"
#include <d3dx9math.h>
#include "LineObject.h"
#include "Wall.h"
#include "Building.h"
#include "gameError.h"
#include "Player.h"
#include "Bullet.h"
#include "debugText.h"
#include "Audio.h"
#include "namespaces.h"
#include "pickup.h"
#include <ctime>
#include "Light.h"
#include "LampPost.h"
#include "Enemy.h"
using std::time;

namespace gameNS {
	const float DAYLEN = 40;
	const int NUM_WALLS = 16;
	const int NUM_BUILDINGS = 12;
	const int PERIMETER = 4;
	const int NUM_BULLETS = 50;
	const int NUM_LIGHTS = 11;
	const float TRANSITIONTIME = 10;
	const D3DXCOLOR NIGHT_SKY_COLOR = D3DXCOLOR(0.049f, 0.049f, 0.2195f, 1.0f);
	const D3DXCOLOR DAY_SKY_COLOR = D3DXCOLOR(0.529f, 0.808f, 0.98f, 1.0f);
	const int MAX_NUM_ENEMIES = 20;
	bool PLAY_MUSIC = true;
	const float FOOTSTEP_GAP = 0.45;
	const int GRASSY_AREA_WIDTH = 110;
	const int FLASHLIGHT_NUM = 2;
}


class ColoredCubeApp : public D3DApp
{
public:
	ColoredCubeApp(HINSTANCE hInstance);
	~ColoredCubeApp();

	void initApp();
	void initOrigin();
	void initPickups();
	void initBullets();
	void initBasicGeometry();
	void initTextStrings();
	void initBasicVariables();
	void initWallPositions();
	void initBuildingPositions();
	void initUniqueObjects();
	void initLights();
	void initLamps();
	void initWaypoints();
	void initEnemies();

	void updateScene(float dt);
	void updatePickups(float dt);
	void placePickups();
	void updateOrigin(float dt);
	void updateWalls(float dt);
	void updateBuildings(float dt);
	void updateUniqueObjects(float dt);
	void updatePlayer(float dt);
	void updateEnemies(float dt);
	void updateCamera();
	void updateDayNight();
	void updateLamps(float dt);
	void updateDebugMode();
	void updateMusic();

	void handleUserInput();
	void handleBuildingCollisions(Vector3 pos);
	void handleWallCollisions(Vector3 pos);
	void handleLampCollisions(Vector3 pos);
	void handlePickupCollisions(float dt);
	void handleEnemyCollisions(float dt);

	void drawScene(); 
	void drawLine(LineObject*);
	void drawOrigin();
	void drawPickups();
	void drawWalls();
	void drawLamps();
	void drawBuildings();

	void onResize();
	Vector3 moveRuggerDirection();
	void printText(DebugText text);
	void printText(string text, int rectPosX, int rectPosY, int rectWidth, int rectHeight, D3DXCOLOR color, int value = -1);
	void setDeviceAndShaderInformation();
	void doEndScreen();
	
private:
	void buildFX();
	void buildVertexLayouts();
 
private:
	Box mWallMesh;
	Box mBuildingMesh;
	Box mEnemyMesh;
	Box mPoleMesh;
	Box mStreetMesh;
	Box mBulletMesh;

	Line rLine, bLine, gLine;
	Box mBox, redBox, brick, bulletBox, eBulletBox, yellowGreenBox, goldBox, blueBox, tealBox, maroonBox, clearBox, whiteBox;
	Box testBox;
	Player player;
	vector<Bullet*> pBullets;
	LineObject xLine, yLine, zLine;
	Wall walls[gameNS::NUM_WALLS];
	Building buildings[gameNS::NUM_BUILDINGS];
	Wall floor;
	vector<LampPost> lamps;
	vector<Pickup> dayPickups;
	vector<Pickup> nightPickups;
	GameObject superLowFloorOffInTheDistanceUnderTheScene;
	Quad menu;

	//Lighting and Camera-specific declarations
	Light mLights[gameNS::NUM_LIGHTS];
	int mLightType; // 0 (parallel), 1 (point), 2 (spot)
	Light sun;
	int mLightNum;
	ID3D10EffectScalarVariable* mfxLightNum;
	D3DXVECTOR3 mEyePos;
	D3DXVECTOR3 target;
	D3DXVECTOR3 perpAxis;
	D3DXVECTOR3 moveAxis;

	//Pathfinding stuff
	Box inactiveLine;
	Box activeLine;
	//GameObject wayLine[100][100];
	GameObject wayLine[WAYPOINT_SIZE*WAYPOINT_SIZE];
	
	Waypoint* dest;
	Waypoint* src;
	
	bool found;
	bool debugMode;
	bool hasntPlayedYet;
	bool nightDayTrans;
	bool placedPickups;
	int dayCount;
	bool won;

	float spinAmount;
	int shotTimer;
	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10InputLayout* mVertexLayout;
	ID3D10EffectMatrixVariable* mfxWVPVar;
	ID3D10EffectMatrixVariable* mfxWorldVar;
	ID3D10EffectVariable* mfxEyePosVar;
	ID3D10EffectVariable* mfxLightVar;
	ID3D10EffectScalarVariable* mfxLightType;
	ID3D10ShaderResourceView* mDiffuseMapRV;
	ID3D10ShaderResourceView* mSpecMapRV;
	ID3D10ShaderResourceView* mDiffuseMapRVBuilding;
	ID3D10ShaderResourceView* mSpecMapRVBuilding;
	ID3D10ShaderResourceView* mDiffuseMapRVEnemy;
	ID3D10ShaderResourceView* mSpecMapRVEnemy;
	ID3D10ShaderResourceView* mDiffuseMapRVPole;
	ID3D10ShaderResourceView* mSpecMapRVPole;
	ID3D10ShaderResourceView* mDiffuseMapRVStreet;
	ID3D10ShaderResourceView* mSpecMapRVStreet;
	ID3D10ShaderResourceView* mDiffuseMapRVBullet;
	ID3D10ShaderResourceView* mSpecMapRVBullet;
	ID3D10EffectShaderResourceVariable* mfxDiffuseMapVar;
	ID3D10EffectShaderResourceVariable* mfxSpecMapVar;
	ID3D10EffectMatrixVariable* mfxTexMtxVar;
	D3DXMATRIX mCompCubeWorld;

	bool night;
	//my addition
	ID3D10EffectVariable* mfxFLIPVar;

	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mWVP;
	D3DXMATRIX mVP;

	//my edits
	D3DXMATRIX worldBox1, worldBox2;

	//Theta represents vertical rotation (z-axis rotation)
	float mTheta;
	//Phi represents horizontal rotation (y-axis rotation)
	float mPhi;
	int incrementedYMargin;

	int score;
	int lineHeight;
	bool firstpass;
	bool startScreen, endScreen;
	float dt;
	DebugText sText, lText, wText;

	float timect;
	string timeOfDay;
	Enemy enemy[gameNS::MAX_NUM_ENEMIES];
	int nightCount;
	float stepTime;
	bool step1;
	bool flashChanged, flashOn;
	float flashChangeTime;
};

ColoredCubeApp::ColoredCubeApp(HINSTANCE hInstance)
: D3DApp(hInstance), mFX(0), mTech(0), mVertexLayout(0),
  mfxWVPVar(0), mTheta(0.0f), mPhi(0.0f), mfxWorldVar(0), 
  mfxEyePosVar(0), mfxLightVar(0), mfxLightType(0)
{
	srand(time(0));
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixIdentity(&mWVP); 
	D3DXMatrixIdentity(&mVP); 
	score = 0;
	firstpass = true;
	startScreen = true;
	endScreen = false;
	night = false;
	found = false;
	timect = 0.0f;
	timeOfDay = "Day";
	srand(time(0));
	debugMode = false;
	nightCount = 0;
	stepTime = 0.0f;
	step1 = true;
	flashChanged = false;
	flashChangeTime = 0.0f;
	flashOn = false;
	placedPickups = false;
	dayCount = 1;
	won = false;
}

ColoredCubeApp::~ColoredCubeApp()
{
	if( md3dDevice )
		md3dDevice->ClearState();

	for(int i=0; i<4; i++)
	{
		//delete [] wayLine[i];
	}

	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);

	//CHANGE if audio needs to be released in d3dApp instead
	safeDelete(audio);
  // 
}



void ColoredCubeApp::initApp()
{
	D3DApp::initApp();
	buildFX();
	buildVertexLayouts();

	SetCursorPos(0,0);
	ShowCursor(false);
	audio->playCue(INTROMUSIC);
	//pos will eventually be player.x, player.height, player.z)
	startScreen = true;
	mEyePos = D3DXVECTOR3(0, 5, 0);
	initBasicGeometry();
	initBasicVariables(); //Like shotTimer, etc.
	initTextStrings(); //Like start/end screen text
	initUniqueObjects(); //Like the floor

	initOrigin();
	initBullets();
	initPickups();
	initWallPositions();
	initBuildingPositions();
	initLights();
	initLamps();
	initEnemies();
	initWaypoints();

	menu.init(md3dDevice, 1.0f, WHITE);
	menu.setPosition(mEyePos);
	menu.setRotYAngle(ToRadian(90));
	menu.setRotZAngle(ToRadian(0));
	menu.setRotXAngle(ToRadian(-90));

	//mClearColor = D3DXCOLOR(0.15f, 0.15f, 0.15f, 1.0f);
	//mClearColor = D3DXCOLOR(0.529f, 0.808f, 0.98f, 1.0f);
	mClearColor = gameNS::DAY_SKY_COLOR;

	player.init(&mBox, pBullets, sqrt(2.0f), Vector3(3,4,0), Vector3(0,0,0), 20, audio, 1, 1, 1, 5);

	mWallMesh.init(md3dDevice, 1.0f, mFX);
	mBuildingMesh.init(md3dDevice, 1.0f, mFX);
	mEnemyMesh.init(md3dDevice, 1.0f, mFX);
	mPoleMesh.init(md3dDevice, 1.0f, mFX);
	mStreetMesh.init(md3dDevice, 1.0f, mFX);
	mBulletMesh.init(md3dDevice, 1.0f, mFX);

	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"bricks.png", 0, 0, &mDiffuseMapRV, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"defaultspec.dds", 0, 0, &mSpecMapRV, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"skyscraper.jpg", 0, 0, &mDiffuseMapRVBuilding, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"defaultspec.dds", 0, 0, &mSpecMapRVBuilding, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"Robot.png", 0, 0, &mDiffuseMapRVEnemy, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"defaultspec.dds", 0, 0, &mSpecMapRVEnemy, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"pole.png", 0, 0, &mDiffuseMapRVPole, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"defaultspec.dds", 0, 0, &mSpecMapRVPole, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"street.png", 0, 0, &mDiffuseMapRVStreet, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"defaultspec.dds", 0, 0, &mSpecMapRVStreet, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"bullet.png", 0, 0, &mDiffuseMapRVBullet, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"defaultspec.dds", 0, 0, &mSpecMapRVBullet, 0 ));

}

void ColoredCubeApp::initLamps() {
	for (int i = 0; i < 4; i++)
		lamps.push_back(LampPost());
	
	lamps[0].init(&brick, Vector3(58.5,0.1,58.5), 1.0f, 1.0f, 1, 1, 1, 0.0f, 2.3456f);
	lamps[1].init(&brick, Vector3(-58.5,0.1,-58.5), 1.0f, 1.0f, 1, 1, 1, 0.0f, 5.49f);
	lamps[2].init(&brick, Vector3(58.5,0.1,-58.5), 1.0f, 1.0f, 1, 1, 1, 0.0f, 3.9359f);
	lamps[3].init(&brick, Vector3(-58.5,0.1,58.5), 1.0f, 1.0f, 1, 1, 1, 0.0f, 0.7944f);
}

void ColoredCubeApp::initPickups() {
	//int speed1 = player.getSpeed();
	//define the pickups

	dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 0, ZIPPER, audio));
	dayPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 15, 1, RELOAD, audio));
	dayPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 15, 2, RELOAD, audio));
	dayPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 15, 3, RELOAD, audio));
	dayPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 15, 4, RELOAD, audio));
	dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 5, ZIPPER, audio));
	dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 6, ZIPPER, audio));
	dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 7, ZIPPER, audio));
	dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 8, ZIPPER, audio));
	dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 8, ZIPPER, audio));
	dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 9, ZIPPER, audio));
	dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 10, ZIPPER, audio));
	dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 11, ZIPPER, audio));
	dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 12, ZIPPER, audio));
	dayPickups.push_back(Pickup(&goldBox, &player.speed, INCREASE, 5, 13, WHOOSH, audio));
	dayPickups.push_back(Pickup(&goldBox, &player.speed, INCREASE, 5, 14, WHOOSH, audio));
	dayPickups.push_back(Pickup(&goldBox, &player.speed, INCREASE, 5, 15, WHOOSH, audio));
	dayPickups.push_back(Pickup(&goldBox, &player.speed, INCREASE, 5, 16, WHOOSH, audio));

	for (int i = 0; i < dayPickups.size(); i++)
		dayPickups[i].startGlowing();
	for (int i = 0; i < nightPickups.size(); i++) 
		nightPickups[i].startGlowing();
}

void ColoredCubeApp::initBullets() {
	for (int i = 0; i < gameNS::NUM_BULLETS; i++) {
		pBullets.push_back(new Bullet());
		pBullets[i]->init(&bulletBox, 2.0f, Vector3(0, 0, 0), Vector3(0,0,0), 0, 1);
	}
	
}

void ColoredCubeApp::initBasicGeometry() {	
	mBox.init(md3dDevice, 2.0f, D3DXCOLOR(0,0,0,0), mFX);
	whiteBox.init(md3dDevice, 1.0f, D3DXCOLOR(1,1,1,0), mFX);
	tealBox.init(md3dDevice, 1.0f, colorNS::TEAL, mFX);
	clearBox.init(md3dDevice, 1.0f, D3DXCOLOR(0,0,0,0), mFX);
	redBox.init(md3dDevice, 1.0f, colorNS::RED, mFX);
	brick.init(md3dDevice, 1.0f, DARKBROWN, mFX);
	bulletBox.init(md3dDevice, 0.25f, D3DXCOLOR(0,0,0,0), mFX);
	eBulletBox.init(md3dDevice, 0.5f, RED, mFX);
	maroonBox.init(md3dDevice, 10000, colorNS::MAROON, mFX);
	yellowGreenBox.init(md3dDevice, 1.f,D3DXCOLOR(0,0,0,0), mFX);
	goldBox.init(md3dDevice, 1.0f, YELLOW, mFX);
	blueBox.init(md3dDevice, 2.0f, BLUE, mFX);
	rLine.init(md3dDevice, 10.0f, RED);
	bLine.init(md3dDevice, 10.0f, BLACK);
	gLine.init(md3dDevice, 10.0f, GREEN);
	activeLine.init(md3dDevice, 1.0f, RED, mFX);
}

void ColoredCubeApp::initTextStrings() {
	

	sText.addLine("WELCOME RUGGER !", 290, 120);
	sText.addLine("Controls: ", 30, 210);
	sText.addLine("Movement: W A S D", 80, 240);
	sText.addLine("Shoot: Left-Click", 80, 265);
	sText.addLine("Flashlight: 'F' to toggle", 80, 290);
	sText.addLine("Objective:", 30, 360);
	sText.addLine("Collect items and shoot enemies to survive as long as you can!", 80, 390);
	sText.addLine("PRESS SPACE BAR TO BEGIN !", 250, 500);
	
	lText.addLine("TOO BAD, RUGGER, I WON!", 260, 180);
	lText.addLine("Press SPACEBAR to exit", 276, 500);

	wText.addLine("RUGGER YOU FIEND! YOU WON!", 250, 180);
	wText.addLine("THOSE MINIONS WERE EXPENSIVE!", 235, 250);
	wText.addLine("Press SPACEBAR to exit", 300, 500);
}

void ColoredCubeApp::initBasicVariables() {
	startScreen = true;
	shotTimer = 0;
	hasntPlayedYet = true;
	nightDayTrans = false;
}

void ColoredCubeApp::initBuildingPositions() {
//					geom,  rad,  position,					sc,	w,		h,	d
	buildings[0].init(&brick, 2.0f, Vector3(150, 0, -150),	1,	20,		50,  20);// Front right corner buildings
	buildings[1].init(&brick, 2.0f, Vector3(150, 0, -50),		1,	20,		50,  20);
	buildings[2].init(&brick, 2.0f, Vector3(50, 0, -150),	1,	20,		50,  20);

	buildings[3].init(&brick, 2.0f, Vector3(150, 0, 150),	1,	20,		50,  20);// Front left corner buildings
	buildings[4].init(&brick, 2.0f, Vector3(150, 0, 50),	1,	20,		50,  20);
	buildings[5].init(&brick, 2.0f, Vector3(50, 0, 150),	1,	20,		50,  20);

	buildings[6].init(&brick, 2.0f, Vector3(-150, 0, -150),1,	20,		50,  20);// Back right corner buildings
	buildings[7].init(&brick, 2.0f, Vector3(-150, 0, -50),	1,	20,		50,  20);
	buildings[8].init(&brick, 2.0f, Vector3(-50, 0, -150),	1,	20,		50,  20);

	buildings[9].init(&brick, 2.0f, Vector3(-150, 0, 150),	1,	20,		50,  20);// Back left corner buildings
	buildings[10].init(&brick, 2.0f, Vector3(-150, 0, 50),	1,	20,		50,  20);
	buildings[11].init(&brick, 2.0f, Vector3(-50, 0, 150),	1,	20,		50,  20);
}

void ColoredCubeApp::initWallPositions() {
	
//				   geom,  rad,  position,				sc,	w,		h,	d
	walls[0].init(&brick, 2.0f, Vector3(125, 0, 250), Vector3(0,0,0), 1, 	1,	125,	10, 10);//	Left/Front wall 
	walls[1].init(&brick, 2.0f, Vector3(-125, 0, -250), Vector3(0,0,0), 1,	1,	125,	10, 10);//	Right/back wall
	walls[2].init(&brick, 2.0f, Vector3(250, 0, 125),	 Vector3(0,0,0), 1,1,	10,		10, 125);//	Front/Left wall
	walls[3].init(&brick, 2.0f, Vector3(-250, 0, -125),	 Vector3(0,0,0), 1,1,	10,		10, 125);//	Back/Right wall

	walls[4].init(&brick, 2.0f, Vector3(-125, 0, 250),	 Vector3(0,0,0), 1,1,	125,	10, 10);//	Left/Back wall 
	walls[5].init(&brick, 2.0f, Vector3(125, 0, -250),	 Vector3(0,0,0), 1,1,	125,	10, 10);//	Right/Front wall
	walls[6].init(&brick, 2.0f, Vector3(250, 0, -125),	 Vector3(0,0,0), 1,1,	10,		10, 125);//	Front/Right wall
	walls[7].init(&brick, 2.0f, Vector3(-250, 0, 125),	 Vector3(0,0,0), 1,1,	10,		10, 125);//	Back/Left wall

	walls[8].init(&brick, 2.0f, Vector3(36, 0, 55),		 Vector3(0,0,0), 1,1,	20,		2.5,	1);//	Left/Front inner wall 
	walls[9].init(&brick, 2.0f, Vector3(-36, 0, -55),	 Vector3(0,0,0), 1,1,	20,		2.5,	1);//	Right/Back inner wall
	walls[10].init(&brick, 2.0f, Vector3(55, 0, 36),	 Vector3(0,0,0), 1,1,	1,		2.5,	20);//	Front/Left inner wall
	walls[11].init(&brick, 2.0f, Vector3(-55, 0, -36),	 Vector3(0,0,0), 1,1,	1,		2.5,	20);//	Back/Right inner wall

	walls[12].init(&brick, 2.0f, Vector3(-36, 0, 55),	 Vector3(0,0,0), 1,1,	20,		2.5,	1);//	Left/Back inner wall 
	walls[13].init(&brick, 2.0f, Vector3(36, 0, -55),	 Vector3(0,0,0), 1,1,	20,		2.5,	1);//	Right/Front inner wall
	walls[14].init(&brick, 2.0f, Vector3(55, 0, -36),	 Vector3(0,0,0), 1,1,	1,		2.5,	20);//	Front/Right inner wall
	walls[15].init(&brick, 2.0f, Vector3(-55, 0, 36),	 Vector3(0,0,0), 1,1,	1,		2.5,	20);//	Back/Left inner wall

}

void ColoredCubeApp::initUniqueObjects() {
	floor.init(&yellowGreenBox, 2.0f, Vector3(0,-1.5f,0), Vector3(0,0,0), 1, 1.0f, 250, 1, 250);
}

void ColoredCubeApp::initEnemies() {
	for(int i=0; i<gameNS::MAX_NUM_ENEMIES; i++) {
		enemy[i].init(&mBox, 2.0f, Vector3(rand()%50,0,rand()%50), Vector3(0,0,0), 1, 1, 1, 2, 1);

		enemy[i].faceObject(&player);
	}
}

void ColoredCubeApp::initOrigin() {
	xLine.init(&rLine, Vector3(0,0,0), 5);
	xLine.setPosition(Vector3(0,0,0));
	yLine.init(&bLine, Vector3(0,0,0), 5);
	yLine.setPosition(Vector3(0,0,0));
	yLine.setRotationZ(ToRadian(90));
	zLine.init(&gLine, Vector3(0,0,0), 5);
	zLine.setPosition(Vector3(0,0,0));
	zLine.setRotationY(ToRadian(90));
}

void ColoredCubeApp::initLights()
{
	mLightType = 1;
	mLightNum = gameNS::NUM_LIGHTS;
 
	// Parallel light.
	mLights[0].dir      = D3DXVECTOR3(0.57735f, -0.57735f, 0.57735f);
	mLights[0].ambient  = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	//mLights[0].ambient  = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	mLights[0].diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	//mLights[0].diffuse  = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	mLights[0].specular = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	//mLights[0].specular = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	mLights[0].pos = D3DXVECTOR3(0, 500, 0);
	mLights[0].range = 1000;
	mLights[0].att.x = 0.0f;
	mLights[0].att.y = 0.01f;
	mLights[0].att.z = 0.0f;
 
	// Pointlight--position is changed every frame to animate.
	//mLights[1].ambient  = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
	mLights[1].ambient  = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	//mLights[1].diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mLights[1].diffuse  = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	//mLights[1].specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mLights[1].specular = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	mLights[1].att.x    = 0.0f;
	mLights[1].att.y    = 0.1f;
	mLights[1].att.z    = 0.0f;
	mLights[1].range    = 50.0f;
	
	// Spotlight--position and direction changed every frame to animate.
	mLights[2].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	//mLights[2].ambient  = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	mLights[2].diffuse  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	//mLights[2].diffuse  = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	mLights[2].specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	//mLights[2].specular = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	mLights[2].att.x    = 1.0f;
	mLights[2].att.y    = 0.0f;
	mLights[2].att.z    = 0.0f;
	mLights[2].spotPow  = 128.0f;
	mLights[2].range    = 0.0f;

	//Inner corner lights
	mLights[3].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	mLights[3].diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	mLights[3].specular = D3DXCOLOR(1.0f, 0.55f, 0.0f, 1.0f);
	mLights[3].att.x    = 0.0f;
	mLights[3].att.y    = 0.55f;
	mLights[3].att.z    = 0.0f;
	mLights[3].range    = 90.0f;
	mLights[3].pos = D3DXVECTOR3(30, 10, 30);
	
	mLights[4].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	mLights[4].diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	mLights[4].specular = D3DXCOLOR(1.0f, 0.55f, 0.0f, 1.0f);
	mLights[4].att.x    = 0.0f;
	mLights[4].att.y    = 0.55f;
	mLights[4].att.z    = 0.0f;
	mLights[4].range    = 90.0f;
	mLights[4].pos = D3DXVECTOR3(-30, 10, 30);

	mLights[5].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	mLights[5].diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	mLights[5].specular = D3DXCOLOR(1.0f, 0.55f, 0.0f, 1.0f);
	mLights[5].att.x    = 0.0f;
	mLights[5].att.y    = 0.55f;
	mLights[5].att.z    = 0.0f;
	mLights[5].range    = 90.0f;
	mLights[5].pos = D3DXVECTOR3(30, 10, -30);

	mLights[6].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	mLights[6].diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	mLights[6].specular = D3DXCOLOR(1.0f, 0.55f, 0.0f, 1.0f);
	mLights[6].att.x    = 0.0f;
	mLights[6].att.y    = 0.55f;
	mLights[6].att.z    = 0.0f;
	mLights[6].range    = 90.0f;
	mLights[6].pos = D3DXVECTOR3(-30, 10, -30);

	//Enemy entry vectors
	mLights[7].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	mLights[7].diffuse  = D3DXCOLOR(0.9f, 0.5f, 0.5f, 1.0f);
	mLights[7].specular = D3DXCOLOR(1.0f, 0.5f, 0.0f, 1.0f);
	mLights[7].att.x    = 0.0f;
	mLights[7].att.y    = 0.45f;
	mLights[7].att.z    = 0.0f;
	mLights[7].range    = 35.0f;
	mLights[7].pos = D3DXVECTOR3(200, 10, 0);

	mLights[8].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	mLights[8].diffuse  = D3DXCOLOR(0.9f, 0.5f, 0.5f, 1.0f);
	mLights[8].specular = D3DXCOLOR(1.0f, 0.5f, 0.0f, 1.0f);
	mLights[8].att.x    = 0.0f;
	mLights[8].att.y    = 0.45f;
	mLights[8].att.z    = 0.0f;
	mLights[8].range    = 35.0f;
	mLights[8].pos = D3DXVECTOR3(-200, 10, 0);

	mLights[9].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	mLights[9].diffuse  = D3DXCOLOR(0.9f, 0.5f, 0.5f, 1.0f);
	mLights[9].specular = D3DXCOLOR(1.0f, 0.5f, 0.0f, 1.0f);
	mLights[9].att.x    = 0.0f;
	mLights[9].att.y    = 0.45f;
	mLights[9].att.z    = 0.0f;
	mLights[9].range    = 35.0f;
	mLights[9].pos = D3DXVECTOR3(0, 10, -200);

	mLights[10].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	mLights[10].diffuse  = D3DXCOLOR(0.9f, 0.5f, 0.5f, 1.0f);
	mLights[10].specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mLights[10].att.x    = 0.0f;
	mLights[10].att.y    = 0.45f;
	mLights[10].att.z    = 0.0f;
	mLights[10].range    = 35.0f;
	mLights[10].pos = D3DXVECTOR3(0, 10, 200);
}

void ColoredCubeApp::initWaypoints()
{
	
}



void ColoredCubeApp::updateScene(float dt)
{
	ColoredCubeApp::dt = dt;
	bool playing = (!endScreen && !startScreen);
	Vector3 oldPos = mEyePos;
	
	if(input->isKeyDown(VK_ESCAPE)) PostQuitMessage(0);

	D3DXMATRIX sunRot;
	D3DXMatrixRotationX(&sunRot, ToRadian(15.0f));
	if (startScreen){
		if(input->isKeyDown(VK_SPACE)){
			startScreen = false;
			playing = true;
			audio->stopCue(INTROMUSIC);
		}
	}
	if(playing){
		timect += dt;
		updateMusic();
		updateCamera();

		updateDebugMode();
		updateDayNight();

		menu.update(dt);
		//General Update
		D3DApp::updateScene(dt);
		updateOrigin(dt);
		handleUserInput();
		updatePlayer(dt);
		updateEnemies(dt);
		placePickups();
		updatePickups(dt);
		updateLamps(dt);
		updateWalls(dt);
		updateBuildings(dt);
		updateUniqueObjects(dt); //Like floor

		//Handle Collisions
		handleWallCollisions(oldPos);
		handleBuildingCollisions(oldPos);
		handlePickupCollisions(dt);
		handleEnemyCollisions(dt);

		if(dayCount >= 3){
			won = true;
			playing = false;
			endScreen = true;
		}
	}
	if(endScreen){
		doEndScreen();
	}
	
	// The point light circles the scene as a function of time, 
	// staying 7 units above the land's or water's surface.
	mLights[1].pos.x = 50.0f*cosf( mTimer.getGameTime() );
	mLights[1].pos.z = 50.0f*sinf( mTimer.getGameTime() );
	mLights[1].pos.y = 7.0f;
	
	// The spotlight takes on the camera position and is aimed in the
	// same direction the camera is looking.  In this way, it looks
	// like we are holding a flashlight.
	mLights[2].pos = mEyePos;
	D3DXVec3Normalize(&mLights[2].dir, &(target-mEyePos));


}

void ColoredCubeApp::updateMusic() {
	if (hasntPlayedYet || nightDayTrans) {
		hasntPlayedYet = false;
		nightDayTrans = false;
		if (gameNS::PLAY_MUSIC) audio->playCue(MUSIC);
	}
}

void ColoredCubeApp::updateDebugMode() {
	if(input->wasKeyPressed(KEY_K)) {
		debugMode = true;
		input->clear(KEY_K);
	} 
	if (input->wasKeyPressed(KEY_L)) {
		mEyePos = D3DXVECTOR3(mEyePos.x, 5, mEyePos.z);
		debugMode = false;
		input->clear(KEY_L);
	}
	if (input->wasKeyPressed(KEY_M)) {
		gameNS::PLAY_MUSIC = false;
		audio->stopCue(MUSIC);
	}
}

void ColoredCubeApp::updateCamera() {
	int dx = input->getMouseRawX();
	int dy = input->getMouseRawY();
	D3DXVECTOR3 pos = player.getPosition();
	if(dx < 0)
		mPhi -= 6.0f*dt;
	if(dx > 0)
		mPhi += 6.0f*dt;
	if(dy < 0)
		mTheta += 3.0f*dt;
	if(dy > 0)
		mTheta -= 3.0f*dt;

	//Restricting the mouse movement
	RECT restrict = {639, 399, 640, 400};
	ClipCursor(&restrict);
	bool walking = false;
	if(input->isKeyDown(KEY_W))
	{
		walking = true;
		moveAxis.y = 0;
		D3DXVec3Normalize(&moveAxis, &moveAxis);
		mEyePos += moveAxis * dt * player.getSpeed();
	}
	if(input->isKeyDown(KEY_S))
	{
		walking = true;
		moveAxis.y = 0;
		D3DXVec3Normalize(&moveAxis, &moveAxis);
	
		mEyePos -= moveAxis * dt * player.getSpeed();
	}
	if(input->isKeyDown(KEY_D))
	{
		walking = true;
		mEyePos -= perpAxis * dt * player.getSpeed();
	}
	if(input->isKeyDown(KEY_A))
	{
		walking = true;
		mEyePos += perpAxis * dt * player.getSpeed();
	}
	
	if (debugMode) 
	if(input->isKeyDown(VK_SPACE))
	{
		mEyePos.y += 20 * dt;
	}
	if (debugMode) 
	if(input->isKeyDown(VK_SHIFT))
	{
		mEyePos.y -= 20 * dt;
	}

	if(input->getMouseLButton())
	{
		if(!player.firedLastFrame){
			player.fired = true;
			if(player.getAmmo() > 0) audio->playCue(FIRE);
		}
		player.firedLastFrame = true; 
	}
	else
	{
		player.firedLastFrame = false;
		player.fired = false;
	}
	if(input->getMouseRButton())
	{
		//Do something
	}

	if (walking) {
		stepTime += 1;
		if (stepTime*dt > gameNS::FOOTSTEP_GAP) {
			if (pos.x < gameNS::GRASSY_AREA_WIDTH/2.0f && pos.x > -gameNS::GRASSY_AREA_WIDTH/2.0f  && pos.z > -gameNS::GRASSY_AREA_WIDTH/2.0f && pos.z < gameNS::GRASSY_AREA_WIDTH/2.0f) { //in grassy area 
				if (step1) audio->playCue(FOOTSTEP3);
				else audio->playCue(FOOTSTEP4);
			} else {
				if (step1) audio->playCue(FOOTSTEP1);
				else audio->playCue(FOOTSTEP2);
			}
			step1 = !step1;
			stepTime = 0.0f;
		}
	}

	// Restrict the angle mPhi and radius mRadius.
	if( mTheta < -(PI/2.0f) + 0.01f)	mTheta = -(PI/2.0f) + 0.01f;
	if( mTheta > PI/2.0f - 0.01f)	mTheta = (PI/2.0f) - 0.01f;
	//target will start pointing in the +x direction and will be rotated according to the camera's net rotations
	//Should eventually be pos.x+1, pos.y, pos.z to make the camera rotate according to its own axis
	//D3DXVECTOR3 target(1, 0, 0);
	//Up remains unchanged.
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

	//Big changes to lookat vector
	D3DXVECTOR4 t4(1, 0, 0, 1);
	Matrix rotZ, rotY, trans;
	D3DXMatrixRotationZ(&rotZ, mTheta);
	D3DXMatrixRotationY(&rotY, mPhi);
	D3DXMatrixTranslation(&trans, mEyePos.x, mEyePos.y, mEyePos.z);
	//Rotate around the Z-axis first, then the Y-axis, then translate to the camera position
	Matrix transform = rotZ * rotY/* * trans*/;
	
	//t4 now holds the final position of the camera target point
	D3DXVec4Transform(&t4, &t4, &transform);
	moveAxis = D3DXVECTOR3(t4.x, t4.y, t4.z);
	D3DXVec3Normalize(&moveAxis, &moveAxis);
	D3DXVec3Cross(&perpAxis, &(D3DXVECTOR3(t4.x, t4.y, t4.z)), &up);
	D3DXVec3Normalize(&perpAxis, &perpAxis);
	D3DXVec4Transform(&t4, &t4, &trans);
	//Assign it into a Vec3 and we should be good to go
	target = D3DXVECTOR3(t4.x, t4.y, t4.z);
	//D3DXVec3Normalize(&target, &target);

	D3DXMatrixLookAtLH(&mView, &mEyePos, &target, &up);
}

void ColoredCubeApp::doEndScreen() {
	if(input->isKeyDown(VK_SPACE))
	{
		endScreen = false;
		PostQuitMessage(0);
	}
}

void ColoredCubeApp::updateUniqueObjects(float dt) {
	floor.update(dt);
	//big floor update must also be added here for it to show.
}

void ColoredCubeApp::updateWalls(float dt) {
	for(int i=0; i<gameNS::NUM_WALLS; i++)
		walls[i].update(dt);
}

void ColoredCubeApp::updateLamps(float dt) {
	for (int i = 0; i < lamps.size(); i++) {
		lamps[i].update(dt);
	}
}

void ColoredCubeApp::updateBuildings(float dt) {
	for(int i=0; i<gameNS::NUM_BUILDINGS; i++)
		buildings[i].update(dt);
}

void ColoredCubeApp::updatePlayer(float dt) {
	//player.setVelocity(moveRuggerDirection() * player.getSpeed());
	player.setPosition(Vector3(mEyePos.x, mEyePos.y-2, mEyePos.z));
	player.update(dt, moveAxis);
	if (player.getHealth() <= 0) {
		Sleep(2000);
		endScreen = true;
		input->clearKeyPress(KEY_SPACE);
	}
}

void ColoredCubeApp::updateEnemies(float dt)
{
	for(int i=0; i<gameNS::MAX_NUM_ENEMIES; i++)
	{
		if(enemy[i].getActiveState())
		{
			if(night)
			{
				if(D3DXVec3LengthSq(&(enemy[i].getPosition() - D3DXVECTOR3(0,0,0))) < 55*55)
				{
					enemy[i].setSpeed(enemyNS::DAY_SPEED);
				}
				else enemy[i].setSpeed(enemyNS::NIGHT_SPEED);
			}
			else enemy[i].setSpeed(enemyNS::DAY_SPEED);		
			
			enemy[i].update(dt, &player);
		}
	}

	vector<D3DXVECTOR3> wp = enemy[0].waypointPositions();
	
	for(int i=0; i<WAYPOINT_SIZE*WAYPOINT_SIZE; i++)
	{
		wayLine[i].init(&activeLine, 1.0f, wp[i], D3DXVECTOR3(0,0,0), 0.0f, 1.0f);
		wayLine[i].update(dt);
	}
}

void ColoredCubeApp::handleUserInput() {

	if (flashChanged)
		flashChangeTime++;

	if (flashChangeTime*dt >= 0.30f) {
		flashChangeTime = 0;
		flashChanged = false;
	}
	
	if (input->isKeyDown(KEY_F) && flashChanged == false) {
		flashChanged = true;	
		flashOn = !flashOn;
		if (flashOn)
			mLights[gameNS::FLASHLIGHT_NUM].range = 100;
		else 
			mLights[gameNS::FLASHLIGHT_NUM].range = 0;
	}
}

void ColoredCubeApp::handleWallCollisions(Vector3 pos) {
	for(int i=0; i<gameNS::NUM_WALLS; i++)
	{
		if(player.collided(&walls[i]))
		{
			mEyePos = pos;
		}

		for (int j = 0; j < pBullets.size(); j++) {
			if (pBullets[j]->collided(&walls[i])) {
				pBullets[j]->setInActive();
				pBullets[j]->setVelocity(D3DXVECTOR3(0,0,0));
				pBullets[j]->setPosition(D3DXVECTOR3(0,0,0));
				shotTimer = 0;
			}		
		}
	}
}

void ColoredCubeApp::handleBuildingCollisions(Vector3 pos) {
	for(int i=0; i<gameNS::NUM_BUILDINGS; i++)
	{
		if(player.collided(&buildings[i]))
			mEyePos = pos;
		for (int j = 0; j < pBullets.size(); j++) {
			if (pBullets[j]->collided(&buildings[i])) {
				pBullets[j]->setInActive();
				pBullets[j]->setVelocity(D3DXVECTOR3(0,0,0));
				pBullets[j]->setPosition(D3DXVECTOR3(0,0,0));
				shotTimer = 0;
			}		
		}
	}
}

void ColoredCubeApp::handleLampCollisions(Vector3 pos) {
		for(int i=0; i<gameNS::NUM_WALLS; i++)
	{
		if(player.collided(&lamps[i]))
			player.setPosition(pos);

		for (int j = 0; j < pBullets.size(); j++) {
			if (pBullets[j]->collided(&lamps[i])) {
				pBullets[j]->setInActive();
				pBullets[j]->setVelocity(D3DXVECTOR3(0,0,0));
				pBullets[j]->setPosition(D3DXVECTOR3(0,0,0));
				shotTimer = 0;
				player.fired = false;
			}		
		}
	}
}

void ColoredCubeApp::handlePickupCollisions(float dt) {
	
}

void ColoredCubeApp::handleEnemyCollisions(float dt)
{
	
	for(int i=0; i<gameNS::MAX_NUM_ENEMIES; i++)
	{
		for(int j=0; j<pBullets.size(); j++)
		{
			if(pBullets[j]->collided(&enemy[i]))
			{
				pBullets[j]->setInActive();
				pBullets[j]->setVelocity(D3DXVECTOR3(0,0,0));
				pBullets[j]->setPosition(D3DXVECTOR3(0,0,0));
				shotTimer = 0;
				enemy[i].damage(50);
			}
		}
		for(int j=0; j<gameNS::NUM_WALLS; j++)
		{
			if(enemy[i].collided(&walls[j]) && D3DXVec3LengthSq(&(enemy[i].getPosition() - player.getPosition())) < 55*55)
			{
				enemy[i].setPosition(enemy[i].getOldPos());
			}
		}
		for(int j=0; j<gameNS::NUM_BUILDINGS; j++)
		{
			if(enemy[i].collided(&buildings[j])) enemy[i].setPosition(enemy[i].getOldPos());
		}
		//for(int j=0; j<gameNS::MAX_NUM_ENEMIES; j++)
		//{
		//	if(enemy[j].getActiveState())
		//		if(enemy[i].collided(&enemy[j])) enemy[i].setPosition(enemy[i].getOldPos());
		//}
	}
}

void ColoredCubeApp::placePickups() {
	if (!placedPickups) {
		int maxNightPickups = 5; //Locations: 0, 13, 14, 15, 16
		int maxDayPickups = 17; //All locations
		vector<int> choices;
		bool day = !night;
		if (day) {
			if (dayPickups.size() > 0) { //otherwise divide by zero when I mod by size
				vector<int> tempUsedIndices;
				for (int i = 0; i < maxDayPickups; i++) {
					bool add = true;
					int choice = rand()%dayPickups.size();
					for (int j = 0; j < tempUsedIndices.size(); j++) { //check that chosen dayPickup mapIndex isn't in the usedMapIndices
						if (tempUsedIndices[j] == dayPickups[choice].getMapIndex())
							add = false; //there is already a pickup in the spot of the chosen day pickup
					}
					if (add) {
						choices.push_back(choice); //add that to displayed pickups
						tempUsedIndices.push_back(dayPickups[choice].getMapIndex()); //record that mapIndex as used
					}
				}
			}
		} else {
			if (nightPickups.size() > 0) {
				vector<int> tempUsedIndices;
				for (int i = 0; i < maxNightPickups; i++) {
					bool add = true;
					int choice = rand()%nightPickups.size();
					for (int j = 0; j < tempUsedIndices.size(); j++) { //check that chosen dayPickup mapIndex isn't in the usedMapIndices
						if (tempUsedIndices[j] == nightPickups[choice].getMapIndex())
							add = false; //there is already a pickup in the spot of the chosen night pickup
					}
					if (add) {
							choices.push_back(choice); //add that to displayed pickups
							tempUsedIndices.push_back(nightPickups[choice].getMapIndex()); //record that mapIndex as used
					}
				}
			}
		}
	
		for (int i = 0; i < nightPickups.size(); i++)
			nightPickups[i].setInActive();
		/*for (int i = 0; i < dayPickups.size(); i++)
			dayPickups[i].setInActive();*/

		if (day)
			for (int i = 0; i < choices.size(); i++) 
				dayPickups[choices[i]].setActive();
		else if (night) 
			for (int i = 0; i < choices.size(); i++) 
				dayPickups[choices[i]].setActive();

		placedPickups = true;
	}
}

void ColoredCubeApp::updatePickups(float dt) {
	for (int i = 0; i < dayPickups.size(); i++) {
			if (player.collided(&dayPickups[i])) {
				dayPickups[i].activate();
			}
			dayPickups[i].update(dt);
	}
	for (int i = 0; i < nightPickups.size(); i++) {
			if (player.collided(&nightPickups[i])) {
				nightPickups[i].activate();
			}
			nightPickups[i].update(dt);
	}
}

void ColoredCubeApp::updateOrigin(float dt) {
	xLine.update(dt);
	yLine.update(dt);
	zLine.update(dt);
}

void ColoredCubeApp::updateDayNight() {
	if(timect >= gameNS::DAYLEN)
	{
		timect = 0;
		night = !night;
		if(night)
		{
			if(timeOfDay == "Evening")
			{
				nightCount++;
				placedPickups = false;

				int x = 0;
				for(int i=0; i<gameNS::MAX_NUM_ENEMIES && x < 4*nightCount; i++)
				{
					if(!enemy[i].getActiveState())
					{
						enemy[i].setActive();
						enemy[i].setHealth(100);
						switch(x%4)
						{
						case 0:
							enemy[i].setPosition(D3DXVECTOR3(50-rand()%100,0,-250));
							break;
						case 1:
							enemy[i].setPosition(D3DXVECTOR3(50-rand()%100,0,250));
							break;
						case 2:
							enemy[i].setPosition(D3DXVECTOR3(250, 0, 50-rand()%100));
							break;
						case 3:
							enemy[i].setPosition(D3DXVECTOR3(-250, 0, 50-rand()%100));
							break;
						}
						x++;
					}
				}

			}
			timeOfDay = "Night";
			mClearColor = gameNS::NIGHT_SKY_COLOR;
			mLights[0].diffuse  = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
			mLights[3].att.y    = 0.05f;
			mLights[4].att.y    = 0.05f;
			mLights[5].att.y    = 0.05f;
			mLights[6].att.y    = 0.05f;
		}
		else
		{
			if (timeOfDay == "Dawn")
			{
				placedPickups = false;
				nightDayTrans = true;
			}
			timeOfDay = "Day";
			mClearColor = gameNS::DAY_SKY_COLOR;
			mLights[0].diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			mLights[3].att.y    = 0.55f;
			mLights[4].att.y    = 0.55f;
			mLights[5].att.y    = 0.55f;
			mLights[6].att.y    = 0.55f;
			dayCount++;
		}
	}
	if(timect >= gameNS::DAYLEN - gameNS::TRANSITIONTIME)
	{
		//mLights[0].diffuse  = D3DXCOLOR(0.45f, 0.45f, 0.45f, 1.0f);
		if(night) 
		{
			timeOfDay = "Dawn";
			mClearColor += D3DXCOLOR(((gameNS::DAY_SKY_COLOR.r-gameNS::NIGHT_SKY_COLOR.r)/(gameNS::TRANSITIONTIME))*dt, ((gameNS::DAY_SKY_COLOR.g-gameNS::NIGHT_SKY_COLOR.g)/(gameNS::TRANSITIONTIME))*dt, ((gameNS::DAY_SKY_COLOR.b-gameNS::NIGHT_SKY_COLOR.b)/(gameNS::TRANSITIONTIME))*dt, 1.0f);
			//mClearColor += (gameNS::DAY_SKY_COLOR - gameNS::NIGHT_SKY_COLOR)/((gameNS::TRANSITIONTIME)*dt);
			mLights[0].diffuse += D3DXCOLOR(((1.0 -0.1)/(gameNS::TRANSITIONTIME))*dt, ((1.0 -0.1)/(gameNS::TRANSITIONTIME))*dt, ((1.0 -0.1)/(gameNS::TRANSITIONTIME))*dt, 0.0f);
			mLights[3].att.y    += ((0.55-0.05f)/(gameNS::TRANSITIONTIME))*dt;
			mLights[4].att.y    += ((0.55-0.05f)/(gameNS::TRANSITIONTIME))*dt;
			mLights[5].att.y    += ((0.55-0.05f)/(gameNS::TRANSITIONTIME))*dt;
			mLights[6].att.y    += ((0.55-0.05f)/(gameNS::TRANSITIONTIME))*dt;
		}
		else 
		{
			timeOfDay = "Evening";
			mClearColor -= D3DXCOLOR(((gameNS::DAY_SKY_COLOR.r-gameNS::NIGHT_SKY_COLOR.r)/(gameNS::TRANSITIONTIME))*dt, ((gameNS::DAY_SKY_COLOR.g-gameNS::NIGHT_SKY_COLOR.g)/(gameNS::TRANSITIONTIME))*dt, ((gameNS::DAY_SKY_COLOR.b-gameNS::NIGHT_SKY_COLOR.b)/(gameNS::TRANSITIONTIME))*dt, 1.0f);
			//mClearColor -= (gameNS::DAY_SKY_COLOR - gameNS::NIGHT_SKY_COLOR)/((gameNS::TRANSITIONTIME)*dt);
			mLights[0].diffuse -= D3DXCOLOR(((1.0-0.1)/(gameNS::TRANSITIONTIME))*dt, ((1.0-0.1)/(gameNS::TRANSITIONTIME))*dt, ((1.0-0.1)/(gameNS::TRANSITIONTIME))*dt, 0.0f);
			mLights[3].att.y    -= ((0.55-0.05f)/(gameNS::TRANSITIONTIME))*dt;
			mLights[4].att.y    -= ((0.55-0.05f)/(gameNS::TRANSITIONTIME))*dt;
			mLights[5].att.y    -= ((0.55-0.05f)/(gameNS::TRANSITIONTIME))*dt;
			mLights[6].att.y    -= ((0.55-0.05f)/(gameNS::TRANSITIONTIME))*dt;
		}
	}
	
}



void ColoredCubeApp::drawScene()
{
	D3DApp::drawScene();
	incrementedYMargin = 5;
	lineHeight = 20;
	bool playing = (!startScreen && !endScreen);

	setDeviceAndShaderInformation();

	if(playing) {		
		mVP = mView*mProj;

		mfxDiffuseMapVar->SetResource(mDiffuseMapRVEnemy);
		mfxSpecMapVar->SetResource(mSpecMapRVEnemy);
		for(int i=0; i<gameNS::MAX_NUM_ENEMIES; i++)enemy[i].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
		if(debugMode) for(int i=0; i<WAYPOINT_SIZE*WAYPOINT_SIZE; i++) wayLine[i].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
		//for(int i=0; i<gameNS::WAYPT_SIZE; i++) for(int j=0; j<gameNS::WAYPT_SIZE; j++) if(waypoints[i][j]->isActive())wayLine[i][j].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
		//drawOrigin();
		mfxDiffuseMapVar->SetResource(mDiffuseMapRVStreet);
		mfxSpecMapVar->SetResource(mSpecMapRVStreet);
		floor.draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
		drawWalls();
		drawBuildings();
		drawPickups();
		drawLamps();

		D3D10_TECHNIQUE_DESC techDesc;
		mTech->GetDesc( &techDesc );
		for(UINT p = 0; p < techDesc.Passes; ++p)
		{
			mWallMesh.draw();
		}

		Matrix mWVP = menu.getWorld() * (mVP);
		mfxWVPVar->SetMatrix((float*)&mWVP);
		mfxWorldVar->SetMatrix((float*)&menu.getWorld());
		//D3D10_TECHNIQUE_DESC techDesc;
		mTech->GetDesc( &techDesc );
		for(UINT p = 0; p < techDesc.Passes; ++p)
		{
			mTech->GetPassByIndex( p )->Apply(0);
			//menu.draw();
		}

		mfxDiffuseMapVar->SetResource(mDiffuseMapRVBullet);
		mfxSpecMapVar->SetResource(mSpecMapRVBullet);
		player.draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
		printText("Score: ", 20, 5, 0, 0, WHITE, player.getScore()); //This has to be the last thing in the draw function.
		printText("Health: ", 20, 25, 0, 0, RED, player.getHealth());
		printText("Ammo: ", 20, 45, 0, 0, BLUE, player.getAmmo());
		printText(timeOfDay + " ", 670, 20, 0, 0, WHITE, dayCount);

	}
	else if(startScreen)
	{
		D3D10_TECHNIQUE_DESC techDesc;
		mTech->GetDesc( &techDesc );
		for(UINT p = 0; p < techDesc.Passes; ++p)
		{
			menu.draw();
		}
		printText(sText);
	}
	else { // End Screen 
		if(!won){
			printText(lText);
			printText("Score: ", 350, 280, 0, 0, player.getScore());
		}
		else {
			printText(wText);
			printText("Score: ", 350, 280, 0, 0, player.getScore());
		}
	}
	
	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	md3dDevice->RSSetState(0);
	mFont->DrawText(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, BLACK);

	mSwapChain->Present(0, 0); //Comment this out for expert mode
}

void ColoredCubeApp::drawWalls() {
	mfxDiffuseMapVar->SetResource(mDiffuseMapRV);
	mfxSpecMapVar->SetResource(mSpecMapRV);
	for(int i=0; i<gameNS::NUM_WALLS; i++)
		walls[i].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
}

void ColoredCubeApp::drawBuildings() {
	mfxDiffuseMapVar->SetResource(mDiffuseMapRVBuilding);
	mfxSpecMapVar->SetResource(mSpecMapRVBuilding);
	for(int i=0; i<gameNS::NUM_BUILDINGS; i++)
		buildings[i].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);

}

void ColoredCubeApp::printText(DebugText text) {
	for (int i = 0; i < text.getSize(); i++)
		{
			int xMargin = text.lines[i].x;
			int yMargin = text.lines[i].y;

			if (xMargin == -1)
				xMargin = 5;
			if (yMargin == -1) {
				yMargin = incrementedYMargin;
				incrementedYMargin += lineHeight;
			}
			printText(text.lines[i].s, xMargin, yMargin, 0, 0, WHITE);
		}
}

void ColoredCubeApp::printText(string text, int rectPosX, int rectPosY, int rectWidth, int rectHeight, D3DXCOLOR color, int value) {
	
	RECT POS = {rectPosX, rectPosY, rectWidth, rectHeight};
	std::wostringstream outs;   
	outs.precision(6);
	if (value != -1)
		outs << text.c_str() << value;
	else 
		outs << text.c_str();
	std::wstring sc = outs.str();
	mFont->DrawText(0, sc.c_str(), -1, &POS, DT_NOCLIP, color);
}

void ColoredCubeApp::drawLine(LineObject* line) {
	mWVP = line->getWorldMatrix()*mView*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	line->setMTech(mTech);
	line->draw();
}

void ColoredCubeApp::buildFX()
{
		DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif

	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;
	hr = D3DX10CreateEffectFromFile(L"lighting.fx", 0, 0, 
		"fx_4_0", shaderFlags, 0, md3dDevice, 0, 0, &mFX, &compilationErrors, 0);
	if(FAILED(hr))
	{
		if( compilationErrors )
		{
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX10CreateEffectFromFile", true);
	} 

	mTech = mFX->GetTechniqueByName("LightTech");
	
	mfxWVPVar		= mFX->GetVariableByName("gWVP")->AsMatrix();
	mfxWorldVar		= mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxEyePosVar	= mFX->GetVariableByName("gEyePosW");
	mfxLightVar		= mFX->GetVariableByName("gLight");
	mfxLightType	= mFX->GetVariableByName("gLightType")->AsScalar();
	mfxDiffuseMapVar= mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	mfxSpecMapVar	= mFX->GetVariableByName("gSpecMap")->AsShaderResource();
	mfxTexMtxVar	= mFX->GetVariableByName("gTexMtx")->AsMatrix();
	mfxLightNum		= mFX->GetVariableByName("gLightNum")->AsScalar();
}

void ColoredCubeApp::buildVertexLayouts()
{
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"DIFFUSE",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"SPECULAR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 56, D3D10_INPUT_PER_VERTEX_DATA, 0},
	};

	// Create the input layout
    D3D10_PASS_DESC PassDesc;
    mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(md3dDevice->CreateInputLayout(vertexDesc, 5, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}

Vector3 ColoredCubeApp::moveRuggerDirection()
{
	Vector3 dir = Vector3(0,0,0);

	return dir;
}

void ColoredCubeApp::setDeviceAndShaderInformation() {
	// Restore default states, input layout and primitive topology
	// because mFont->DrawText changes them.  Note that we can 
	// restore the default states by passing null.
	md3dDevice->OMSetDepthStencilState(0, 0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);

    md3dDevice->IASetInputLayout(mVertexLayout);
    md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set per frame constants.
	mfxEyePosVar->SetRawValue(&mEyePos, 0, sizeof(D3DXVECTOR3));
	mfxLightVar->SetRawValue(&mLights, 0, mLightNum*sizeof(Light));
	mfxLightType->SetInt(mLightType);
	mfxWVPVar->SetMatrix((float*)&mWVP);
	mfxWorldVar->SetMatrix((float*)&mCompCubeWorld);

	mfxLightNum->SetInt(mLightNum);
	D3DXMATRIX tm;
	Identity(&tm);
	mfxTexMtxVar->SetMatrix((float*)&tm);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif


	ColoredCubeApp theApp(hInstance);
	
	theApp.initApp();

	return theApp.run();
}

void ColoredCubeApp::onResize()
{
	D3DApp::onResize();

	float aspect = (float)mClientWidth/mClientHeight;
	D3DXMatrixPerspectiveFovLH(&mProj, 0.25f*PI, aspect, 1.0f, 1000.0f);
}

void ColoredCubeApp::drawOrigin() {
	//draw the lines
	drawLine(&xLine);
	drawLine(&yLine);
	drawLine(&zLine);
}

void ColoredCubeApp::drawPickups() {
	//Set mVP to be view*projection, so we can pass that into GO::draw(..)
	
	for (int i = 0; i < dayPickups.size(); i++)
		if (dayPickups[i].getActiveState()) dayPickups[i].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
	for (int i = 0; i < nightPickups.size(); i++) 
		if (nightPickups[i].getActiveState()) nightPickups[i].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);

}

void ColoredCubeApp::drawLamps() {
	mfxDiffuseMapVar->SetResource(mDiffuseMapRVPole);
	mfxSpecMapVar->SetResource(mSpecMapRVPole);
	for (int i = 0; i < lamps.size(); i++)
		lamps[i].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
}