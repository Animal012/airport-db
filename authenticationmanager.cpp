#include "authenticationmanager.h"
#include "databasehelper.h"
#include "menuwindow.h"

QString AuthenticationManager::currentUserLogin;

AuthenticationManager::AuthenticationManager(QObject *parent) : QObject(parent) {}

bool AuthenticationManager::authenticate(const QString &login, const QString &password)
{
    // Проверяем аутентификацию пользователя в базе данных
    bool authenticated = DatabaseHelper::checkUser(login, password);
    if (authenticated) {
        // Если аутентификация успешна, создаем и показываем меню
        QStringList allowedButtons = AuthenticationManager::getAllowedButtons(login); // Используем имя класса
        menuwindow menuWindow(allowedButtons); // Передаем список разрешенных кнопок
        menuWindow.exec();
        currentUserLogin = login;
    } else {
        // Если аутентификация не успешна, вывести сообщение об ошибке или выполнить другие действия
    }
    return authenticated;
}

bool AuthenticationManager::saveUserInfo(const QString &login)
{
    // Сохраняем логин пользователя
    currentUserLogin = login;
    return true; // Возвращаем true, чтобы указать успешное сохранение
}

QString AuthenticationManager::getCurrentUserLogin() const
{
    return currentUserLogin;
}

QStringList AuthenticationManager::getAllowedButtons(const QString &login)
{
    QStringList allowedButtons;
    if (login == "admin2") {
        allowedButtons = {"Пассажиры" ,"Стойка регистрации", "Сотрудники", "Билеты", "Самолеты", "Пункт назначения", "Рейс", "Рейс_Сотрудник"};
    } else if (login == "employee") {
        allowedButtons = {"Пассажиры" ,"Стойка регистрации", "Сотрудники", "Билеты", "Самолеты", "Пункт назначения", "Рейс", "Рейс_Сотрудник"};
    } else if (login == "passenger") {
        allowedButtons = {"Пассажиры", "Билеты", "Рейс"};
    }
    return allowedButtons;
}
