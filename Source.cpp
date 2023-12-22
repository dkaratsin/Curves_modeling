#include <stdlib.h>
#include <glut.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

float inverse[4][4];
float abcdMatrix[4] = { 0.0, 0.0, 0.0, 0.0 };
bool readyToDraw = false;

//https://www.sanfoundry.com/c-program-find-inverse-matrix/
float determinant(float a[4][4], float k)
{
    float s = 1, det = 0, b[4][4];
    int i, j, m, n, c;
    if (k == 1)
    {
        return (a[0][0]);
    }
    else
    {
        det = 0;
        for (c = 0; c < k; c++)
        {
            m = 0;
            n = 0;
            for (i = 0; i < k; i++)
            {
                for (j = 0; j < k; j++)
                {
                    b[i][j] = 0;
                    if (i != 0 && j != c)
                    {
                        b[m][n] = a[i][j];
                        if (n < (k - 2))
                            n++;
                        else
                        {
                            n = 0;
                            m++;
                        }
                    }
                }
            }
            det = det + s * (a[0][c] * determinant(b, k - 1));
            s = -1 * s;
        }
    }

    return (det);
}
//https://www.sanfoundry.com/c-program-find-inverse-matrix/
void transpose(float num[4][4], float fac[4][4], float r)
{
    int i, j;
    float b[4][4], d;

    for (i = 0; i < r; i++)
    {
        for (j = 0; j < r; j++)
        {
            b[i][j] = fac[j][i];
        }
    }
    d = determinant(num, r);
    for (i = 0; i < r; i++)
    {
        for (j = 0; j < r; j++)
        {
            inverse[i][j] = b[i][j] / d;
        }
    }
    printf("\n\n\nThe inverse of matrix is : \n");

    for (i = 0; i < r; i++)
    {
        for (j = 0; j < r; j++)
        {
            printf("\t%f", inverse[i][j]);
        }
        printf("\n");
    }
}
//https://www.sanfoundry.com/c-program-find-inverse-matrix/
void cofactor(float num[4][4], float f)
{
    float b[4][4], fac[4][4];
    int p, q, m, n, i, j;
    for (q = 0; q < f; q++)
    {
        for (p = 0; p < f; p++)
        {
            m = 0;
            n = 0;
            for (i = 0; i < f; i++)
            {
                for (j = 0; j < f; j++)
                {
                    if (i != q && j != p)
                    {
                        b[m][n] = num[i][j];
                        if (n < (f - 2))
                            n++;
                        else
                        {
                            n = 0;
                            m++;
                        }
                    }
                }
            }
            fac[q][p] = pow(-1, q + p) * determinant(b, f - 1);
        }
    }
    transpose(num, fac, f);
}

// Define a simple structure for 3D points
struct Point {
	GLfloat x, y, z;
	Point() : x(0), y(0), z(0) {};
	Point(GLfloat x, GLfloat y, GLfloat z) : x(x), y(y), z(z) {}
};

float a[4][4], k = 4;
int clicksLeft = 7;
int nodeNum = 0;
int option = 1;
GLsizei windowWidth = 1000;
GLsizei windowHeight = 1000;
Point points[10];

#define M_PI 3.14159265358979323846

void menu(int menuOption) {
	if (menuOption == 1) option = 1;
	if (menuOption == 2) option = 2;
	if (menuOption == 3) option = 3;
	if (menuOption == 4) option = 4;

	glutPostRedisplay();
}

void myinit(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glViewport(0, 0, 1000, 1000);
	glMatrixMode(GL_PROJECTION);
	glOrtho(0.0, 1000.0, 0.0, 1000.0, 1.0, -1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glutCreateMenu(menu);

	// Add menu items
	glutAddMenuEntry("2 part cubic curve with C0 continuity (7 points)", 1);
	glutAddMenuEntry("6th degree closed bezier curve (6+1 points) ", 2);
	glutAddMenuEntry("2 part cubic bezier splines with C1 continuity", 3);
	glutAddMenuEntry("bicubic surface (16 points)", 4);

	// Associate a mouse button with menu
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void reshape(int newWidth, int newHeight) {
	glutReshapeWindow(1000, 1000);
}

void idle(void) {
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		y = 1000 - y;
		points[nodeNum] = Point(x, y, 0);
        a[nodeNum][0] = pow(points[nodeNum].x, 3);
        a[nodeNum][1] = pow(points[nodeNum].x, 2);
        a[nodeNum][2] = points[nodeNum].x;
        a[nodeNum][3] = 1;
        nodeNum++;
        if(nodeNum==4){
            float d = determinant(a, k);
            if (d == 0)
                printf("\nInverse of Entered Matrix is not possible\n");
            else {
                cofactor(a, k);
                for (int i = 0; i < 4; i++) {
                    abcdMatrix[i] = 0.0;
                    for (int j = 0; j < 4; j++) {
                        abcdMatrix[i] += inverse[i][j] * points[j].y;
                    }
                }
                printf("a=%f, b=%f, c=%f, d=%f", abcdMatrix[0], abcdMatrix[1], abcdMatrix[2], abcdMatrix[3]);
                readyToDraw = true;
            }
                

        }
	}
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glPointSize(10);
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	for (int i = 0; i < 4; i++) {
		glVertex3f(points[i].x, points[i].y, points[i].z);
	}
	glEnd();

    if (readyToDraw) {
        float maxX = points[0].x;
        for (int i = 0; i < 4; i++) {
            if (points[i].x > maxX) {
                maxX = points[i].x;
            }
        }


        glBegin(GL_LINE_STRIP);
        for (double x = points[0].x; x <= maxX; x++) {
            glVertex3f(x, abcdMatrix[0] * pow(x, 3) + abcdMatrix[1] * pow(x, 2) + abcdMatrix[2] * x + abcdMatrix[3], 0);
        }
        glEnd();
    }
    

	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); /* default, not needed */
	glutInitWindowSize(windowWidth, windowHeight); /* 1000 x 1000 pixel window */
	glutInitWindowPosition(0, 0); /* place window top left on display */
	glutCreateWindow("Model_Project"); /* window title */

	glDrawBuffer(GL_BACK);
	myinit(); /* set attributes */
	glutDisplayFunc(display); /* display callback invoked when window opened */
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutMouseFunc(mouse);
	glutMainLoop(); /* enter event loop */
	
	return 1;
}