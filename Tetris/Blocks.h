#pragma once

struct Block
{
	int xPos;
	int yPos;

	Block();
	Block(int x, int y);

	void moveTo(int x, int y);
};

enum Direction { left, right, down };