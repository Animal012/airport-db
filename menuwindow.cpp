#include "menuwindow.h"
#include "ui_menuwindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include "databasehelper.h"
#include <QMessageBox>
#include "registrationwindow.h"
#include "onb1.h"
#include "onb2.h"
#include "onb3.h"
#include "onb4.h"
#include "onb5.h"
#include "onb6.h"
#include "onb7.h"
#include "onb8.h"
menuwindow::menuwindow(const QStringList &allowedButtons, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::menuwindow)
{
    ui->setupUi(this); // Подключение формы

    QVBoxLayout *layout = new QVBoxLayout(this); // Создание layout

    // Добавляем кнопки в меню
    for (const QString &buttonText : allowedButtons) {
        QPushButton *button = new QPushButton(buttonText, this);
        button->setStyleSheet("QPushButton {"
                              "    background-color: #A9A9A9;" // Серый цвет фона
                              "    border-radius: 10px;"       // Закругленные углы
                              "}"
                              "QPushButton:hover {"
                              "    background-color: #808080;" // Цвет при наведении
                              "}"
                              "QPushButton:pressed {"
                              "    background-color: #696969;" // Цвет при нажатии
                              "}");
        layout->addWidget(button);

        // Подключение слотов к сигналу clicked() каждой кнопки
        if (buttonText == "Пассажиры") {
            connect(button, &QPushButton::clicked, this, &menuwindow::onb11);
        } else if (buttonText == "Стойка регистрации") {
            connect(button, &QPushButton::clicked, this, &menuwindow::onb22);
        } else if (buttonText == "Сотрудники") {
            connect(button, &QPushButton::clicked, this, &menuwindow::onb33);
        } else if (buttonText == "Билеты") {
            connect(button, &QPushButton::clicked, this, &menuwindow::onb44);
        } else if (buttonText == "Самолеты") {
            connect(button, &QPushButton::clicked, this, &menuwindow::onb55);
        } else if (buttonText == "Пункт назначения") {
            connect(button, &QPushButton::clicked, this, &menuwindow::onb66);
        } else if (buttonText == "Рейс") {
            connect(button, &QPushButton::clicked, this, &menuwindow::onb77);
        } else if (buttonText == "Рейс_Сотрудник") {
            connect(button, &QPushButton::clicked, this, &menuwindow::onb88);
        }
    }

    // Создаем кнопку возврата в окно регистрации
    returnToRegistrationButton = new QPushButton("Вернуться к регистрации", this);
    returnToRegistrationButton->setStyleSheet("QPushButton {"
                                              "    background-color: blue;" // Синий цвет фона
                                              "    border-radius: 10px;"       // Закругленные углы
                                              "}"
                                              "QPushButton:hover {"
                                              "    background-color: #808080;" // Цвет при наведении
                                              "}"
                                              "QPushButton:pressed {"
                                              "    background-color: #696969;" // Цвет при нажатии
                                              "}");
    layout->addWidget(returnToRegistrationButton);
    connect(returnToRegistrationButton, &QPushButton::clicked, this, &menuwindow::onReturnToRegistrationClicked);
    // Создаем кнопку выхода только один раз
    exitButton = new QPushButton("Выход", this);
    exitButton->setStyleSheet("QPushButton {"
                              "    background-color: red;" // Серый цвет фона
                              "    border-radius: 10px;"       // Закругленные углы
                              "}"
                              "QPushButton:hover {"
                              "    background-color: #808080;" // Цвет при наведении
                              "}"
                              "QPushButton:pressed {"
                              "    background-color: #696969;" // Цвет при нажатии
                              "}");
    layout->addWidget(exitButton);
    connect(exitButton, &QPushButton::clicked, this, &menuwindow::onExitButtonClicked);
}

menuwindow::~menuwindow()
{
    delete ui;
    // Не удаляйте layout здесь, он будет удален автоматически при удалении родительского виджета
}

// Реализация слотов для каждой кнопки
void menuwindow::onb11()
{
    // Проверяем подключение к базе данных
    if (DatabaseHelper::isDatabaseConnected()) {
        // Если подключение успешно, открываем окно бронирования
        onb1 *onbDialog = new onb1(this);
        onbDialog->show();
        this->hide(); // Скрываем текущее окно MenuWindow
    } else {
        // Если подключение не установлено, выводим сообщение об ошибке
        QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных.");
    }
}
void menuwindow::onb22()
{
    // Проверяем подключение к базе данных
    if (DatabaseHelper::isDatabaseConnected()) {
        // Если подключение успешно, открываем окно бронирования
        onb2 *onbDialog = new onb2(this);
        onbDialog->show();
        this->hide(); // Скрываем текущее окно MenuWindow
    } else {
        // Если подключение не установлено, выводим сообщение об ошибке
        QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных.");
    }
}

void menuwindow::onb33()
{
    // Проверяем подключение к базе данных
    if (DatabaseHelper::isDatabaseConnected()) {
        // Если подключение успешно, открываем окно бронирования
        onb3 *onbDialog = new onb3(this);
        onbDialog->show();
        this->hide(); // Скрываем текущее окно MenuWindow
    } else {
        // Если подключение не установлено, выводим сообщение об ошибке
        QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных.");
    }
}
void menuwindow::onb44()
{
    // Проверяем подключение к базе данных
    if (DatabaseHelper::isDatabaseConnected()) {
        // Если подключение успешно, открываем окно бронирования
        onb4 *onbDialog = new onb4(this);
        onbDialog->show();
        this->hide(); // Скрываем текущее окно MenuWindow
    } else {
        // Если подключение не установлено, выводим сообщение об ошибке
        QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных.");
    }
}

void menuwindow::onb55()
{
    // Проверяем подключение к базе данных
    if (DatabaseHelper::isDatabaseConnected()) {
        // Если подключение успешно, открываем окно бронирования
        onb5 *onbDialog = new onb5(this);
        onbDialog->show();
        this->hide(); // Скрываем текущее окно MenuWindow
    } else {
        // Если подключение не установлено, выводим сообщение об ошибке
        QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных.");
    }
}

void menuwindow::onb66()
{
    // Проверяем подключение к базе данных
    if (DatabaseHelper::isDatabaseConnected()) {
        // Если подключение успешно, открываем окно бронирования
        onb6 *onbDialog = new onb6(this);
        onbDialog->show();
        this->hide(); // Скрываем текущее окно MenuWindow
    } else {
        // Если подключение не установлено, выводим сообщение об ошибке
        QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных.");
    }
}

void menuwindow::onb77()
{
    // Проверяем подключение к базе данных
    if (DatabaseHelper::isDatabaseConnected()) {
        // Если подключение успешно, открываем окно бронирования
        onb7 *onbDialog = new onb7(this);
        onbDialog->show();
        this->hide(); // Скрываем текущее окно MenuWindow
    } else {
        // Если подключение не установлено, выводим сообщение об ошибке
        QMessageBox::critical(this, "Ошибка", "Не удалось подключиться к базе данных.");
    }
}

void menuwindow::onb88()
{

}
void menuwindow::onReturnToRegistrationClicked()
{
    close(); // Закрываем текущее окно меню
    registrationwindow *registrationWindow = new registrationwindow(); // Создаем новое окно регистрации
    registrationWindow->show(); // Отображаем окно регистрации
}
void menuwindow::onExitButtonClicked()
{
    // Обработка нажатия кнопки выхода
    close();
}


