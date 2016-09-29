#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gl/glut.h>
#include <direct.h>
#include <cstring>
#include "gmath.h"

// 매크로 상수 정의
int width, height, depth;
unsigned char *pVolData = NULL;
unsigned char *pImage = NULL;
unsigned char *pScaledImage = NULL;
int SliceIdx = 0;
int N = 1;

std::vector<GSphere> SphereList;
std::vector<GLight> LightList;

// 콜백 함수 선언
void Render();
void Reshape(int w, int h);
void KeyBoard(unsigned char key, int x, int y);

// 광선 추적 함수
void LoadData(char *fime_name);
void CreateImage();
int GetIdx(int i, int j);
int GetIdx(int i, int j, int k);

int main(int argc, char **argv)
{
	// OpenGL 초기화, 윈도우 크기 설정, 디스플레이 모드 설정
	LoadData(".\\data\\bighead.txt");

	glutInit(&argc, argv);
	glutInitWindowSize(width * N, height * N);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// 윈도우 생성 및 콜백 함수 등록
	glutCreateWindow("Slice Viewer");
	glutDisplayFunc(Render);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(KeyBoard);

	// 이미지를 생성
	CreateImage();

	// 이벤트를 처리를 위한 무한 루프로 진입한다.
	glutMainLoop();

	delete[] pVolData;
	delete[] pImage;
	delete[] pScaledImage;
	return 0;
}

void KeyBoard(unsigned char key, int x, int y) {
	if (key == '1') SliceIdx++;
	if (SliceIdx >= depth) SliceIdx = depth - 1;

	if (key == '2') SliceIdx--;
	if (SliceIdx <= 0) SliceIdx = 0;

	printf("Slide No. = %d\n", SliceIdx);
	CreateImage();
	glutPostRedisplay();
}

void LoadData(char *file_name) {
	FILE *fp;
	fopen_s(&fp, file_name, "r");
	fscanf_s(fp, "%d%d%d", &width, &height, &depth);

	char vol_file_name[256];
	fscanf_s(fp, "%s", vol_file_name,256);
	fclose(fp);

	std::string file_path(file_name);
	int idx = file_path.rfind("\\");
	file_path = file_path.substr(0, idx);
	_chdir(file_path.c_str());

	pVolData = new unsigned char[depth * height * width];
	fopen_s(&fp, vol_file_name, "rb");
	fread(pVolData, sizeof(unsigned char), depth * height * width, fp);
	fclose(fp);
}

void Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void Render()
{
	// 칼라 버퍼와 깊이 버퍼 지우기
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (pScaledImage == NULL)
		pScaledImage = new unsigned char[width * N * height * N * 3];

	for (int i = 0; i < height *N; i++)
	{
		for (int j = 0; j < width * N; j++)
		{
			unsigned char r = pImage[i / N * width * 3 + j / N * 3];
			unsigned char g = pImage[i / N * width * 3 + j / N * 3 + 1];
			unsigned char b = pImage[i / N * width * 3 + j / N * 3 + 2];

			pScaledImage[i * width * N * 3 + j * 3] = r;
			pScaledImage[i * width * N * 3 + j * 3+1 ] = g;
			pScaledImage[i * width * N * 3 + j * 3+2 ] = b;
		}
	}

	// 칼라 버퍼에 Image 데이터를 직접 그린다.
	glDrawPixels(width*N, height*N, GL_RGB, GL_UNSIGNED_BYTE, pScaledImage);

	// 칼라 버퍼 교환한다
	glutSwapBuffers();
}


void CreateImage()
{
	if (pImage == NULL)
		pImage = new unsigned char[width * height * 3];

	for (int j = 0; j < height; ++j)
	{
		for (int k = 0; k < width; ++k)
		{
			int vidx = GetIdx(SliceIdx, j, k);
			int pidx = GetIdx(j, k);
			pImage[pidx] = pVolData[vidx];
			pImage[pidx+1] = pVolData[vidx];
			pImage[pidx+2] = pVolData[vidx];
		}
	}
}

int GetIdx(int i, int j) {
	return (height - 1 - i) * width * 3 + j * 3;
}

int GetIdx(int i, int j, int k) {
	return i * (width * height) + j * width + k;
}