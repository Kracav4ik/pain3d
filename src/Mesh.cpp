#include "Mesh.h"
namespace p3d {

    Vertex3 operator*(float f, const Vertex3 &vec) {
        return Vertex3(f * vec.x, f * vec.y, f * vec.z);
    }

    float dot(const Vertex3 &vec1, const Vertex3 &vec2) {
        return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
    }

}