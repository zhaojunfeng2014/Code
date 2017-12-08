#include <shapefil.h>
#include <stdlib.h>

int main()
{
	//读取shp
	const char * pszShapeFile = "F:\\Data\\libspatialindex_test_data\\data\\JT_L_Project.shp";
	SHPHandle hShp = SHPOpen(pszShapeFile, "r");
	int nShapeType, nVertices;
	int nEntities = 0;
	double* minB = new double[4];
	double* maxB = new double[4];
	SHPGetInfo(hShp, &nEntities, &nShapeType, minB, maxB);
	printf("ShapeType:%d\n", nShapeType);
	printf("Entities:%d\n", nEntities);
	for (int i = 0; i < nEntities; i++)
	{
		int iShape = i;
		SHPObject *obj = SHPReadObject(hShp, iShape);
		printf("--------------Feature:%d------------\n", iShape);
		int parts = obj->nParts;
		int verts = obj->nVertices;
		printf("nParts:%d\n", parts);
		printf("nVertices:%d\n", verts);
		for (size_t i = 0; i < verts; i++)
		{
			double x = obj->padfX[i];
			double y = obj->padfY[i];
			printf("%f,%f;", x, y);
		}
		printf("\n");
	}
	SHPClose(hShp);
	system("pause");

}