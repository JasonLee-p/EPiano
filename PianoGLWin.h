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

#include "Key.h"

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
    std::vector<Key*> pressedWkeys;                            /*���а��µİ׼�*/ 
    std::vector<Key*> pressedBkeys;                            /*���а��µĺڼ�*/
    Key* hoveredKey = nullptr; 							       /*�����ļ�*/
    Key* mousePressedKey = nullptr; 						   /*��갴�µļ�*/
    PianoGLWin(QWidget* parent = nullptr);
    ~PianoGLWin();

    void add_pressedKeys(std::vector<Key*>& pressedKeys);      /*���밴�µļ�*/
    void remove_pressedKeys(std::vector<Key*>& pressedKeys);   /*�Ƴ����µļ�*/

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void initPiano(int w, int h);                              /*��ʼ������*/
    void drawLines(QOpenGLBuffer* vbo,                         /*������*/
        int start, int count, QVector4D color);
    void drawKeyRange(QOpenGLBuffer* vbo,                      /*���Ƽ���*/
        int start, int count, QVector4D color, bool inLoop);
    void paintPiano(); 				                           /*���Ƹ���*/
    void updatePressedKeys(std::vector<Key*>& pressedKeys);    /*���°��µļ�*/
    Key* findKey(int x, int y); 						       /*����������Ҽ�*/

    bool updating = false; 								       /*�Ƿ����ڸ��»�����*/

    QOpenGLShaderProgram* program;                             // ��ɫ������
    QOpenGLVertexArrayObject* vao;                             // �����������
    QOpenGLBuffer* vbo_bk;                                     // ���㻺����󣨺ڼ���
    QOpenGLBuffer* vbo_wk;                                     // ���㻺����󣨰׼���
    QOpenGLBuffer* vbo_wkLine; 		                           // ���㻺����󣨰׼��߿�

    int colorLocation; 						       			   // ��ɫ����ɫ���е�λ��
    Key keys[88];                                              // ����ʵ��
    float WKWidth, BKWidth, WKHeight, BKHeight;                // ���Ʋ���
};