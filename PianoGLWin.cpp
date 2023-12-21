#include "PianoGLWin.h"

#define DRAW_TRIANGLE_STRIP program->enableAttributeArray(0); program->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0); glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


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

	// ��������ͶӰ����
	QMatrix4x4 projectionMatrix;
	projectionMatrix.ortho(0.0f, static_cast<float>(width()), static_cast<float>(height()), 0.0f, -1.0f, 1.0f);
	program->setUniformValue("projection", projectionMatrix);
}


void PianoGLWin::resizeGL(int w, int h)
{
	// ���ü��̴�С��ͶӰ����
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
	// ��vao��program
	vao->bind();
	program->bind();
	// ���Ƹ���
	paintPiano();
	// ���vao��program
	vao->release();
	program->release();
}

void PianoGLWin::initPiano(int w, int h)
{
	// ������ټ��̵Ŀ��
	WKWidth = w / 52.;
	BKWidth = w / 88.;
	WKHeight = h;
	BKHeight = h * 0.6;

	// ��ʼ���������
	float vertices_wkLine[55 * 4] = {};
	float vertices_bk[36 * 8] = {};

	// �׼��߿����ߣ�������
	for (int i = 0; i < 53; i++) {
		vertices_wkLine[i * 4] = i * WKWidth;
		vertices_wkLine[i * 4 + 1] = 0.5;
		vertices_wkLine[i * 4 + 2] = i * WKWidth;
		vertices_wkLine[i * 4 + 3] = WKHeight - 0.5;
	}
	// �׼��߿򣨺��ߣ�������
	int _base = 53 * 4;
	vertices_wkLine[_base] = 0.5;
	vertices_wkLine[_base + 1] = WKHeight - 0.5;
	vertices_wkLine[_base + 2] = w;
	vertices_wkLine[_base + 3] = WKHeight - 0.5;
	vertices_wkLine[_base + 4] = 0.5;
	vertices_wkLine[_base + 5] = 0.5;
	vertices_wkLine[_base + 6] = w;
	vertices_wkLine[_base + 7] = 0.5;


	// �ڼ�������
	int bi = 0;
	for (int i = 0; i < 88; i++) {
		keys[i] = Key(i + 21);
		if (!keys[i].isWhite) {
			keys[i].paintPosX = (i - bi) * WKWidth - BKWidth / 2;
			int base = bi * 8;
			vertices_bk[base] = keys[i].paintPosX;
			vertices_bk[base + 1] = 0;
			vertices_bk[base + 2] = keys[i].paintPosX;
			vertices_bk[base + 3] = BKHeight;
			vertices_bk[base + 4] = keys[i].paintPosX + BKWidth;
			vertices_bk[base + 5] = 0;
			vertices_bk[base + 6] = keys[i].paintPosX + BKWidth;
			vertices_bk[base + 7] = BKHeight;
			bi++;
		}
		else {
			keys[i].paintPosX = (i - bi) * WKWidth;
		}
	}

	// �������ļ�������
	float vertices_hk[8] = {-10, -10, -10, -10, -10, -10, -10, -10};

	// �������� vbo
	vbo_wkLine = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	vbo_wkLine->create();
	vbo_wkLine->bind();
	vbo_wkLine->allocate(vertices_wkLine, 55 * 4 * sizeof(float));

	vbo_bk = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	vbo_bk->create();
	vbo_bk->bind();
	vbo_bk->allocate(vertices_bk, 36 * 8 * sizeof(float));

	vbo_hoveredKey = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	vbo_hoveredKey->create();
	vbo_hoveredKey->bind();
	vbo_hoveredKey->allocate(vertices_hk, 4 * 2 * sizeof(float));

	// ��� vbo
	vbo_wkLine->release();
	vbo_bk->release();
}


void PianoGLWin::updateHoveredKey(Key* key)
{
	if (key == nullptr) return;
	// ����vbo_hoveredKey
	hoveredKey = key;
    float vertices_hk[8];
	if (key->isWhite) {
		vertices_hk[0] = key->paintPosX;                  vertices_hk[1] = 0;
		vertices_hk[2] = key->paintPosX;                  vertices_hk[3] = WKHeight;
		vertices_hk[4] = key->paintPosX + WKWidth;        vertices_hk[5] = 0;
		vertices_hk[6] = key->paintPosX + WKWidth;        vertices_hk[7] = WKHeight;
	}
	else {
		vertices_hk[0] = key->paintPosX;                  vertices_hk[1] = 0;
		vertices_hk[2] = key->paintPosX;                  vertices_hk[3] = BKHeight;
		vertices_hk[4] = key->paintPosX + BKWidth;        vertices_hk[5] = 0;
		vertices_hk[6] = key->paintPosX + BKWidth;        vertices_hk[7] = BKHeight;
	}
	vbo_hoveredKey->bind();
	vbo_hoveredKey->write(0, vertices_hk, 4 * 2 * sizeof(float));
	vbo_hoveredKey->release();
}


void PianoGLWin::paintPiano()
{
	// ������������İ׼�
	if (hoveredKey != nullptr && hoveredKey->isWhite) {
		program->setUniformValue(colorLocation, WHITE_HOVERED);
		vbo_hoveredKey->bind();
		DRAW_TRIANGLE_STRIP
		vbo_hoveredKey->release();
	}

	// ���Ʊ����µİ׼�
	program->setUniformValue(colorLocation, WHITE_PRESSED);
	for (int i = 0; i < pressedKeys.size(); i++) {
		if (pressedKeys[i]->isWhite) {
			vbo_hoveredKey->bind();
			DRAW_TRIANGLE_STRIP
			vbo_hoveredKey->release();
		}
	}

	// ���ư׼�����
	program->setUniformValue(colorLocation, BLACK);
	vbo_wkLine->bind();
	program->enableAttributeArray(0);
	program->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);
	glDrawArrays(GL_LINES, 0, 55 * 2);
	vbo_wkLine->release();

	// ���ƺڼ�
	program->setUniformValue(colorLocation, BLACK);
	vbo_bk->bind();
	program->enableAttributeArray(0);
	program->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);
	for (int i = 0; i < 36; i++) glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4);
	vbo_bk->release();

	// ������������ĺڼ�
	if (hoveredKey != nullptr && !hoveredKey->isWhite) {
		program->setUniformValue(colorLocation, BLACK_HOVERED);
		vbo_hoveredKey->bind();
		DRAW_TRIANGLE_STRIP
		vbo_hoveredKey->release();
	}

	// ���Ʊ����µĺڼ�
	program->setUniformValue(colorLocation, BLACK_PRESSED);
	for (int i = 0; i < pressedKeys.size(); i++) {
		if (!pressedKeys[i]->isWhite) {
			vbo_hoveredKey->bind();
			DRAW_TRIANGLE_STRIP
			vbo_hoveredKey->release();
		}
	}

}

Key* PianoGLWin::findKey(int x, int y)
{
	Key* newHoveredKey = nullptr;
	for (int i = 0; i < 88; i++) { // ���Һڼ�
		if (!keys[i].isWhite && x >= keys[i].paintPosX && x <= keys[i].paintPosX + BKWidth && y >= 0 && y <= BKHeight) {
			newHoveredKey = &keys[i];
		}
	}
	if (newHoveredKey == nullptr) {
		for (int i = 0; i < 88; i++) { // ���Ұ׼�
			if (keys[i].isWhite && x >= keys[i].paintPosX && x <= keys[i].paintPosX + WKWidth && y >= 0 && y <= WKHeight) {
				newHoveredKey = &keys[i];
			}
		}
	}
	return newHoveredKey;
}

void PianoGLWin::mouseMoveEvent(QMouseEvent* event)
{
	// ��⵱ǰ������ڵļ�
	int posX = event->pos().x();
	int posY = event->pos().y();
	Key* tmp_key = findKey(posX, posY);
	if (event->buttons() == Qt::MouseButton::LeftButton) { // ���������£����±����µļ�
		mousePressedKey = tmp_key;
		tmp_key = nullptr;
	}
	// ����vbo_hoveredKey
	float vertices_hk[8];
	if (tmp_key == nullptr) {
		vertices_hk[0] = vertices_hk[1] = vertices_hk[2] = vertices_hk[3] = vertices_hk[4] = vertices_hk[5] = vertices_hk[6] = vertices_hk[7] = -10;
		vbo_hoveredKey->bind();
		vbo_hoveredKey->write(0, vertices_hk, 4 * 2 * sizeof(float));
		vbo_hoveredKey->release();
	}
	else if (tmp_key != hoveredKey) {
		updateHoveredKey(tmp_key);
	}
	// ���»���
	update();
}

void PianoGLWin::mousePressEvent(QMouseEvent* event)
{
	// ��⵱ǰ������ڵļ�
	int posX = event->pos().x();
	int posY = event->pos().y();
	Key* tmp_key = findKey(posX, posY);
	// �ÿձ������ļ�
	hoveredKey = nullptr;
	if (event->buttons() == Qt::MouseButton::LeftButton) { // ���������£����±����µļ�
		mousePressedKey = tmp_key;
	}
	// ���»���
	update();
}

void PianoGLWin::mouseReleaseEvent(QMouseEvent* event)
{
	// ��⵱ǰ������ڵļ�
	int posX = event->pos().x();
	int posY = event->pos().y();
	Key* tmp_key = findKey(posX, posY);
	// ����vbo_hoveredKey
	float vertices_hk[8];
	if (tmp_key == nullptr) {
		vertices_hk[0] = vertices_hk[1] = vertices_hk[2] = vertices_hk[3] = vertices_hk[4] = vertices_hk[5] = vertices_hk[6] = vertices_hk[7] = -10;
		vbo_hoveredKey->bind();
		vbo_hoveredKey->write(0, vertices_hk, 4 * 2 * sizeof(float));
		vbo_hoveredKey->release();
	}
	else updateHoveredKey(tmp_key);
	if (event->buttons() == Qt::MouseButton::LeftButton) { // �������ͷţ����±����µļ�
		mousePressedKey = nullptr;
	}
	// ���»���
	update();
}

