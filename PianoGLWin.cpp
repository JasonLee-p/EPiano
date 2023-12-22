#include "PianoGLWin.h"

#define CHECK_UPDATE if (updating) return; updating = true;
#define END_UPDATE updating = false;

const float FLOAT_EMPTY8[8] = { -10, -10, -10, -10, -10, -10, -10, -10 };

PianoGLWin::PianoGLWin(QWidget* parent)
	: QOpenGLWidget(parent)
{
	setMouseTracking(true);
}

PianoGLWin::~PianoGLWin() {}


void PianoGLWin::initializeGL()
{
	initializeOpenGLFunctions();

	program = new QOpenGLShaderProgram();
	program->addShaderFromSourceFile(QOpenGLShader::Vertex, "shader/vertex.glsl");
	program->addShaderFromSourceFile(QOpenGLShader::Fragment, "shader/fragment.glsl");
	program->link();
	program->bind();

	colorLocation = program->uniformLocation("color");

	initPiano(width(), height());

	vao = new QOpenGLVertexArrayObject();
	vao->create();
	vao->bind();

	vbo_bk->bind();
	program->enableAttributeArray(0);
	program->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);  // Black keys
	vbo_bk->release();

	vbo_wkLine->bind();
	program->enableAttributeArray(0);
	program->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);  // White key lines
	vbo_wkLine->release();

	vao->release();

	// 设置正交投影矩阵
	QMatrix4x4 projectionMatrix;
	projectionMatrix.ortho(0.0f, static_cast<float>(width()), static_cast<float>(height()), 0.0f, -1.0f, 1.0f);
	program->setUniformValue("projection", projectionMatrix);
}


void PianoGLWin::resizeGL(int w, int h)
{
	// 重置键盘大小和投影矩阵
	initPiano(w, h);
	QMatrix4x4 projectionMatrix;
	projectionMatrix.ortho(0.0f, static_cast<float>(w), static_cast<float>(h), 0.0f, -1.0f, 1.0f);
	program->setUniformValue("projection", projectionMatrix);
	glViewport(0, 0, w, h);
}


void PianoGLWin::paintGL()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	// 绑定vao和program
	vao->bind();
	program->bind();
	// 绘制钢琴
	paintPiano();
	// 解绑vao和program
	vao->release();
	program->release();
}

void PianoGLWin::initPiano(int w, int h)
{
	// 计算钢琴键盘的宽高
	WKWidth = w / 52.;
	BKWidth = w / 88.;
	WKHeight = h;
	BKHeight = h * 0.6;

	// 初始化数组对象
	float vertices_wkLine[55 * 4] = {};
	float vertices_wk[8 * 52] = {};
	float vertices_bk[8 * 36] = {};


	// 白键边框（竖线）的数组
	for (int i = 0; i < 53; i++) {
		vertices_wkLine[i * 4] = i * WKWidth;
		vertices_wkLine[i * 4 + 1] = 0.5;
		vertices_wkLine[i * 4 + 2] = i * WKWidth;
		vertices_wkLine[i * 4 + 3] = WKHeight - 0.5;
	}
	// 白键边框（横线）的数组
	int _base = 53 * 4;
	vertices_wkLine[_base] = 0.5;
	vertices_wkLine[_base + 1] = WKHeight - 0.5;
	vertices_wkLine[_base + 2] = w;
	vertices_wkLine[_base + 3] = WKHeight - 0.5;
	vertices_wkLine[_base + 4] = 0.5;
	vertices_wkLine[_base + 5] = 0.5;
	vertices_wkLine[_base + 6] = w;
	vertices_wkLine[_base + 7] = 0.5;
	// 键盘的数组
	int wi = 0;
	int bi = 0;
	for (int i = 0; i < 88; i++) {
		keys[i] = Key(i + 21);
		if (keys[i].isWhite) {
			keys[i].paintPosX = (i - bi) * WKWidth;
			keys[i].index = wi;
			int base = wi * 8;
			vertices_wk[base] = keys[i].paintPosX;                 vertices_wk[base + 1] = 0;
			vertices_wk[base + 2] = keys[i].paintPosX;             vertices_wk[base + 3] = WKHeight;
			vertices_wk[base + 4] = keys[i].paintPosX + WKWidth;   vertices_wk[base + 5] = 0;
			vertices_wk[base + 6] = keys[i].paintPosX + WKWidth;   vertices_wk[base + 7] = WKHeight;
			wi++;
		}
		else {
			keys[i].paintPosX = (i - bi) * WKWidth - BKWidth / 2;
			keys[i].index = bi;
			int base = bi * 8;
			vertices_bk[base] = keys[i].paintPosX;				   vertices_bk[base + 1] = 0;
			vertices_bk[base + 2] = keys[i].paintPosX;			   vertices_bk[base + 3] = BKHeight;
			vertices_bk[base + 4] = keys[i].paintPosX + BKWidth;   vertices_bk[base + 5] = 0;
			vertices_bk[base + 6] = keys[i].paintPosX + BKWidth;   vertices_bk[base + 7] = BKHeight;
			bi++;
		}
	}

	// 创建并绑定 vbo
	vbo_wkLine = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	vbo_wkLine->create();
	vbo_wkLine->bind();
	vbo_wkLine->allocate(vertices_wkLine, 55 * 4 * sizeof(float));

	vbo_bk = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	vbo_bk->create();
	vbo_bk->bind();
	vbo_bk->allocate(vertices_bk, 8 * 36 * sizeof(float));

	vbo_wk = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	vbo_wk->create();
	vbo_wk->bind();
	vbo_wk->allocate(vertices_wk, 8 * 52 * sizeof(float));
}

void PianoGLWin::updatePressedKeys(std::vector<Key*>& pressedKeys)
{
	// 更新vbo_pressedWkeys
}


void PianoGLWin::drawLines(QOpenGLBuffer* vbo, int start, int count, QVector4D color)
{
	program->setUniformValue(colorLocation, color);
	vbo->bind();
	program->enableAttributeArray(0);
	program->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);
	glDrawArrays(GL_LINES, start, count);
	vbo->release();
}


void PianoGLWin::drawKeyRange(QOpenGLBuffer* vbo, int start, int count, QVector4D color, bool inLoop=false)
{
	program->setUniformValue(colorLocation, color);
	vbo->bind();
	program->enableAttributeArray(0);
	program->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);
	if (inLoop) {
		for (int i = 0; i < count; i += 4) {
			glDrawArrays(GL_TRIANGLE_STRIP, start + i, 4);
		}
	}
	else {
		glDrawArrays(GL_TRIANGLE_STRIP, start, count);
	}
	vbo->release();
}


void PianoGLWin::paintPiano()
{
	// 绘制鼠标悬浮的白键
	if (hoveredKey != nullptr && hoveredKey->isWhite) {
		drawKeyRange(vbo_wk, hoveredKey->index * 4, 4, WHITE_HOVERED);
	}

	// 绘制被按下的白键
	if (mousePressedKey != nullptr && mousePressedKey->isWhite) {
		drawKeyRange(vbo_wk, mousePressedKey->index * 4, 4, WHITE_PRESSED);
	}
	if (pressedWkeys.size() > 0) {
		// TODO
	}

	// 绘制白键边线
	drawLines(vbo_wkLine, 0, 55 * 2, BLACK);

	// 绘制黑键
	drawKeyRange(vbo_bk, 0, 36 * 4, BLACK, true);

	// 绘制鼠标悬浮的黑键
	if (hoveredKey != nullptr && !hoveredKey->isWhite) {
		drawKeyRange(vbo_bk, hoveredKey->index * 4, 4, BLACK_HOVERED);
	}

	// 绘制被按下的黑键
	if (mousePressedKey != nullptr && !mousePressedKey->isWhite) {
		drawKeyRange(vbo_bk, mousePressedKey->index * 4, 4, BLACK_PRESSED);
	}
	if (pressedBkeys.size() > 0) {
		// TODO
	}
}

Key* PianoGLWin::findKey(int x, int y)
{
	Key* newHoveredKey = nullptr;
	for (int i = 0; i < 88; i++) { // 先找黑键
		if (!keys[i].isWhite && x >= keys[i].paintPosX && x <= keys[i].paintPosX + BKWidth && y >= 0 && y <= BKHeight) {
			newHoveredKey = &keys[i];
		}
	}
	if (newHoveredKey == nullptr) {
		for (int i = 0; i < 88; i++) { // 再找白键
			if (keys[i].isWhite && x >= keys[i].paintPosX && x <= keys[i].paintPosX + WKWidth && y >= 0 && y <= WKHeight) {
				newHoveredKey = &keys[i];
			}
		}
	}
	return newHoveredKey;
}

void PianoGLWin::mouseMoveEvent(QMouseEvent* event)
{
	// 监测当前鼠标所在的键
	int posX = event->pos().x();
	int posY = event->pos().y();
	Key* tmp_key = findKey(posX, posY);
	// 鼠标左键按下，更新被按下的键
	if (event->buttons() == Qt::MouseButton::LeftButton) {
		mousePressedKey = tmp_key;
		hoveredKey = nullptr;
	}
	// 鼠标左键未按下，更新悬浮的键
	else {
		hoveredKey = tmp_key;
		mousePressedKey = nullptr;
	}
	// 更新画面
	update();
}

void PianoGLWin::mousePressEvent(QMouseEvent* event)
{
	// 监测当前鼠标所在的键
	int posX = event->pos().x();
	int posY = event->pos().y();
	// 左键按下，更新被按下的键
	if (event->buttons() == Qt::MouseButton::LeftButton) {
		if (hoveredKey != nullptr) {
			mousePressedKey = hoveredKey;
			hoveredKey = nullptr;
			mousePressedKey->play();
		}
	}
	// 更新画面
	update();
}

void PianoGLWin::mouseReleaseEvent(QMouseEvent* event)
{
	// 监测当前鼠标所在的键
	int posX = event->pos().x();
	int posY = event->pos().y();
	// 左键释放，更新被按下的键
	if (event->button() == Qt::MouseButton::LeftButton) {
		if (mousePressedKey != nullptr) {
			mousePressedKey->stop();
			hoveredKey = mousePressedKey;
			mousePressedKey = nullptr;
		}
	}
	// 更新画面
	update();
}

void PianoGLWin::add_pressedKeys(std::vector<Key*>& pressedKeys)
{
	for (int i = 0; i < pressedKeys.size(); i++) {
		if (pressedKeys[i]->isWhite) {
			pressedWkeys.push_back(pressedKeys[i]);
		}
		else {
			pressedBkeys.push_back(pressedKeys[i]);
		}
	}
	// 更新主界面的按键内容显示Label
}

void PianoGLWin::remove_pressedKeys(std::vector<Key*>& pressedKeys)
{
	for (int i = 0; i < pressedKeys.size(); i++) {
		if (pressedKeys[i]->isWhite) {
			pressedWkeys.erase(std::remove(pressedWkeys.begin(), pressedWkeys.end(), pressedKeys[i]), pressedWkeys.end());
		}
		else {
			pressedBkeys.erase(std::remove(pressedBkeys.begin(), pressedBkeys.end(), pressedKeys[i]), pressedBkeys.end());
		}
	}
	// 更新主界面的按键内容显示Label
}

