#include"PointF.h"
#include "VData.h"
#pragma  comment(lib, "glew32.lib")

float SPACE_SIZE = 512.0f;						//512 * 512 * 512
int cResolution = 256;							//2^N
vector<PointF>pointCloud;
vector<Voxel>voxelData;
float xrot = 0.0f;
float yrot = 0.0f;
float zrot = 0.0f;
bool wirecubes = true;

void InitializeGlutCallbacks();
void RenderSceneCB();
void myReshape(GLsizei w, GLsizei h);
void myKeyboard(unsigned char key, int x, int y);

void CalculatePointFCloud(vector<PointF>& pointCloud);
vector<Voxel> ConvertToVoxelData(vector<PointF> pointCloud);
void CalculateIntersectPoint(vector<Voxel>& voxelData);
void read_raw_file();

int model_id = 2;
int total_model_number = 4;
string model_path = "";
int L, H, W;
int resolution_limit;
vector<PointF>model;

int target_iso = 100;
bool rotate_switch = true;
float zoom = -1000.0f;


int main(int argc, char* argv[]) 
{
	
	cout << "Program start." << endl;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1200, 768);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Final Project: Marching Cubes");

	GLenum res = glewInit();
	if (res != GLEW_OK) {
		fprintf(stderr, "Error:'%s'\n", glewGetErrorString(res));
		return 1;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	read_raw_file();

	glutReshapeFunc(myReshape);
	glutKeyboardFunc(myKeyboard);
	pointCloud = vector<PointF>();
	CalculatePointFCloud(pointCloud);
	voxelData = ConvertToVoxelData(pointCloud);
	CalculateIntersectPoint(voxelData);
	InitializeGlutCallbacks();
	glutMainLoop();

	return 0;
}


void InitializeGlutCallbacks() {
	glutDisplayFunc(RenderSceneCB);
	glutIdleFunc(RenderSceneCB);
}

void myReshape(GLsizei w, GLsizei h)
{
	//visiable area
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, 1.0 * (GLfloat)w / (GLfloat)h, 0.1, 10000.0);
}

void RenderSceneCB() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0, 0.0f, zoom);
	glRotatef(xrot, 1, 0, 0);
	glRotatef(yrot, 0, 1, 0);
	glRotatef(zrot, 0, 0, 1);
	
	

	for (int i = 0; i < voxelData.size(); i++) {
		if (wirecubes)
			voxelData[i].DrawWireCubes();
		voxelData[i].DrawISOSurface();
	}
	if (rotate_switch)
	{
		xrot += 0.05f;
		yrot += 0.05f;
		zrot += 0.05f;

	}
	

	glutSwapBuffers();
}

void myKeyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'w':
		if (cResolution * 2 <= resolution_limit)
		{
			cResolution = cResolution * 2;
		}	
		cout << "resolution up" << endl;
		break;
	case 's':
		cResolution = cResolution / 2;
		if (cResolution <= 2)
			cResolution = 2;
		cout << "resolution down" << endl;
		break;
	case 48:
		wirecubes = !wirecubes;
		break;
	default:

		break;
	}

	//change model
	switch (key)
	{
	case 'a':	//press 'a' to get last model
		model_id = (model_id - 1) % total_model_number;
		read_raw_file();
		cout << "last model" << endl;
		break;
	case 'd':	//press 'a' to get next model
		model_id = (model_id + 1) % total_model_number;
		read_raw_file();
		cout << "next model" << endl;
		break;
	default:
		break;
	}

	switch (key)
	{
	case 'i':	//press 'a' to get last model
		target_iso = target_iso + 10;
		cout << "iso + 10" << endl;
		break;
	case 'o':	//press 'a' to get next model
		target_iso = target_iso - 10;
		cout << "iso - 10" << endl;
		break;
	default:
		break;
	}

	switch (key)
	{
	case 32:
		rotate_switch = !rotate_switch;
		break;
	default:
		break;
	}

	switch (key)
	{
	case 'z':	//close
		zoom = zoom + 100.0f;
		break;
	case 'x':	//far
		zoom = zoom - 100.0f;
		break;
	default:
		break;
	}

	glutReshapeFunc(myReshape);
	//glutKeyboardFunc(myKeyboard);
	pointCloud = vector<PointF>();

	CalculatePointFCloud(pointCloud);
	voxelData = ConvertToVoxelData(pointCloud);
	CalculateIntersectPoint(voxelData);
	glutSwapBuffers();
}

void CalculatePointFCloud(vector<PointF>& pointCloud) 
{
	if (pointCloud.size() != 0)
		pointCloud.clear();

	float spaceBegin = -SPACE_SIZE / 2.0f;
	float step = SPACE_SIZE / cResolution;
	float x, y, z;
	int cur = 0;
	int l, w, h = 0;
	int s = resolution_limit / cResolution;
	for (int zz = 0; zz <= cResolution; zz++) {
		l = zz * s;
		for (int yy = 0; yy <= cResolution; yy++) {
			w = yy * s;
			for (int xx = 0; xx <= cResolution; xx++) {
				h = xx * s;
				x = spaceBegin + xx * step;
				y = spaceBegin + yy * step;
				z = spaceBegin + zz * step;
				PointF tempPoint = PointF(x, y, z, -1);
				if (zz * s < L && yy * s < H && xx * s < W && cur < model.size())
				{
					cur = l * W * H + w * H + h;
					tempPoint.SetI(model[cur].GetI());	
				}		
				else
				{
					
				}				
				tempPoint.SetInModel(target_iso);
				pointCloud.push_back(tempPoint);
			}
		}
	}
}

vector<Voxel> ConvertToVoxelData(vector<PointF> pointCloud) {
	vector<Voxel> voxelData = vector<Voxel>();
	int voxelIndex;
	int layer, row, colum;
	int R = cResolution;
	int RR = cResolution * cResolution;
	int RRR = RR * cResolution;
	int v3 = 0;
	int v0, v1, v2, v4, v5, v6, v7;
	for (int i = 0; i < RRR; i++) {
		voxelIndex = i;
		layer = voxelIndex / (cResolution * cResolution);
		voxelIndex = voxelIndex % RR;
		row = voxelIndex / R;
		colum = voxelIndex % R;

		v3 = layer * (R + 1) * (R + 1) + row * (R + 1) + colum;

		Voxel tempVoxel = Voxel();

		v0 = v3 + R + 1;
		v1 = v0 + 1;
		v2 = v3 + 1;
		v7 = v3 + (R + 1) * (R + 1);
		v6 = v7 + 1;
		v4 = v7 + (R + 1);
		v5 = v4 + 1;

		tempVoxel.SetVertex(pointCloud[v0], 0);
		tempVoxel.SetVertex(pointCloud[v1], 1);
		tempVoxel.SetVertex(pointCloud[v2], 2);
		tempVoxel.SetVertex(pointCloud[v3], 3);
		tempVoxel.SetVertex(pointCloud[v4], 4);
		tempVoxel.SetVertex(pointCloud[v5], 5);
		tempVoxel.SetVertex(pointCloud[v6], 6);
		tempVoxel.SetVertex(pointCloud[v7], 7);

		tempVoxel.CalculateiIndex();
		voxelData.push_back(tempVoxel);
	}
	return voxelData;
}

void read_raw_file()
{
	model = vector<PointF>();
	FILE* fptr;
	switch (model_id)
	{
	case 0:	// Bonsai
		model_path = "./raw_files/Bonsai_512_512_154.raw";
		cResolution = 128;
		resolution_limit = 512;
		L = 154; H = 512; W = 512;
		target_iso = 40;
		break;
	case 1:	// Teapot
		model_path = "./raw_files/BostonTeapot_256_256_178.raw";
		cResolution = 128;
		resolution_limit = 256;
		L = 178; H = 256; W = 256;
		target_iso = 80;
		break;
	case 2:	// Bucky
		model_path = "./raw_files/Bucky_32_32_32.raw";
		cResolution = 32;
		resolution_limit = 32;
		L = 32; H = 32; W = 32;
		target_iso = 130;
		break;
	case 3:	// Head
		model_path = "./raw_files/Head_256_256_225.raw";
		cResolution = 128;
		resolution_limit = 256;
		L = 225; H = 256; W = 256;
		target_iso = 130;
		break;
	default:
		break;
	}
	if ((fptr = fopen(model_path.c_str(), "rb")) == NULL)
	{
		cout << "failed to open model file" << endl;;
		return;
	}
	int i, j, k;
	int c;
	for (i = 0; i < L; i++) {
		for (j = 0; j < H; j++) {
			for (k = 0; k < W; k++) {
				if ((c = fgetc(fptr)) == EOF) {
					fprintf(stderr, "Unexpected end of file\n");
					exit(-1);
				}
				PointF p = PointF(i, j, k, c);
				model.push_back(p);
			}
		}
	}
	fclose(fptr);
	cout << "finished reading model file" << endl;
	return;
}



void CalculateIntersectPoint(vector<Voxel>& voxelData) {
	if (voxelData.size() == 0)
		return;

	for (int i = 0; i < voxelData.size(); i++) {
		voxelData[i].CalculateIntersectVertex();
	}
}