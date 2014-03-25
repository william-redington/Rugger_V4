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
#include "gameError.h"
#include "Player.h"
#include "Bullet.h"
#include "debugText.h"
#include "Audio.h"
#include "Money.h"
#include "namespaces.h"
#include "pickup.h"
#include <ctime>
using std::time;
#include "Light.h"

namespace gameNS {
	const int NUM_WALLS = 41;
	const int PERIMETER = 4;
	const int NUM_MONEY = 200;
	const int NUM_BULLETS = 5;
	const int NUM_PICKUPS = 4;
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
	void initMoneyPositions();
	void initBasicVariables();
	void initWallPositions();
	void initUniqueObjects();

	void updateScene(float dt);
	void updatePickups(float dt);
	void updateOrigin(float dt);
	void updateWalls(float dt);
	void updateUniqueObjects(float dt);
	void updatePlayer(float dt);
	void updateCamera();
	
	void handleUserInput();
	void handleWallCollisions(Vector3 pos);
	void handlePickupCollisions(float dt);

	void drawScene(); 
	void drawLine(LineObject*);
	void drawOrigin();
	void drawPickups();
	void drawWalls();
	void drawMoney();

	void onResize();
	Vector3 moveRuggerDirection();
	void printText(DebugText text);
	void printText(string text, int rectPosX, int rectPosY, int rectWidth, int rectHeight, int value = -1);
	void setDeviceAndShaderInformation();
	void doEndScreen();
	void firstPassCleanup();

private:
	void buildFX();
	void buildVertexLayouts();
 
private:
	Line rLine, bLine, gLine;
	Box mBox, redBox, brick, bulletBox, eBulletBox, yellowGreenBox, goldBox, blueBox, tealBox, maroonBox;
	Player player;
	vector<Bullet*> pBullets;
	LineObject xLine, yLine, zLine;
	Wall walls[gameNS::NUM_WALLS];
	Wall floor;
	Money money[gameNS::NUM_MONEY];
	vector<Pickup> pickups;
	GameObject superLowFloorOffInTheDistanceUnderTheScene;

	//Lighting-specific declarations
	//Light mLights[3];
	//int mLightType; // 0 (parallel), 1 (point), 2 (spot)

	float spinAmount;
	int shotTimer;
	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10InputLayout* mVertexLayout;
	ID3D10EffectMatrixVariable* mfxWVPVar;
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
	DebugText sText, eText;
};

ColoredCubeApp::ColoredCubeApp(HINSTANCE hInstance)
: D3DApp(hInstance), mFX(0), mTech(0), mVertexLayout(0),
  mfxWVPVar(0), mTheta(0.0f), mPhi(0.0f)
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
}

ColoredCubeApp::~ColoredCubeApp()
{
	if( md3dDevice )
		md3dDevice->ClearState();

	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);

	//CHANGE if audio needs to be released in d3dApp instead
	safeDelete(audio);
  // 
}



void ColoredCubeApp::initApp()
{
	D3DApp::initApp();

	initBasicGeometry();
	initBasicVariables(); //Like shotTimer, etc.
	initTextStrings(); //Like start/end screen text
	initUniqueObjects(); //Like the floor

	initOrigin();
	initBullets();
	initPickups();
	initWallPositions();
	initMoneyPositions();

	
	
		
	player.init(&mBox, pBullets, sqrt(2.0f), Vector3(-90,0,85), Vector3(0,0,0), 0, 1);

	buildFX();
	buildVertexLayouts();
	audio->playCue(MUSIC);
}

void ColoredCubeApp::initPickups() {
	for (int i = 0; i < gameNS::NUM_PICKUPS; i++)
		pickups.push_back(Pickup());

	pickups[0].init(&tealBox, Vector3(-35,0,95));
	pickups[1].init(&tealBox, Vector3(-6,0,-80));
	pickups[2].init(&tealBox, Vector3(-84,0,-56));
	pickups[3].init(&tealBox, Vector3(10,0,80));
}

void ColoredCubeApp::initBullets() {
	for (int i = 0; i < gameNS::NUM_BULLETS; i++) {
		pBullets.push_back(new Bullet());
		pBullets[i]->init(&bulletBox, 2.0f, Vector3(0,0,0), Vector3(0,0,0), 0, 1);
	}
}

void ColoredCubeApp::initBasicGeometry() {
	mBox.init(md3dDevice, 1.0f, WHITE);
	tealBox.init(md3dDevice, 1.0f, colorNS::TEAL);
	brick.init(md3dDevice, 1.0f, DARKBROWN);
	bulletBox .init(md3dDevice, 0.5f, BEACH_SAND);
	eBulletBox.init(md3dDevice, 0.5f, RED);
	maroonBox.init(md3dDevice, 10000, colorNS::MAROON);
	//redBox.init(md3dDevice, 0.00001f, RED);
	yellowGreenBox.init(md3dDevice, 1.f, LIGHT_YELLOW_GREEN);
	goldBox.init(md3dDevice, 1.0f, YELLOW);
	blueBox.init(md3dDevice, 2.0f, BLUE);
	rLine.init(md3dDevice, 10.0f, RED);
	bLine.init(md3dDevice, 10.0f, BLACK);
	gLine.init(md3dDevice, 10.0f, GREEN);
}

void ColoredCubeApp::initTextStrings() {
	sText.addLine("WELCOME RUGGER !", 10, 10);
	sText.addLine("WASD TO MOVE", 10, 30);
	sText.addLine("ARROW KEYS TO SHOOT", 10, 50);
	sText.addLine("HOLD SHIFT TO SPRINT!", 10, 70);
	sText.addLine("GO KILL DUNSTAN FOR ME!", 10, 90);
	sText.addLine("PRESS ANY KEY TO BEGIN !", 250, 300);
	eText.addLine("CONGRATS RUGGER I WON!", 250, 300);
}

void ColoredCubeApp::initMoneyPositions() {
	for(int i=0; i<gameNS::NUM_MONEY; i++)
		money[i].init(&goldBox, 2.0f, Vector3(rand()%190 - 90, 0, rand()%180 - 90), Vector3(0,0,0), 0, 1, rand()%2);
}

void ColoredCubeApp::initBasicVariables() {
	startScreen = false;
	shotTimer = 0;
}

void ColoredCubeApp::initWallPositions() {
	
//				   geom,  rad,  position,			sc,	w,  h,  d
	walls[0].init(&brick, 2.0f, Vector3(0, 0, 100),	1, 100, 10, 1);
	walls[1].init(&brick, 2.0f, Vector3(0, 0, -100),1, 100, 10, 1);
	walls[2].init(&brick, 2.0f, Vector3(100, 0, 0), 1, 1,	10, 100);
	walls[3].init(&brick, 2.0f, Vector3(-100, 0, 0),1, 1,	2, 100);
	walls[4].init(&brick, 2.0f, Vector3(-80,0,75),	1, 1,	2,	25);
	walls[5].init(&brick, 2.0f, Vector3(-80,0,25),	1, 20,	2,	1);
	walls[6].init(&brick, 2.0f, Vector3(-53.5,0,0),	1, 46.5,2,	1);
	walls[7].init(&brick, 2.0f, Vector3(-30,0, 25),	1, 1,	2,  25);
	walls[8].init(&brick, 2.0f, Vector3(-10,0, 50),	1, 10,	2,  1);
	walls[9].init(&brick, 2.0f, Vector3(0,0, 74),	1, 1,	2,  26);
	walls[10].init(&brick, 2.0f, Vector3(-5,0, 12),	1, 25,	2,  1);
	walls[11].init(&brick, 2.0f, Vector3(28.5,0, 0),1,21.5,2,  1);
	walls[12].init(&brick, 2.0f, Vector3(0.5,0, -37), 1, 1,	2,  37);
	walls[13].init(&brick, 2.0f, Vector3(20,0, 40), 1, 1,	2,  40);
	walls[14].init(&brick, 2.0f, Vector3(50,0, 80), 1, 30,	2,  1);
	walls[15].init(&brick, 2.0f, Vector3(75,0, 60), 1, 25,	2,  1);
	walls[16].init(&brick, 2.0f, Vector3(50,0, 40), 1, 30,	2,  1);
	walls[17].init(&brick, 2.0f, Vector3(75,0, 20), 1, 25,	2,  1);
	walls[18].init(&brick, 2.0f, Vector3(50,0, 10), 1, 30,	2,  1);
	walls[19].init(&brick, 2.0f, Vector3(80,0, 0), 1, 20,	2,  1);
	walls[20].init(&brick, 2.0f, Vector3(48.5,0, -30), 1, 1,	2,  30);
	walls[21].init(&brick, 2.0f, Vector3(60,0, -22.5), 1, 1,	2,  23);
	walls[22].init(&brick, 2.0f, Vector3(60, 0, -60), 1, 11.5,2,  1);
	walls[23].init(&brick, 2.0f, Vector3(-15, 0, -75), 1, 1, 2, 25);
	walls[24].init(&brick, 2.0f, Vector3(-40, 0, -50), 1, 40, 2, 1);
	walls[25].init(&brick, 2.0f, Vector3(-30, 0, -35), 1, 20, 2, 1);
	walls[26].init(&brick, 2.0f, Vector3(-30, 0, -15), 1, 20, 2, 1);
	walls[27].init(&brick, 2.0f, Vector3(-30, 0, -8), 1, 1, 2, 8);
	walls[28].init(&brick, 2.0f, Vector3(-30, 0, -42), 1, 1, 2, 8);
	walls[29].init(&brick, 2.0f, Vector3(-60, 0, -31), 1, 1, 2, 18);
	walls[30].init(&brick, 2.0f, Vector3(-70, 0, -31), 1, 1, 2, 18);
	walls[31].init(&brick, 2.0f, Vector3(-79, 0, -30), 1, 1, 2, 19);
	walls[32].init(&brick, 2.0f, Vector3(-79, 0, -85), 1, 1, 2, 15);
	walls[33].init(&brick, 2.0f, Vector3(-95, 0, -70), 1, 5, 2, 1);
	walls[34].init(&brick, 2.0f, Vector3(-95, 0, -30), 1, 5, 2, 1);
	walls[35].init(&brick, 2.0f, Vector3(80, 0, -25), 1, 1, 2, 25);
	walls[36].init(&brick, 2.0f, Vector3(48.5, 0, -70), 1, 1, 2, 15);
	walls[37].init(&brick, 2.0f, Vector3(29.5, 0, -85), 1, 20, 2, 1);
	walls[38].init(&brick, 2.0f, Vector3(-50, 0, 49), 1, 20, 2, 1);
	walls[39].init(&brick, 2.0f, Vector3(-50, 0, 70), 1, 20, 2, 1);
	walls[40].init(&brick, 2.0f, Vector3(-30, 0, 70), 1, 1, 2, 20);

}

void ColoredCubeApp::initUniqueObjects() {
	floor.init(&yellowGreenBox, 2.0f, Vector3(0,-1.5f,0), 1.0f, 100, 0.01, 100);
	superLowFloorOffInTheDistanceUnderTheScene.init(&maroonBox, 2.0f, Vector3(0,-10.0f,0), Vector3(0,0,0), 0, 100000);
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



void ColoredCubeApp::updateScene(float dt)
{
	bool playing = (!endScreen && !startScreen);
	Vector3 oldPos = player.getPosition();
	//firstPassCleanup(); //I don't think we will need this, since money isn't being randomly placed. Or is it?

	//if(input->getMouseRawX() < 0)
	if(input->isKeyDown(KEY_A))
	{
		mPhi -= 2.0f*dt;
	}
	//if(input->getMouseRawX() > 0)
	if(input->isKeyDown(KEY_D))
	{
		mPhi += 2.0f*dt;
	}
	//if(input->getMouseRawY() < 0)
	if(input->isKeyDown(KEY_W))
	{
		mTheta += 2.0f*dt;
	}
	//if(input->getMouseRawY() > 0)
	if(input->isKeyDown(KEY_S))
	{
		mTheta -= 2.0f*dt;
	}
	// Restrict the angle mPhi and radius mRadius.
	if( mTheta < -(PI/2.0f) + 0.01f)	mTheta = -(PI/2.0f) + 0.01f;
	if( mTheta > PI/2.0f - 0.01f)	mTheta = (PI/2.0f) - 0.01f;

	if(playing)
	{	
		//General Update
		D3DApp::updateScene(dt);
		updateOrigin(dt);
		handleUserInput();
		updatePlayer(dt);
		updatePickups(dt);
		updateWalls(dt);
		updateUniqueObjects(dt); //Like floor


		//Handle Collisions
		handleWallCollisions(oldPos);
		handlePickupCollisions(dt);
	}

	else if (startScreen) if(input->anyKeyPressed()) startScreen = false;
	else doEndScreen();

	updateCamera();
}

void ColoredCubeApp::firstPassCleanup() {
	//For the first update pass, we want to remove any money that is colliding with cameras or walls
	if(firstpass)
	{
		firstpass = false;
		for(int i=0; i<gameNS::NUM_WALLS; i++)
		{
			for(int k=0; k<gameNS::NUM_MONEY; k++)
			{
				if(money[k].getActiveState() && money[k].collided(&walls[i]))
				{
					money[k].setInActive();
				}
			}
		}
	}
}

void ColoredCubeApp::updateCamera() {
	//D3DXVECTOR3 pos(player.getPosition().x - 25, player.getPosition().y + 50, player.getPosition().z);
	//D3DXVECTOR3 target(player.getPosition());

	//pos will eventually be player.x, player.height, player.z)
	D3DXVECTOR3 pos(0, 5, 0);
	//target will start pointing in the +x direction and will be rotated according to the camera's net rotations
	//Should eventually be pos.x+1, pos.y, pos.z to make the camera rotate according to its own axis
	//D3DXVECTOR3 target(1, 0, 0);

	D3DXVECTOR4 t4(1, 0, 0, 1);
	Matrix rotZ, rotY, trans;
	D3DXMatrixRotationZ(&rotZ, mTheta);
	D3DXMatrixRotationY(&rotY, mPhi);
	D3DXMatrixTranslation(&trans, pos.x, pos.y, pos.z);
	//Rotate around the Z-axis first, then the Y-axis, then translate to the camera position
	Matrix transform = rotZ * rotY * trans;
	//t4 now holds the final position of the camera target point
	D3DXVec4Transform(&t4, &t4, &transform);
	//Assign it into a Vec3 and we should be good to go
	D3DXVECTOR3 target(t4.x, t4.y, t4.z);

	//Up remains unchanged.
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&mView, &pos, &target, &up);
}

void ColoredCubeApp::doEndScreen() {
	Sleep(2000);
	if(input->anyKeyPressed())
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

void ColoredCubeApp::updatePlayer(float dt) {
	player.setVelocity(moveRuggerDirection() * player.getSpeed());
	player.update(dt);
}

void ColoredCubeApp::handleUserInput() {
	if(input->isKeyDown(VK_UP)) player.shoot(UP);
	if(input->isKeyDown(VK_DOWN)) player.shoot(DOWN);
	if(input->isKeyDown(VK_LEFT)) player.shoot(LEFT);
	if(input->isKeyDown(VK_RIGHT)) player.shoot(RIGHT);
	if(input->isKeyDown(VK_SHIFT)) player.setSpeed(40);
	else player.setSpeed(20);

}

void ColoredCubeApp::handleWallCollisions(Vector3 pos) {
	for(int i=0; i<gameNS::NUM_WALLS; i++)
	{
		if(player.collided(&walls[i]))
			player.setPosition(pos);

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

void ColoredCubeApp::handlePickupCollisions(float dt) {
	for(int i=0; i<gameNS::NUM_MONEY; i++)
	{
		if(player.collided(&money[i]))
		{
			money[i].setInActive();
			score += money[i].getPoints();
			audio->playCue(CASH);
		}
		money[i].update(dt);
	}
}

void ColoredCubeApp::updatePickups(float dt) {
	for (int i = 0; i < pickups.size(); i++) {
			if (player.collided(&pickups[i])) {
				pickups[i].setInActive();
				player.charge();
			}
			pickups[i].update(dt);
	}
}

void ColoredCubeApp::updateOrigin(float dt) {
	xLine.update(dt);
	yLine.update(dt);
	zLine.update(dt);
}



void ColoredCubeApp::drawScene()
{
	D3DApp::drawScene();
	incrementedYMargin = 5;
	lineHeight = 20;
	bool playing = (!startScreen && !endScreen);

	setDeviceAndShaderInformation();

	if(playing) {		

		drawOrigin();
		floor.draw(mfxWVPVar, mTech, &mVP);
		drawWalls();
		drawPickups();
		drawMoney();
		player.draw(mfxWVPVar, mTech, &mVP);

		printText("Score: ", 5, 5, 0, 0, score); //This has to be the last thing in the draw function.
	}
	else if(startScreen)
		printText(sText);
	else { // End Screen 
		printText(eText);
		printText("Score: ", 300, 350, 0, 0, score);
	}
	
	mSwapChain->Present(0, 0); //Comment this out for expert mode
}

void ColoredCubeApp::drawWalls() {
	for(int i=0; i<gameNS::NUM_WALLS; i++)
		walls[i].draw(mfxWVPVar, mTech, &mVP);
}

void ColoredCubeApp::drawMoney() {
	for(int i=0; i<gameNS::NUM_MONEY; i++) 
		money[i].draw(mfxWVPVar, mTech, &mVP);
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
			printText(text.lines[i].s, xMargin, yMargin, 0, 0);
		}
}

void ColoredCubeApp::printText(string text, int rectPosX, int rectPosY, int rectWidth, int rectHeight, int value) {
	
	RECT POS = {rectPosX, rectPosY, rectWidth, rectHeight};
	std::wostringstream outs;   
	outs.precision(6);
	if (value != -1)
		outs << text.c_str() << score;
	else 
		outs << text.c_str();
	std::wstring sc = outs.str();
	mFont->DrawText(0, sc.c_str(), -1, &POS, DT_NOCLIP, WHITE);
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
	hr = D3DX10CreateEffectFromFile(L"color.fx", 0, 0, 
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

	mTech = mFX->GetTechniqueByName("ColorTech");
	
	mfxWVPVar = mFX->GetVariableByName("gWVP")->AsMatrix();
	mfxFLIPVar = mFX->GetVariableByName("flip");

}

void ColoredCubeApp::buildVertexLayouts()
{
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
    D3D10_PASS_DESC PassDesc;
    mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(md3dDevice->CreateInputLayout(vertexDesc, 2, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}

Vector3 ColoredCubeApp::moveRuggerDirection()
{
	Vector3 dir = Vector3(0,0,0);

	//if(input->isKeyDown(KEY_W))
	//{
	//	dir.x = 1;
	//}
	//if(input->isKeyDown(KEY_S)) dir.x = -1;
	//if(input->isKeyDown(KEY_A)) dir.z = 1;
	//if(input->isKeyDown(KEY_D)) dir.z = -1;

	//if(dir != Vector3(0,0,0))
	//		D3DXVec3Normalize(&dir, &dir);

	return dir;
}

void ColoredCubeApp::setDeviceAndShaderInformation() {
	//Restore Default States
	md3dDevice->OMSetDepthStencilState(0, 0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);
	md3dDevice->IASetInputLayout(mVertexLayout);
	// set some variables for the shader
	int foo[1];	foo[0] = 0;
	// set the point to the shader technique
	D3D10_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);
	//setting the color flip variable in the shader
	mfxFLIPVar->SetRawValue(&foo[0], 0, sizeof(int));
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
	mVP = mView*mProj;
	for (int i = 0; i < pickups.size(); i++)
	pickups[i].draw(mfxWVPVar, mTech, &mVP);
}
