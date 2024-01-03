#ifndef PIANOGLWIN_H
#define PIANOGLWIN_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QVector4D>
#include <QMatrix4x4>
#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMutexLocker>

#include "loadConfig.h"
#include "Key.h"
#include "keysettingbutton.h"


const QVector4D BLACK =             QVector4D(0.0f, 0.0f, 0.0f, 1.0f);
const QVector4D WHITE =             QVector4D(1.0f, 1.0f, 1.0f, 1.0f);
const QVector4D WHITE_HOVERED =     QVector4D(1.0f, 0.95f, 0.95f, 1.0f);
const QVector4D BLACK_HOVERED =     QVector4D(0.3f, 0.2f, 0.2f, 1.0f);
const QVector4D WHITE_PRESSED =     QVector4D(1.0f, 0.6f, 0.6f, 1.0f);
const QVector4D BLACK_PRESSED =     QVector4D(0.8f, 0.2f, 0.2f, 1.0f);


class PianoGLWin : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:

    std::vector<Key*> pressedWkeys;                             // ���а��µİ׼�
    std::vector<Key*> pressedBkeys;                             // ���а��µĺڼ�
    Key* hoveredKey = nullptr;                                  // �����ļ�
    Key* mousePressedKey = nullptr; 						    // ��갴�µļ�
    PianoGLWin(QWidget* parent = nullptr);
    ~PianoGLWin();

    void add_pressedKey(Key* key);                              // ���밴�µļ�
    void remove_pressedKey(Key* key);                           // �Ƴ����µļ�
    void add_pressedKeys(std::vector<Key*>& pressedKeys);       // ���밴�µļ�
    void remove_pressedKeys(std::vector<Key*>& pressedKeys);    // �Ƴ����µļ�

signals:
	void updatePressedKeysMessage();                            // ���°��µļ�����Ϣ

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void leaveEvent(QEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    void connectMidiSignal();                                   // ����midi�ź�
    void setPiano(int w, int h, bool init=false);               // ��ʼ������
    void drawLines(QOpenGLBuffer* vbo,                          // ������
        int start, int count, QVector4D color);
    void drawKeyRange(QOpenGLBuffer* vbo,                       // ���Ƽ���
        int start, int count, QVector4D color, bool inLoop);
    void drawIndexs(QOpenGLBuffer* vbo,                         // ���������Ƽ���
        std::vector<int>& indexs, QVector4D color);
    void paintPiano(); 				                            // ���Ƹ���
    void updatePressedKeys(std::vector<Key*>& pressedKeys);     // ���°��µļ�
    Key* findKey(int x, int y); 						        // ����������Ҽ�

    bool updating = false; 								        // �Ƿ����ڸ��»�����

    QOpenGLShaderProgram*       program;                        // ��ɫ������
    QOpenGLVertexArrayObject*   vao;                            // �����������
    QOpenGLBuffer*              vbo_bk;                         // ���㻺����󣨺ڼ���
    QOpenGLBuffer*              vbo_wk;                         // ���㻺����󣨰׼���
    QOpenGLBuffer*              vbo_wkLine; 	                // ���㻺����󣨰׼��߿�

    QMutex          mutex;										// ������
    int             colorLocation; 						       	// ��ɫ����ɫ���е�λ��
    Key             keys[88];                                   // ����ʵ��
    float           WKWidth, BKWidth, WKHeight, BKHeight;       // ���Ʋ���
};

#endif // !PIANOGLWIN_H