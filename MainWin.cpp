#include "MainWin.h"

SettingsWin::SettingsWin(QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	this->hide();
}
SettingsWin::~SettingsWin() {}



MainWin::MainWin(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	// ��ʼ��settingsDialog
	settingsDialog = new SettingsWin(this);
	// ���������ļ���ʼ������Slider��LineEdit��ComboBox
	initInputConfig();
	// �����źźͲ�
	connect(ui.pianoGLWin, &PianoGLWin::updatePressedKeysMessage, this, &MainWin::updatePressedKeysMessage);
	connectActions();
	QPixmap pixmap("data/pianoDarker.png");
	this->setWindowIcon(pixmap);
}
MainWin::~MainWin() {}

/*	���Ӳ˵����Ķ�����ۺ���	*/
void MainWin::connectActions() {
	connect(ui.settingMenu, &QMenu::triggered, settingsDialog, &SettingsWin::show);
	connect(ui.oiSetting, &QAction::triggered, this, &MainWin::on_oiSetting_triggered);
	connect(ui.keysAction, &QAction::triggered, this, &MainWin::on_keyActionSetting_triggered);
	connect(ui.otherSetting, &QAction::triggered, this, &MainWin::on_otherSetting_triggered);
	connect(ui.aboutAuthor, &QAction::triggered, this, &MainWin::on_aboutAuthor_triggered);
	connect(ui.aboutQt, &QAction::triggered, this, &MainWin::on_aboutQt_triggered);
}

/*	���������ļ���ʼ������Slider��LineEdit��ComboBox	*/
void MainWin::initInputConfig() {
	// ��ʼ��slider��ֵ
	ui.sliderA->setValue(LoadConfig::ENVELOPE_PARAMETERS[0]);
	ui.sliderD->setValue(LoadConfig::ENVELOPE_PARAMETERS[1]);
	ui.sliderS->setValue(LoadConfig::ENVELOPE_PARAMETERS[2]);
	ui.sliderR->setValue(LoadConfig::ENVELOPE_PARAMETERS[3]);
	ui.sliderBase->setValue(LoadConfig::HARMONIC_INTENSITIES[0]);
	ui.slider2->setValue(LoadConfig::HARMONIC_INTENSITIES[1]);
    ui.slider3->setValue(LoadConfig::HARMONIC_INTENSITIES[2]);
	ui.slider4->setValue(LoadConfig::HARMONIC_INTENSITIES[3]);
	ui.slider5->setValue(LoadConfig::HARMONIC_INTENSITIES[4]);
	ui.slider6->setValue(LoadConfig::HARMONIC_INTENSITIES[5]);
	ui.slider7->setValue(LoadConfig::HARMONIC_INTENSITIES[6]);
	ui.slider8->setValue(LoadConfig::HARMONIC_INTENSITIES[7]);
	ui.slider9->setValue(LoadConfig::HARMONIC_INTENSITIES[8]);
	ui.slider10->setValue(LoadConfig::HARMONIC_INTENSITIES[9]);
	ui.slider11->setValue(LoadConfig::HARMONIC_INTENSITIES[10]);
	ui.slider12->setValue(LoadConfig::HARMONIC_INTENSITIES[11]);
	ui.slider13->setValue(LoadConfig::HARMONIC_INTENSITIES[12]);
	// ��ʼ�����ý���ĳ�ֵ
	settingsDialog->ui.sampleRateLineEdit->setText(QString::number(LoadConfig::SAMPLE_RATE));
	settingsDialog->ui.bufferSizeLineEdit->setText(QString::number(LoadConfig::BUFFER_SIZE));


	// ��slider��mouseRelease�ź���ENVELOPE_PARAMETERS���޸�
#define BIND_SLIDER_RELEASED(slider, index) \
	connect(slider, &QSlider::sliderReleased, this, [this]() { \
		mutex.lock(); \
		LoadConfig::ENVELOPE_PARAMETERS[index] = slider->value(); \
		mutex.unlock(); \
	});
	BIND_SLIDER_RELEASED(ui.sliderA, 0);
	BIND_SLIDER_RELEASED(ui.sliderD, 1);
	BIND_SLIDER_RELEASED(ui.sliderS, 2);
	BIND_SLIDER_RELEASED(ui.sliderR, 3);

	// ��slider��mouseRelease�ź���HARMONIC_INTENSITIES���޸�
#define BIND_HARMONIC_SLIDER_RELEASED(slider, index) \
	connect(slider, &QSlider::sliderReleased, this, [this]() { \
		mutex.lock(); \
		LoadConfig::HARMONIC_INTENSITIES[index] = slider->value(); \
		mutex.unlock(); \
	});
	BIND_HARMONIC_SLIDER_RELEASED(ui.sliderBase, 0);
	BIND_HARMONIC_SLIDER_RELEASED(ui.slider2, 1);
	BIND_HARMONIC_SLIDER_RELEASED(ui.slider3, 2);
	BIND_HARMONIC_SLIDER_RELEASED(ui.slider4, 3);
	BIND_HARMONIC_SLIDER_RELEASED(ui.slider5, 4);
	BIND_HARMONIC_SLIDER_RELEASED(ui.slider6, 5);
	BIND_HARMONIC_SLIDER_RELEASED(ui.slider7, 6);
	BIND_HARMONIC_SLIDER_RELEASED(ui.slider8, 7);
	BIND_HARMONIC_SLIDER_RELEASED(ui.slider9, 8);
	BIND_HARMONIC_SLIDER_RELEASED(ui.slider10, 9);
	BIND_HARMONIC_SLIDER_RELEASED(ui.slider11, 10);
	BIND_HARMONIC_SLIDER_RELEASED(ui.slider12, 11);
	BIND_HARMONIC_SLIDER_RELEASED(ui.slider13, 12);

	// �����ý����sampleRateLineEdit��editingFinished�ź���SAMPLE_RATE���޸�
	connect(settingsDialog->ui.sampleRateLineEdit, &QLineEdit::editingFinished, this, [this]() {
		mutex.lock();
		LoadConfig::SAMPLE_RATE = settingsDialog->ui.sampleRateLineEdit->text().toInt();
		Key::synth->initAudioDevice();
		mutex.unlock();
	});
	connect(settingsDialog->ui.bufferSizeLineEdit, &QLineEdit::editingFinished, this, [this]() {
		mutex.lock();
		LoadConfig::BUFFER_SIZE = settingsDialog->ui.bufferSizeLineEdit->text().toInt();
		Key::synth->initAudioDevice();
		mutex.unlock();
	});
}

void MainWin::updatePressedKeysMessage() {
	// ����״̬��
	ui.statusBar->showMessage("Last: " + ui.currentType->text() + " " + ui.currentNotes->text());
	// ����Ƿ�û�а����κμ�
	if (ui.pianoGLWin->pressedWkeys.size() == 0 && ui.pianoGLWin->pressedBkeys.size() == 0) {
		// û������ʾNone
		ui.currentNotes->setText("None");
		ui.currentType->setText("None");
		ui.currentNotes->update();
		ui.currentType->update();
		return;
	}
	// ��ȡ��ǰ���µ����м�
	QMutexLocker locker(&mutex);  // ����
	std::vector<Music::Note*> pressedNotes;
	for (auto& key : ui.pianoGLWin->pressedWkeys) {
		pressedNotes.push_back(&key->note);
	}
	for (auto& key : ui.pianoGLWin->pressedBkeys) {
		pressedNotes.push_back(&key->note);
	}
	Music::Chord chord = Music::Chord(pressedNotes);
	ui.currentNotes->setText(chord.totalName.c_str());
	ui.currentType->setText(chord.type.c_str());
	ui.currentNotes->update();
	ui.currentType->update();
}

void MainWin::on_oiSetting_triggered() {
	// ��OI���ô���
	this->settingsDialog->show();
	this->settingsDialog->ui.tabWidget->setCurrentIndex(0);
}

void MainWin::on_keyActionSetting_triggered() {
	// �򿪰������ô���
	this->settingsDialog->show();
	this->settingsDialog->ui.tabWidget->setCurrentIndex(1);
}

void MainWin::on_otherSetting_triggered() {
	// ���������ô���
	this->settingsDialog->show();
	this->settingsDialog->ui.tabWidget->setCurrentIndex(2);
}

void MainWin::on_aboutAuthor_triggered() {
	// ��Github��ҳ
	ShellExecute(NULL, L"open", L"https://github.com/JasonLee-p", NULL, NULL, SW_SHOWNORMAL);
}

void MainWin::on_aboutQt_triggered() {
	// ��Qt����
	ShellExecute(NULL, L"open", L"https://www.qt.io/", NULL, NULL, SW_SHOWNORMAL);
}