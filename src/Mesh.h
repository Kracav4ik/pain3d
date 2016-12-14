#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QFile>

namespace p3d {
    struct Vertex3 {
        union {
            struct {
                float x, y, z;
            };
            float a[3];
        };

        void set(float xx, float yy, float zz) {
            x = xx;
            y = yy;
            z = zz;
        }
    };

    struct TexCoords {
        union {
            struct {
                float u, v;
            };
            float a[2];
        };

        void set(float uu, float vv) {
            u = uu;
            v = vv;
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
        MeshPoint(const Vertex3& v, const TexCoords& t, const Vertex3& n) :
                vertex(v),
                tex_coord(t),
                normal(n) {}

        Vertex3 vertex;
        TexCoords tex_coord;
        Vertex3 normal;

    };

    class Mesh : private QOpenGLFunctions {
    private:
        std::vector<MeshPoint> points;
        QMatrix4x4 mat;
        QOpenGLShaderProgram* program;
        GLuint a_pos;
        GLuint a_uv;
        GLuint u_mvp;
        GLuint u_tex;
        GLuint texture;
    public:

        void render() {
            program->bind();


            glVertexAttribPointer(a_pos, 3, GL_FLOAT, GL_FALSE, sizeof(MeshPoint), points.data());
            glVertexAttribPointer(a_uv, 2, GL_FLOAT, GL_FALSE, sizeof(MeshPoint),
                                  ((char*) points.data()) + sizeof(Vertex3));

            glEnableVertexAttribArray(a_pos);
            glEnableVertexAttribArray(a_uv);

            glBindTexture(GL_TEXTURE_2D, texture);

            program->setUniformValue(u_mvp, mat);

            glDrawArrays(GL_TRIANGLES, 0, points.size());

            glDisableVertexAttribArray(a_pos);
            glDisableVertexAttribArray(a_uv);


            program->release();

        }

        void init_gl(GLuint tex) {
            initializeOpenGLFunctions();
            texture = tex;
            glEnable(GL_LINE_SMOOTH);
            glLineWidth(1.2f);
            program = new QOpenGLShaderProgram();
            program->addShaderFromSourceCode(QOpenGLShader::Vertex, R"(
attribute vec3 a_pos;
attribute vec2 a_uv;
uniform mat4 u_mvp;

varying vec2 UV;

void main(){
    UV = a_uv;
    gl_Position = u_mvp*vec4(a_pos, 1.0);

}
)");
            program->addShaderFromSourceCode(QOpenGLShader::Fragment, R"(
varying vec2 UV;
uniform sampler2D u_tex;

void main(){
    gl_FragColor = texture2D( u_tex, UV );
    //gl_FragColor = vec4(UV, 0, 1);
}
)");
            program->link();
            a_pos = (GLuint) program->attributeLocation("a_pos");
            a_uv  = (GLuint) program->attributeLocation("a_uv");
            u_mvp = (GLuint) program->uniformLocation("u_mvp");
            u_tex = (GLuint) program->uniformLocation("u_tex");
        }


        void load() {
            std::vector<Vertex3> vertices;
            std::vector<TexCoords> tex_coords;
            std::vector<Vertex3> normals;
            points.clear();

            QFile file("../example.obj");
            file.open(QIODevice::ReadOnly | QIODevice::Text);
            QTextStream in(&file);
            while (!in.atEnd()) {
                QString line = in.readLine();
                QStringList list_line = line.split(" ");
                if (list_line[0] == "v") {
                    Vertex3 ver;
                    ver.set(list_line[1].toFloat(), list_line[2].toFloat(), list_line[3].toFloat());
                    vertices.push_back(ver);
                }
                else if (list_line[0] == "vt") {
                    TexCoords tex;
                    tex.set(list_line[1].toFloat(), list_line[2].toFloat());
                    tex_coords.push_back(tex);
                }
                else if (list_line[0] == "vn") {
                    Vertex3 nor;
                    nor.set(list_line[1].toFloat(), list_line[2].toFloat(), list_line[3].toFloat());
                    normals.push_back(nor);
                }
                else if (list_line[0] == "f") {
                    for (int i : {1, 2, 3}) {
                        QStringList list_idx = list_line[i].split("/");
                        int vi = list_idx[0].toInt() - 1;
                        int ti = list_idx[1].toInt() - 1;
                        int ni = list_idx[2].toInt() - 1;
                        points.push_back(MeshPoint(vertices[vi], tex_coords[ti], normals[ni]));
                    }
                }
            }
        }

        void set_rotate(int i) {
            mat.setToIdentity();
            mat.scale(0.7);
            mat.rotate(i, 1, 1, 1);
        }
    };
}
