#include "onb8.h"
#include "ui_onb8.h"
#include "menuwindow.h"
#include "databasehelper.h"
#include "authenticationmanager.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QTimer>
#include <QFile>
onb8::onb8(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::onb8)
{
    ui->setupUi(this);

    connect(ui->btn1, SIGNAL(clicked(bool)), this, SLOT(add()));
    connect(ui->btn2, SIGNAL(clicked(bool)), this, SLOT(remove()));
    connect(ui->btnSearch, &QPushButton::clicked, this, &onb8::search);
    connect(ui->btnReport, &QPushButton::clicked, this, &onb8::generateReportRequest);
    // Вызываем метод selectAll() через таймер с нулевой задержкой
    QTimer::singleShot(0, this, SLOT(selectAll()));

    // Устанавливаем количество столбцов в таблице
    ui->tw->setColumnCount(2); // Измените на количество столбцов в вашей таблице

    // Задаем заголовки столбцов таблицы
    ui->tw->setHorizontalHeaderItem(0, new QTableWidgetItem("Сотрудник"));
    ui->tw->setHorizontalHeaderItem(1, new QTableWidgetItem("Рейс"));
    // Добавьте заголовки для остальных столбцов, если они есть

    // Устанавливаем растягивание последнего столбца при изменении размера формы
    ui->tw->horizontalHeader()->setStretchLastSection(true);

    // Включаем возможность прокрутки содержимого таблицы
    ui->tw->setAutoScroll(true);

    // Устанавливаем режим выделения ячеек: только одна строка
    ui->tw->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tw->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Разрешаем пользователю сортировать данные по столбцам
    ui->tw->setSortingEnabled(true);
    ui->tw->sortByColumn(0, Qt::AscendingOrder);

    // Запрещаем редактирование ячеек таблицы
    ui->tw->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Populate combo boxes
    populate1ComboBox();
    populate2ComboBox();
}

onb8::~onb8()
{
    delete ui;
}


void onb8::populate1ComboBox()
{
    ui->cmb1->clear();
    QSqlQuery query("SELECT employee_id, full_name FROM Employee ORDER BY full_name");
    while (query.next()) {
        QString pu1 = query.value("employee_id").toString();
        QString pu2 = query.value("full_name").toString();
        ui->cmb1->addItem(pu2, pu1);
    }
}

void onb8::populate2ComboBox()
{
    ui->cmb2->clear();
    QSqlQuery query("SELECT flight_id FROM Flight ORDER BY flight_id");
    while (query.next()) {
        QString pu1 = query.value("flight_id").toString();

        ui->cmb2->addItem(pu1);
    }
}


void onb8::selectAll()
{

    if (!DatabaseHelper::isDatabaseConnected())
    {
        QMessageBox::critical(this, "Ошибка", "База данных не подключена.");
        return;
    }


    ui->tw->clearContents();


    QString sqlstr = "SELECT  "
                     "(SELECT full_name FROM Employee WHERE employee_id = Flight_Employee.employee_id) AS employee_name, "
                     "flight_id "
                     "FROM Flight_Employee "
                     "ORDER BY flight_id, employee_name";


    QSqlQuery query(DatabaseHelper::getDatabaseConnection());
    if (!query.exec(sqlstr))
    {
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
        return;
    }


    QMap<QString, QStringList> dataMap;


    while (query.next())
    {
        QString pu1 = query.value("employee_name").toString();
        QString pu2 = query.value("flight_id").toString();


        if (!dataMap.contains(pu1))
            dataMap[pu1] = QStringList();


        dataMap[pu1].append(pu2);
    }


    ui->tw->setRowCount(dataMap.size());


    int row = 0;
    for (auto it = dataMap.begin(); it != dataMap.end(); ++it)
    {
        QString pu1 = it.key();
        QStringList pu2 = it.value();

        QTableWidgetItem *pu11 = new QTableWidgetItem(pu1);
        ui->tw->setItem(row, 0, pu11);


        QString pu22 = pu2.join(", ");
        QTableWidgetItem *pu222 = new QTableWidgetItem(pu22);
        ui->tw->setItem(row, 1, pu222);


        ++row;
    }


    ui->tw->resizeColumnsToContents();
}

void onb8::add()
{
    if (!DatabaseHelper::isDatabaseConnected())
    {
        QMessageBox::critical(this, "Ошибка", "База данных не подключена.");
        return;
    }

    QString pu1 = ui->cmb1->currentData().toString();
    QString pu2 = ui->cmb2->currentText();

    // Проверяем существование значения в таблице
    QSqlQuery periodQuery(DatabaseHelper::getDatabaseConnection());
    periodQuery.prepare("SELECT COUNT(*) FROM Flight WHERE flight_id = :flight_id");
    periodQuery.bindValue(":flight_id", pu2);
    if (!periodQuery.exec() || !periodQuery.next() || periodQuery.value(0).toInt() == 0) {
        QMessageBox::warning(this, "Ошибка", "Значение не существует в таблице.");
        return;
    }

    // Проверяем, не существует ли уже запись с указанными значениями
    QSqlQuery checkQuery(DatabaseHelper::getDatabaseConnection());
    checkQuery.prepare("SELECT * FROM Flight_Employee WHERE flight_id = :Period_Number AND employee_id = :Check_Number");
    checkQuery.bindValue(":Period_Number", pu2);
    checkQuery.bindValue(":Check_Number", pu1);
    if (checkQuery.exec() && checkQuery.next()) {
        QMessageBox::warning(this, "Предупреждение", "Запись уже существует.");
        return;
    }

    QSqlDatabase::database().transaction();

    // Создаем объект запроса
    QSqlQuery query;

    // Подготавливаем строку SQL-запроса для вставки данных в таблицу Energy_Supply_Commission
    QString sqlstr = "INSERT INTO Flight_Employee(flight_id, employee_id) VALUES (?, ?)";

    // Подготавливаем запрос к выполнению
    query.prepare(sqlstr);

    // Привязываем значения к параметрам запроса
    query.bindValue(0, pu1);
    query.bindValue(1, pu2);

    // Выполняем запрос
    if (!query.exec())
    {
        // Откатываем транзакцию при ошибке
        QSqlDatabase::database().rollback();
        QMessageBox::critical(this, "Ошибка", "Не удалось выполнить запрос: " + query.lastError().text() +
                                                  "\nЗапрос: " + query.executedQuery());
        return;
    }

    // Фиксируем транзакцию
    QSqlDatabase::database().commit();

    // Обновляем таблицу
    selectAll();
}
void onb8::remove()
{
    if (!DatabaseHelper::isDatabaseConnected())
    {
        QMessageBox::critical(this, "Ошибка", "База данных не подключена");
        return;
    }

    int curRow = ui->tw->currentRow();

    if (curRow < 0)
    {
        QMessageBox::critical(this, "Ошибка", "Ряд не выбран!");
        return;
    }

    QTableWidgetItem *pu = ui->tw->item(curRow, 0);

    if (!pu)
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось получить идентификатор.");
        return;
    }

    QString pu1 = pu->text();

    qDebug() << "Selected ID:" << pu1;

    // Получаем ID из таблицы subscriber по full_name
    QSqlQuery subscriberQuery(DatabaseHelper::getDatabaseConnection());
    subscriberQuery.prepare("SELECT ID FROM Employee WHERE full_name = :full_name");
    subscriberQuery.bindValue(":full_name", pu1);
    if (!subscriberQuery.exec() || !subscriberQuery.next())
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось найти ID абонента: " + pu1);
        return;
    }

    QString subscriberID = subscriberQuery.value(0).toString();

    qDebug() << " ID:" << subscriberID;

    QSqlQuery query(DatabaseHelper::getDatabaseConnection());

    QString sqlstr = "DELETE FROM Flight_Employee WHERE employee_id = :subscriber_ID";

    query.prepare(sqlstr);
    query.bindValue(":subscriber_ID", subscriberID);

    if (!query.exec())
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось удалить: " + pu1 + "\nОшибка: " + query.lastError().text());
        return;
    }

    qDebug() << "Успешно :" << pu1;

    ui->tw->removeRow(curRow);
}

void onb8::on_btnExit_clicked()
{

    AuthenticationManager authenticationManager;

    // Получаем логин текущего пользователя через созданный объект
    QString currentUser = authenticationManager.getCurrentUserLogin();

    // Получаем список разрешенных кнопок для текущего пользователя
    QStringList allowedButtons = authenticationManager.getAllowedButtons(currentUser);

    // Закрываем текущее окно гостей
    close();

    // Создаем и отображаем новое окно главного меню с передачей списка разрешенных кнопок
    menuwindow *menuWindow = new menuwindow(allowedButtons);
    menuWindow->show();
}

void onb8::search()
{
    // Получаем текст из поля поиска
    QString currentSearchQuery = ui->leSearch->text().trimmed(); // Обрезаем начальные и конечные пробелы

    // Проверяем, пусто ли поле ввода
    if (currentSearchQuery.isEmpty())
    {
        // Если поле ввода пустое, вызываем функцию selectAll для выделения всех строк
        selectAll();
        return; // Завершаем выполнение функции
    }

    // Проверяем, изменилось ли значение поиска с предыдущего запроса
    if (currentSearchQuery != m_lastSearchQuery)
    {
        // Если значение изменилось, обновляем переменную с последним значением поиска
        m_lastSearchQuery = currentSearchQuery;

        // Сбрасываем индекс последнего найденного совпадения
        m_lastFoundIndex = -1;
    }

    // Очищаем выделение в таблице
    ui->tw->clearSelection();

    // Флаг для отслеживания найденных результатов
    bool foundMatch = false;

    // Проходим по всем строкам таблицы и ищем нужную строку
    for (int row = m_lastFoundIndex + 1; row < ui->tw->rowCount(); ++row)
    {
        // Получаем текст в каждой ячейке строки таблицы
        QString rowData;
        for (int col = 0; col < ui->tw->columnCount(); ++col)
        {
            QTableWidgetItem *item = ui->tw->item(row, col);
            if (item)
                rowData += item->text() + " ";
        }

        // Проверяем, содержит ли текст строки искомую подстроку
        if (rowData.contains(currentSearchQuery, Qt::CaseInsensitive))
        {
            // Выделяем найденную строку
            ui->tw->selectRow(row);

            // Прокручиваем таблицу к найденной строке
            ui->tw->scrollToItem(ui->tw->item(row, 0));

            // Устанавливаем флаг найденного элемента в true
            foundMatch = true;

            // Обновляем индекс последнего найденного совпадения
            m_lastFoundIndex = row;

            // Выходим из цикла, так как строка найдена
            break;
        }
    }

    // Если совпадение не было найдено, выводим сообщение об этом
    if (!foundMatch)
    {
        QMessageBox::information(this, "Поиск", "Больше совпадений не найдено.");
        // Сбрасываем индекс последнего найденного совпадения
        m_lastFoundIndex = -1;
    }
}

void onb8::generateReportRequest()
{
    // Вызываем функцию selectAll для получения данных о комиссиях из таблицы
    selectAll();

    QString reportText = "Запрос на формирование отчета о Рейсах и Сотрудниках:\n\n";
    reportText += "Уважаемые коллеги,\n\n";
    reportText += "Подготовлен отчет:\n";

    // Добавляем информацию о каждой комиссии из таблицы
    for (int row = 0; row < ui->tw->rowCount(); ++row) {
        QString periodNumber = ui->tw->item(row, 0)->text();
        QString checkNumbers = ui->tw->item(row, 1)->text();
        reportText += "- Сотрудник: " + periodNumber + " Рейс: " + checkNumbers + ";\n";
    }

    reportText += "\nС уважением,\n";
    reportText += "Ваше имя";

    QString fileName = "рейссотрудник.txt"; // Имя файла для сохранения отчета

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << reportText;
        file.close();
        QMessageBox::information(this, "Успех", "Запрос на формирование отчета о комиссиях на энергоснабжение успешно сохранен в файле " + fileName);

        // После сохранения отчета можно продолжить работу с приложением или завершить его
        // Например, добавив опцию для закрытия диалогового окна или для продолжения работы с приложением
        // QMessageBox::StandardButton reply = QMessageBox::question(this, "Вопрос", "Желаете продолжить работу?", QMessageBox::Yes|QMessageBox::No);
        // if (reply == QMessageBox::No) {
        //     QApplication::quit(); // Закрыть приложение
        // }
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать файл для сохранения отчета о комиссиях на энергоснабжение.");
    }
}
