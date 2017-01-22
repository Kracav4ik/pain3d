#include "Arrows.h"

static const float ANGLE_ARR = (float) (M_PI / 12);
static const float RADIUS_ARR = 3;
static const Color DEF_COL_ARR = Color(1.f, 1.f, 1.f);

Arrows::Arrows()
        : program(nullptr)
        , u_mvp((GLuint) -1)
        , a_color((GLuint) -1)
        , a_position((GLuint) -1) {
    generate_points();
}

void Arrows::render(const QMatrix4x4 &mvp) {
    program->bind();

    glVertexAttribPointer(a_position, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredPoint), &points.data()[0].vertex);
    glVertexAttribPointer(a_color, 4, GL_FLOAT, GL_FALSE, sizeof(ColoredPoint), &points.data()[0].color);

    program->setUniformValue(u_mvp, mvp);

    glEnableVertexAttribArray(a_position);
    glEnableVertexAttribArray(a_color);
    glLineWidth(3.f);

    glDrawArrays(GL_LINES, 0, points.size());

    glDisableVertexAttribArray(a_position);
    glDisableVertexAttribArray(a_color);

    program->release();


}

void Arrows::init_gl() {
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

void Arrows::generate_points() {
    points.push_back(ColoredPoint(RADIUS_ARR * Vector3(0, 0, 0), DEF_COL_ARR));
    points.push_back(ColoredPoint(RADIUS_ARR * Vector3(0, 1, 0), DEF_COL_ARR));
    points.push_back(ColoredPoint(RADIUS_ARR * Vector3(0, 1, 0), DEF_COL_ARR));
    points.push_back(ColoredPoint(RADIUS_ARR * Vector3(.5f * sinf(ANGLE_ARR), 1 - .5f * cosf(ANGLE_ARR), 0), DEF_COL_ARR));
    points.push_back(ColoredPoint(RADIUS_ARR * Vector3(0, 1, 0), DEF_COL_ARR));
    points.push_back(ColoredPoint(RADIUS_ARR * Vector3(-.5f * sinf(ANGLE_ARR), 1 - .5f * cosf(ANGLE_ARR), 0), DEF_COL_ARR));

    }

