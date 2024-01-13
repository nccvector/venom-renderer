#include<iostream>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

const C_STRUCT aiScene *scene = NULL;
int scene_list = 0;
C_STRUCT aiVector3D scene_min, scene_max, scene_center;

int main() {
    scene = aiImportFile("assets/sponza", aiProcessPreset_TargetRealtime_MaxQuality);
    return 0;
}
