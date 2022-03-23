/*********
 CTIS164 - Project
----------
STUDENT : Ahmet Burak Yuksel
SECTION : 3
----------
PROBLEMS:
----------
ADDITIONAL FEATURES:
1.Two players can play game.(Mouse clicks fire missile, Arrow keys controls UFO's vision)
2.If UFO sees the missile, missile flee from ufo.
3.Pause-Restart Menu
4.Score is changing with every successfull shot
5.Timer - Players has 20 seconds
6.Players can restart game after time finishes
*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "vec.h"

#define WINDOW_WIDTH  1000
#define WINDOW_HEIGHT 600

#define TIMER_PERIOD  20 // Period for the timer.
#define TIMER_PERIOD2  1000 // Period for the timers.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer
#define TIMESEC 20  

#define D2R 0.0174532

#define PLAY 0
#define OVER 1
#define STOPPED 2
#define STARTAGAIN 3

#define FOV  15
#define ENEMYSPEED 3
#define PLAYERSPEED 1

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

int mode = PLAY;
int duration = TIMESEC;
int timer = 1;
int score;
int rDuration = 3;
int shoot = 1;

typedef struct {
	vec_t pos;
	float angle;
} ufo_t;

typedef struct {
	vec_t pos;
	vec_t vel;  //velocity
	int angle;
	bool  visibility; // visibility: true if the player sees the enemy
} missile_t;

float clickX, clickY;
int mx;
int my;

ufo_t U = { { 0, 0 }, 0 };
missile_t  E = { { clickX, clickY },0,0,0 };

float A = 100;
float fq = 1;
float C = 0;
float B = 0;
float angle = 0;

//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void print(int x, int y, const char *string, void *font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void *font, const char *string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char *string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

int starX[11] = { -400 ,-330,-260 ,-130,-50,150,240, 300,370, 420, 490, };
int starY[11] = { 230, 220, 215, 240, 200 , 245, 245, 260 , 220, 230, 200 };

void background() {
	//stars
	glColor3f(1, 1, 1);
	glPointSize(3);
	glBegin(GL_POINTS);
	for (int i = 0; i < 10; i++) {
		glVertex2f(starX[i], starY[i]);
		glVertex2f(starX[i] - 200, starY[i] - 100);
		glVertex2f(starX[i] - 50, starY[i] - 220);
		glVertex2f(starX[i] - 150, starY[i] - 330);
		glVertex2f(starX[i] - 300, starY[i] - 450);
	}
	glEnd();

	//earth
	glColor3f(0.0f, 0.1f, 0.7f);
	circle(500, -800, 900);
	glEnd();
	glColor3f(0.1f, 0.5f, 0.1f);
	circle(900, -300, 500);
	glEnd();
	glColor3f(0.1f, 0.5f, 0.1f);
	circle(440, -300, 170);
	glEnd();
	glColor3f(0.1f, 0.5f, 0.1f);
	circle(500, -170, 100);
	glEnd();
	glColor3f(0.1f, 0.5f, 0.1f);
	circle(500, -170, 100);
	glEnd();
	glColor3f(0.1f, 0.5f, 0.1f);
	circle(100, -240, 100);
	glEnd();
	glColor3f(0.1f, 0.5f, 0.1f);
	circle(0, -300, 100);
	glEnd();

	//moon
	glColor3f(0.3, 0.3, 0.3);
	circle(-500, 350, 200);
	glEnd();
	glColor3f(0.1f, 0.1f, 0.1f);
	circle(-480, 280, 20);
	glEnd();
	glColor3f(0.1f, 0.1f, 0.1f);
	circle(-380, 260, 30);
	glEnd();
	glColor3f(0.1f, 0.1f, 0.1f);
	circle(-460, 210, 40);
	glEnd();

}

void ufo(float x, float y) {
	glColor3f(1, 1, 1);
	glBegin(GL_TRIANGLES);
	glVertex2f(x, y + 10);
	glVertex2f(x - 42, y - 27);
	glVertex2f(x + 42, y - 27);
	glEnd();
	glColor3f(0.1f, 0.1f, 0.1f);
	glBegin(GL_TRIANGLES);
	glVertex2f(x, y);
	glVertex2f(x - 39, y - 25);
	glVertex2f(x + 39, y - 25);
	glEnd();
	//inside
	glColor3f(1, 1, 1);
	circle(x, y, 15);
	glEnd();
	glColor3f(0.1f, 0.1f, 0.1f);
	circle(x, y, 13);
	glEnd();

	glColor3f(1, 1, 1);
	glRectf(x - 30, y - 35, x + 30, y - 25);
	glEnd();
	glColor3f(0.1f, 0.1f, 0.1f);
	glRectf(x - 28, y - 33, x + 28, y - 25);
	glEnd();

	glColor3f(1, 1, 1);
	circle(x, y - 24, 3);
	glEnd();
	glColor3f(1, 1, 1);
	circle(x - 10, y - 24, 3);
	glEnd();
	glColor3f(1, 1, 1);
	circle(x + 10, y - 24, 3);
	glEnd();
	glColor3f(1, 1, 1);
	circle(x - 20, y - 24, 3);
	glEnd();
	glColor3f(1, 1, 1);
	circle(x + 20, y - 24, 3);
	glEnd();
}

/*void missile(missile_t e) {
	glColor3f(0.2f, 0.2f, 0.2f);
	glBegin(GL_TRIANGLES);
	glVertex2f(e.pos.x, e.pos.y+5 );
	glVertex2f(e.pos.x - 15, e.pos.y - 10);
	glVertex2f(e.pos.x + 15, e.pos.y - 10);
	glEnd();

	glColor3f(0.2f, 0.2f, 0.2f);
	circle(e.pos.x, e.pos.y +15,8);
	glEnd();
	glColor3f(0.1f, 0.5f, 0.1f);
	circle(e.pos.x, e.pos.y + 15,6);
	glEnd();

	glColor3f(0.2f, 0.2f, 0.2f);
	glRectf(e.pos.x -7, e.pos.y + 15, e.pos.x + 7, e.pos.y - 40);
	glEnd();

	glColor3f(0.1f, 0.5f, 0.1f);
	glBegin(GL_TRIANGLES);
	glVertex2f(e.pos.x, e.pos.y);
	glVertex2f(e.pos.x - 12, e.pos.y - 8);
	glVertex2f(e.pos.x + 12, e.pos.y - 8);
	glEnd();

	glColor3f(0.2f, 0.2f, 0.2f);
	glBegin(GL_TRIANGLES);
	glVertex2f(e.pos.x, e.pos.y -20);
	glVertex2f(e.pos.x - 13, e.pos.y - 45);
	glVertex2f(e.pos.x + 13, e.pos.y - 45);
	glEnd();
	glColor3f(0.1f, 0.5f, 0.1f);
	glBegin(GL_TRIANGLES);
	glVertex2f(e.pos.x, e.pos.y - 23);
	glVertex2f(e.pos.x - 8, e.pos.y - 43);
	glVertex2f(e.pos.x + 8, e.pos.y - 43);
	glEnd();

	glColor3f(0.1f, 0.5f, 0.1f);
	glRectf(e.pos.x - 4, e.pos.y + 15, e.pos.x + 4, e.pos.y - 38);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 0.5f, 1.0f);
	glVertex2f(e.pos.x -5 , e.pos.y -46);
	glColor3f(0.0f, 0.5f, 1.0f);
	glVertex2f(e.pos.x +5, e.pos.y - 46);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(e.pos.x, e.pos.y -75);
	glEnd();
	glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 0.5f, 1.0f);
	glVertex2f(e.pos.x - 13, e.pos.y - 46);
	glColor3f(0.0f, 0.5f, 1.0f);
	glVertex2f(e.pos.x - 7, e.pos.y - 46);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(e.pos.x, e.pos.y - 65);
	glEnd();
	glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 0.5f, 1.0f);
	glVertex2f(e.pos.x + 7, e.pos.y - 46);
	glColor3f(0.0f, 0.5f, 1.0f);
	glVertex2f(e.pos.x + 13, e.pos.y - 46);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(e.pos.x, e.pos.y - 65);
	glEnd();
}*/

double RotateX(int ang, int x, int y) {
	return (x * cos(ang * D2R)) - (y * sin(ang * D2R));
}

double RotateY(int ang, int x, int y) {
	return (x * sin(ang * D2R)) + (y * cos(ang * D2R));
}

void missile(missile_t e) {
	//head ofmissile
	glBegin(GL_TRIANGLES);
	glColor3f(0.2f, 0.2f, 0.2f);
	glVertex2f(RotateX(e.angle, 10, 10) + e.pos.x, RotateY(e.angle, 10, 10) + e.pos.y);
	glVertex2f(RotateX(e.angle, 30, 0) + e.pos.x, RotateY(e.angle, 30, 0) + e.pos.y);
	glVertex2f(RotateX(e.angle, 10, -10) + e.pos.x, RotateY(e.angle, 10, -10) + e.pos.y);
	glEnd();
	glBegin(GL_TRIANGLES);
	glColor3f(0.1f, 0.5f, 0.1f);
	glVertex2f(RotateX(e.angle, 12, 7) + e.pos.x, RotateY(e.angle, 12, 7) + e.pos.y);
	glVertex2f(RotateX(e.angle, 25, 0) + e.pos.x, RotateY(e.angle, 25, 0) + e.pos.y);
	glVertex2f(RotateX(e.angle, 12, -7) + e.pos.x, RotateY(e.angle, 12, -7) + e.pos.y);
	glEnd();
	//body
	glBegin(GL_TRIANGLES);
	glColor3f(0.2f, 0.2f, 0.2f);
	glVertex2f(RotateX(e.angle, -13, 16) + e.pos.x, RotateY(e.angle, -13, 16) + e.pos.y);
	glVertex2f(RotateX(e.angle, 0, 0) + e.pos.x, RotateY(e.angle, 0, 0) + e.pos.y);
	glVertex2f(RotateX(e.angle, -13, -16) + e.pos.x, RotateY(e.angle, -13, -16) + e.pos.y);
	glEnd();
	glBegin(GL_TRIANGLES);
	glColor3f(0.2f, 0.2f, 0.2f);
	glVertex2f(RotateX(e.angle, 10, 6) + e.pos.x, RotateY(e.angle, 10, 6) + e.pos.y);
	glVertex2f(RotateX(e.angle, -40, 6) + e.pos.x, RotateY(e.angle, -40, 6) + e.pos.y);
	glVertex2f(RotateX(e.angle, 10, -6) + e.pos.x, RotateY(e.angle, 10, -6) + e.pos.y);
	glEnd();
	glBegin(GL_TRIANGLES);
	glColor3f(0.2f, 0.2f, 0.2f);
	glVertex2f(RotateX(e.angle, -40, 6) + e.pos.x, RotateY(e.angle, -40, 6) + e.pos.y);
	glVertex2f(RotateX(e.angle, 10, -6) + e.pos.x, RotateY(e.angle, 10, -6) + e.pos.y);
	glVertex2f(RotateX(e.angle, -40, -6) + e.pos.x, RotateY(e.angle, -40, -6) + e.pos.y);
	glEnd();
	glBegin(GL_TRIANGLES);
	glColor3f(0.1f, 0.5f, 0.1f);
	glVertex2f(RotateX(e.angle, 15, 3) + e.pos.x, RotateY(e.angle, 15, 3) + e.pos.y);
	glVertex2f(RotateX(e.angle, -37, 3) + e.pos.x, RotateY(e.angle, -37, 3) + e.pos.y);
	glVertex2f(RotateX(e.angle, 15, -3) + e.pos.x, RotateY(e.angle, 15, -3) + e.pos.y);
	glEnd();
	glBegin(GL_TRIANGLES);
	glColor3f(0.1f, 0.5f, 0.1f);
	glVertex2f(RotateX(e.angle, -37, 3) + e.pos.x, RotateY(e.angle, -37, 3) + e.pos.y);
	glVertex2f(RotateX(e.angle, 15, -3) + e.pos.x, RotateY(e.angle, 15, -3) + e.pos.y);
	glVertex2f(RotateX(e.angle, -37, -3) + e.pos.x, RotateY(e.angle, -37, -3) + e.pos.y);
	glEnd();
	//tail
	glBegin(GL_TRIANGLES);
	glColor3f(0.2f, 0.2f, 0.2f);
	glVertex2f(RotateX(e.angle, -40, 15) + e.pos.x, RotateY(e.angle, -40, 15) + e.pos.y);
	glVertex2f(RotateX(e.angle, -25, 0) + e.pos.x, RotateY(e.angle, -25, 0) + e.pos.y);
	glVertex2f(RotateX(e.angle, -40, -15) + e.pos.x, RotateY(e.angle, -40, -15) + e.pos.y);
	glEnd();
	glBegin(GL_TRIANGLES);
	glColor3f(0.1f, 0.5f, 0.1f);
	glVertex2f(RotateX(e.angle, -37, 10) + e.pos.x, RotateY(e.angle, -37, 10) + e.pos.y);
	glVertex2f(RotateX(e.angle, -30, 0) + e.pos.x, RotateY(e.angle, -30, 0) + e.pos.y);
	glVertex2f(RotateX(e.angle, -37, -10) + e.pos.x, RotateY(e.angle, -37, -10) + e.pos.y);
	glEnd();
	glBegin(GL_TRIANGLES);
	glColor3f(0.1f, 0.5f, 0.1f);
	glVertex2f(RotateX(e.angle, -10, 13) + e.pos.x, RotateY(e.angle, -10, 13) + e.pos.y);
	glVertex2f(RotateX(e.angle, -6, 0) + e.pos.x, RotateY(e.angle, -6, 0) + e.pos.y);
	glVertex2f(RotateX(e.angle, -10, -13) + e.pos.x, RotateY(e.angle, -10, -13) + e.pos.y);
	glEnd();

	//engines
	glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 0.5f, 1.0f);
	glVertex2f(RotateX(e.angle, -40, 3) + e.pos.x, RotateY(e.angle, -40, 3) + e.pos.y);
	glColor3f(0.0f, 0.5f, 1.0f);
	glVertex2f(RotateX(e.angle, -40, -3) + e.pos.x, RotateY(e.angle, -40, -3) + e.pos.y);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(RotateX(e.angle, -80, 0) + e.pos.x, RotateY(e.angle, -80, 0) + e.pos.y);
	glEnd();
	glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 0.5f, 1.0f);
	glVertex2f(RotateX(e.angle, -40, 7) + e.pos.x, RotateY(e.angle, -40, 7) + e.pos.y);
	glColor3f(0.0f, 0.5f, 1.0f);
	glVertex2f(RotateX(e.angle, -40, 10) + e.pos.x, RotateY(e.angle, -40, 10) + e.pos.y);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(RotateX(e.angle, -65, 0) + e.pos.x, RotateY(e.angle, -65, 0) + e.pos.y);
	glEnd();
	glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 0.5f, 1.0f);
	glVertex2f(RotateX(e.angle, -40, -7) + e.pos.x, RotateY(e.angle, -40, -7) + e.pos.y);
	glColor3f(0.0f, 0.5f, 1.0f);
	glVertex2f(RotateX(e.angle, -40, -10) + e.pos.x, RotateY(e.angle, -40, -10) + e.pos.y);
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(RotateX(e.angle, -65, 0) + e.pos.x, RotateY(e.angle, -65, 0) + e.pos.y);
	glEnd();
}

void drawUFO(ufo_t p) {
	vec_t vEnd = addV(p.pos, pol2rec({ 30, p.angle }));
	vec_t E1 = addV(p.pos, pol2rec({ 1000, p.angle - FOV }));
	vec_t E2 = addV(p.pos, pol2rec({ 1000, p.angle + FOV }));

	// Field of View Area
	glColor4f(0, 1, 0, 0.2);
	glBegin(GL_TRIANGLES);
	glVertex2f(p.pos.x, p.pos.y);
	glVertex2f(E1.x, E1.y);
	glVertex2f(E2.x, E2.y);
	glEnd();

	ufo(p.pos.x, p.pos.y);
}

/*void drawMissile(missile_t e) {
	// if player sees the enemy, its color is green.
	if (E.visibility)
		glColor3f(0, 1, 0);
	else
		glColor3f(1, 0, 0);

	missile(e.pos.x, e.pos.y);
}*/

void MovingUFO(float x, float y) {
	float r = fabs(A)*1.5;
	glColor3f(1, 0, 0);
	U.pos.x = r * cos(angle * D2R) + 0;
	U.pos.y = r * sin(angle*D2R) + 0;
	drawUFO(U);
}

float f(float x) {
	return A * sin((fq * x + C) * D2R) + B;
}

void drun() {
	background();
	MovingUFO(U.pos.x, U.pos.y);
	missile(E);

	//Background for informations
	glColor3f(1, 1, 1);
	glRectf(-500, -300, 500, -270);
	glEnd();
	glColor3f(0.1f, 0.1f, 0.1f);
	glRectf(-500, -300, 500, -272);
	glEnd();

	//Mode Status
	glColor3f(1, 1, 1);
	vprint(-490, -290, GLUT_BITMAP_8_BY_13, "   P1: Click to Fire Missile          P2: Use <- -> Keys to Defence         F1 : Pause                        Score:  ^%d^", score);

	//Background for informations
	glColor3f(1, 1, 1);
	glRectf(-500, 270, 500, 300);
	glEnd();
	glColor3f(0.1f, 0.1f, 0.1f);
	glRectf(-500, 272, 500, 300);
	glEnd();

	//Information
	glColor3f(1, 1, 1);
	vprint(-490, 280, GLUT_BITMAP_8_BY_13, "       Ahmet Burak Yuksel   Homework #3                                                               Time:  ^0:%d^", duration);
}

void dpause() {
	background();
	MovingUFO(U.pos.x, U.pos.y);
	missile(E);

	//Background for informations
	glColor3f(1, 1, 1);
	glRectf(-500, -300, 500, -270);
	glEnd();
	glColor3f(0.1f, 0.1f, 0.1f);
	glRectf(-500, -300, 500, -272);
	glEnd();

	//Mode Status
	glColor3f(1, 1, 1);
	vprint(-490, -290, GLUT_BITMAP_8_BY_13, "                PRESS F1 TO CONTINUE       PRESS F2 TO RESTART                                           Score:  ^%d^", score);

	//Background for informations
	glColor3f(1, 1, 1);
	glRectf(-500, 270, 500, 300);
	glEnd();
	glColor3f(0.1f, 0.1f, 0.1f);
	glRectf(-500, 272, 500, 300);
	glEnd();

	//Information
	glColor3f(1, 1, 1);
	vprint(-490, 280, GLUT_BITMAP_8_BY_13, "       Ahmet Burak Yuksel   Homework #3                                                               Time:  ^0:%d^", duration);
	glColor3f(1, 1, 1);

	glBegin(GL_POLYGON);
	glVertex2f(-100, 20);
	glVertex2f(-100, -30);
	glVertex2f(120, -30);
	glVertex2f(120, 20);
	glEnd();

	glColor3f(0.3, 0.3, 0.3);

	glBegin(GL_POLYGON);
	glVertex2f(-98, 18);
	glVertex2f(-98, -28);
	glVertex2f(118, -28);
	glVertex2f(118, 18);
	glEnd();

	glColor3f(0, 1, 0);
	vprint(-20, 5, GLUT_BITMAP_8_BY_13, " PAUSED");
	vprint(-75, -10, GLUT_BITMAP_8_BY_13, " PRESS F1 TO CONTINUE");
	vprint(-70, -25, GLUT_BITMAP_8_BY_13, " PRESS F2 TO RESTART ");
}

void dreset() {
	background();
	MovingUFO(U.pos.x, U.pos.y);
	missile(E);

	//Background for informations
	glColor3f(1, 1, 1);
	glRectf(-500, -300, 500, -270);
	glEnd();
	glColor3f(0.1f, 0.1f, 0.1f);
	glRectf(-500, -300, 500, -272);
	glEnd();

	//Mode Status
	glColor3f(1, 1, 1);

	vprint(-490, -290, GLUT_BITMAP_8_BY_13, "   P1: Click to Fire Missile          P2: Use <- -> Keys to Defence         F1 : Pause                        Score:  ^%d^", score);
	//Background for informations
	glColor3f(1, 1, 1);
	glRectf(-500, 270, 500, 300);
	glEnd();
	glColor3f(0.1f, 0.1f, 0.1f);
	glRectf(-500, 272, 500, 300);
	glEnd();

	//Information
	glColor3f(1, 1, 1);
	vprint(-490, 280, GLUT_BITMAP_8_BY_13, "       Ahmet Burak Yuksel   Homework #3                                                               Time:  ^0:%d^", duration);
	glColor3f(1, 1, 1);

	glBegin(GL_POLYGON);
	glVertex2f(-100, 20);
	glVertex2f(-100, -30);
	glVertex2f(120, -30);
	glVertex2f(120, 20);
	glEnd();

	glColor3f(0.3, 0.3, 0.3);

	glBegin(GL_POLYGON);
	glVertex2f(-98, 18);
	glVertex2f(-98, -28);
	glVertex2f(118, -28);
	glVertex2f(118, 18);
	glEnd();

	glColor3f(0, 1, 0);
	vprint(-40, 0, GLUT_BITMAP_8_BY_13, " RESTARTING");
	vprint(-20, -20, GLUT_BITMAP_8_BY_13, "00 : 0%d", rDuration);
}

void dover() {
	background();
	MovingUFO(U.pos.x, U.pos.y);
	missile(E);

	//Background for informations
	glColor3f(1, 1, 1);
	glRectf(-500, -300, 500, -270);
	glEnd();
	glColor3f(0.1f, 0.1f, 0.1f);
	glRectf(-500, -300, 500, -272);
	glEnd();

	//Mode Status
	glColor3f(1, 1, 1);

	vprint(-490, -290, GLUT_BITMAP_8_BY_13, "   P1: Click to Fire Missile          P2: Use <- -> Keys to Defence         F1 : Pause                        Score:  ^%d^", score);
	//Background for informations
	glColor3f(1, 1, 1);
	glRectf(-500, 270, 500, 300);
	glEnd();
	glColor3f(0.1f, 0.1f, 0.1f);
	glRectf(-500, 272, 500, 300);
	glEnd();

	//Information
	glColor3f(1, 1, 1);
	vprint(-490, 280, GLUT_BITMAP_8_BY_13, "       Ahmet Burak Yuksel   Homework #3                                                               Time:  ^0:%d^", duration);

	glColor3f(1, 1, 1);

	glBegin(GL_POLYGON);
	glVertex2f(-100, 20);
	glVertex2f(-100, -30);
	glVertex2f(120, -30);
	glVertex2f(120, 20);
	glEnd();

	glColor3f(0.3, 0.3, 0.3);

	glBegin(GL_POLYGON);
	glVertex2f(-98, 18);
	glVertex2f(-98, -28);
	glVertex2f(118, -28);
	glVertex2f(118, 18);
	glEnd();

	glColor3f(0, 1, 0);
	vprint(-30, 5, GLUT_BITMAP_8_BY_13, " GAME OVER");
	vprint(-45, -10, GLUT_BITMAP_8_BY_13, " YOUR SCORE : %d ", score);
	vprint(-70, -23, GLUT_BITMAP_8_BY_13, " PRESS F1 TO RESTART ");
}

int ifHit(double *x, double *y) {
	if (E.pos.y< *y + 5 && E.pos.y>*y - 50) {
		if (E.pos.x < *x + 20 && E.pos.x > *x - 10) {
			score++;
			*x = (485 + 1 - 250) + 250;
			*y = -400;
			E.pos.x = 900;

			return 1;
		}
	}
	return 0;
}

void reset() {
	mode = PLAY;
	duration = TIMESEC;
	rDuration = 3;
	timer = 1;
	shoot = 1;
	score = 0;
}

//
// To display onto window using OpenGL commands
//
void display() {
	//
	// clear window to black
	//
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	switch (mode) {
	case PLAY: drun();
		break;
	case STOPPED: dpause();
		break;
	case STARTAGAIN: dreset();
		break;
	case OVER: dover();
		break;
	}


	glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = true; break;
	case GLUT_KEY_DOWN: down = true; break;
	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;

	case GLUT_KEY_F1: {
		if (mode == PLAY) {
			timer = 0;
			mode = STOPPED;
		}
		else if (mode == STOPPED) {
			timer = 1;
			mode = PLAY;
		}
		if (mode == OVER) {
			timer = 1;
			mode = STARTAGAIN;
		}
		break;
	}
	case GLUT_KEY_F2: {
		if (mode == STOPPED)
			mode = STARTAGAIN;
		break;
	}
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
	// Write your codes here.
	if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN) {
		E.pos.x = mx;
		E.pos.y = my;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function   
	glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
	// Write your codes here.
	mx = x - winWidth / 2;
	my = winHeight / 2 - y;

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.

	if (timer) {
		// ufo's vision
		if (left) U.angle += 3;
		if (right) U.angle -= 3;

		//motion of ufo
		angle += 2;
		if (angle > 360)
			angle -= 360;
		U.angle += 1;

		// Checking ufo sees missile or not
		vec_t V = pol2rec({ 1, U.angle });
		vec_t W = unitV(subV(E.pos, U.pos));
		E.visibility = dotP(V, W) > cos(FOV * D2R);

		// Catch of flee		
		E.vel = mulV(ENEMYSPEED, unitV(subV(U.pos, E.pos)));

		// if ufo sees missile, missile flees		
		vec_t prevPos = E.pos;
		if (E.visibility == true) {
			// flee
			E.pos = subV(E.pos, E.vel);
			E.angle = angleV(unitV(subV(U.pos, E.pos)));
		}
		else {
			// catch
			E.pos = addV(E.pos, E.vel);
			E.angle = angleV(unitV(subV(U.pos, E.pos)));
		}
		// Missile can not leave screen
		if (E.pos.x > 500 || E.pos.x < -500 || E.pos.y > 300 || E.pos.y < -300) {
			E.pos = prevPos;
		}
		if (U.pos.x < 300) {
			ifHit(&U.pos.x, &U.pos.y);
		}
		else {
			U.pos.y = -390;
		}
	}
	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

#if TIMER_ON == 1
void onTimer2(int v) {
	glutTimerFunc(TIMER_PERIOD2, onTimer2, 0);
	//timer for remaining time
	if (mode == PLAY) {
		if (duration > 0)
			duration--;
		else {
			timer = 0;
			mode = OVER;
		}
	}
	if (mode == STARTAGAIN) {
		if (rDuration > 0)
			rDuration--;
		else
			reset();
	}
	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()
}
#endif

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	//glutInitWindowPosition(100, 100);
	glutCreateWindow("Protecting The Earth by Ahmet Burak Yuksel");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	glutTimerFunc(TIMER_PERIOD2, onTimer2, 0);
#endif

	Init();

	glutMainLoop();
}
