#pragma once

namespace p3d {
    struct Vector3 {
        union {
            struct {
                float x, y, z;
            };
            float v[3];
        };
        void set(float xx, float yy, float zz){
            x = xx;
            y = yy;
            z = zz;
        }
    };

    struct Color {
        union {
            struct {
                float r, g, b, a;
            };
            float c[4];
        };
    };

    struct MeshPoint {
        Vector3 pos;
        Color   col;
    };
}
