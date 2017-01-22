#include "TransformDisplay.h"

void TransformDisplay::generate_points() {
    for (int i = 0; i <= fabsf(angle); i ++) {
        float cosf1 = (float) cos((i / 180.f) * M_PI);
        float sinf1 = (float) sin((i / 180.f) * M_PI);
        const Color& color = Color((i%30) * 0.01f + .2f, i/180.f, 1 - i/180.f);
        color_points.push_back(ColoredPoint(rad * (cosf1 * vec1 + sinf1 * vec2), color));
    }
}

void TransformDisplay::set_params(const Vector3 &vector) {
    float a = vector.x;
    float b = vector.y;
    float c = vector.z;

    float z;
    float x;
    float y;

    float xp;
    float yp;
    float zp;


    if(a == 0){
        x = 1;
        xp = 0;
        if (b == 0){
            y = 1;
            z = 0;
            yp = 1;
            zp = 0;
        } else{
            z = 1;
            y = - z*c/b;
            zp = 0;
            yp = 0;
        }
    } else{
        y = 1;
        z = 1;
        x = -(b*y + c*z)/a;
        yp = 1;
        zp = 0;
        xp = -(b*yp + c*zp)/a;
    }

    vec1 = Vector3(x, y, z).norm();
    vec2 = Vector3(xp, yp, zp);
    vec2 = (vec2 - dot(vec2,vec1)*vec1).norm();
}

TransformDisplay::TransformDisplay(const Vector3 &vec, float angle, float rad)
        : vec1(0,0,0)
        , vec2(0,0,0)
        , angle(angle)
        , program(nullptr)
        , u_mvp((GLuint) -1)
        , a_color((GLuint) -1)
        , a_position((GLuint) -1)
        , rad(rad)
{
    set_params(vec);
    generate_points();
}

void TransformDisplay::render(const QMatrix4x4 &mvp) {
    program->bind();

    glVertexAttribPointer(a_position, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredPoint), &color_points.data()[0].vertex);
    glVertexAttribPointer(a_color, 4, GL_FLOAT, GL_FALSE, sizeof(ColoredPoint), &color_points.data()[0].color);

    program->setUniformValue(u_mvp, mvp);

    glEnableVertexAttribArray(a_position);
    glEnableVertexAttribArray(a_color);
    glLineWidth(3.f);

    glDrawArrays(GL_LINE_STRIP, 0, color_points.size());

    printf("size is %d\n",color_points.size());
    printf("color_points [%f, %f,..]\n",color_points.data()[0], color_points.data()[1]);

    glDisableVertexAttribArray(a_position);
    glDisableVertexAttribArray(a_color);

    program->release();

}

void TransformDisplay::init_gl() {
    initializeOpenGLFunctions();

    glEnable(GL_LINE_SMOOTH);
    program = new QOpenGLShaderProgram();
    program->addShaderFromSourceCode(QOpenGLShader::Vertex, R"(
attribute vec3 a_position;
attribute vec4 a_color;
uniform mat4 u_mvp;

out vec4 COL;

void main(){
    COL = a_color;
    gl_Position = u_mvp*vec4(a_position, 1.0);
}
)");
    program->addShaderFromSourceCode(QOpenGLShader::Fragment, R"(
in vec4 COL;
void main(){
    gl_FragColor = COL;
}
)");
    program->link();
    a_position = (GLuint) program->attributeLocation("a_position");
    a_color = (GLuint) program->attributeLocation("a_color");
    u_mvp = (GLuint) program->uniformLocation("u_mvp");
}
