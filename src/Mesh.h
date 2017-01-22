#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QFile>
#include "RenderItem.h"

namespace p3d {
    struct Vector3 {
        union {
            struct {
                float x, y, z;
            };
            float a[3];
        };

        Vector3(float xx, float yy, float zz): x(xx), y(yy), z(zz) {}

        Vector3 operator+(const Vector3& vec) const;

        Vector3 operator-(const Vector3& vec) const;

        Vector3 operator-() const;

        void set(float xx, float yy, float zz) {
            x = xx;
            y = yy;
            z = zz;
        }

        Vector3 norm();
    };

    Vector3 operator*(float f, const Vector3& vec);
    float dot(const Vector3& vec1, const Vector3& vec2);

    struct TexCoords {
        union {
            struct {
                float u, v;
            };
            float a[2];
        };

        void set(float uu, float vv);
    };

    struct Color {
        union {
            struct {
                float r, g, b, a;
            };
            float c[4];
        };
        Color(float rr, float gg, float bb, float aa=1);

        void set(float rr, float gg, float bb, float aa=1);
    };

    struct MeshPoint {
        MeshPoint(const Vector3& v, const TexCoords& t, const Vector3& n);

        Vector3 vertex;
        TexCoords tex_coord;
        Vector3 normal;
    };

    struct ColoredPoint {
        ColoredPoint(const Vector3& v, const Color& c=Color(0,0,0));
        Vector3 vertex;
        Color color;
    };

    class Mesh : RenderItem, private QOpenGLFunctions {
    private:
        std::vector<MeshPoint> points;
        QOpenGLShaderProgram* program;
        GLuint a_pos;
        GLuint a_uv;
        GLuint u_mvp;
        GLuint u_tex;
        GLuint u_light_pos;
        GLuint u_light_col;
        GLuint a_norm;
        GLuint texture;
    public:

        void render(const QMatrix4x4& mvp) override;

        void set_tex(GLuint tex);

        void init_gl() override;


        void load();
    };
}
