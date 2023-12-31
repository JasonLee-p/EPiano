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

    std::vector<Key*> pressedWkeys;                             // 所有按下的白键
    std::vector<Key*> pressedBkeys;                             // 所有按下的黑键
    Key* hoveredKey = nullptr;                                  // 悬浮的键
    Key* mousePressedKey = nullptr; 						    // 鼠标按下的键
    PianoGLWin(QWidget* parent = nullptr);
    ~PianoGLWin();

    void add_pressedKey(Key* key);                              // 加入按下的键
    void remove_pressedKey(Key* key);                           // 移除按下的键
    void add_pressedKeys(std::vector<Key*>& pressedKeys);       // 加入按下的键
    void remove_pressedKeys(std::vector<Key*>& pressedKeys);    // 移除按下的键

signals:
	void updatePressedKeysMessage();                            // 更新按下的键的信息

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
    void connectMidiSignal();                                   // 连接midi信号
    void setPiano(int w, int h, bool init=false);               // 初始化钢琴
    void drawLines(QOpenGLBuffer* vbo,                          // 绘制线
        int start, int count, QVector4D color);
    void drawKeyRange(QOpenGLBuffer* vbo,                       // 绘制键盘
        int start, int count, QVector4D color, bool inLoop);
    void drawIndexs(QOpenGLBuffer* vbo,                         // 按索引绘制键盘
        std::vector<int>& indexs, QVector4D color);
    void paintPiano(); 				                            // 绘制钢琴
    void updatePressedKeys(std::vector<Key*>& pressedKeys);     // 更新按下的键
    Key* findKey(int x, int y); 						        // 根据坐标查找键

    bool updating = false; 								        // 是否正在更新缓冲区

    QOpenGLShaderProgram*       program;                        // 着色器程序
    QOpenGLVertexArrayObject*   vao;                            // 顶点数组对象
    QOpenGLBuffer*              vbo_bk;                         // 顶点缓冲对象（黑键）
    QOpenGLBuffer*              vbo_wk;                         // 顶点缓冲对象（白键）
    QOpenGLBuffer*              vbo_wkLine; 	                // 顶点缓冲对象（白键边框）

    QMutex          mutex;										// 互斥锁
    int             colorLocation; 						       	// 颜色在着色器中的位置
    Key             keys[88];                                   // 按键实例
    float           WKWidth, BKWidth, WKHeight, BKHeight;       // 绘制参数
};

#endif // !PIANOGLWIN_H