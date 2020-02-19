#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include<stdlib.h>
#include<conio.h>

float dispx = 200;
float dispy = 100;

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

bool pm;
int px=-2, py=-2;

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
			else if (cx == en.x && cy == en.y)
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
		k++;
	}
	std::cout << "\n" << nl+1 << "\n";
	for (i = 0; i <= nl; i++)
	{
		std::cout << node[0][i] << " " << node[1][i] << " " << node[2][i] << " " << node[3][i] << " " << node[4][i] << "\n";
	}
	std::cout << "abc\n";
	free(node[0]);
	free(node[1]);
	free(node[2]);
	free(node[3]);
	free(node[4]);
}
void bfs()
{
	int cy = st.i, cx = st.j;
	int ey = en.i, ex = en.j;
	int cn = -1;
	int* node[5];
	int* advan[2];
	int i, j, nl = -1, c;
	int msize = mazev * mazeh;
	bool fl = false;
	int k = 0;
	enum directions { u, r, d, l, nd } dir = nd, pd = nd;
	node[0] = (int*)malloc(sizeof(int) * msize);  //x
	node[1] = (int*)malloc(sizeof(int) * msize);  //y
	node[2] = (int*)malloc(sizeof(int) * msize);  //prev node
	node[3] = (int*)malloc(sizeof(int) * msize);  //in direc
	node[4] = (int*)malloc(sizeof(int) * msize);  //out direc
	memset(node[0], -1, sizeof(int) * msize);
	memset(node[1], -1, sizeof(int) * msize);
	memset(node[2], -1, sizeof(int) * msize);
	memset(node[3], -1, sizeof(int) * msize);
	memset(node[4], -1, sizeof(int) * msize);
	advan[0] = (int*)malloc(sizeof(int) * msize);
	advan[1] = (int*)malloc(sizeof(int) * msize);
	memset(advan[0], -1, sizeof(int)* msize);
	memset(advan[1], -1, sizeof(int)* msize);

	while (true)
	{

	}

	free(node[0]);
	free(node[1]);
	free(node[2]);
	free(node[3]);
	free(node[4]);
	free(advan[0]);
	free(advan[1]);
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
	glfwGetCursorPos(window, &x, &y);
	if (pm == false)
	{
		if (x > 50 && x < 100 && y<sch - 50 && y>sch - 100)
		{
			sel = 0;
		}
		else if (x > 150 && x < 200 && y<sch - 50 && y>sch - 100)
		{
			sel = 1;
		}
		else if (x > 250 && x < 300 && y<sch - 50 && y>sch - 100)
		{
			sel = 2;
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
					if ((x > maz[i][j].x&& x < maz[i][j].x + 50) && (y > maz[i][j].y&& y < maz[i][j].y + 50))
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

int main()
{
	test();
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(scw, sch, "LearnOpenGL", NULL, NULL);
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
	
	glUniform4i(glGetUniformLocation(sp, "col"), 255, 0, 0, 255);
	glUseProgram(sp);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	//glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);
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