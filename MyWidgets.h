#pragma once
/*
	����Qt�����ؼ�
*/

#include <iostream>
#include <QApplication>
#include <QMenu>
#include <QMenuBar>
#include <QLineEdit>
#include <QSlider>
#include <QToolTip>
#include <QAction>
#include <QList>
#include <QEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QIntValidator>


#define CHECK_UPDATE if (updating) while (updating) {}; updating = true;
#define END_UPDATE updating = false;


class Menu : public QMenu
{
    Q_OBJECT
public:
    Menu(QWidget* parent = nullptr) : QMenu(parent)
    {}

protected:
    void enterEvent(QEnterEvent* event) override
    {
		// ������ʱ��ʾ�Ӳ˵�
		QMenu::enterEvent(event);
		show();
	}

    void leaveEvent(QEvent* event) override
    {
        // ����뿪ʱ�����Ӳ˵�
        QMenu::leaveEvent(event);
        hide();
    }
};



class IntEdit : public QLineEdit
{
	Q_OBJECT

public:
	IntEdit(QWidget* parent = nullptr) : QLineEdit(parent) {
		setValidator(new QIntValidator(min, max, this));
	}

	~IntEdit() {}

	void setStep(int step) {
		this->step = step;
	}

	void setRange(int* min, int* max) {
		if (min != nullptr) this->min = *min;
		if (max != nullptr) this->max = *max;
	}

private:
	int min = 0;
	int max = 16777215;
	int step = 1;
	bool updating = false;

	void wheelEvent(QWheelEvent* event) override {
		CHECK_UPDATE
		if (event->angleDelta().y() > 0 && text().toInt() + step <= max) {
			setText(QString::number(text().toInt() + step));
		}
		else if (event->angleDelta().y() < 0 && text().toInt() - step >= min) {
			setText(QString::number(text().toInt() - step));
		}
		END_UPDATE
	}
};



class Slider : public QSlider
{
	Q_OBJECT

public:
    Slider(QWidget* parent = nullptr) : QSlider(parent)
	{}
    ~Slider() {}

protected:
    void mousePressEvent(QMouseEvent* event) override
    {
		setValue(valueFromPosition(event->pos()));
		QSlider::mousePressEvent(event);
		// ������Ҳ���ʾ��ǰֵ
		QToolTip::showText(event->globalPos(), QString::number(value()), this);
	}

    void mouseMoveEvent(QMouseEvent* event) override
    {
		if (event->buttons() & Qt::LeftButton) {
			setValue(valueFromPosition(event->pos()));
			// ������Ҳ���ʾ��ǰֵ
			QToolTip::showText(event->globalPos(), QString::number(value()), this);
		}
	}

    void mouseReleaseEvent(QMouseEvent* event) override
    {
		setValue(valueFromPosition(event->pos()));
		QSlider::mouseReleaseEvent(event);
		// ������Ҳ���ʾ��ǰֵ
		QToolTip::showText(event->globalPos(), QString::number(value()), this);
	}

	int valueFromPosition(const QPoint& pos)
	{
		if (orientation() == Qt::Horizontal) {
			return minimum() + ((maximum() - minimum()) * pos.x()) / width();
		}
		else {
			return minimum() + ((maximum() - minimum()) * (height() - pos.y())) / height();
		}
	}
};