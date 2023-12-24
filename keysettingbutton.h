#ifndef KEYSETTINGBUTTON_H
#define KEYSETTINGBUTTON_H

#include <QPushButton>
#include <QStatusBar>
#include <QLineEdit>
#include <QLayout>
#include <QMouseEvent>
#include <iostream>
#include <fstream>
#include "key.h"


class KeySettingButton : public QPushButton
{
	Q_OBJECT

public:
	static KeySettingButton* currentButton;

	QLineEdit keyEdit = QLineEdit(this);

	KeySettingButton(QWidget* parent = nullptr) : QPushButton(parent) 
	{
		// 输出当前按键的text
		// std::string keyName = this->objectName().toStdString();
		// std::cout << keyName << std::endl;
		this->setCheckable(true);
		this->setLayout(new QHBoxLayout);
		this->layout()->addWidget(&keyEdit);
	}
	~KeySettingButton() {
	
	};

private:
	void mousePressEvent(QMouseEvent* event) override
	{
		QPushButton::mousePressEvent(event);
		if (event->button() == Qt::LeftButton) {
			if (currentButton != this) {
				if (currentButton != nullptr) currentButton->setChecked(false);
				currentButton = this;
				currentButton->setChecked(true);
			}
			else {
				currentButton->setChecked(false);
				currentButton = nullptr;
			}
		}
	}

	void mouseReleaseEvent(QMouseEvent* event) override
	{	
	}
};

#endif // !KEYSETTINGBUTTON_H
