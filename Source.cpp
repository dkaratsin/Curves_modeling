#include <stdlib.h>
#include <glut.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <cmath>

float inverse[4][4];
float abcdMatrix[4][2] = { 
    0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0};
bool readyToDraw1 = false;
bool readyToDraw2 = false;
bool readyToDrawBez = false;
bool readyToDrawBez1 = false;
bool readyToDrawBez2 = false;
float a[4][4],b[4][4], k = 4;
int nodeNum = 0;
int option = 1;
GLsizei windowWidth = 1000;
GLsizei windowHeight = 1000;
float click_x = 0;
float click_y = 0;
float drag_x = 0;
float drag_y = 0;
int selectedNodeIndex = -1;
double startX = 0;  
double startY = 0;

struct Point {
    GLfloat x, y, z;
    Point() : x(0), y(0), z(0) {};
    Point(GLfloat x, GLfloat y, GLfloat z) : x(x), y(y), z(z) {}
};
Point points[8];
Point pointsBez[7];
Point pointsBezCubic[7];

#define M_PI 3.14159265358979323846


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

void menu(int menuOption) {
    if (menuOption == 1) { 
        option = 1; 
        nodeNum = 0;
    }
    if (menuOption == 2) {
        option = 2;
        nodeNum = 0;
    }
    if (menuOption == 3) {
        option = 3;
        nodeNum = 0;
    }
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

//u range 0 to 1
float lerp(float a, float b, float u) {
    return (a * (1.0 - u)) + (b * u);
}

void onDrag(int x, int y) {
    if (option == 1) {
        drag_x = (click_x - x);
        drag_y = (click_y - (1000 - y));
        if (selectedNodeIndex == 3 || selectedNodeIndex == 4) {
            points[3].x = startX - drag_x;
            points[3].y = startY - drag_y;
            points[4].x = startX - drag_x;
            points[4].y = startY - drag_y;
        }
        else {
            points[selectedNodeIndex].x = startX - drag_x;
            points[selectedNodeIndex].y = startY - drag_y;
        }
        if (selectedNodeIndex < 3) {
            a[selectedNodeIndex][0] = pow(points[selectedNodeIndex].x, 3);
            a[selectedNodeIndex][1] = pow(points[selectedNodeIndex].x, 2);
            a[selectedNodeIndex][2] = points[selectedNodeIndex].x;
            a[selectedNodeIndex][3] = 1;

            float d = determinant(a, k);
            if (d == 0)
                printf("\nInverse of Entered Matrix is not possible\n");
            else {
                cofactor(a, k);
                for (int i = 0; i < 4; i++) {
                    abcdMatrix[i][0] = 0.0;
                    for (int j = 0; j < 4; j++) {
                        abcdMatrix[i][0] += inverse[i][j] * points[j].y;
                    }
                }
                printf("a=%f, b=%f, c=%f, d=%f", abcdMatrix[0][0], abcdMatrix[1][0], abcdMatrix[2][0], abcdMatrix[3][0]);
            }
        }
        else if (selectedNodeIndex > 4) {
            b[selectedNodeIndex % 4][0] = pow(points[selectedNodeIndex].x, 3);
            b[selectedNodeIndex % 4][1] = pow(points[selectedNodeIndex].x, 2);
            b[selectedNodeIndex % 4][2] = points[selectedNodeIndex].x;
            b[selectedNodeIndex % 4][3] = 1;

            float d = determinant(b, k);
            if (d == 0)
                printf("\nInverse of Entered Matrix is not possible\n");
            else {
                cofactor(b, k);
                for (int i = 0; i < 4; i++) {
                    abcdMatrix[i][1] = 0.0;
                    for (int j = 0; j < 4; j++) {
                        abcdMatrix[i][1] += inverse[i][j] * points[j + 4].y;
                    }
                }
                printf("a=%f, b=%f, c=%f, d=%f", abcdMatrix[0][1], abcdMatrix[1][1], abcdMatrix[2][1], abcdMatrix[3][1]);
            }
        }
        else {
            a[3][0] = pow(points[selectedNodeIndex].x, 3);
            a[3][1] = pow(points[selectedNodeIndex].x, 2);
            a[3][2] = points[selectedNodeIndex].x;
            a[3][3] = 1;
            b[0][0] = a[3][0];
            b[0][1] = a[3][1];
            b[0][2] = a[3][2];
            b[0][3] = a[3][3];

            float d = determinant(a, k);
            if (d == 0)
                printf("\nInverse of Entered Matrix is not possible\n");
            else {
                cofactor(a, k);
                for (int i = 0; i < 4; i++) {
                    abcdMatrix[i][0] = 0.0;
                    for (int j = 0; j < 4; j++) {
                        abcdMatrix[i][0] += inverse[i][j] * points[j].y;
                    }
                }
                printf("a=%f, b=%f, c=%f, d=%f", abcdMatrix[0][0], abcdMatrix[1][0], abcdMatrix[2][0], abcdMatrix[3][0]);
            }

            d = determinant(b, k);
            if (d == 0)
                printf("\nInverse of Entered Matrix is not possible\n");
            else {
                cofactor(b, k);
                for (int i = 0; i < 4; i++) {
                    abcdMatrix[i][1] = 0.0;
                    for (int j = 0; j < 4; j++) {
                        abcdMatrix[i][1] += inverse[i][j] * points[j + 4].y;
                    }
                }
                printf("a=%f, b=%f, c=%f, d=%f", abcdMatrix[0][1], abcdMatrix[1][1], abcdMatrix[2][1], abcdMatrix[3][1]);
            }
        }
        glutPostRedisplay();
    }
    else if (option == 2) {
        drag_x = (click_x - x);
        drag_y = (click_y - 1000 + y);
        pointsBez[selectedNodeIndex].x = startX - drag_x;
        pointsBez[selectedNodeIndex].y = startY - drag_y;        
        glutPostRedisplay();
    }
    else if (option == 3) {
        drag_x = (click_x - x);
        drag_y = (click_y - 1000 + y);
        pointsBezCubic[selectedNodeIndex].x = startX - drag_x;
        pointsBezCubic[selectedNodeIndex].y = startY - drag_y;
        if (selectedNodeIndex == 2) {
            pointsBezCubic[4] = Point(2 * pointsBezCubic[3].x - pointsBezCubic[2].x, 2 * pointsBezCubic[3].y - pointsBezCubic[2].y, 0);
        }
        else if (selectedNodeIndex == 4) {
            pointsBezCubic[2] = Point(2 * pointsBezCubic[3].x - pointsBezCubic[4].x, 2 * pointsBezCubic[3].y - pointsBezCubic[4].y, 0);
        }
        else if (selectedNodeIndex == 3) {
            pointsBezCubic[4] = Point(2 * pointsBezCubic[3].x - pointsBezCubic[2].x, 2 * pointsBezCubic[3].y - pointsBezCubic[2].y, 0);
        }
        glutPostRedisplay();
    }
}

void mouse(int button, int state, int x, int y) {
    if (option == 1) {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            if (readyToDraw1 && readyToDraw2) {
                glutMotionFunc(onDrag);
                drag_x = 0;
                drag_y = 0;
                click_x = x;
                click_y = 1000 - y;
                int minDistancePointIndex = 0;
                for (int i = 0; i < 8; i++) {
                    if (sqrt(pow(points[i].x - click_x, 2) + pow(points[i].y - click_y, 2)) < sqrt(pow(points[minDistancePointIndex].x - click_x, 2) + pow(points[minDistancePointIndex].y - click_y, 2))) {
                        minDistancePointIndex = i;
                    }
                }
                if (sqrt(pow(points[minDistancePointIndex].x - click_x, 2) + pow(points[minDistancePointIndex].y - click_y, 2)) < 7) {
                    selectedNodeIndex = minDistancePointIndex;
                    startX = points[selectedNodeIndex].x;
                    startY = points[selectedNodeIndex].y;
                }
                else {
                    glutMotionFunc(NULL);
                }
            }
            else {
                y = 1000 - y;
                points[nodeNum] = Point(x, y, 0);
                if (nodeNum < 4) {
                    a[nodeNum][0] = pow(points[nodeNum].x, 3);
                    a[nodeNum][1] = pow(points[nodeNum].x, 2);
                    a[nodeNum][2] = points[nodeNum].x;
                    a[nodeNum][3] = 1;
                }
                else {
                    b[nodeNum % 4][0] = pow(points[nodeNum].x, 3);
                    b[nodeNum % 4][1] = pow(points[nodeNum].x, 2);
                    b[nodeNum % 4][2] = points[nodeNum].x;
                    b[nodeNum % 4][3] = 1;
                }

                nodeNum++;
                if (nodeNum % 4 == 0) {
                    float d;
                    if (readyToDraw1) {
                        d = determinant(b, k);
                        if (d == 0)
                            printf("\nInverse of Entered Matrix is not possible\n");
                        else {
                            cofactor(b, k);
                            readyToDraw2 = true;
                            for (int i = 0; i < 4; i++) {
                                abcdMatrix[i][1] = 0.0;
                                for (int j = 0; j < 4; j++) {
                                    abcdMatrix[i][1] += inverse[i][j] * points[j + 4].y;
                                }
                            }
                            printf("a=%f, b=%f, c=%f, d=%f", abcdMatrix[0][1], abcdMatrix[1][1], abcdMatrix[2][1], abcdMatrix[3][1]);
                        }
                    }
                    else {
                        d = determinant(a, k);
                        if (d == 0)
                            printf("\nInverse of Entered Matrix is not possible\n");
                        else {
                            cofactor(a, k);
                            readyToDraw1 = true;
                            b[0][0] = a[3][0];
                            b[0][1] = a[3][1];
                            b[0][2] = a[3][2];
                            b[0][3] = a[3][3];
                            points[nodeNum] = Point(x, y, 0);
                            nodeNum++;
                            for (int i = 0; i < 4; i++) {
                                abcdMatrix[i][0] = 0.0;
                                for (int j = 0; j < 4; j++) {
                                    abcdMatrix[i][0] += inverse[i][j] * points[j].y;
                                }
                            }
                            printf("a=%f, b=%f, c=%f, d=%f", abcdMatrix[0][0], abcdMatrix[1][0], abcdMatrix[2][0], abcdMatrix[3][0]);
                        }
                    }
                }
            }
        }
    }
    else if (option == 2) {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            if (readyToDrawBez) {
                glutMotionFunc(onDrag);
                drag_x = 0;
                drag_y = 0;
                click_x = x;
                click_y = 1000 - y;
                int minDistancePointIndex = 0;
                for (int i = 0; i < 6; i++) {
                    if (sqrt(pow(pointsBez[i].x - click_x, 2) + pow(pointsBez[i].y - click_y, 2)) < sqrt(pow(pointsBez[minDistancePointIndex].x - click_x, 2) + pow(pointsBez[minDistancePointIndex].y - click_y, 2))) {
                        minDistancePointIndex = i;
                    }
                }
                if (sqrt(pow(pointsBez[minDistancePointIndex].x - click_x, 2) + pow(pointsBez[minDistancePointIndex].y - click_y, 2)) < 20) {
                    selectedNodeIndex = minDistancePointIndex;
                    startX = pointsBez[selectedNodeIndex].x;
                    startY = pointsBez[selectedNodeIndex].y;
                }
                else {
                    glutMotionFunc(NULL);
                }
            }
            else {
                y = 1000 - y;
                pointsBez[nodeNum] = Point(x, y, 0);
                nodeNum++;
                printf("%d", nodeNum);
                if (nodeNum == 6) {
                    readyToDrawBez = true;
                }
            }
        }
    }
    else if (option == 3) {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            if (readyToDrawBez1 && readyToDrawBez2) {
                glutMotionFunc(onDrag);
                drag_x = 0;
                drag_y = 0;
                click_x = x;
                click_y = 1000 - y;
                int minDistancePointIndex = 0;
                for (int i = 0; i < 6; i++) {
                    if (sqrt(pow(pointsBezCubic[i].x - click_x, 2) + pow(pointsBezCubic[i].y - click_y, 2)) < sqrt(pow(pointsBezCubic[minDistancePointIndex].x - click_x, 2) + pow(pointsBezCubic[minDistancePointIndex].y - click_y, 2))) {
                        minDistancePointIndex = i;
                    }
                }
                if (sqrt(pow(pointsBezCubic[minDistancePointIndex].x - click_x, 2) + pow(pointsBezCubic[minDistancePointIndex].y - click_y, 2)) < 20) {
                    selectedNodeIndex = minDistancePointIndex;
                    startX = pointsBezCubic[selectedNodeIndex].x;
                    startY = pointsBezCubic[selectedNodeIndex].y;
                }
                else {
                    glutMotionFunc(NULL);
                }
            }
            else {
                y = 1000 - y;
                pointsBezCubic[nodeNum] = Point(x, y, 0);
                nodeNum++;
                printf("%d", nodeNum);
                if (nodeNum == 4) {
                    readyToDrawBez1 = true;
                    pointsBezCubic[nodeNum] = Point(2 * pointsBezCubic[3].x - pointsBezCubic[2].x, 2 * pointsBezCubic[3].y - pointsBezCubic[2].y, 0);
                    nodeNum++;
                }
                else if (nodeNum == 7) {
                    readyToDrawBez2 = true;
                }
            }
        }
    }
}

void displayOption1(void) {
    glPointSize(10);
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 8; i++) {
        glVertex3f(points[i].x, points[i].y, points[i].z);
    }
    glEnd();

    if (readyToDraw1) {
        float maxX = points[0].x;
        float minX = points[0].x;
        for (int i = 0; i < 4; i++) {
            if (points[i].x > maxX) {
                maxX = points[i].x;
            }
            if (points[i].x < minX) {
                minX = points[i].x;
            }
        }
        glColor3f(0.7f, 0.0f, 1.0f);

        glBegin(GL_LINE_STRIP);
        for (double x = minX; x <= maxX; x++) {
            glVertex3f(x, abcdMatrix[0][0] * pow(x, 3) + abcdMatrix[1][0] * pow(x, 2) + abcdMatrix[2][0] * x + abcdMatrix[3][0], 0);
        }
        glEnd();
    }
    if (readyToDraw2) {
        float maxX = points[4].x;
        float minX = points[4].x;
        for (int i = 4; i < 8; i++) {
            if (points[i].x > maxX) {
                maxX = points[i].x;
            }
            if (points[i].x < minX) {
                minX = points[i].x;
            }
        }
        glColor3f(0.7f, 0.0f, 1.0f);

        glBegin(GL_LINE_STRIP);
        for (double x = minX; x <= maxX; x++) {
            glVertex3f(x, abcdMatrix[0][1] * pow(x, 3) + abcdMatrix[1][1] * pow(x, 2) + abcdMatrix[2][1] * x + abcdMatrix[3][1], 0);
        }
        glEnd();
    }
}

void displayOption2(void) {
    glPointSize(10);
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 6; i++) {
        glVertex3f(pointsBez[i].x, pointsBez[i].y, pointsBez[i].z);
    }
    glEnd();

    if (readyToDrawBez == true) {
        float maxX = pointsBez[0].x;
        float minX = pointsBez[0].x;
        for (int i = 0; i < 6; i++) {
            if (pointsBez[i].x > maxX) {
                maxX = pointsBez[i].x;
            }
            if (pointsBez[i].x < minX) {
                minX = pointsBez[i].x;
            }
        }
        glColor3f(0.7f, 0.0f, 1.0f);

        glBegin(GL_LINE_STRIP);
        for (double u=0; u <= 1.0; u+=1.0/(maxX-minX)) {
            glVertex3f(
                //x interpolation
                lerp(lerp(lerp(lerp(lerp(lerp(pointsBez[0].x, pointsBez[1].x, u), lerp(pointsBez[1].x, pointsBez[2].x, u), u),
                lerp(lerp(pointsBez[1].x, pointsBez[2].x, u), lerp(pointsBez[2].x, pointsBez[3].x, u), u), u),
                lerp(lerp(lerp(pointsBez[1].x, pointsBez[2].x, u), lerp(pointsBez[2].x, pointsBez[3].x, u), u),
                    lerp(lerp(pointsBez[2].x, pointsBez[3].x, u), lerp(pointsBez[3].x, pointsBez[4].x, u), u), u), u),
                lerp(lerp(lerp(lerp(pointsBez[1].x, pointsBez[2].x, u), lerp(pointsBez[2].x, pointsBez[3].x, u), u),
                    lerp(lerp(pointsBez[2].x, pointsBez[3].x, u), lerp(pointsBez[3].x, pointsBez[4].x, u), u), u),
                    lerp(lerp(lerp(pointsBez[2].x, pointsBez[3].x, u), lerp(pointsBez[3].x, pointsBez[4].x, u), u),
                        lerp(lerp(pointsBez[3].x, pointsBez[4].x, u), lerp(pointsBez[4].x, pointsBez[5].x, u), u), u), u), u),
                lerp(lerp(lerp(lerp(lerp(pointsBez[1].x, pointsBez[2].x, u), lerp(pointsBez[2].x, pointsBez[3].x, u), u),
                    lerp(lerp(pointsBez[2].x, pointsBez[3].x, u), lerp(pointsBez[3].x, pointsBez[4].x, u), u), u),
                    lerp(lerp(lerp(pointsBez[2].x, pointsBez[3].x, u), lerp(pointsBez[3].x, pointsBez[4].x, u), u),
                        lerp(lerp(pointsBez[3].x, pointsBez[4].x, u), lerp(pointsBez[4].x, pointsBez[5].x, u), u), u), u),
                    lerp(lerp(lerp(lerp(pointsBez[2].x, pointsBez[3].x, u), lerp(pointsBez[3].x, pointsBez[4].x, u), u),
                        lerp(lerp(pointsBez[3].x, pointsBez[4].x, u), lerp(pointsBez[4].x, pointsBez[5].x, u), u), u),
                        lerp(lerp(lerp(pointsBez[3].x, pointsBez[4].x, u), lerp(pointsBez[4].x, pointsBez[5].x, u), u),
                            lerp(lerp(pointsBez[4].x, pointsBez[5].x, u), lerp(pointsBez[5].x, pointsBez[0].x, u), u), u), u), u),u)
                ,
                //y interpolation
                lerp(lerp(lerp(lerp(lerp(lerp(pointsBez[0].y, pointsBez[1].y, u), lerp(pointsBez[1].y, pointsBez[2].y, u),u),
                            lerp(lerp(pointsBez[1].y, pointsBez[2].y, u), lerp(pointsBez[2].y, pointsBez[3].y, u),u),u),
                        lerp(lerp(lerp(pointsBez[1].y, pointsBez[2].y, u), lerp(pointsBez[2].y, pointsBez[3].y, u), u),
                            lerp(lerp(pointsBez[2].y, pointsBez[3].y, u), lerp(pointsBez[3].y, pointsBez[4].y, u), u), u),u),
                    lerp(lerp(lerp(lerp(pointsBez[1].y, pointsBez[2].y, u), lerp(pointsBez[2].y, pointsBez[3].y, u), u),
                        lerp(lerp(pointsBez[2].y, pointsBez[3].y, u), lerp(pointsBez[3].y, pointsBez[4].y, u), u), u),
                        lerp(lerp(lerp(pointsBez[2].y, pointsBez[3].y, u), lerp(pointsBez[3].y, pointsBez[4].y, u), u),
                            lerp(lerp(pointsBez[3].y, pointsBez[4].y, u), lerp(pointsBez[4].y, pointsBez[5].y, u), u), u), u),u),
                lerp(lerp(lerp(lerp(lerp(pointsBez[1].y, pointsBez[2].y, u), lerp(pointsBez[2].y, pointsBez[3].y, u), u),
                    lerp(lerp(pointsBez[2].y, pointsBez[3].y, u), lerp(pointsBez[3].y, pointsBez[4].y, u), u), u),
                    lerp(lerp(lerp(pointsBez[2].y, pointsBez[3].y, u), lerp(pointsBez[3].y, pointsBez[4].y, u), u),
                        lerp(lerp(pointsBez[3].y, pointsBez[4].y, u), lerp(pointsBez[4].y, pointsBez[5].y, u), u), u), u),
                    lerp(lerp(lerp(lerp(pointsBez[2].y, pointsBez[3].y, u), lerp(pointsBez[3].y, pointsBez[4].y, u), u),
                        lerp(lerp(pointsBez[3].y, pointsBez[4].y, u), lerp(pointsBez[4].y, pointsBez[5].y, u), u), u),
                        lerp(lerp(lerp(pointsBez[3].y, pointsBez[4].y, u), lerp(pointsBez[4].y, pointsBez[5].y, u), u),
                            lerp(lerp(pointsBez[4].y, pointsBez[5].y, u), lerp(pointsBez[5].y, pointsBez[0].y, u), u), u), u), u), u)
                //z
                , 0);
        }
        glEnd();
    }
}

void displayOption3(void) {
    glPointSize(10);
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 7; i++) {
        glVertex3f(pointsBezCubic[i].x, pointsBezCubic[i].y, pointsBezCubic[i].z);
    }
    glEnd();

    if (readyToDrawBez1 == true) {
        float maxX = pointsBezCubic[0].x;
        float minX = pointsBezCubic[0].x;
        for (int i = 0; i < 4; i++) {
            if (pointsBezCubic[i].x > maxX) {
                maxX = pointsBezCubic[i].x;
            }
            if (pointsBezCubic[i].x < minX) {
                minX = pointsBezCubic[i].x;
            }
        }
        glColor3f(0.7f, 0.0f, 1.0f);

        glBegin(GL_LINE_STRIP);
        for (double u = 0; u <= 1.0; u += 1.0 / (maxX - minX)) {
            glVertex3f(
                //x interpolation
                lerp(lerp(lerp(pointsBezCubic[0].x, pointsBezCubic[1].x, u), lerp(pointsBezCubic[1].x, pointsBezCubic[2].x, u), u),
                    lerp(lerp(pointsBezCubic[1].x, pointsBezCubic[2].x, u), lerp(pointsBezCubic[2].x, pointsBezCubic[3].x, u), u), u)
                ,
                //y interpolation
                lerp(lerp(lerp(pointsBezCubic[0].y, pointsBezCubic[1].y, u), lerp(pointsBezCubic[1].y, pointsBezCubic[2].y, u), u),
                    lerp(lerp(pointsBezCubic[1].y, pointsBezCubic[2].y, u), lerp(pointsBezCubic[2].y, pointsBezCubic[3].y, u), u), u)
                //z
                , 0);
        }
        glEnd();
    }
    if (readyToDrawBez2 == true) {
        float maxX = pointsBezCubic[3].x;
        float minX = pointsBezCubic[3].x;
        for (int i = 3; i < 7; i++) {
            if (pointsBezCubic[i].x > maxX) {
                maxX = pointsBezCubic[i].x;
            }
            if (pointsBezCubic[i].x < minX) {
                minX = pointsBezCubic[i].x;
            }
        }
        glColor3f(0.7f, 0.0f, 1.0f);

        glBegin(GL_LINE_STRIP);
        for (double u = 0; u <= 1.0; u += 1.0 / (maxX - minX)) {
            glVertex3f(
                //x interpolation
                lerp(lerp(lerp(pointsBezCubic[3].x, pointsBezCubic[4].x, u), lerp(pointsBezCubic[4].x, pointsBezCubic[5].x, u), u),
                    lerp(lerp(pointsBezCubic[4].x, pointsBezCubic[5].x, u), lerp(pointsBezCubic[5].x, pointsBezCubic[6].x, u), u), u)
                ,
                //y interpolation
                lerp(lerp(lerp(pointsBezCubic[3].y, pointsBezCubic[4].y, u), lerp(pointsBezCubic[4].y, pointsBezCubic[5].y, u), u),
                    lerp(lerp(pointsBezCubic[4].y, pointsBezCubic[5].y, u), lerp(pointsBezCubic[5].y, pointsBezCubic[6].y, u), u), u)
                //z
                , 0);
        }
        glEnd();
    }

}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	

    if (option == 1) {
        displayOption1();
    }
    else if (option == 2) {
        displayOption2();
    }
    else if (option == 3) {
        displayOption3();
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