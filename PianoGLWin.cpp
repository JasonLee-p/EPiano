#include "PianoGLWin.h"

#define CHECK_UPDATE if (updating) while (updating) {}; updating = true;
#define END_UPDATE updating = false;
constexpr auto PIANO_ORG_WIDTH = 1400;
constexpr auto PIANO_ORG_HEIGHT = 140;



std::map<int, int> keyMap = {
	// G3 - G6
	{Qt::Key_Z, 55},			{Qt::Key_S, 56},			{Qt::Key_X, 57},			{Qt::Key_D, 58},
	{Qt::Key_C, 59},			{Qt::Key_V, 60},			{Qt::Key_G, 61},			{Qt::Key_B, 62},
	{Qt::Key_H, 63},			{Qt::Key_N, 64},			{Qt::Key_M, 65},			{Qt::Key_K, 66},
	{Qt::Key_Comma, 67},		{Qt::Key_L, 68},			{Qt::Key_Period, 69},		{Qt::Key_Semicolon, 70},
	{Qt::Key_Slash, 71},		{Qt::Key_Q, 72},			{Qt::Key_2, 73},			{Qt::Key_W, 74},
	{Qt::Key_3, 75},			{Qt::Key_E, 76},			{Qt::Key_R, 77},			{Qt::Key_5, 78},
	{Qt::Key_T, 79},			{Qt::Key_6, 80},			{Qt::Key_Y, 81},			{Qt::Key_7, 82},
	{Qt::Key_U, 83},			{Qt::Key_I, 84},			{Qt::Key_9, 85},			{Qt::Key_O, 86},
	{Qt::Key_0, 87},			{Qt::Key_P, 88},			{Qt::Key_BracketLeft, 89},	{Qt::Key_Equal, 90},
	{Qt::Key_BracketRight, 91},	{Qt::Key_Backspace, 92},	{Qt::Key_Backslash, 93},	{Qt::Key_AsciiTilde, 94}
};


PianoGLWin::PianoGLWin(QWidget* parent)
	: QOpenGLWidget(parent)
{
	setMouseTracking(true);
}

PianoGLWin::~PianoGLWin() {
}


void PianoGLWin::initializeGL()
{
	initializeOpenGLFunctions();

	program = new QOpenGLShaderProgram();
	program->addShaderFromSourceFile(QOpenGLShader::Vertex, "shader/vertex.glsl");
	program->addShaderFromSourceFile(QOpenGLShader::Fragment, "shader/fragment.glsl");
	program->link();
	program->bind();

	colorLocation = program->uniformLocation("color");

	setPiano(width(), height(), true);
	QMatrix4x4 projectionMatrix;
	projectionMatrix.ortho(0.0f, static_cast<float>(width()), static_cast<float>(height()), 0.0f, -1.0f, 1.0f);
	program->setUniformValue("projection", projectionMatrix);

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

}


void PianoGLWin::resizeGL(int w, int h)
{
	// ���ü��̴�С��ͶӰ����
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

void PianoGLWin::setPiano(int w, int h, bool init)
{
	// ������ټ��̵Ŀ��
	WKWidth = w / 52.;
	BKWidth = w / 88.;
	WKHeight = h;
	BKHeight = h * 0.6;

	// ��ʼ���������
	float vertices_wkLine[55 * 4] = {};
	float vertices_wk[8 * 52] = {};
	float vertices_bk[8 * 36] = {};


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
	// ���̵�����
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

	// �������� vbo
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
	// ���±����µļ�
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

	if (inLoop)  for (int i = 0; i < count; i += 4)	glDrawArrays(GL_TRIANGLE_STRIP, start + i, 4);
	else glDrawArrays(GL_TRIANGLE_STRIP, start, count);

	vbo->release();
}

void PianoGLWin::drawIndexs(QOpenGLBuffer* vbo, std::vector<int>& indexs, QVector4D color)
{
	program->setUniformValue(colorLocation, color);
	vbo->bind();
	program->enableAttributeArray(0);
	program->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);

	for (int i = 0; i < indexs.size(); i++) glDrawArrays(GL_TRIANGLE_STRIP, indexs[i] * 4, 4);

	vbo->release();
}


void PianoGLWin::paintPiano()
{
	// ������������İ׼�
	if (hoveredKey != nullptr && hoveredKey->isWhite) {
		drawKeyRange(vbo_wk, hoveredKey->index * 4, 4, WHITE_HOVERED);
	}

	// ���Ʊ����µİ׼�
	if (mousePressedKey != nullptr && mousePressedKey->isWhite) {
		drawKeyRange(vbo_wk, mousePressedKey->index * 4, 4, WHITE_PRESSED);
	}
	if (pressedWkeys.size() > 0) {
		std::vector<int> indexs;
		for (int i = 0; i < pressedWkeys.size(); i++) {
			indexs.push_back(pressedWkeys[i]->index);
		}
		drawIndexs(vbo_wk, indexs, WHITE_PRESSED);
	}

	// ���ư׼�����
	drawLines(vbo_wkLine, 0, 55 * 2, BLACK);

	// ���ƺڼ�
	drawKeyRange(vbo_bk, 0, 36 * 4, BLACK, true);

	// ������������ĺڼ�
	if (hoveredKey != nullptr && !hoveredKey->isWhite) {
		drawKeyRange(vbo_bk, hoveredKey->index * 4, 4, BLACK_HOVERED);
	}

	// ���Ʊ����µĺڼ�
	if (mousePressedKey != nullptr && !mousePressedKey->isWhite) {
		drawKeyRange(vbo_bk, mousePressedKey->index * 4, 4, BLACK_PRESSED);
	}
	if (pressedBkeys.size() > 0) {
		std::vector<int> indexs;
		for (int i = 0; i < pressedBkeys.size(); i++) {
			indexs.push_back(pressedBkeys[i]->index);
		}
		drawIndexs(vbo_bk, indexs, BLACK_PRESSED);
	}
}

Key* PianoGLWin::findKey(int x, int y)
{
	// �������������
	x = x * PIANO_ORG_WIDTH / width();
	y = y * PIANO_ORG_HEIGHT / height();
	Key* onKey = nullptr;
	for (int i = 0; i < 88; i++) { // ���Һڼ�
		if (!keys[i].isWhite && x >= keys[i].paintPosX && x <= keys[i].paintPosX + BKWidth && y >= 0 && y <= BKHeight) {
			onKey = &keys[i];
		}
	}
	if (onKey == nullptr) { // û���ҵ��ڼ�
		for (int i = 0; i < 88; i++) { // ���Ұ׼�
			if (keys[i].isWhite && x >= keys[i].paintPosX && x <= keys[i].paintPosX + WKWidth && y >= 0 && y <= WKHeight) {
				onKey = &keys[i];
			}
		}
	}
	return onKey;
}

void PianoGLWin::leaveEvent(QEvent* event)
{
	hoveredKey = nullptr;
	mousePressedKey = nullptr;
	// ���»���
	update();
}

void PianoGLWin::mouseMoveEvent(QMouseEvent* event)
{
	// ��⵱ǰ������ڵļ�
	int posX = event->pos().x();
	int posY = event->pos().y();
	Key* tmp_key = findKey(posX, posY);
	// ���������£����±����µļ�
	if (event->buttons() == Qt::MouseButton::LeftButton) {
		if (mousePressedKey != tmp_key) {
			if (mousePressedKey != nullptr) {
				remove_pressedKey(mousePressedKey);
			}
			// ���±����µļ�
			if (tmp_key != nullptr) {
				add_pressedKey(tmp_key);
				tmp_key->play();
			}
			
		}
		mousePressedKey = tmp_key;
		hoveredKey = nullptr;
	}
	// ������δ���£����������ļ�
	else {
		hoveredKey = tmp_key;
		mousePressedKey = nullptr;
	}
	// ���»���
	update();
}

void PianoGLWin::mousePressEvent(QMouseEvent* event)
{
	// ��⵱ǰ������ڵļ�
	int posX = event->pos().x();
	int posY = event->pos().y();
	// ������£����±����µļ�
	if (event->buttons() == Qt::MouseButton::LeftButton) {
		if (hoveredKey != nullptr) {
			// �л�
			mousePressedKey = hoveredKey;
			hoveredKey = nullptr;
			mousePressedKey->play();
			// ���±����µļ�
			add_pressedKey(mousePressedKey);
		}
	}
	// ���»���
	update();
}

void PianoGLWin::mouseReleaseEvent(QMouseEvent* event)
{
	// ��⵱ǰ������ڵļ�
	int posX = event->pos().x();
	int posY = event->pos().y();
	// ����ͷţ����±����µļ�
	if (event->button() == Qt::MouseButton::LeftButton) {
		if (mousePressedKey != nullptr) {
			// ���±����µļ�
			remove_pressedKey(mousePressedKey);
			// �л�
			mousePressedKey->stop();
			hoveredKey = mousePressedKey;
			mousePressedKey = nullptr;
		}
	}
	// ���»���
	update();
}

void PianoGLWin::keyPressEvent(QKeyEvent* event)
{
	if (keyMap.find(event->key()) != keyMap.end()) {
		int keyNumber = keyMap[event->key()];
		Key* key = &keys[keyNumber - 21];
		add_pressedKey(key);
		key->play();
	}
	update();
}

void PianoGLWin::keyReleaseEvent(QKeyEvent* event)
{
	if (keyMap.find(event->key()) != keyMap.end()) {
		int keyNumber = keyMap[event->key()];
		Key* key = &keys[keyNumber - 21];
		remove_pressedKey(key);
		key->stop();
	}
	update();
}

void PianoGLWin::wheelEvent(QWheelEvent* event)
{
	// TODO
}

void PianoGLWin::add_pressedKey(Key* pressedKey)
{
	if (pressedKey == nullptr) {
		throw std::runtime_error("PianoGLWin::add_pressedKey() : Empty pressedKey");
	}
	QMutexLocker locker(&mutex);  // ����
	if (pressedKey->isWhite) {
		pressedWkeys.push_back(pressedKey);
	}
	else {
		pressedBkeys.push_back(pressedKey);
	}
	emit updatePressedKeysMessage();
}


void PianoGLWin::remove_pressedKey(Key* pressedKey)
{
	if (pressedKey == nullptr) {
		throw std::runtime_error("PianoGLWin::remove_pressedKey() : Empty pressedKey");
	}
	QMutexLocker locker(&mutex);  // ����
	if (pressedKey->isWhite) {
		pressedWkeys.erase(std::remove(pressedWkeys.begin(), pressedWkeys.end(), pressedKey), pressedWkeys.end());
	}
	else {
		pressedBkeys.erase(std::remove(pressedBkeys.begin(), pressedBkeys.end(), pressedKey), pressedBkeys.end());
	}
	emit updatePressedKeysMessage();
}


void PianoGLWin::add_pressedKeys(std::vector<Key*>& pressedKeys)
{
	if (pressedKeys.size() == 0) {
		throw std::runtime_error("PianoGLWin::add_pressedKeys() : Empty pressedKeys");
	}
	QMutexLocker locker(&mutex);  // ����
	for (int i = 0; i < pressedKeys.size(); i++) {
		if (pressedKeys[i]->isWhite) {
			pressedWkeys.push_back(pressedKeys[i]);
		}
		else {
			pressedBkeys.push_back(pressedKeys[i]);
		}
	}
	emit updatePressedKeysMessage();
}

void PianoGLWin::remove_pressedKeys(std::vector<Key*>& pressedKeys)
{
	QMutexLocker locker(&mutex);  // ����
	for (int i = 0; i < pressedKeys.size(); i++) {
		if (pressedKeys[i]->isWhite) {
			pressedWkeys.erase(std::remove(pressedWkeys.begin(), pressedWkeys.end(), pressedKeys[i]), pressedWkeys.end());
		}
		else {
			pressedBkeys.erase(std::remove(pressedBkeys.begin(), pressedBkeys.end(), pressedKeys[i]), pressedBkeys.end());
		}
	}
	emit updatePressedKeysMessage();
}