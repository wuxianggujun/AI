#ifndef AI_CORE
#define AI_CORE

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>

#include <vector>
#include <map>
#include <string>

#include <painter.h>

namespace Ai {
	void renderAiInit();

	void renderAi();

	void addLineShader();
	void addCanvasShader();

	void addLine(unsigned int id, float startPointX, float startPointY, float endPointX, float endPointY);
	void addLine(unsigned int id, float startPointX, float startPointY, float endPointX, float endPointY, 
		float red, float green, float blue);

	void addTriangle(unsigned int id, float xscale, float yscale, float xpos, float ypos);
	void addTriangle(unsigned int id, float xscale, float yscale, float xpos, float ypos, float red, float green, float blue);

	void addSquare(unsigned int id, float xscale, float yscale, float xpos, float ypos);
	void addSquare(unsigned int id, float xscale, float yscale, float xpos, float ypos, float red, float green, float blue);
}

#endif