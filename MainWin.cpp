
#include "MainWin.h"



MainWin::MainWin(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	// ��PianoGLWin��updatePressedKeysMessage�źŷ���ʱ������MainWin��updatePressedKeysMessage�ۺ���
	connect(ui.pianoGLWin, &PianoGLWin::updatePressedKeysMessage, this, &MainWin::updatePressedKeysMessage);
}

MainWin::~MainWin()
{}

void MainWin::updatePressedKeysMessage() {
	if (ui.pianoGLWin->pressedWkeys.size() == 0 && ui.pianoGLWin->pressedBkeys.size() == 0) {
		ui.currentNotes->setText("None");
		ui.currentType->setText("None");
		ui.currentNotes->update();
		ui.currentType->update();
		return;
	}
	// ��ȡ��ǰ���µ����м�
	QMutexLocker locker(&mutex);  // ����
	std::vector<Note*> pressedNotes;
	for (auto& key : ui.pianoGLWin->pressedWkeys) {
		pressedNotes.push_back(&key->note);
	}
	for (auto& key : ui.pianoGLWin->pressedBkeys) {
		pressedNotes.push_back(&key->note);
	}
	MusicELements::Chord chord = MusicELements::Chord(pressedNotes);
	ui.currentNotes->setText(chord.totalName.c_str());
	ui.currentType->setText(chord.type.c_str());
	ui.currentNotes->update();
	ui.currentType->update();
}
