#include "PianoGLWin.h"

#define CHECK_UPDATE if (updating) while (updating) {}; updating = true;
#define END_UPDATE updating = false;
constexpr auto PIANO_ORG_WIDTH = 1400;
constexpr auto PIANO_ORG_HEIGHT = 140;


PianoGLWin::PianoGLWin(QWidget* parent)
	: QOpenGLWidget(parent)
{
	connectMidiSignal();
	setMouseTracking(true);
}

PianoGLWin::~PianoGLWin() {
	// 释放Synth
	delete Key::synth;
	// 释放设备管理器
	delete MidiDeviceHandler::currentHandler;
	// 释放渲染资源
	delete program;
	delete vao;
	delete vbo_bk;
	delete vbo_wkLine;
	delete vbo_wk;
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
	program->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);  // 黑键
	vbo_bk->release();

	vbo_wkLine->bind();
	program->enableAttributeArray(0);
	program->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);  // 白键
	vbo_wkLine->release();

	vao->release();

}


void PianoGLWin::resizeGL(int w, int h)
{
	// 重置键盘大小和投影矩阵
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

void PianoGLWin::setPiano(int w, int h, bool init)
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
	// 更新被按下的键
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
	// 绘制鼠标悬浮的白键
	if (hoveredKey != nullptr && hoveredKey->isWhite) {
		drawKeyRange(vbo_wk, hoveredKey->index * 4, 4, WHITE_HOVERED);
	}

	// 绘制被按下的白键
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
		std::vector<int> indexs;
		for (int i = 0; i < pressedBkeys.size(); i++) {
			indexs.push_back(pressedBkeys[i]->index);
		}
		drawIndexs(vbo_bk, indexs, BLACK_PRESSED);
	}
}

Key* PianoGLWin::findKey(int x, int y)
{
	// 对坐标进行缩放
	x = x * PIANO_ORG_WIDTH / width();
	y = y * PIANO_ORG_HEIGHT / height();
	Key* onKey = nullptr;
	for (int i = 0; i < 88; i++) { // 先找黑键
		if (!keys[i].isWhite && x >= keys[i].paintPosX && x <= keys[i].paintPosX + BKWidth && y >= 0 && y <= BKHeight) {
			onKey = &keys[i];
		}
	}
	if (onKey == nullptr) { // 没有找到黑键
		for (int i = 0; i < 88; i++) { // 再找白键
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
	// 更新画面
	update();
}

void PianoGLWin::mouseMoveEvent(QMouseEvent* event)
{
	// 监测当前鼠标所在的键
	int posX = event->pos().x();
	int posY = event->pos().y();
	Key* tmp_key = findKey(posX, posY);
	// 鼠标左键按下，更新被按下的键
	if (event->buttons() == Qt::MouseButton::LeftButton) {
		if (mousePressedKey != tmp_key) {
			if (mousePressedKey != nullptr) {
				remove_pressedKey(mousePressedKey);
			}
			// 更新被按下的键
			if (tmp_key != nullptr) {
				add_pressedKey(tmp_key);
			}
			
		}
		mousePressedKey = tmp_key;
		hoveredKey = nullptr;
	}
	// 鼠标左键未按下，更新悬浮的键
	else {
		if (mousePressedKey != nullptr) mousePressedKey->stop();
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
			// 切换
			mousePressedKey = hoveredKey;
			hoveredKey = nullptr;
			// 更新被按下的键
			add_pressedKey(mousePressedKey);
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
			// 更新被按下的键
			remove_pressedKey(mousePressedKey);
			// 切换
			hoveredKey = mousePressedKey;
			mousePressedKey = nullptr;
		}
	}
	// 更新画面
	update();
}

void PianoGLWin::keyPressEvent(QKeyEvent* event)
{
	if (LoadConfig::KEYMAP.find(event->key()) != LoadConfig::KEYMAP.end() && event->isAutoRepeat() == false) {
		int keyNumber = LoadConfig::KEYMAP[event->key()];
		if (keyNumber == NONE_NOTE) return;
		Key* key = &keys[keyNumber - 21];
		// 如果已经在被按下的键中，不再重复添加
		if (std::find(pressedWkeys.begin(), pressedWkeys.end(), key) != pressedWkeys.end() ||
			std::find(pressedBkeys.begin(), pressedBkeys.end(), key) != pressedBkeys.end()) {
			return;
		}
		else add_pressedKey(key);
	}
	update();
}

void PianoGLWin::keyReleaseEvent(QKeyEvent* event)
{
	if (LoadConfig::KEYMAP.find(event->key()) != LoadConfig::KEYMAP.end()) {
		int keyNumber = LoadConfig::KEYMAP[event->key()];
		if (keyNumber == NONE_NOTE) return;
		Key* key = &keys[keyNumber - 21];
		remove_pressedKey(key);
	}
	update();
}

void PianoGLWin::wheelEvent(QWheelEvent* event)
{
	// TODO
}

void PianoGLWin::connectMidiSignal() {
	// 链接midi信号到add_pressedKey()和remove_pressedKey()
	connect(MidiDeviceHandler::currentHandler, &MidiDeviceHandler::sendNoteOnMessage, this, 
		[=](unsigned int keyNumber, unsigned int velocity) {
			if (keyNumber >= 21 && keyNumber <= 108) {
				Key* key = &keys[keyNumber - 21];
				add_pressedKey(key);
			}
		}
	);
	connect(MidiDeviceHandler::currentHandler, &MidiDeviceHandler::sendNoteOffMessage, this,
		[=](unsigned int keyNumber, unsigned int velocity) {
			if (keyNumber >= 21 && keyNumber <= 108) {
				Key* key = &keys[keyNumber - 21];
				remove_pressedKey(key);
			}
		}
	);
}

void PianoGLWin::add_pressedKey(Key* pressedKey)
{
	// 在此函数，即使已经在被按下的键中，也会开始播放，但不添加到pressedWkeys和pressedBkeys中
	if (pressedKey == nullptr) {
		throw std::runtime_error("PianoGLWin::add_pressedKey() : Empty pressedKey");
	}
	QMutexLocker locker(&mutex);
	if (pressedKey->isWhite && std::find(pressedWkeys.begin(), pressedWkeys.end(), pressedKey) == pressedWkeys.end()) {
		pressedWkeys.push_back(pressedKey);
	}
	else if (!pressedKey->isWhite && std::find(pressedBkeys.begin(), pressedBkeys.end(), pressedKey) == pressedBkeys.end()) {
		pressedBkeys.push_back(pressedKey);
	}
	pressedKey->play();
	emit updatePressedKeysMessage(); // 发送给MainWin
	this->update();
}


void PianoGLWin::remove_pressedKey(Key* pressedKey)
{
	if (pressedKey == nullptr) {
		throw std::runtime_error("PianoGLWin::remove_pressedKey() : Empty pressedKey");
	}
	QMutexLocker locker(&mutex);  // 加锁
	pressedKey->stop();
	if (pressedKey->isWhite) {
		pressedWkeys.erase(std::remove(pressedWkeys.begin(), pressedWkeys.end(), pressedKey), pressedWkeys.end());
	}
	else {
		pressedBkeys.erase(std::remove(pressedBkeys.begin(), pressedBkeys.end(), pressedKey), pressedBkeys.end());
	}
	emit updatePressedKeysMessage(); // 发送给MainWin
	this->update();
}


void PianoGLWin::add_pressedKeys(std::vector<Key*>& pressedKeys)
{
	if (pressedKeys.size() == 0) {
		throw std::runtime_error("PianoGLWin::add_pressedKeys() : Empty pressedKeys");
	}
	QMutexLocker locker(&mutex);  // 加锁
	for (int i = 0; i < pressedKeys.size(); i++) {
		pressedKeys[i]->play();
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
	QMutexLocker locker(&mutex);  // 加锁
	for (int i = 0; i < pressedKeys.size(); i++) {
		pressedKeys[i]->stop();
		if (pressedKeys[i]->isWhite) {
			pressedWkeys.erase(std::remove(pressedWkeys.begin(), pressedWkeys.end(), pressedKeys[i]), pressedWkeys.end());
		}
		else {
			pressedBkeys.erase(std::remove(pressedBkeys.begin(), pressedBkeys.end(), pressedKeys[i]), pressedBkeys.end());
		}
	}
	emit updatePressedKeysMessage();
}