#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include<stdlib.h>
#include<conio.h>
#include<ft2build.h>
#include FT_FREETYPE_H
//----------------------------------------------------------------------------------------textbox
struct ivec2
{
	int x;
	int y;
};
struct stor
{
	unsigned int id;
	ivec2 size;
	ivec2 start;

}ch[255];
struct textbox
{
	int x,y;
	int w,h;
	int val;
	bool sel;
	int cpos;
}tb1 , tb2;
//-----------------------------------------------------------------------------textbox
float dispx = 200;
float dispy = 100;
GLFWwindow* window;
//screen and shader vars
int scw = 1000, sch = 600;
unsigned int sp,spt;
int sel = 0;
bool stop = false;
const char* vss =
"#version 460 core\n"
"layout (location=0) in vec2 pos;\n"
"void main()\n"
"{\n"
"	gl_Position=vec4(pos,0.0,1.0);\n"
"}\0";
const char* fss =
"#version 460 core\n"
"out vec4 fc;\n"
"void main()\n"
"{\n"
"	fc=vec4(1.0,1.0,1.0,1.0);\n"
"}\0";
const char* vssc =
"#version 460 core\n"
"layout (location=0) in vec2 pos;\n"
"out vec4 c;\n"
"uniform ivec4 col;\n"
"void main()\n"
"{\n"
"	c=vec4(col.r/255.0,col.g/255.0,col.b/255.0,col.a/255.0);\n"
"	gl_Position=vec4(pos,0.0,1.0);\n"
"}\0";
const char* fssc =
"#version 460 core\n"
"in vec4 c;"
"out vec4 fc;\n"
"void main()\n"
"{\n"
"	fc=c;"
"}\0";

//----------------------TEXT--------------------------
const char* ivss =
"#version 460 core\n"
"layout (location=0) in vec2 vert;\n"
"layout (location=1) in vec2 texcor;\n"
"out vec2 texcoord;\n"
"void main()\n"
"{\n"
"	gl_Position=vec4(vert,0.0,1.0);\n"
"	texcoord=texcor;\n"
"}\0";
const char* ifss =
"#version 460 core\n"
"in vec2 texcoord;\n"
"out vec4 col;\n"
"uniform sampler2D tex;\n"
"uniform vec4 texcol;\n"
"void main()\n"
"{\n"
"	col=vec4(1.0,1.0,1.0,texture(tex,texcoord).r);\n"
"	col=col*texcol/255.0;\n"
"}\0";
//----------------------TEXT--------------------------
//end the above here

int mazev = 5, mazeh = 7;
int id = 0;

bool pm, pmb = false;
int px = -2, py = -2;
int mpx, mpy;
int resbl, resdl;
double resb, resd;

struct pos
{
	float x, y;
	int i, j;
};
struct pos st = { 0, 0, -1, -1 }, en = { 50, 0, -1, -1 };
struct box
{
	bool u, d, l, r;
	float x, y;
	int vis;
	int id;
};
struct box** maz;
bool ab = false;
float calcpos(float a, bool s)
{
	float o;
	if (s == 0)
	{
		o = a * 2 / scw;
		return o - 1;
	}
	else
	{
		o = a * 2 / sch;
		return -(o - 1);
	}
}
void initmaze(int mazev, int mazeh)
{
	int i, j;
	float l = 50.0, h = 50.0;
	maz = (struct box**)malloc(mazev * sizeof(struct box*));
	for (i = 0; i < mazev; i++)
		maz[i] = (struct box*)malloc(mazeh * sizeof(struct box));
	if (maz[i] == NULL)
	{
		std::cout << "some err";
		free(maz);
		exit(-5);
	}
	for (i = 0; i < mazev; i++)
	{
		for (j = 0; j < mazeh; j++)
		{
			maz[i][j] = { true,true,true,true,l * j + dispx,h * i + dispy,0,id++ };
		}
	}
	::mazev = mazev;
	::mazeh = mazeh;
	tb1 = { scw - 400,sch - 100,100,50,mazev,false,0 };
	tb2 = { scw - 250,sch - 100,100,50,mazeh,false,0 };
}
void reinitmaze(int mazev = ::mazev, int mazeh = ::mazeh, int rst = 0)
{
	int i, j;
	float l = 50.0, h = 50.0;
	
	maz = (struct box**)realloc(maz, mazev * sizeof(struct box*));
	if (maz == NULL)
	{
		for (i = 0; i < ::mazev; i++)
			free(maz[i]);
		free(maz);
		std::cout << "Mem ERROR";
		exit(-6);
	}
	if (mazev > ::mazev)
	{
		for (i = 0; i < ::mazev; i++)
		{
			maz[i] = (struct box*)realloc(maz[i], mazeh * sizeof(struct box));
			if (maz[i] == NULL)
			{
				for (j = 0; j < i; j++)
					free(maz[j]);
				for (j = i + 1; j < ::mazev; j++)
					free(maz[i]);
				free(maz);
				std::cout << "Mem error";
				exit(-7);
			}
		}
		for (i = ::mazev; i < mazev; i++)
		{
			maz[i] = (struct box*)malloc(mazeh * sizeof(struct box));
			if (maz[i] == NULL)
			{
				for (j = 0; j < i; j++)
					free(maz[j]);
				for (j = i + 1; j < ::mazev; j++)
					free(maz[i]);
				free(maz);
				std::cout << "Mem error";
				exit(-8);
			}
		}
		if (mazeh > ::mazeh)
		{
			for (i = ::mazev; i < mazev; i++)
			{
				for (j = 0; j < ::mazeh; j++)
				{
					maz[i][j] = { true, true, true, true, maz[i - 1][j].x, maz[i - 1][j].y + h };
				}
			}
			for (i = 0; i < mazev; i++)
			{
				for (j = ::mazeh; j < mazeh; j++)
				{
					maz[i][j] = { true, true, true, true, maz[i][j - 1].x + l, maz[i][j - 1].y };
				}
			}
		}
		else
		{
			for (i = 0; i < mazeh; i++)
			{
				for (j = ::mazev; j < mazev; j++)
				{
					maz[j][i] = { true, true, true, true, maz[j - 1][i].x, maz[j - 1][i].y + h };
				}
			}
			if (st.j >= mazeh)
			{
				st = { 0,0,-1,-1 };
			}
			if (en.j >= mazeh)
			{
				en = { 50,0,-1,-1 };
			}
		}
		
	}
	else
	{
		for (i = 0; i < mazev; i++)
		{
			maz[i] = (struct box*)realloc(maz[i], mazeh * sizeof(struct box));
			if (maz[i] == NULL)
			{
				for (j = 0; j < i; j++)
					free(maz[j]);
				for (j = i + 1; j < ::mazev; j++)
					free(maz[i]);
				free(maz);
				std::cout << "Mem error";
				exit(-9);
			}
		}
		if (mazeh > ::mazeh)
		{
			for (i = ::mazeh; i < mazeh; i++)
			{
				for (j = 0; j < mazev; j++)
				{
					maz[j][i] = { true, true, true, true, maz[j][i - 1].x + l, maz[j][i - 1].y };
				}
			}
		}
		if (st.i >= mazev || st.j >= mazeh)
		{
			st = { 0,0,-1,-1 };
		}
		if (en.i >= mazev || en.j >= mazeh)
		{
			en = { 50,0,-1,-1 };
		}
	}
	if (rst == 1)
	{
		for (i = 0; i < mazev; i++)
		{
			for (j = 0; j < mazeh; j++)
			{
				maz[i][j] = { true,true,true,true,l * j + dispx,h * i + dispy };
			}
		}
		st = { 0,0,-1,-1 };
		en = { 50,0,-1,-1 };
	}
	::mazev = mazev;
	::mazeh = mazeh;

	std::cout << "\n\n\n\n";
	id = 0;
	for (i = 0; i < mazev; i++)
	{
		for (j = 0; j < mazeh; j++)
		{
			maz[i][j].id = id++;
			maz[i][j].vis = 0;
			std::cout << maz[i][j].x << " " << maz[i][j].y << "*-*";
			if (i == 0)
			{
				maz[i][j].u = true;
			}
			if (i == mazev - 1)
			{
				maz[i][j].d = true;
			}
			if (j == 0)
			{
				maz[i][j].l = true;
			}
			if (j == mazeh - 1)
			{
				maz[i][j].r = true;
			}
		}
		std::cout << "\n\n";
	}

}
void resvisit()
{
	int i, j;
	for (i = 0; i < mazev; i++)
	{
		for (j = 0; j < mazeh; j++)
		{
			maz[i][j].vis = 0;
		}
	}
}
void destmaze()
{
	int i;
	for (i = 0; i < mazev; i++)
		free(maz[i]);
	free(maz);
}

//gl stuff
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	scw = width;
	sch = height;
	glViewport(0, 0, width, height);
	tb1.x = scw-400;
	tb1.y = sch-100;
	tb2.x = scw-250;
	tb2.y = sch-100;
}
void line(float x1, float y1, float x2, float y2, int r = 255, int g = 255, int b = 255, int a = 255)
{
	unsigned int vao, vbo;
	float arr[4] = { calcpos(x1,0),calcpos(y1,1), calcpos(x2,0), calcpos(y2,1) };
	glUniform4i(glGetUniformLocation(sp, "col"), r, g, b, a);
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(arr), arr, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glDrawArrays(1, 0, 2);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}
void drbox(float x1, float y1, float x2, float y2, int r = 255, int g = 255, int b = 255, int a = 255)
{
	/*void box(top,left,bottom,right,red,green,blue.alpha);*/
	unsigned int vao, vbo;
	x1 = calcpos(x1, 0);
	y1 = calcpos(y1, 1);
	x2 = calcpos(x2, 0);
	y2 = calcpos(y2, 1);
	float arr[8] = { x1,y1,x2,y1,x2,y2,x1,y2 };
	glUniform4i(glGetUniformLocation(sp, "col"), r, g, b, a);
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(arr), arr, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glDrawArrays(6, 0, 4);//6=GL_TRIANGLEFAN
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glUniform4i(glGetUniformLocation(sp, "col"), 255, 255, 255, 255);

}
void renderchar(char c, float x1, float y1, float x2, float y2, int r = 255, int g = 255, int b = 255, int a = 255)
{

	unsigned int vao, vbo;
	float xlen = abs(x2 - x1), ylen = abs(y2 - y1);
	float ory = y1 + ylen * 0.85;
	float cw = (ch[c].size.x / 50.0) * xlen, che = (ch[c].size.y / 50.0) * ylen;
	float xb = (ch[c].start.x / 50.0) * xlen, yb = (ch[c].start.y / 50.0) * ylen;
	x1 = x1 + xlen * 0.1;
	x2 = calcpos(x1+xb+cw, 0);
	x1 = calcpos(x1+xb, 0);
	y2 = calcpos(ory-yb+che, 1);
	y1 = calcpos(ory-yb, 1);
	float arr[16] = {
			x1,y1,	0.0,0.0,
			x2,y1,	1.0,0.0,
			x2,y2,	1.0,1.0,
			x1,y2,	0.0,1.0,
	};

	glUseProgram(spt);


	glUniform4f(glGetUniformLocation(spt, "texcol"), r, g, b, a);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ch[c].id);

	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(arr), arr, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glDrawArrays(6, 0, 4);

	glUseProgram(sp);

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void rendertext(const char* a, float x1, float y1, float x2, float y2, int r = 255, int g = 255, int b = 255, int al = 255)
{
	float xlen = x1 > x2 ? x1 - x2 : x2 - x1;
	int i, len = strlen(a);
	xlen = xlen / len;
	for (i = 0; i < len; i++)
	{
		renderchar(a[i], x1, y1, x1 + xlen, y2,r,g,b,al);
		x1 = x1 + xlen;
	}
}
//end gl stuff

void drawmaze(int opt=0)
{
	int i, j, r = 255, g = 255, b = 255, a = 255;
	float l = 50, h = 50;
	char s[4];
	//maz[1][0].u = false;
	//maz[2][0].d = false;
	/*line(0,600,1000,0);
	line(0, 600, 600, 0);
	line(0, 600, 400, 0);
	line(0, 600, 1000, 300);
	line(0, 600, 1000, 200);
	line(0, 600, 1000, 400);*/

	if (opt == 1)
	{
		for (i = 0; i < mazev; i++)
		{
			for (j = 0; j < mazeh; j++)
			{
				if (maz[i][j].vis == 1)
				{
					drbox(maz[i][j].x, maz[i][j].y + 1, maz[i][j].x + l - 1, maz[i][j].y + h - 1, 255, 0, 0);
				}
				else if (maz[i][j].vis == 2)
				{
					drbox(maz[i][j].x, maz[i][j].y + 10, maz[i][j].x + l, maz[i][j].y + h - 10, 0, 150, 0);
				}
				else if (maz[i][j].vis == 3)
				{
					drbox(maz[i][j].x + 10, maz[i][j].y, maz[i][j].x + l - 10, maz[i][j].y + h, 0, 150, 0);
				}
				else if (maz[i][j].vis == 4)
				{
					drbox(maz[i][j].x, maz[i][j].y + 10, maz[i][j].x + l - 10, maz[i][j].y + h - 10, 0, 150, 0);
					drbox(maz[i][j].x + 10, maz[i][j].y + 10, maz[i][j].x + l - 10, maz[i][j].y + h, 0, 150, 0);
				}
				else if (maz[i][j].vis == 5)
				{
					drbox(maz[i][j].x + 10, maz[i][j].y + 10, maz[i][j].x + l, maz[i][j].y + h - 10, 0, 150, 0);
					drbox(maz[i][j].x + 10, maz[i][j].y + 10, maz[i][j].x + l - 10, maz[i][j].y + h, 0, 150, 0);
				}
				else if (maz[i][j].vis == 6)
				{
					drbox(maz[i][j].x, maz[i][j].y + 10, maz[i][j].x + l - 10, maz[i][j].y + h - 10, 0, 150, 0);
					drbox(maz[i][j].x + 10, maz[i][j].y, maz[i][j].x + l - 10, maz[i][j].y + h - 10, 0, 150, 0);
				}
				else if (maz[i][j].vis == 7)
				{
					drbox(maz[i][j].x + 10, maz[i][j].y + 10, maz[i][j].x + l, maz[i][j].y + h - 10, 0, 150, 0);
					drbox(maz[i][j].x + 10, maz[i][j].y, maz[i][j].x + l - 10, maz[i][j].y + h - 10, 0, 150, 0);
				}
			}
		}
	}
	for (i = 0; i < mazev; i++)
	{
		for (j = 0; j < mazeh; j++)
		{
			if (maz[i][j].u == true)
				line(maz[i][j].x, maz[i][j].y, maz[i][j].x + l, maz[i][j].y, r, g, b, a);
			if (maz[i][j].d == true)
				line(maz[i][j].x, maz[i][j].y + h, maz[i][j].x + l, maz[i][j].y + h, r, g, b, a);
			if (maz[i][j].l == true)
				line(maz[i][j].x, maz[i][j].y, maz[i][j].x, maz[i][j].y + h, r, g, b, a);
			if (maz[i][j].r == true)
				line(maz[i][j].x + l, maz[i][j].y, maz[i][j].x + l, maz[i][j].y + h, r, g, b, a);
			sprintf_s(s, 4, "%03d", maz[i][j].id);
			rendertext(s, maz[i][j].x, maz[i][j].y, maz[i][j].x + l, maz[i][j].y + h);
		}
	}

	drbox(st.x + 2, st.y + 3, st.x + l - 3, st.y + h - 2, 0, 255, 0);
	drbox(en.x + 2, en.y + 3, en.x + l - 3, en.y + h - 2, 0, 0, 255);
	//line(-1, 0, 1, 0);
	//line(0, -1, 0, 1);
	//ab = true;
}
void drawui()
{
	int r = 255, g = 255, b = 255;
	char a[5];
	drbox(50.0f, sch - 50.0f, 100.0f, sch - 100.0f, 255, 255, 255);
	drbox(51.0f, sch - 51.0f, 99.0f, sch - 99.0f, 0, 0, 0);
	drbox(150.0f, sch - 50.0f, 200.0f, sch - 100.0f, 0, 255, 0);
	drbox(250.0f, sch - 50.0f, 300.0f, sch - 100.0f, 0, 0, 255);
	if (sel == 0)
	{
		r = 0, g = 0, b = 0;
	}
	else if (sel == 1)
	{
		r = 0, g = 255, b = 0;
	}
	else if (sel == 2)
	{
		r = 0, g = 0, b = 255;
	}
	drbox(350.0f, sch - 50.0f, 450.0f, sch - 100.0f, 255, 255, 255);
	drbox(351.0f, sch - 51.0f, 449.0f, sch - 99.0f, r, g, b);
	drbox(350.0f, 10, 450.0f, 60, 255, 255, 255);

	drbox(351.0f, 11, 449.0f, 59, 255, 0, 0);
	rendertext("Reset", 360.0f, 20, 440.0f, 50);

	drbox(tb1.x, tb1.y, tb1.x + tb1.w, tb1.y + tb1.h);
	sprintf_s(a, 4, "%02d", tb1.val);
	rendertext(a, tb1.x+2, tb1.y+2, tb1.x + tb1.w-2, tb1.y-2 + tb1.h,255,0,0,255);

	drbox(tb2.x, tb2.y, tb2.x + tb2.w, tb2.y + tb2.h);
	sprintf_s(a, 4, "%02d", tb2.val);
	rendertext(a, tb2.x+2, tb2.y+2, tb2.x + tb2.w-2, tb2.y + tb2.h-2, 255, 0, 0, 255);

	drbox(tb2.x + 125, tb2.y, tb2.x + tb2.w + 125, tb2.y + tb2.h ,0, 255, 0, 150);
	rendertext("RESIZE", tb2.x + 127, tb2.y+2, tb2.x + tb2.w + 123, tb2.y + tb2.h-2);
}
void test()
{
	int i, j;
	initmaze(mazev, mazeh);
	for (i = 0; i < mazev; i++)
	{
		/*for (j = 0; j < mazeh; j++)
		{
			std::cout << "  " << maz[i][j].u << "   ";
		}
		std::cout << std::endl;
		for (j = 0; j < 10; j++)
		{
			std::cout << " " << maz[i][j].l << " " << maz[i][j].r << "  ";
		}
		std::cout << std::endl;
		for (j = 0; j < 10; j++)
		{
			std::cout << "  " << maz[i][j].d << "   ";
		}*/
		for (j = 0; j < mazeh; j++)
		{
			std::cout << maz[i][j].x << " " << maz[i][j].y << "*-*";
		}
		std::cout << "\n\n";
	}
	//destmaze();
}
void bfs()
{
	double stt, ent, t1, t2;
	int cy = st.i, cx = st.j;
	int* advan[4];
	int i, j, c=0, al = 0;
	int msize = mazev * mazeh;
	char str[10];
	enum directions { u, r, d, l, nd } dir = nd, pd = nd;
	std::cout << "Hello from bfs\n";

	advan[0] = (int*)malloc(sizeof(int) * msize);	//x
	advan[1] = (int*)malloc(sizeof(int) * msize);	//y
	advan[2] = (int*)malloc(sizeof(int) * msize);	//
	advan[3] = (int*)malloc(sizeof(int) * msize);	//
	memset(advan[0], -1, sizeof(int) * msize);
	memset(advan[1], -1, sizeof(int) * msize);
	memset(advan[2], -1, sizeof(int) * msize);
	memset(advan[3], -1, sizeof(int)* msize);

	if (st.i == -1 || st.j == -1 || en.i == -1 || en.j == -1 || (maz[st.i][st.j].u == true && maz[st.i][st.j].d == true && maz[st.i][st.j].l == true && maz[st.i][st.j].r == true) || (maz[en.i][en.j].u == true && maz[en.i][en.j].d == true && maz[en.i][en.j].l == true && maz[en.i][en.j].r == true))
	{
		std::cout << st.j << " " << st.i << " " << en.j << " " << en.i << "\nPlease assign start and end\n";
		free(advan[0]);
		free(advan[1]);
		free(advan[2]);
		free(advan[3]);
		return;
	}

	resvisit();

	stt = glfwGetTime();
	advan[0][0] = cx;
	advan[1][0] = cy;
	advan[2][0] = nd;
	advan[3][0] = -1;
	maz[cy][cx].vis = 1;
	
	for (i = 0; i <= al; i++)
	{
		cx = advan[0][i];
		cy = advan[1][i];
		dir = (directions)advan[2][i];
		if (maz[cy][cx].u == false && dir != u && maz[cy - 1][cx].vis == 0) 
		{
			al++;
			advan[0][al] = cx;
			advan[1][al] = cy - 1;
			advan[2][al] = d;
			advan[3][al] = i;
			maz[cy - 1][cx].vis = 1;
			if (cx == en.j && cy - 1 == en.i)
			{
				break;
			}
		}
		if (maz[cy][cx].r == false && dir != r && maz[cy][cx + 1].vis == 0)
		{
			al++;
			advan[0][al] = cx + 1;
			advan[1][al] = cy;
			advan[2][al] = l;
			advan[3][al] = i;
			maz[cy][cx + 1].vis = 1;
			if (cx + 1 == en.j && cy == en.i)
			{
				break;
			}
		}
		if (maz[cy][cx].d == false && dir != d && maz[cy + 1][cx].vis == 0)
		{
			al++;
			advan[0][al] = cx;
			advan[1][al] = cy + 1;
			advan[2][al] = u;
			advan[3][al] = i;
			maz[cy + 1][cx].vis = 1;
			if (cx == en.j && cy + 1 == en.i)
			{
				break;
			}
		}
		if (maz[cy][cx].l == false && dir != l && maz[cy][cx - 1].vis == 0)
		{
			al++;
			advan[0][al] = cx - 1;
			advan[1][al] = cy;
			advan[2][al] = r;
			advan[3][al] = i;
			maz[cy][cx - 1].vis = 1;
			if (cx - 1 == en.j && cy == en.i)
			{
				break;
			}
		}
	}
	ent = glfwGetTime();
	
	/*t1 = glfwGetTime();
	for (i = 0; i <= al; i++)
	{
		std::cout << advan[0][i] << " " << advan[1][i] << "\n";
		glClear(GL_COLOR_BUFFER_BIT);
		drawmaze();
		drbox(maz[advan[1][i]][advan[0][i]].x + 10, maz[advan[1][i]][advan[0][i]].y + 10, maz[advan[1][i]][advan[0][i]].x + 40, maz[advan[1][i]][advan[0][i]].y + 40);
		drawui();
		
		glfwSwapBuffers(window);
		while (glfwGetTime() - t1 < 0.5);
		t1 = glfwGetTime();
	}*/
	resvisit();
	i = al;
	do
	{
		cx = advan[0][i];
		cy = advan[1][i];
		if (i == 0)
		{
			maz[cy][cx].vis = 1;
			break;
		} 
		else
		{
			if (cx == en.j && cy == en.i)
			{
				maz[cy][cx].vis = 1;
			}
			cx = advan[0][advan[3][i]];
			cy = advan[1][advan[3][i]];
			dir = (directions)advan[2][i];
			dir = (directions)(dir == 4 ? 4 : dir == 0 ? 2 : dir == 1 ? 3 : dir == 2 ? 0 : dir == 3 ? 1 : 4);
			pd = (directions)advan[2][advan[3][i]];
			
			if ((dir == l && pd == r) || (dir == r && pd == l))
			{
				maz[cy][cx].vis = 2;
			}
			else if ((dir == u && pd == d) || (dir == d && pd == u))
			{
				maz[cy][cx].vis = 3;
			}
			else if ((dir == l && pd == d) || (dir == d && pd == l))
			{
				maz[cy][cx].vis = 4;
			}
			else if ((dir == r && pd == d) || (dir == d && pd == r))
			{
				maz[cy][cx].vis = 5;
			}
			else if ((dir == l && pd == u) || (dir == u && pd == l))
			{
				maz[cy][cx].vis = 6;
			}
			else if ((dir == r && pd == u) || (dir == u && pd == r))
			{
				maz[cy][cx].vis = 7;
			}
			i = advan[3][i];
		}
		c++;
	} while (dir != nd);
	glClear(GL_COLOR_BUFFER_BIT);
	sprintf_s(str, 10, "%.2f", (ent - stt) * 10E+6);
	rendertext(str, scw-300, 300, scw-100, 350);
	sprintf_s(str, 10, "%03d", c);
	rendertext(str, scw - 300, 350, scw - 150, 400);
	rendertext("Press any key to continue", scw - 425, 100, scw - 50, 150);
	resb = (ent - stt) * 10E+6;
	resbl = c;
	drawui();
	drawmaze(1);
	glfwSwapBuffers(window);
	resvisit();
	std::cout << "Press any key to continue\n";
	stop = true;
	free(advan[0]);
	free(advan[1]);
	free(advan[2]);
	free(advan[3]);
}
void dfs2()
{
	double startt, endt;
	int* node[5];
	int stt = 0;
	int cx, cy;
	int msize = mazev * mazeh;
	int i, j, c = 0;
	int fl = 0, fl2 = 0;
	char str[20];
	enum directions { u, r, d, l, nd } dir = nd, pd = nd;
	node[0] = (int*)malloc(sizeof(int) * msize);	//x
	node[1] = (int*)malloc(sizeof(int) * msize);	//y
	node[2] = (int*)malloc(sizeof(int) * msize);	//in dir
	node[3] = (int*)malloc(sizeof(int) * msize);	//possible out no
	node[4] = (int*)malloc(sizeof(int) * msize);	//out already done
	memset(node[0], -1, sizeof(int)* msize);
	memset(node[1], -1, sizeof(int)* msize);
	memset(node[2], -1, sizeof(int)* msize);
	memset(node[3], -1, sizeof(int)* msize);
	memset(node[4], -1, sizeof(int)* msize);
	resvisit();

	startt = glfwGetTime();
	if (st.i == -1 || st.j == -1 || en.i == -1 || en.j == -1 || (maz[st.i][st.j].u == true && maz[st.i][st.j].d == true && maz[st.i][st.j].l == true && maz[st.i][st.j].r == true) || (maz[en.i][en.j].u == true && maz[en.i][en.j].d == true && maz[en.i][en.j].l == true && maz[en.i][en.j].r == true))
	{
		std::cout << st.j << " " << st.i << " " << en.j << " " << en.i << "\nPlease assign start and end\n";
		free(node[0]);
		free(node[1]);
		free(node[2]);
		free(node[3]);
		free(node[4]);
		return;
	}
	node[0][0] = st.j;
	node[1][0] = st.i;
	node[2][0] = 4;
	node[3][0] = 0;
	node[4][0] = 0;
	if (maz[st.i][st.j].u == false)
		node[3][0]++;
	if (maz[st.i][st.j].r == false)
		node[3][0]++;
	if (maz[st.i][st.j].d == false)
		node[3][0]++;
	if (maz[st.i][st.j].l == false)
		node[3][0]++;

	while (stt != -1)
	{
		if (node[0][stt] == en.j && node[1][stt] == en.i)
		{
			break;
		}
		while (node[3][stt] == node[4][stt])
		{
			stt--;
			if(stt <= -1)
			{
				fl = 1;
				break;
			}
		}
		if (fl == 1)
		{
			break;
		}
		cx = node[0][stt];
		cy = node[1][stt];
		fl2 = nd;
		c = 0;
		if (node[2][stt] != u && maz[cy][cx].u == false)
		{
			c++;
			if (c == node[4][stt] + 1)
			{
				fl2 = u;
			}
		}
		if (node[2][stt] != r && maz[cy][cx].r == false && fl2 == nd)
		{
			c++;
			if (c == node[4][stt] + 1)
			{
				fl2 = r;
			}
		}
		if (node[2][stt] != d && maz[cy][cx].d == false && fl2 == nd)
		{
			c++;
			if (c == node[4][stt] + 1)
			{
				fl2 = d;
			}
		}
		if (node[2][stt] != l && maz[cy	][cx].l == false && fl2 == nd)
		{
			c++;
			if (c == node[4][stt] + 1)
			{
				fl2 = l;
			}
		}
		if (fl2 == nd)
		{
			break;
		}
		else
		{
			c = 0;
			if (fl2 == u)
			{
				cy--;
			}
			if (fl2 == r)
			{
				cx++;
			}
			if (fl2 == d)
			{
				cy++;
			}
			if (fl2 == l)
			{
				cx--;
			}
			if (maz[cy][cx].vis == 0)
			{

				node[4][stt]++;
				stt++;
				node[0][stt] = cx;
				node[1][stt] = cy;
				node[2][stt] = (directions)(fl2 == 4 ? 4 : fl2 == 0 ? 2 : fl2 == 1 ? 3 : fl2 == 2 ? 0 : fl2 == 3 ? 1 : 4); //ret opdir;
				node[4][stt] = 0;
				if (node[2][stt] != u && maz[cy][cx].u == false)
					c++;
				if (node[2][stt] != r && maz[cy][cx].r == false)
					c++;
				if (node[2][stt] != d && maz[cy][cx].d == false)
					c++;
				if (node[2][stt] != l && maz[cy][cx].l == false)
					c++;
				node[3][stt] = c;
				maz[cy][cx].vis = 1;
			}
			else
			{
				node[4][stt]++;
			}
		}
	/*	for (i = 0; i <= stt; i++)
		{
			for (j = 0; j < 5; j++)
			{
				std::cout << node[j][i] << " ";
			} 
			std::cout << "\n";
		}
		std::cout << "--------\n";*/
	}
	endt = glfwGetTime();

	resvisit();
	for (i = 0; i <= stt; i++)
	{
		for (j = 0; j < 5; j++)
		{
			std::cout << node[j][i] << " ";
		}
		std::cout << "\n";
		if (i == 0 || i == stt)
		{
			maz[node[1][i]][node[0][i]].vis = 1;
		}
		else
		{
			fl2 = node[2][i + 1];
			fl2 = (directions)(fl2 == 4 ? 4 : fl2 == 0 ? 2 : fl2 == 1 ? 3 : fl2 == 2 ? 0 : fl2 == 3 ? 1 : 4);
			if ((node[2][i] == l && fl2 == r) || (node[2][i] == r && fl2 == l))
			{
				maz[node[1][i]][node[0][i]].vis = 2;
			}
			else if ((node[2][i] == u && fl2 == d) || (node[2][i] == d && fl2 == u))
			{
				maz[node[1][i]][node[0][i]].vis = 3;
			}
			else if ((node[2][i] == l && fl2 == d) || (node[2][i] == d && fl2 == l))
			{
				maz[node[1][i]][node[0][i]].vis = 4;
			}
			else if ((node[2][i] == r && fl2 == d) || (node[2][i] == d && fl2 == r))
			{
				maz[node[1][i]][node[0][i]].vis = 5;
			}
			else if ((node[2][i] == l && fl2 == u) || (node[2][i] == u && fl2 == l))
			{
				maz[node[1][i]][node[0][i]].vis = 6;
			}
			else if ((node[2][i] == r && fl2 == u) || (node[2][i] == u && fl2 == r))
			{
				maz[node[1][i]][node[0][i]].vis = 7;
			}
		}
	}
	glClear(GL_COLOR_BUFFER_BIT);
	sprintf_s(str, 20, "%.2f", (endt - startt) * 10E+6);
	rendertext(str, scw-300, 300, scw-100, 350);
	sprintf_s(str, 20, "%03d", stt);
	rendertext(str, scw - 300, 350, scw - 150, 400);
	rendertext("Press any key to continue", scw - 425, 100, scw - 50, 150);
	drawmaze(1);
	drawui();
	glfwSwapBuffers(window);
	resd = (endt - startt) * 10E+6;
	resdl = stt;
	stop = true;
	resvisit();	
	free(node[0]);
	free(node[1]);
	free(node[2]);
	free(node[3]);
	free(node[4]);
}
void doboth()
{
	char str[30];
	dfs2();
	bfs();
	resvisit();
	glClear(GL_COLOR_BUFFER_BIT);
	if (resb < resd)
	{
		sprintf_s(str, 20, "BFS Time: %0.2f", resb);
		rendertext(str, scw - 400, sch - 300, scw - 50, sch - 250, 0, 255, 0);
		sprintf_s(str, 20, "DFS Time: %0.2f", resd);
		rendertext(str, scw - 400, sch - 250, scw - 50, sch - 200);
	}
	else if (resd < resb)
	{
		sprintf_s(str, 20, "BFS Time: %0.2f", resb);
		rendertext(str, scw - 400, sch - 300, scw - 50, sch - 250);
		sprintf_s(str, 20, "DFS Time: %0.2f", resd);
		rendertext(str, scw - 400, sch - 250, scw - 50, sch - 200, 0, 255, 0);
	}
	else
	{
		sprintf_s(str, 20, "BFS Time: %0.2f", resb);
		rendertext(str, scw - 400, sch - 300, scw - 50, sch - 250);
		sprintf_s(str, 20, "DFS Time: %0.2f", resd);
		rendertext(str, scw - 400, sch - 250, scw - 50, sch - 200);
	}
	
	sprintf_s(str, 20, "BFS Length: %03d", resbl);
	rendertext(str, scw - 400, sch - 200, scw - 50, sch - 150);
	sprintf_s(str, 20, "BFS Length: %03d", resdl);
	rendertext(str, scw - 400, sch - 150, scw - 50, sch - 100);
	rendertext("Press any key to continue ", scw - 400, sch - 400, scw - 50, sch - 350);
	rendertext("\"W\" - DFS || \"E\" - DFS", scw - 400, sch - 450, scw - 50, sch - 400);
	drawmaze();
	drawui();
	glfwSwapBuffers(window);

}
void dls(int n)
{

}
void mouse_callback(GLFWwindow* window, int b, int a, int m)
{
	int i, j;
	double x, y;
	bool fl = false;
	if (b == 4 && a == 1)
	{
		dfs2();
	}
	else if (b == 3 && a == 1)
	{
		bfs();
	}
	glfwGetCursorPos(window, &x, &y);
	std::cout << x << "  " << y << " " << b << " " << a << " " << m << std::endl;
	if (pm == false)
	{
		if (x > 50 && x < 100 && y < sch - 50.0 && y > sch - 100.0)
		{
			sel = 0;
		}
		else if (x > 150 && x < 200 && y < sch - 50.0 && y > sch - 100.0)
		{
			sel = 1;
		}
		else if (x > 250 && x < 300 && y < sch - 50.0 && y > sch - 100.0)
		{
			sel = 2;
		}
	}
	if (pmb == false)
	{
		if (x > 350 && x < 450 && y>10 && y < 60 && b == 0 && a == 1)
		{
			pmb = true;
			mpx = x;
			mpy = y;
		}
		if (x > tb1.x && x < tb1.x + tb1.w && y > tb1.y && y < tb1.y + tb1.h &&b == 0 && a == 1)
		{
			tb1.sel = true;
			tb1.cpos = 0;
		}
		if (x > tb2.x&& x < tb2.x + tb2.w && y > tb2.y&& y < tb2.y + tb2.h && b == 0 && a == 1)
		{
			tb2.sel = true;
			tb2.cpos = 0;
		}

		if (tb1.sel == true && b == 0 && a == 1 && !(x > tb1.x&& x < tb1.x + tb1.w && y > tb1.y && y < tb1.y + tb1.h ))
		{
			tb1.sel = false;
		}
		if (tb2.sel == true && b == 0 && a == 1 && !(x > tb2.x&& x < tb2.x + tb2.w && y > tb2.y&& y < tb2.y + tb2.h))
		{
			tb2.sel = false;
		}
		if (b == 0 && a == 1 && x > tb2.x+125 && x < tb2.x + tb2.w+125 && y > tb2.y && y < tb2.y + tb2.h)
		{
			pmb = true;
			mpx = x;
			mpy = y;
		}
	}
	else
	{
		pmb = false;
		if (x > 350 && x < 450 && y>10 && y < 60 && mpx > 350 && mpx < 450 && mpy>10 && mpy < 60 && b == 0 && a == 0)
		{
			reinitmaze(mazev, mazeh, 1);
		}
		if (b == 0 && a == 0 && x > tb2.x + 125 && x < tb2.x + tb2.w + 125 && y > tb2.y && y < tb2.y + tb2.h)
		{
			reinitmaze(tb1.val,tb2.val,0);
		}
	}
	if (sel == 0)
	{
		if (b == 0 && a == 1 && pm == false)
		{
			for (i = 0; i < mazev; i++)
			{
				for (j = 0; j < mazeh; j++)
				{
					if ((x > maz[i][j].x&& x < maz[i][j].x + 50.0) && (y > maz[i][j].y&& y < maz[i][j].y + 50.0) /*&& (i > 0) && (j > 0) && (i < mazev - 1) && (j < mazeh - 1)*/)
					{
						/*std::cout << "TRIGGERED " << i << " " << j << "\n";
						maz[i][j].u = false;
						maz[i][j].d = false;
						maz[i][j].l = false;
						maz[i][j].r = false;
						maz[i - 1][j].d = false;
						maz[i + 1][j].u = false;
						maz[i][j - 1].r = false;
						maz[i][j + 1].l = false;*/
						px = j;
						py = i;
						break;
					}
				}
			}
			pm = true;
		}
		if (b == 0 && a == 0 && pm == true)
		{
			for (i = 0; i < mazev; i++)
			{
				for (j = 0; j < mazeh; j++)
				{
					if ((x > maz[i][j].x&& x < maz[i][j].x + 50) && (y > maz[i][j].y&& y < maz[i][j].y + 50) /*&& (i > 0) && (j > 0) && (i < mazev - 1) && (j < mazeh - 1)*/)
					{
						if (px == j - 1 && py == i)
						{
							maz[i][j].l = false;
							maz[i][j - 1].r = false;
						}
						else if (px == j + 1 && py == i)
						{
							maz[i][j].r = false;
							maz[i][j + 1].l = false;
						}
						else if (px == j && py == i - 1)
						{
							maz[i][j].u = false;
							maz[i - 1][j].d = false;
						}
						else if (px == j && py == i + 1)
						{
							maz[i][j].d = false;
							maz[i + 1][j].u = false;
						}
					}
				}
			}
			pm = false;
			px = -2;
			py = -2;
		}
	}
	else if (sel == 1)
	{
		if (b == 0 && a == 1)
		{
			for (i = 0; i < mazev; i++)
			{
				for (j = 0; j < mazeh; j++)
				{
					if ((x > maz[i][j].x&& x < (double)maz[i][j].x + 50) && (y > maz[i][j].y&& y < (double)maz[i][j].y + 50))
					{
						st.x = maz[i][j].x;
						st.y = maz[i][j].y;
						st.i = i;
						st.j = j;
						fl = true;
						break;
					}
				}
				if (fl == true)
				{
					fl = false;
					break;
				}
			}
		}
	}
	else if (sel == 2)
	{
		if (b == 0 && a == 1)
		{
			for (i = 0; i < mazev; i++)
			{
				for (j = 0; j < mazeh; j++)
				{
					if ((x > maz[i][j].x&& x < maz[i][j].x + 50) && (y > maz[i][j].y&& y < maz[i][j].y + 50))
					{
						en.x = maz[i][j].x;
						en.y = maz[i][j].y;
						en.i = i;
						en.j = j;
						fl = true;
						break;
					}
				}
				if (fl == true)
				{
					fl = false;
					break;
				}
			}
		}
	}
	
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	int k = -1;
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS)
		k = 1;
	else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
		k = 2;
	else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS)
		k = 3;
	else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS)
		k = 4;
	else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS)
		k = 5;
	else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS)
		k = 6;
	else if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_7) == GLFW_PRESS)
		k = 7;
	else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS)
		k = 8;
	else if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_9) == GLFW_PRESS)
		k = 9;
	else if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_0) == GLFW_PRESS)
		k = 0;
	else
		k = -1;
	if (tb1.sel == true)
	{
		if (k != -1)
		{
			if (tb1.cpos == 0)
			{
				tb1.val = k;
				tb1.cpos = 1;
			}
			else
			{
				tb1.val = tb1.val * 10 + k;
				tb1.cpos = 0;
			}
		}
	}
	if (tb2.sel == true)
	{
		if (k != -1)
		{
			if (tb2.cpos == 0)
			{
				tb2.val = k;
				tb2.cpos = 1;
			}
			else
			{
				tb2.val = tb2.val * 10 + k;
				tb2.cpos = 0;
			}
		}
	}
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		doboth();
	}
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		dfs2();
	}
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		bfs();
	}
	if(action == GLFW_RELEASE && key != GLFW_KEY_Q && key != GLFW_KEY_W && key != GLFW_KEY_E)
		stop = false;
}
void initfreetype()
{
	unsigned char c;
	FT_Library ft;
	FT_Face face;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library\n";

	if (FT_New_Face(ft, "OpenSans-Regular.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font\n";
	FT_Set_Pixel_Sizes(face, 50, 50);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (c = 0; c < 255; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			exit(10);
		}
		unsigned int tex;
		glGenTextures(1, &ch[c].id);
		glBindTexture(GL_TEXTURE_2D, ch[c].id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
		//renderchar(c, 700, 150, 750, 200, 255, 255, 255, 255);
		ch[c].size = { (int)face->glyph->bitmap.width, (int)face->glyph->bitmap.rows };
		ch[c].start = { face->glyph->bitmap_left, face->glyph->bitmap_top };
		/*for (int i = 0; i < face->glyph->bitmap.rows; i++)
		{
			for (int j = 0; j < face->glyph->bitmap.width; j++)
			{
				std::cout << face->glyph->bitmap.buffer[i * face->glyph->bitmap.width + j];
			}
			std::cout << std::endl;
		}
		std::cout << face->glyph->bitmap.width << " " << face->glyph->bitmap.rows << "\n";
		std::cout << c<<" "<<ch[c].id << "\n";*/
		//std::cout << c << " " << ch[c].size.x << " " << ch[c].size.y << " " << ch[c].start.x << " " << ch[c].start.y << "\n";
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	std::cout << "=====================================================================\n";
}

int main()
{
	test();
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(scw, sch, "Maze", NULL, NULL);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -2;
	}

	unsigned int vs = glCreateShader(GL_VERTEX_SHADER), fs = glCreateShader(GL_FRAGMENT_SHADER);
	sp = glCreateProgram();
	glShaderSource(vs, 1, &vssc, NULL);
	glCompileShader(vs);
	/*int  success;
	char infoLog[512];
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vs, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}*/

	glShaderSource(fs, 1, &fssc, NULL);
	glCompileShader(fs);
	/*glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fs, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}*/

	glAttachShader(sp, vs);
	glAttachShader(sp, fs);
	glLinkProgram(sp);
	/*glGetProgramiv(sp, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(sp, 512, NULL, infoLog);
		std::cout << "LINK ERR: \n" << infoLog << std::endl;
	}*/
	//-------------------------------------------------------------------------------------------------------
	
	glDeleteShader(vs);
	glDeleteShader(fs);
	spt = glCreateProgram();
	vs = glCreateShader(GL_VERTEX_SHADER);
	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vs, 1, &ivss, NULL);
	glCompileShader(vs);
	/*glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vs, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}*/

	glShaderSource(fs, 1, &ifss, NULL);
	glCompileShader(fs);
	/*glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fs, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}*/

	glAttachShader(spt, vs);
	glAttachShader(spt, fs);
	glLinkProgram(spt);
	/*glGetProgramiv(sp, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(sp, 512, NULL, infoLog);
		std::cout << "LINK ERR: \n" << infoLog << std::endl;
	}*/

	glDeleteShader(vs);
	glDeleteShader(fs);
	//---------------------------------------------------------------------------------------------------------


	glUseProgram(sp);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetMouseButtonCallback(window, mouse_callback);
	glfwSetKeyCallback(window, key_callback);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	initfreetype();
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		processInput(window);
		if (stop == false)
		{
			glClear(GL_COLOR_BUFFER_BIT);
			drawmaze();
			drawui();
			glfwSwapBuffers(window);
		}
	}

	glfwTerminate();
	destmaze();
	return 0;
}