#ifndef gpmat_H_
#define gpmat_H_

#include "gameplay.h"

using namespace gameplay;

#define GPMAT_ID_SCOLOR (0)
#define GPMAT_ID_LCOLOR (1)
#define GPMAT_ID_STEX (2)
#define GPMAT_ID_LTEX (3)

#define GPMAT_FLAG_NONE (0)
#define GPMAT_FLAG_ENTRY (1)

#define GPMAT_MODE_2D (1)
#define GPMAT_MODE_3D (2)

//  HGIMG4 Material Object
class gpmat {
public:
	gpmat();
	~gpmat();
	void reset( int id );
	int setParameter( char *name, float value );
	int setParameter( char *name, Vector3 *value );
	int setParameter( char *name, Vector4 *value );
	int setState( char *name, char *value );

	short _flag;						// 存在フラグ
	short _mark;						// マーク処理用
	int _mode;							// モード(GPMAT_MODE_*)
	int _id;							// マテリアルオブジェクトID
	Material *_material;				// 生成されたMaterial
	MeshBatch *_mesh;					// MeshBatch(2D用)
	int _sx,_sy;						// Texture X,Y Size(2D用)
	float _texratex;					// Texture UV rate (1.0/xsize)
	float _texratey;					// Texture UV rate (1.0/ysize)
};


#endif
