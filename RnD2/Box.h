//=======================================================================================
// Box.h by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#ifndef BOX_H
#define BOX_H

//#include "d3dUtil.h"
#include "d3dUtil.h"

class Box
{
public:

	Box();
	~Box();

	void init(ID3D10Device* device, float scale);
	void init(ID3D10Device* device, float scale, D3DXCOLOR c);
	void draw();
	//public static const int NUM_VERTICES = 8;
private:
	DWORD mNumVertices;
	DWORD mNumFaces;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;

	//Vertex vertices[NUM_VERTICES];

};


#endif // BOX_H
