#include "Pacman.h"

#include <sstream>

Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), _cPacmanFrameTime(250), _cMunchieFrameTime(500), _cCherryFrameTime(500)
{
	_pacman = new Player();
	_pacman->dead = false;
	_cherry = new Enemy();
	_pop = new SoundEffect();
	_death = new SoundEffect();


	_pacman->Direction = 0;
	_pacman->CurrentFrameTime = 0;
	_pacman->Frame = 0;
	_pacman->_cPacmanSpeed = 0.20f;
	_pacman->speedMultiplayer = 1.0f;
	_cherry->CurrentFrameTime = 0;
	_cherry->frameCount = 0;
	_cherry->eat = false;

	_ghosts[0] = new MovingEnemy();
	_ghosts[0]->direction = 0;
	_ghosts[0]->speed = 0.2f;

	

	int i;

	for (i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchie[i] = new Enemy();
		_munchie[i]->frameCount = rand() % 1;
		_munchie[i]->CurrentFrameTime = 0;
		_munchie[i]->eat = false;
		
	}


	_paused = false;

	//Initialise important Game aspects
	Audio::Initialise();
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Pacman::~Pacman()
{
	delete _pacman->Texture;
	delete _pacman->SourceRect;
	delete _pacman->Position;
	delete _pacman;
	delete _cherry->Texture;
	delete _cherry->Rect;
	delete _ghosts[0]->texture;
	delete _ghosts[0]->sourceRect;
	delete _ghosts[0]->position;
	delete _pop;
	delete _death;
	

	delete _munchie[0]->Texture;

	int nCount = 0;
	for (nCount = 0; nCount < MUNCHIECOUNT; nCount++)
	{
		delete _munchie[nCount]->Position;
		delete _munchie[nCount]->Rect;
		delete _munchie[nCount];
	}

	delete[] _munchie;

}


void Pacman::LoadContent()
{
	// Load Pacman
	_pacman->Texture = new Texture2D();
	_pacman->Texture->Load("Textures/Pacman.tga", false);
	_pacman->Position = new Vector2(350.0f, 350.0f);
	_pacman->SourceRect = new Rect(0.0f, 0.0f, 32, 32);
	
	_ghosts[0]->texture = new Texture2D();
	_ghosts[0]->texture->Load("Textures/GhostBlue.png", false);
	_ghosts[0]->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
	_ghosts[0]->sourceRect = new Rect(0.0f, 0.0f, 20, 20);
	

	Texture2D* munchieTex = new Texture2D();
	munchieTex->Load("Textures/MunchieTexture.png", false);

	int i;

	for (i = 0; i < MUNCHIECOUNT; i++)
	{


		// Load Munchie
		_munchie[i]->Texture = munchieTex;
		_munchie[i]->Position = new Vector2((rand() % Graphics::GetViewportHeight()), (rand() % Graphics::GetViewportHeight()));
		
		_munchie[i]->Rect = new Rect(0.0f, 0.0f, 12, 12);
	}




	_cherry->Texture = new Texture2D();
	_cherry->Texture->Load("Textures/CherryTexture.png", false);
	_cherry->Position = new Vector2(500.0f, 350.0f);
	_cherry->Rect = new Rect(0.0f, 0.0f, 32, 32);


	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);

	// Set Menu Paramters 
	_menuBackground = new Texture2D();
	_menuBackground->Load("Textures/Transparency.png", false);
	_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(),Graphics::GetViewportHeight());
	_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);

	_pop->Load("Sounds/pop.wav");
	_death->Load("Sounds/Death.wav");
	
}

void Pacman::Update(int elapsedTime)
{
	// Gets the current state of the keyboard
	
	_pacman->CurrentFrameTime += elapsedTime;
	_pacman->SourceRect->Y = _pacman->SourceRect->Height * _pacman->Direction;
	_pacman->SourceRect->X = _pacman->SourceRect->Width * _pacman->Frame;

	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
	
	
	CheckPaused(keyboardState, Input::Keys::P);


		if (!_paused)
		{
			// Calling the input function
			Input(elapsedTime, keyboardState);
			
			Input(elapsedTime, keyboardState);
			UpdatePacman(elapsedTime);
			UpdateCherry(elapsedTime);
			CheckViewportCollision();
			UpdateGhost(_ghosts[0], elapsedTime);
			CheckGhostCollisions();
			CheckMunchieCollisions();

			
			for (int i = 0; i < MUNCHIECOUNT; i++)
			{
				UpdateMunchie(_munchie[i], elapsedTime);
			}

		}


	
	
}

void Pacman::Input(int elapsedTime, Input::KeyboardState* keyboardState)
{
	float pacmanSpeed = _pacman->_cPacmanSpeed * elapsedTime * _pacman->speedMultiplayer;

	if (keyboardState->IsKeyDown(Input::Keys::D))
		{
			_pacman->Position->X += pacmanSpeed; //Moves Pacman across X axis 
			_pacman->Direction = 0;
		}

		else if (keyboardState->IsKeyDown(Input::Keys::W))
		{
			_pacman->Position->Y += -pacmanSpeed;	
			_pacman->Direction = 3;
		}

		else if (keyboardState->IsKeyDown(Input::Keys::A))
		{
			_pacman->Position->X += -pacmanSpeed;
			_pacman->Direction = 2;
			
		}

		else if (keyboardState->IsKeyDown(Input::Keys::S))
		{
			_pacman->Position->Y += pacmanSpeed;
			_pacman->Direction = 1;	
		}

	if (keyboardState->IsKeyDown(Input::Keys::LEFTSHIFT))
	{
		_pacman->speedMultiplayer = 2.0f;
	}
	else
	{
		_pacman->speedMultiplayer = 1.0f;
	}

	if (keyboardState->IsKeyDown(Input::Keys::R))
	{
		_cherry->Position = new Vector2((rand() % Graphics::GetViewportHeight()), (rand() % Graphics::GetViewportHeight()));
	}


}


void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Pacman X: " << _pacman->Position->X << " Y: " << _pacman->Position->Y;

	SpriteBatch::BeginDraw(); // Starts Drawing
	
	if (!_pacman->dead)
	{
		SpriteBatch::Draw(_pacman->Texture, _pacman->Position, _pacman->SourceRect); // Draws Pacman
	}


	int i;

	for (i = 0; i < MUNCHIECOUNT; i++)
	{
		if (!_munchie[i]->eat)
		{
			SpriteBatch::Draw(_munchie[i]->Texture, _munchie[i]->Position, _munchie[i]->Rect);
		}
		

	}
	
	if (!_cherry->eat)
	{
		SpriteBatch::Draw(_cherry->Texture, _cherry->Position, _cherry->Rect);
	}


	SpriteBatch::Draw(_ghosts[0]->texture, _ghosts[0]->position, _ghosts[0]->sourceRect);

	if (_paused)
	{
		std::stringstream menuStream;
		menuStream << "Game Paused";

		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
	}
	
	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);
	SpriteBatch::EndDraw(); // Ends Drawing
}


void Pacman::UpdatePacman(int elapsedTime)
{

	if (_pacman->CurrentFrameTime > _cPacmanFrameTime)
	{
		_pacman->Frame++;

		if (_pacman->Frame >= 2)
			_pacman->Frame = 0;

		_pacman->CurrentFrameTime = 0;

	}


}

void Pacman::UpdateMunchie(Enemy*munchie, int elapsedTime)
{

	munchie->CurrentFrameTime += elapsedTime;

			if (munchie->CurrentFrameTime > _cMunchieFrameTime)
			{
				munchie->frameCount++;

				if (munchie->frameCount >= 2)
					munchie->frameCount = 0;

				munchie->CurrentFrameTime = 0;
				munchie->Rect->X = munchie->Rect->Width * munchie->frameCount;
			}

}


void Pacman::UpdateCherry(int elapsedTime)
{

	_cherry->CurrentFrameTime += elapsedTime;

	if (_cherry->CurrentFrameTime > _cCherryFrameTime)
	{
		_cherry->frameCount++;

		if (_cherry->frameCount >= 2)
			_cherry->frameCount = 0;

		_cherry->CurrentFrameTime = 0;
		_cherry->Rect->X = _cherry->Rect->Width * _cherry->frameCount;
	}

	
}

void Pacman::CheckViewportCollision()
{

	// Checks if Pacman is trying to disappear
			if (_pacman->Position->X + _pacman->SourceRect->Width > 1024) // 1024 is the game Width
			{
				// Pacman hits the right wall - reset his position 
				_pacman->Position->X = 1024 - _pacman->SourceRect->Width;

			}

			if (_pacman->Position->X + _pacman->SourceRect->Width < 32)
			{
				_pacman->Position->X = 32 - _pacman->SourceRect->Width;
			}

			if (_pacman->Position->Y + _pacman->SourceRect->Width < 32)
			{
				_pacman->Position->Y = 32 - _pacman->SourceRect->Width;
			}

			if (_pacman->Position->Y + _pacman->SourceRect->Width > 768)
			{
				_pacman->Position->Y = 768 - _pacman->SourceRect->Width;
			}

			if (_ghosts[0]->position->X + _ghosts[0]->sourceRect->Width > 1024) // 1024 is the game Width
			{
				
				_ghosts[0]->position->X = 1024 - _ghosts[0]->sourceRect->Width;

			}

			if (_ghosts[0]->position->X + _ghosts[0]->sourceRect->Width < 32)
			{
				_ghosts[0]->position->X = 32 - _ghosts[0]->sourceRect->Width;
			}

			if (_ghosts[0]->position->Y + _ghosts[0]->sourceRect->Width < 32)
			{
				_ghosts[0]->position->Y = 32 - _ghosts[0]->sourceRect->Width;
			}

			if (_ghosts[0]->position->Y + _ghosts[0]->sourceRect->Width > 768)
			{
				_ghosts[0]->position->Y = 768 - _ghosts[0]->sourceRect->Width;
			}

}

void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey)
{

	if (state->IsKeyDown(pauseKey) && !_pKeyDown)
	{
		_pKeyDown = true;
		_paused = !_paused;
	}

	if (state->IsKeyUp(pauseKey))
		_pKeyDown = false;
		




}

void Pacman::UpdateGhost(MovingEnemy* ghost, int elapsedTime)
{
	if (ghost->direction == 0)
	{
		ghost->position->X += ghost->speed * elapsedTime;
	}
	else if (ghost->direction == 1)
	{
		ghost->position->X -= ghost->speed * elapsedTime;
	}


	if (ghost->position->X + ghost->sourceRect->Width > -Graphics::GetViewportWidth())
	{
		ghost->direction = 1;
	}
	else if (ghost->position->X <= 0)
	{
		ghost->direction = 0;
	}

	
}

void Pacman::CheckGhostCollisions()
{
	int i = 0;
	int bottom1 = _pacman->Position->Y + _pacman->SourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->Position->X;
	int left2 = 0;
	int right1 = _pacman->Position->X + _pacman->SourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->Position->Y;
	int top2 = 0;

	for (i = 0; i < GHOSTCOUNT; i++)
	{
		bottom2 = _ghosts[i]->position->Y + _ghosts[i]->sourceRect->Height;
		left2 = _ghosts[i]->position->X;
		right2 = _ghosts[i]->position->X + _ghosts[i]->sourceRect->Width;
		top2 = _ghosts[i]->position->Y;

		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			Audio::Play(_death);
			_pacman->dead = true;
			i = GHOSTCOUNT;
			
		}
	}


}

void Pacman::CheckMunchieCollisions()
{
	int i = 0;
	int bottom1 = _pacman->Position->Y + _pacman->SourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->Position->X;
	int left2 = 0;
	int right1 = _pacman->Position->X + _pacman->SourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->Position->Y;
	int top2 = 0;
	

	for (i = 0; i < MUNCHIECOUNT; i++)
	{
		bottom2 = _munchie[i]->Position->Y + _munchie[i]->Rect->Height;
		left2 = _munchie[i]->Position->X;
		right2 = _munchie[i]->Position->X + _munchie[i]->Rect->Width;
		top2 = _munchie[i]->Position->Y;
		

		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			Audio::Play(_pop);
			_munchie[i]->eat = true;
			_munchie[i]->Position->X = -10;
			i = MUNCHIECOUNT;
			
		}
	}


}


void Pacman::CheckCherryCollisions()
{
	int i = 0;
	int bottom1 = _pacman->Position->Y + _pacman->SourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->Position->X;
	int left2 = 0;
	int right1 = _pacman->Position->X + _pacman->SourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->Position->Y;
	int top2 = 0;

	for (i = 0; i < MUNCHIECOUNT; i++)
	{
		bottom2 = _cherry->Position->Y + _cherry->Rect->Height;
		left2 = _cherry->Position->X;
		right2 = _cherry->Position->X + _cherry->Rect->Width;
		top2 = _cherry->Position->Y;

		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			_cherry->eat = true;
		}
	}
}