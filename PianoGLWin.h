// PianoGLWin.h
#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QVector4D>
#include <QMatrix4x4>
#include <QMouseEvent>

#include "Key.h" // Assuming Key class is defined elsewhere

const QVector4D BLACK = QVector4D(0.0f, 0.0f, 0.0f, 1.0f);
const QVector4D WHITE = QVector4D(1.0f, 1.0f, 1.0f, 1.0f);
const QVector4D WHITE_HOVERED = QVector4D(1.0f, 0.95f, 0.95f, 1.0f);
const QVector4D BLACK_HOVERED = QVector4D(0.3f, 0.2f, 0.2f, 1.0f);
const QVector4D WHITE_PRESSED = QVector4D(0.6f, 0.5f, 0.5f, 1.0f);
const QVector4D BLACK_PRESSED = QVector4D(0.5f, 0.4f, 0.4f, 1.0f);


class PianoGLWin : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    std::vector<Key*> pressedKeys;
    Key* hoveredKey = nullptr;
    Key* mousePressedKey = nullptr;
    PianoGLWin(QWidget* parent = nullptr);
    ~PianoGLWin();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void initPiano(int w, int h);
    void paintPiano();
    void updateHoveredKey(Key* key);
    void updatePressedKeys();
    Key* findKey(int x, int y);

    bool updating = false;

    QOpenGLShaderProgram* program;
    QOpenGLVertexArrayObject* vao;
    QOpenGLBuffer* vbo_bk;
    QOpenGLBuffer* vbo_wkLine;
    QOpenGLBuffer* vbo_hoveredKey;
    int colorLocation;
    Key keys[88];
    float WKWidth, BKWidth, WKHeight, BKHeight;
};