#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include<stdlib.h>
#include<conio.h>
#include<Windows.h>
#include<ft2build.h>
#include FT_FREETYPE_H

float dispx = 200;
float dispy = 100;
GLFWwindow* window;
//screen and shader vars
int scw=1000, sch=600;
unsigned int sp;
int sel = 0;
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
//end the above here

int mazev = 5, mazeh = 7;

bool pm, pmb = false;
int px=-2, py=-2;
int mpx, mpy;

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
		return -(o-1);
	}
}
void initmaze(int mazev,int mazeh)
{
	int i, j;
	float l = 50.0, h = 50.0;
	maz = (struct box**)malloc(mazev*sizeof(struct box*));
	for (i = 0; i < mazev; i++)
		maz[i] = (struct box*)malloc(mazeh * sizeof(struct box));
		if(maz[i]==NULL)
		{
			std::cout << "some err";
			free(maz);
			exit(-5);
		}
	for(i = 0; i < mazev;i++)
	{
		for (j = 0; j < mazeh; j++)
		{
			maz[i][j] = { true,true,true,true,l*j+dispx,h*i+dispy };
		}
	}
	::mazev = mazev;
	::mazeh = mazeh;
}
void reinitmaze(int mazev = ::mazev, int mazeh = ::mazeh, int rst = 0)
{
	int i, j;
	float l = 50.0, h = 50.0;
	maz = (struct box**)realloc(maz,mazev * sizeof(struct box*));
	for (i = 0; i < mazev; i++)
		maz[i] = (struct box*)realloc(maz[i],mazeh * sizeof(struct box));
	if (maz[i] == NULL)
	{
		std::cout << "some err";
		free(maz);
		exit(-5);
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
}
void line(float x1,float y1,float x2,float y2,int r = 255, int g = 255, int b = 255, int a = 255)
{
	unsigned int vao, vbo;
	float arr[4] = { calcpos(x1,0),calcpos(y1,1), calcpos(x2,0), calcpos(y2,1) };
	glUniform4i(glGetUniformLocation(sp, "col"), r, g, b, a);
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(arr), arr, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glDrawArrays(1, 0, 2);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}
void drbox(float x1, float y1, float x2, float y2,int r=255,int g=255,int b=255,int a=255)
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
//end gl stuff

void drawmaze()
{
	int i, j, r=255, g=255, b=255, a=255;
	float l =50 ,h=50;
	//maz[1][0].u = false;
	//maz[2][0].d = false;
	/*line(0,600,1000,0);
	line(0, 600, 600, 0);
	line(0, 600, 400, 0);
	line(0, 600, 1000, 300);
	line(0, 600, 1000, 200);
	line(0, 600, 1000, 400);*/
	
	drbox(st.x + 2, st.y + 3, st.x + l - 3, st.y + l - 2, 0, 255, 0);
	drbox(en.x + 2, en.y + 3, en.x + l - 3, en.y + l - 2, 0, 0, 255);
	for (i = 0; i < mazev; i++)
	{
		for (j = 0; j < mazeh; j++)
		{
			if (maz[i][j].u == true)
				line(maz[i][j].x, maz[i][j].y, maz[i][j].x + l, maz[i][j].y, r, g, b, a);
			if (maz[i][j].d == true)
				line(maz[i][j].x, maz[i][j].y + h, maz[i][j].x + l, maz[i][j].y + h,r ,g ,b, a);
			if (maz[i][j].l == true)
				line(maz[i][j].x, maz[i][j].y, maz[i][j].x, maz[i][j].y+h, r, g, b, a);
			if (maz[i][j].r == true)
				line(maz[i][j].x + l, maz[i][j].y, maz[i][j].x + l, maz[i][j].y + h, r, g, b, a);
		}
	}
	//line(-1, 0, 1, 0);
	//line(0, -1, 0, 1);
	ab = true;
}
void drawui()
{
	int r=255, g=255, b=255;
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

void dfs()
{
	std::cout << "Hello from dfs\n";
	int cy = st.i, cx = st.j;
	int ey = en.i, ex = en.j;
	int cn = -1;
	int *node[5];
	int i, j,nl=-1,c;
	int msize = mazev * mazeh;
	bool fl = false;
	int k = 0;
	enum directions{u,r,d,l,nd} dir = nd, pd=nd ;
	node[0] = (int*)malloc(sizeof(int) * msize);  //x
	node[1] = (int*)malloc(sizeof(int) * msize);  //y
	node[2] = (int*)malloc(sizeof(int) * msize);  //prev node
	node[3] = (int*)malloc(sizeof(int) * msize);  //in direc
	node[4] = (int*)malloc(sizeof(int) * msize);  //out direc
	memset(node[0], -1, sizeof(int) * msize);
	memset(node[1], -1, sizeof(int) * msize);
	memset(node[2], -1, sizeof(int)* msize);
	memset(node[3], -1, sizeof(int)* msize);
	memset(node[4], -1, sizeof(int)* msize);
	
	while (true)
	{
		c = 0;
		dir = nd;
		if (maz[cy][cx].u == false)
		{
			c++;
		}
		if (maz[cy][cx].r == false)
		{
			c++;
		}
		if (maz[cy][cx].d == false)
		{
			c++;
		}
		if (maz[cy][cx].l == false)
		{
			c++;
		}
		if(c == 0)
		{
			if (nl == -1)
			{
				nl++;
				node[0][nl] = cx;
				node[1][nl] = cy;
				node[2][nl] = -1;
				node[3][nl] = -1;
				node[4][nl] = -1;
				cn = nl;
			}
			break;
		}
		if (c == 1)
		{
			if (maz[cy][cx].u == false)
			{
				dir = u;
			}
			else if (maz[cy][cx].r == false)
			{
				dir = r;
			}
			else if (maz[cy][cx].d == false)
			{
				dir = d;
			}
			else if (maz[cy][cx].l == false)
			{
				dir = l;
			}
			else
			{
				dir = nd;
			}

			if (nl == -1)
			{
				nl++;
				node[0][nl] = cx;
				node[1][nl] = cy;
				node[2][nl] = -1;
				node[3][nl] = 4;
				node[4][nl] = dir;
				pd = nd;
				cn = nl;		
			}
			else if (cx == en.j && cy == en.i)
			{
				nl++;
				node[0][nl] = cx;
				node[1][nl] = cy;
				node[2][nl] = cn;
				node[3][nl] = dir;
				node[4][nl] = 4;
				cn = nl;
				break;
			}
			else
			{
				cx = node[0][cn];
				cy = node[1][cn];
				dir = nd;
			}
		}
		else if (c == 2)
		{
			if (nl != -1)
			{
				pd = /*from here*/ (directions)(pd == 4 ? 4 : pd == 0 ? 2 : pd == 1 ? 3 : pd == 2 ? 0 : pd == 3 ? 1 : 4); //ret opdir
			}
			if (maz[cy][cx].l == false && pd != l)
			{
				dir = l;
			}
			if (maz[cy][cx].d == false && pd != d)
			{
				dir = d;
			}
			if (maz[cy][cx].r == false && pd != r)
			{
				dir = r;
			}
			if (maz[cy][cx].u == false && pd != u)
			{
				dir = u;
			}
			if (st.j == cx && st.i == cy)
			{
				if (nl == -1)
				{
					nl++;
					node[0][nl] = cx;
					node[1][nl] = cy;
					node[2][nl] = -1;
					node[3][nl] = 4;
					node[4][nl] = dir;
					pd = nd;
					cn = nl;
				}
				else
				{
					if (maz[cy][cx].l == false && node[4][0] != l)
					{
						dir = l;
					}
					if (maz[cy][cx].d == false && node[4][0] != d)
					{
						dir = d;
					}
					if (maz[cy][cx].r == false && node[4][0] != r)
					{
						dir = r;
					}
					node[4][0] = dir;
				}
			}
			else if (cx == en.j && cy == en.i)
			{
				nl++;
				node[0][nl] = cx;
				node[1][nl] = cy;
				node[2][nl] = cn;
				node[3][nl] = pd;
				node[4][nl] = nd;
				cn = nl;
				break;
			}
		}
		else if (c >= 3)
		{
			if (nl != -1)
			{
				pd = /*from here*/ (directions)(pd == 4 ? 4 : pd == 0 ? 2 : pd == 1 ? 3 : pd == 2 ? 0 : pd == 3 ? 1 : 4); //ret opdir
			}
			if (maz[cy][cx].l == false && pd != l)
			{
				dir = l;
			}
			if (maz[cy][cx].d == false && pd != d)
			{
				dir = d;
			}
			if (maz[cy][cx].r == false && pd != r)
			{
				dir = r;
			}
			if (maz[cy][cx].u == false && pd != u)
			{
				dir = u;
			}
			if (nl == -1)
			{
				nl++;
				node[0][nl] = cx;
				node[1][nl] = cy;
				node[2][nl] = -1;
				node[3][nl] = 4;
				node[4][nl] = dir;
				pd = nd;
				cn = nl;
			}
			else if (cx == en.j && cy == en.i)
			{
				nl++;
				node[0][nl] = cx;
				node[1][nl] = cy;
				node[2][nl] = cn;
				node[3][nl] = pd;
				node[4][nl] = nd;
				cn = nl;
				break;
			}
			else
			{
				for (i = 0; i <= nl; i++)
				{
					if (node[0][i] == cx && node[1][i] == cy)
					{
						break;
					}
				}
				if (i == nl+1)
				{
					nl++;
					node[0][nl] = cx;
					node[1][nl] = cy;
					node[2][nl] = cn;
					node[3][nl] = pd;
					node[4][nl] = dir;
					cn = nl;
				}
				else
				{
					while (node[4][cn] == nd)
					{
						cn = node[2][cn];
					}
					cx = node[0][cn];
					cy = node[1][cn];
					dir = nd;
					if (maz[cy][cx].l == false && node[3][cn] != l && node[4][cn] != l)
					{
						dir = l;
					}
					if (maz[cy][cx].d == false && node[3][cn] != d && (node[4][cn] != d && node[4][cn] != l))
					{
						dir = d;
					}
					if (maz[cy][cx].r == false && node[3][cn] != r && (node[4][cn] != r && node[4][cn] != d && node[4][cn] != l))
					{
						dir = r;
					}
					if (maz[cy][cx].u == false && node[3][cn] != u && (node[4][cn] != u && node[4][cn] != r && node[4][cn] != d && node[4][cn] != l))
					{
						dir = u;
					}
					
					node[4][cn] = dir;

					std::cout << node[4][cn] << "===========\n";
					for (i = 0; i <= nl; i++)
					{
						std::cout << node[0][i] << " " << node[1][i] << " " << node[2][i] << " " << node[3][i] << " " << node[4][i] << "\n";
					}
				}
			}
		}
		if (dir == u)
		{
			cy--;
		}
		else if (dir == r)
		{
			cx++;
		}
		else if (dir == d)
		{
			cy++;
		}
		else if (dir == l)
		{
			cx--;
		}
		pd = dir;
		std::cout << cx << " " << cy << "\n";
	}
	std::cout << "\n" << nl+1 << "\n";
	for (i = 0; i <= nl; i++)
	{
		std::cout << node[0][i] << " " << node[1][i] << " " << node[2][i] << " " << node[3][i] << " " << node[4][i] << "\n";
	}
	std::cout << "abc\n";

	cx = node[0][0];
	cy = node[1][0];
	drbox(maz[cy][cx].x, maz[cy][cx].y, maz[cy][cx].x + 50, maz[cy][cx].y + 50, 255, 0, 0);
	for (i = 0; i < nl; i++)
	{
		//glClear(GL_COLOR_BUFFER_BIT);
		while (node[4][i] == nd || i == nl)
		{
			i++;
		}
		dir = (directions)node[4][i];
		while (cx != node[0][i + 1] || cy != node[1][i + 1])
		{
			if (dir == u)
			{
				cy--;
			}
			else if (dir == r)
			{
				cx++;
			}
			else if (dir == d)
			{
				cy++;
			}
			else if (dir == l)
			{
				cx--;
			}

			pd = /*from here*/ (directions)(dir == 4 ? 4 : dir == 0 ? 2 : dir == 1 ? 3 : dir == 2 ? 0 : dir == 3 ? 1 : 4); //ret opdir

			if (maz[cy][cx].u == false && pd != u)
			{
				dir = u;
			}
			if (maz[cy][cx].r == false && pd != r)
			{
				dir = r;
			}
			if (maz[cy][cx].d == false && pd != d)
			{
				dir = d;
			}
			if (maz[cy][cx].l == false && pd != l)
			{
				dir = l;
			}

			if ((dir == l && pd == r) || (dir == r && pd == l))
			{
				drbox(maz[cy][cx].x, maz[cy][cx].y + 10, maz[cy][cx].x + 50, maz[cy][cx].y + 40, 150, 150, 0);
			}
			else if((dir == u && pd == d) || (dir == d && pd == u))
			{
				drbox(maz[cy][cx].x + 10, maz[cy][cx].y, maz[cy][cx].x + 40, maz[cy][cx].y + 50, 150, 150, 0);
			}
			else if ((dir == l && pd == u) || (dir == u && pd == l))
			{
				drbox(maz[cy][cx].x + 10, maz[cy][cx].y, maz[cy][cx].x + 40, maz[cy][cx].y + 40, 150, 150, 0);
				drbox(maz[cy][cx].x, maz[cy][cx].y+10, maz[cy][cx].x + 10, maz[cy][cx].y + 40, 150, 150, 0);
			}
			else if ((dir == l && pd == d) || (dir == d && pd == l))
			{
				drbox(maz[cy][cx].x + 10, maz[cy][cx].y+10, maz[cy][cx].x + 40, maz[cy][cx].y + 50, 150, 150, 0);
				drbox(maz[cy][cx].x, maz[cy][cx].y + 10, maz[cy][cx].x + 10, maz[cy][cx].y + 40, 150, 150, 0);
			}
			else if ((dir == r && pd == d) || (dir == d && pd == r))
			{
				drbox(maz[cy][cx].x + 10, maz[cy][cx].y+10, maz[cy][cx].x + 40, maz[cy][cx].y + 50, 150, 150, 0);
				drbox(maz[cy][cx].x + 40, maz[cy][cx].y + 10, maz[cy][cx].x + 50, maz[cy][cx].y + 40, 150, 150, 0);
			}
			else if ((dir == r && pd == u) || (dir == u && pd == r))
			{
				drbox(maz[cy][cx].x + 10, maz[cy][cx].y, maz[cy][cx].x + 40, maz[cy][cx].y + 40, 150, 150, 0);
				drbox(maz[cy][cx].x+40, maz[cy][cx].y + 10, maz[cy][cx].x+50, maz[cy][cx].y + 40, 150, 150, 0);
			}
		}
		drbox(maz[cy][cx].x, maz[cy][cx].y, maz[cy][cx].x + 50, maz[cy][cx].y + 50, 255, 0, 0);
	}
	drawmaze();
	drawui();
	glfwSwapBuffers(window);
	std::cout << "Press anything to continue";
	_getch();
	free(node[0]);
	free(node[1]);
	free(node[2]);
	free(node[3]);
	free(node[4]);
}
void bfs()
{
	std::cout << "Hello from bfs\n";
	
	int cy = st.i, cx = st.j;
	int ey = en.i, ex = en.j;
	int cn = -1;
	int* node[3];
	int* advan[3];
	int i, j, nl = -1, c,al=-1;
	int msize = mazev * mazeh;
	bool fl = false;
	int k = 0;
	enum directions { u, r, d, l, nd } dir = nd, pd = nd;
	node[0] = (int*)malloc(sizeof(int) * msize);  //x
	node[1] = (int*)malloc(sizeof(int) * msize);  //y
	node[2] = (int*)malloc(sizeof(int) * msize);  //in direc
	memset(node[0], -1, sizeof(int) * msize);
	memset(node[1], -1, sizeof(int) * msize);
	memset(node[2], -1, sizeof(int) * msize);
	advan[0] = (int*)malloc(sizeof(int) * msize);
	advan[1] = (int*)malloc(sizeof(int) * msize);
	advan[2] = (int*)malloc(sizeof(int) * msize);
	memset(advan[0], -1, sizeof(int)* msize);
	memset(advan[1], -1, sizeof(int)* msize);
	memset(advan[2], -1, sizeof(int)* msize);

	node[0][0] = cx;
	node[0][1] = cy;
	node[0][2] = nd;
	nl++;
	advan[0][0] = cx;
	advan[1][0] = cy;
	advan[2][0] = nd;
	al++;
	for (i = 0; i <= al; i++)
	{
		cx = advan[0][i];
		cy = advan[1][i];
		dir = (directions)advan[2][i];
		if ((cx == en.j && cy == en.i) || cx == -1 ||cy == -1)
		{
			al++;
			advan[0][al] = cx;
			advan[1][al] = cy;
			advan[2][al] = d;
			break;
		}
		if (maz[cy][cx].u == false && dir != u)
		{
			al++;
			advan[0][al] = cx;
			advan[1][al] = cy - 1;
			advan[2][al] = d;
		}
		if (maz[cy][cx].r == false && dir != r)
		{
			al++;
			advan[0][al] = cx + 1;
			advan[1][al] = cy;
			advan[2][al] = l;
		}
		if (maz[cy][cx].d == false && dir != d)
		{
			al++;
			advan[0][al] = cx;
			advan[1][al] = cy + 1;
			advan[2][al] = u;
		}
		if (maz[cy][cx].l == false && dir != l)
		{
			al++;
			advan[0][al] = cx - 1;
			advan[1][al] = cy;
			advan[2][al] = r;
		}
	}
	for (i = 0; i <= al; i++)
	{
		std::cout << advan[0][i] << " " << advan[1][i] << "\n";
		glClear(GL_COLOR_BUFFER_BIT);
		drbox(maz[advan[1][i]][advan[0][i]].x + 10, maz[advan[1][i]][advan[0][i]].y + 10, maz[advan[1][i]][advan[0][i]].x + 40, maz[advan[1][i]][advan[0][i]].y + 40);
		drawui();
		drawmaze();
		glfwSwapBuffers(window);
		Sleep(1000);
	}
	free(node[0]);
	free(node[1]);
	free(node[2]);
	free(advan[0]);
	free(advan[1]);
	free(advan[2]);
}

void mouse_callback(GLFWwindow* window, int b, int a, int m)
{
	int i, j;
	double x, y;
	bool fl = false;
	if (b == 4 && a == 1)
	{
		dfs();
	}
	else if (b == 3 && a == 1)
	{
		bfs();
	}
	glfwGetCursorPos(window, &x, &y);
	if (pm == false)
	{
		if (x > 50 && x < 100 && y < sch - 50.0f && y > sch - 100.0f)
		{
			sel = 0;
		}
		else if (x > 150 && x < 200 && y < sch - 50.0f && y > sch - 100.0f)
		{
			sel = 1;
		}
		else if (x > 250 && x < 300 && y < sch - 50.0f && y > sch - 100.0f)
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
	}
	else
	{
		pmb = false;
		if (x > 350 && x < 450 && y>10 && y < 60 && mpx > 350 && mpx < 450 && mpy>10 && mpy < 60 && b == 0 && a == 0)
		{
			reinitmaze(mazev,mazeh,1);
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
			std::cout << x << "  " << y << " " << b << " " << a << " " << m << std::endl;
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

void initfreetype()
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library\n";
	FT_Face face;
	if (FT_New_Face(ft, "OpenSans-Regular.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font\n";

	if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
		std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;

	FT_Set_Pixel_Sizes(face, 0, 48);
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
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
	glUseProgram(sp);
	glDeleteShader(vs);
	glDeleteShader(fs);
	//exit(-1);
/*	float vert[] = {
		-0.5,0.5,
		0.5,0.5,
		0.5,-0.5,
		-0.5,-0.5
	};
	

	unsigned int vao,vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
*/
	
	//glUniform4i(glGetUniformLocation(sp, "col"), 255, 0, 0, 255);
	glUseProgram(sp);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	//glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	initfreetype();
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		glClear(GL_COLOR_BUFFER_BIT);
		//glBindVertexArray(vao);
		//glDrawArrays(4, 0, 6);
		//line(0.0, 0.0, 1.0, 1.0);
		drawmaze();
		drawui();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
//	glDeleteVertexArrays(1, &vao);
	//glDeleteBuffers(1, &vbo);
	glfwTerminate();
	destmaze();
	return 0;
}