#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

#define MUNCHIECOUNT 50

#define GHOSTCOUNT 1

// Just need to include main header file
#include "S2D/S2D.h"

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;



// Structure Definition
struct Player
{	
	bool dead;
	float _cPacmanSpeed;
	int CurrentFrameTime;
	int Direction;
	int Frame;
	Rect* SourceRect;
	Texture2D* Texture;
	Vector2* Position;
	float speedMultiplayer;
	
};

struct Enemy 
{
	// Data to represent Munchie/Cherry
	int frameCount;
	int CurrentFrameTime;
	Rect* Rect;
	Texture2D* Texture;
	Vector2* Position;
	bool eat;

};

struct MovingEnemy
{
	Vector2* position;
	Texture2D* texture;
	Rect* sourceRect;
	int direction;
	float speed;
};

// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
class Pacman : public Game
{
private:
	//Input Methods
	void Input(int elapsedTime, Input::KeyboardState* state);

	//Check Methods
	void CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey);
	void CheckViewportCollision();

	//Update methods
	void UpdatePacman(int elapsedTime);
	void UpdateMunchie(Enemy*, int elapsedTime);
	void UpdateCherry(int elapsedTime);
	
	Player* _pacman;
	Enemy* _munchie[MUNCHIECOUNT];
	Enemy* _cherry;
	MovingEnemy* _ghosts[GHOSTCOUNT];
	
	SoundEffect* _pop;
	SoundEffect* _death;
	
	// Position for String
	Vector2* _stringPosition;

	// Constant data for Game Variables 
	const int _cMunchieFrameTime;
	const int _cPacmanFrameTime;
	const int _cCherryFrameTime;
	
	
	// Data for Menu
	Texture2D* _menuBackground;
	Rect* _menuRectangle;
	Vector2* _menuStringPosition;
	bool _paused;
	bool _pKeyDown;
	bool _started;
	
	void CheckGhostCollisions();
	void UpdateGhost(MovingEnemy*, int elapsedTime);
	void CheckMunchieCollisions();
	void CheckCherryCollisions();


public:
	/// <summary> Constructs the Pacman class. </summary>
	Pacman(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Pacman();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);



};