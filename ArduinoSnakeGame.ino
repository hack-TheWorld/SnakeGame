#include <U8glib.h>

U8GLIB_ST7920_128X64_4X u8g(13, 11, 12);

//button positions and values
int left = 2, right = 3, down = 4, up = 5;
int leftV = 0, rightV = 0, downV = 0, upV = 0;

//snakedirection:
//0: left
//1: right
//2: down
//3: up
int snakeDir = 1;

struct gameItem {
	int X; // x position
	int Y;  //y position
};

//diameter of the gameItemSize
int gameItemSize = 4;

//default snake size
unsigned int snakeSize = 5;

//array to store all snake body part positions
gameItem snake[100];

//snake food item
gameItem snakeFood;

//GameStates
enum gameState {
	setupGame,
	pausedGame,
	inGame,
	gameOver,
};

gameState state = setupGame;

void draw() {
	drawSnake();
	drawFood();
}
void draw2() {
	drawText("Game Over", 30, 30);
}
void drawText(String text, int x, int y) {
	//char array to print
	char charBuff[10];
	//convert the string to char array
	text.toCharArray(charBuff, 10);
	//prepare the font
	u8g.setFont(u8g_font_5x8);
	//print the text
	u8g.drawStr(x, y, charBuff);
}

void drawSnake() {
	for (int i = 0; i < snakeSize; i++) {
		u8g.drawFrame(snake[i].X, snake[i].Y, gameItemSize, gameItemSize);
	}

}

void drawFood() {
	u8g.drawBox(snakeFood.X, snakeFood.Y, gameItemSize, gameItemSize);
}

void handleColisions() {

	//check if snake eats food
	if (snake[0].X == snakeFood.X && snake[0].Y == snakeFood.Y) {
		//increase snakeSize
		snakeSize++;
		//regen food
		spawnSnakeFood();
	}

	//check if snake collides with itself
	else {
		for (int i = 1; i < snakeSize; i++) {
			if (snake[0].X == snake[i].X && snake[0].Y == snake[i].Y) {
				state = gameOver;
			}
		}
	}
	//check for wall collisions
	if ((snake[0].X < 1) || (snake[0].Y < 1) || (snake[0].X > 127) || (snake[0].Y > 63)) {
		state = gameOver;
	}
}


void spawnSnakeFood() {
	//generate snake Food position
	do {
		snakeFood.X = random(2, 126);
	} while (snakeFood.X % 4 != 0);
	do {
		snakeFood.Y = random(2, 62);
	} while (snakeFood.Y % 4 != 0);
}

void gameSetup() {
	//Reset snake size
	snakeSize = 5;

	//snakeHead initial position

	int sHeadX;
	int sHeadY;
	do {
		sHeadX = random(2, 126);
		
	} while (sHeadX % gameItemSize != 0);
	do {
		sHeadY = random(2, 62);
	} while (sHeadY % gameItemSize != 0);

	//set first part of the snake
	snake[0].X = sHeadX;
	snake[0].Y = sHeadY;
	//generate random positions for the rest of the snake
	for (int i = 1; i < snakeSize; i++) {
		//choose random direction to build snake
		int dir = random(0, 3);

		if (dir == 0)//means left
		{
			snake[i].X = snake[i-1].X - gameItemSize;
			snake[i].Y = snake[i - 1].Y;
		}

		else if (dir == 1)//means right
		{
			snake[i].X = snake[i - 1].X + gameItemSize;
			snake[i].Y = snake[i - 1].Y;
		}

		else if (dir == 2)//means up
		{
			snake[i].X = snake[i - 1].X;
			snake[i].Y = snake[i - 1].Y - gameItemSize;
		}

		else if (dir == 3)//means down
		{
			snake[i].X = snake[i - 1].X;
			snake[i].Y = snake[i - 1].Y + gameItemSize;
		}
	}

	//generate snake Food position
	spawnSnakeFood();

	//change state to inGame
	state = pausedGame;
}

void handleInput() {
	//read left button
	leftV = digitalRead(left);
	//read right button
	rightV = digitalRead(right);
	//read up button
	upV = digitalRead(up);
	//read down button
	downV = digitalRead(down);


	if (leftV != 0 && snakeDir != 1) {
		snakeDir = 0;
	}

	if (rightV != 0 && snakeDir != 0) {
		snakeDir = 1;
	}

	if (downV != 0 && snakeDir != 3) {
		snakeDir = 2;
	}

	if (upV != 0 && snakeDir != 2) {
		snakeDir = 3;
	}
}

void updateValues() {
	//update all body parts of the snake excpet the head
	for (int i = snakeSize - 1; i > 0; i--) {
		snake[i] = snake[i - 1];
	}

	//Now update the head
	//move left
	if (snakeDir == 0) {

		snake[0].X -= gameItemSize;
	}
	//move right
	else if (snakeDir == 1) {

		snake[0].X += gameItemSize;
	}

	//move down
	else if (snakeDir == 2) {

		snake[0].Y += gameItemSize;
	}

	//move up
	else if (snakeDir == 3) {

		snake[0].Y -= gameItemSize;
	}

}

void playGame() {
	handleColisions();
	handleInput();
	updateValues();
	
	u8g.firstPage();
	do {
		
		draw();
		delay(50);
	} while (u8g.nextPage());

}

void gamePaused() {
	//read left button
	leftV = digitalRead(left);
	//read right button
	rightV = digitalRead(right);
	//read up button
	upV = digitalRead(up);
	//read down button
	downV = digitalRead(down);


	if (leftV != 0) {
		state = inGame;
	}

	if (rightV != 0) {
		state = inGame;
	}

	if (downV != 0) {
		state = inGame;
	}

	if (upV != 0) {
		state = inGame;
	}

	u8g.firstPage();
	do {

		draw();
		delay(50);
	} while (u8g.nextPage());
}
void gameLost() {
	u8g.firstPage();
	do {

		draw2();
		delay(50);
	} while (u8g.nextPage());
	delay(2000);
	state = setupGame;
}

void setup() {
	//Set the pinmodes
	pinMode(left, INPUT);
	pinMode(right, INPUT);
	pinMode(down, INPUT);
	pinMode(up, INPUT);
	// random seed for the random function
	randomSeed(analogRead(0));

	if (u8g.getMode() == U8G_MODE_R3G3B2)
		u8g.setColorIndex(255); // white
	else if (u8g.getMode() == U8G_MODE_GRAY2BIT)
		u8g.setColorIndex(3); // max intensity
	else if (u8g.getMode() == U8G_MODE_BW)
		u8g.setColorIndex(1); // pixel on
}

void loop() {
	switch (state)
	{
	case setupGame: gameSetup();
		break;

	case pausedGame: gamePaused();
		break;
	case inGame: playGame();
		break;
	case gameOver: gameLost();
		break;
	}

}
