#include "GLOutput.h"

LightSource::LightSource() :pos(3, 3, -5), col(1, 1, 1, 1){}

const Vector3 &LightSource::get_pos() {return pos;}

const Color &LightSource::get_col() {return col;}

void Camera::zoom_in() {
    scale *= 1.2f;
}

void Camera::zoom_out() {
    scale /= 1.2f;
}

QMatrix4x4 Camera::get_matrix() const {
    QMatrix4x4 res;
    res.scale(scale);
    res.rotate(rot_y, 1, 0, 0);
    res.rotate(rot_x, 0, 0, -1);
    res.translate(-pos_x, -pos_y, -pos_z);
    return res;
}

void Camera::rotate(float angle_x, float angle_y) {
    rot_x += angle_x;
    rot_y += angle_y;
}

void Camera::move_x(float dist) {
    QMatrix4x4 m;
    m.rotate(rot_x, 0, 0, 1);
    QPointF step = m * QPointF(dist, 0);
    pos_x += step.x();
    pos_y += step.y();
}

void Camera::move_y(float dist) {
    QMatrix4x4 m;
    m.rotate(rot_x, 0, 0, 1);
    QPointF step = m * QPointF(0, dist);
    pos_x += step.x();
    pos_y += step.y();
}

void GLOutput::wheelEvent(QWheelEvent *event) {
    if (event->angleDelta().y() > 0) {
        cam.zoom_in();
    } else {
        cam.zoom_out();
    }
    refresh_mvp();
}

void GLOutput::mouseMoveEvent(QMouseEvent *event) {
    QPoint point = event->pos();
    if (pos.isNull()) {
        pos = point;
        return;
    }

    if (event->buttons() & Qt::LeftButton) {
        QPoint delta = pos - point;
        cam.rotate(.5f * delta.x(), .5f * delta.y());
        refresh_mvp();
    } else if (event->buttons() & Qt::RightButton) {
        QPoint delta2 = pos - point;
        rot_x2 += .5 * delta2.x();
        rot_y2 += .5 * delta2.y();
        refresh_mvp();
    }
    pos = point;
}

void GLOutput::keyPressEvent(QKeyEvent *event) {
    printf("%s\n", event->text().toUtf8().data());
    static const float STEP = .25f;
    switch(event->key()){
        case Qt::Key_W:
            cam.move_y(STEP);
            break;
        case Qt::Key_S:
            cam.move_y(-STEP);
            break;
        case Qt::Key_A:
            cam.move_x(-STEP);
            break;
        case Qt::Key_D:
            cam.move_x(STEP);
            break;
        default:break;
    }
    refresh_mvp();
}

void GLOutput::refresh_mvp() {
    mvp = cam.get_matrix();
    mvp.rotate(rot_i, 1, 1, 1);

    mvp2 = mvp;
    mvp2.rotate(rot_x2, 0, 1, 0);
    mvp2.rotate(rot_y2, 1, 0, 0);

    update();
}

void GLOutput::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);
    for (int i = 0; i < render_items.size(); ++i) {
        render_items[i]->render(mvp);
    }
    program->bind();

    GLfloat vertices[] = {
            -1, -1, 0, 0,
            -1, 1, 0, 1,
            1, -1, 1, 0,
            1, 1, 1, 1,
    };

    GLubyte indices[] = {
            0, 1, 2, 1, 2, 3
    };

    glVertexAttribPointer(pos_attr, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), vertices);
    glVertexAttribPointer(uv_attr, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), vertices + 2);

    program->setUniformValue(u_mvp, mvp2);

    glBindTexture(GL_TEXTURE_2D, texture);

    glEnableVertexAttribArray(pos_attr);
    glEnableVertexAttribArray(uv_attr);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);

    glDisableVertexAttribArray(pos_attr);
    glDisableVertexAttribArray(uv_attr);


    program->release();
    mesh.render(mvp2);
}

void GLOutput::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(.3, 0, .3, 1);

    texture = loadBMP_custom("../tex.bmp");
    mesh.set_tex(loadBMP_custom("../aaa.bmp"));
    mesh.init_gl();

    for (int i = 0; i < render_items.size(); ++i) {
        render_items[i]->init_gl();
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(.75f);

    program = new QOpenGLShaderProgram(this);
    program->addShaderFromSourceCode(QOpenGLShader::Vertex, R"(
// Input vertex data, different for all executions of this shader.
attribute vec2 a_position;
attribute vec2 a_vertexUV;
uniform mat4 u_mvp;

// Output data ; will be interpolated for each fragment.
varying vec2 UV;

void main(){
    gl_Position = u_mvp * vec4(a_position, 0, 1);
    // UV of the vertex. No special space for this one.
    UV = a_vertexUV;
}
)");
    program->addShaderFromSourceCode(QOpenGLShader::Fragment, R"(
// Interpolated values from the vertex shaders
varying vec2 UV;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;

void main(){

    // Output color = color of the texture at the specified UV
    gl_FragColor = texture2D( myTextureSampler, UV );
    //gl_FragColor = vec4(UV, 1, 1);
}
)");
    program->link();
    pos_attr = (GLuint) program->attributeLocation("a_position");
    uv_attr = (GLuint) program->attributeLocation("a_vertexUV");
    u_mvp = (GLuint) program->uniformLocation("u_mvp");
    tex_attr = (GLuint) program->uniformLocation("myTextureSampler");
}

GLOutput::GLOutput(QWidget *parent) :
        QOpenGLWidget(parent),
        rot_x2(0),
        rot_y2(0){
    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
    setFocusPolicy(Qt::ClickFocus);
    render_items.push_back(new Grid());
    render_items.push_back(new TransformDisplay(Vector3(0, 0, 1), 270, 7));
    render_items.push_back(new Arrows());

    mesh.load();
    setMouseTracking(true);
//        timer.start(100);
}

void GLOutput::set_rotate(int i) {
    rot_i = i;
    refresh_mvp();
}

GLuint GLOutput::loadBMP_custom(const char *imagepath) {
    unsigned char header[54];
    unsigned int dataPos;
    unsigned int width, height;
    unsigned int imageSize;
    unsigned char* data;

    FILE* file = fopen(imagepath, "rb");
    fread(header, 1, 54, file);

    dataPos = *(unsigned int*) &(header[0x0A]);
    imageSize = *(unsigned int*) &(header[0x22]);
    width = *(unsigned int*) &(header[0x12]);
    height = *(unsigned int*) &(header[0x16]);
    if (imageSize == 0) {
        imageSize = width * height * 3;
    }
    data = new unsigned char[imageSize];

    fread(data, 1, imageSize, file);

    fclose(file);

    GLuint textureID;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    return textureID;
}
