#include "Mesh.h"
namespace p3d {

    Vector3 operator*(float f, const Vector3& vec) {
        return Vector3(f * vec.x, f * vec.y, f * vec.z);
    }

    float dot(const Vector3& vec1, const Vector3& vec2) {
        return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
    }

    void Mesh::render(const QMatrix4x4 &mvp) {
        program->bind();

        glVertexAttribPointer(a_pos, 3, GL_FLOAT, GL_FALSE, sizeof(MeshPoint), &points.data()[0].vertex);
        glVertexAttribPointer(a_uv, 2, GL_FLOAT, GL_FALSE, sizeof(MeshPoint),  &points.data()[0].tex_coord);
        glVertexAttribPointer(a_norm, 2, GL_FLOAT, GL_FALSE, sizeof(MeshPoint),  &points.data()[0].normal);

        glEnableVertexAttribArray(a_pos);
        glEnableVertexAttribArray(a_uv);
        glEnableVertexAttribArray(a_norm);

        glBindTexture(GL_TEXTURE_2D, texture);

        QMatrix4x4 m;
        m.setToIdentity();
        m.rotate(90, 0, 1);
        m.translate(2, 0);
        program->setUniformValue(u_mvp, mvp*m);

        glDrawArrays(GL_TRIANGLES, 0, points.size());

        glDisableVertexAttribArray(a_pos);
        glDisableVertexAttribArray(a_uv);
        glDisableVertexAttribArray(a_norm);

        program->release();
    }

    void Mesh::set_tex(GLuint tex) {
        texture = tex;
    }

    void Mesh::init_gl() {
        initializeOpenGLFunctions();
        glEnable(GL_LINE_SMOOTH);
        glLineWidth(1.2f);
        program = new QOpenGLShaderProgram();
        program->addShaderFromSourceCode(QOpenGLShader::Vertex, R"(
attribute vec3 a_pos;
attribute vec2 a_uv;
attribute vec3 a_norm;
uniform mat4 u_mvp;

varying vec2 UV;
varying vec3 NORM;

void main(){
    UV = a_uv;
    NORM = (u_mvp * vec4(a_norm, 0)).xyz;
    gl_Position = u_mvp*vec4(a_pos, 1.0);

}
)");
        program->addShaderFromSourceCode(QOpenGLShader::Fragment, R"(
varying vec2 UV;
varying vec3 NORM;
uniform sampler2D u_tex;
uniform vec3 u_light_pos;
uniform vec4 u_light_col;


void main(){
//  gl_FragColor = vec4(UV, 0, 1);
//  gl_FragColor = vec4( 0.5 * NORM + vec3(0.5, 0.5, 0.5), 1 );
    gl_FragColor = texture2D( u_tex, UV );
}
)");
        program->link();
        a_pos = (GLuint) program->attributeLocation("a_pos");
        a_uv = (GLuint) program->attributeLocation("a_uv");
        a_norm = (GLuint) program->attributeLocation("a_norm");
        u_mvp = (GLuint) program->uniformLocation("u_mvp");
        u_tex = (GLuint) program->uniformLocation("u_tex");
        u_light_pos = (GLuint) program->uniformLocation("u_light_pos");
        u_light_col = (GLuint) program->uniformLocation("u_light_col");
    }

    void Mesh::load() {
        std::vector<Vector3> vertices;
        std::vector<TexCoords> tex_coords;
        std::vector<Vector3> normals;
        points.clear();

        QFile file("../ddd.obj");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList list_line = line.split(" ");
            if (list_line[0] == "v") {
                Vector3 ver(list_line[1].toFloat(), list_line[2].toFloat(), list_line[3].toFloat());
                vertices.push_back(ver);
            }
            else if (list_line[0] == "vt") {
                TexCoords tex;
                tex.set(list_line[1].toFloat(), list_line[2].toFloat());
                tex_coords.push_back(tex);
            }
            else if (list_line[0] == "vn") {
                Vector3 nor(list_line[1].toFloat(), list_line[2].toFloat(), list_line[3].toFloat());
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

    ColoredPoint::ColoredPoint(const Vector3 &v, const Color &c) :
            vertex(v),
            color(c) {}

    MeshPoint::MeshPoint(const Vector3 &v, const TexCoords &t, const Vector3 &n) :
            vertex(v),
            tex_coord(t),
            normal(n) {}

    void Color::set(float rr, float gg, float bb, float aa) {
        r = rr;
        g = gg;
        b = bb;
        a = aa;
    }

    Color::Color(float rr, float gg, float bb, float aa) : r(rr), g(gg), b(bb), a(aa) {}

    void TexCoords::set(float uu, float vv) {
        u = uu;
        v = vv;
    }

    Vector3 Vector3::norm() {
        float d = sqrtf(x*x + y*y + z*z);
        if (d == 0){
            return *this;
        }
        return Vector3(x/d, y/d, z/d);
    }

    Vector3 Vector3::operator-() const {
        return Vector3(-x, -y, -z);
    }

    Vector3 Vector3::operator-(const Vector3 &vec) const {
        return *this + (-vec);
    }

    Vector3 Vector3::operator+(const Vector3 &vec) const {
        return Vector3(vec.x + x, vec.y + y, vec.z + z);
    }
}