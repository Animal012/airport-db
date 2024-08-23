#include "onb7.h"
#include "ui_onb7.h"
#include "authenticationmanager.h"
#include "databasehelper.h"
#include "menuwindow.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QTimer>
#include <QDateTime>
#include <QHBoxLayout>
#include <QSqlRecord>
#include <QFile>
#include <QTextStream>
onb7::onb7(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::onb7)
{
    ui->setupUi(this);

    // Подключение сигналов к слотам
    connect(ui->btn1, SIGNAL(clicked(bool)), this, SLOT(add()));
    connect(ui->btn2, SIGNAL(clicked(bool)), this, SLOT(edit()));
    connect(ui->btn3, SIGNAL(clicked(bool)), this, SLOT(remove()));
    connect(ui->btn4, SIGNAL(clicked(bool)), this, SLOT(clean()));
    connect(ui->btnSearch, &QPushButton::clicked, this, &onb7::search);
    connect(ui->btnReport, &QPushButton::clicked, this, &onb7::generateReportRequest);
    // Вызываем метод selectAll() через таймер с нулевой задержкой, чтобы он выполнился после инициализации интерфейса
    QTimer::singleShot(0, this, &onb7::selectAll);

    // Задаем количество столбцов в компоненте таблицы
    ui->tw->setColumnCount(7);

    // Задаем заголовки столбцов таблицы
    ui->tw->setHorizontalHeaderItem(0, new QTableWidgetItem("ID"));
    ui->tw->setHorizontalHeaderItem(1, new QTableWidgetItem("Номер"));
    ui->tw->setHorizontalHeaderItem(2, new QTableWidgetItem("Дата"));
    ui->tw->setHorizontalHeaderItem(3, new QTableWidgetItem("Выход"));
    ui->tw->setHorizontalHeaderItem(4, new QTableWidgetItem("Самолет"));
    ui->tw->setHorizontalHeaderItem(5, new QTableWidgetItem("Пункт Назначения"));
    ui->tw->setHorizontalHeaderItem(6, new QTableWidgetItem("Стойка"));

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

    // Заполнение выпадающих списков
    populate1ComboBox();
    populate2ComboBox();
    populate3ComboBox();
}

onb7::~onb7()
{
    delete ui;
}


void onb7::on_btnExit_clicked()
{
    // Создаем объект AuthenticationManager
    AuthenticationManager authenticationManager;

    // Получаем логин текущего пользователя через созданный объект
    QString currentUser = authenticationManager.getCurrentUserLogin();

    // Получаем список разрешенных кнопок для текущего пользователя
    QStringList allowedButtons = authenticationManager.getAllowedButtons(currentUser);


    close();

    // Создаем и отображаем новое окно главного меню с передачей списка разрешенных кнопок
    menuwindow *menuWindow = new menuwindow(allowedButtons);
    menuWindow->show();
}



void onb7::populate1ComboBox()
{
    ui->cmb1->clear();
    QSqlQuery query("SELECT aircraft_id, model FROM Aircraft ORDER BY model");
    while (query.next()) {
        QString pu1 = query.value("aircraft_id").toString();
        QString pu2 = query.value("model").toString();
        ui->cmb1->addItem(pu2, pu1);
    }
}

void onb7::populate2ComboBox()
{
    ui->cmb2->clear();
    QSqlQuery query("SELECT destination_id, city FROM Destination ORDER BY city");
    while (query.next()) {
        QString pu1 = query.value("destination_id").toString();
        QString pu2 = query.value("city").toString();
        ui->cmb2->addItem(pu2, pu1);
    }
}


void onb7::populate3ComboBox()
{
    ui->cmb3->clear();
    QSqlQuery query("SELECT counter_id, counter_number FROM Check_in_counter ORDER BY counter_number");
    while (query.next()) {
        QString pu1 = query.value("counter_id").toString();
        QString pu2 = query.value("counter_number").toString();
        ui->cmb3->addItem(pu2, pu1);
    }
}

void onb7::selectAll()
{
    // Проверяем, открыто ли соединение с базой данных
    if (!DatabaseHelper::isDatabaseConnected())
    {
        QMessageBox::critical(this, "Ошибка", "База данных не подключена.");
        return;
    }

    QSqlQuery query(DatabaseHelper::getDatabaseConnection());

    QString sqlstr = "SELECT flight_id, "
                     "number, "
                     "date, "
                     "gate, "
                     "(SELECT model FROM Aircraft WHERE aircraft_id = Flight.aircraft_id) AS aircr, "
                     "(SELECT city FROM Destination WHERE destination_id = Flight.destination_id) AS dest_city, "
                     "(SELECT counter_number FROM Check_in_counter WHERE counter_id = Flight.counter_id) AS count "
                     "FROM Flight "
                     "ORDER BY aircr, flight_id, dest_city, count";

    if (!query.exec(sqlstr))
    {
        QMessageBox::critical(this, "Ошибка", query.lastError().text());
        return;
    }


    ui->tw->clearContents();


    ui->tw->setRowCount(0);

    int rowCount = 0;


    while (query.next())
    {

        ui->tw->insertRow(rowCount);


        for (int col = 0; col < ui->tw->columnCount(); ++col)
        {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setData(Qt::DisplayRole, query.value(col));
            ui->tw->setItem(rowCount, col, item);
        }

        rowCount++;
    }


    ui->tw->resizeColumnsToContents();
}

void onb7::add()
{
    QString pu2 = ui->le2->text();
    QString pu3 = ui->le3->text();
    QString pu4 = ui->le4->text();
    QString cmb = ui->cmb1->currentData().toString();
    QString cmbb = ui->cmb2->currentData().toString();
    QString cmbbb = ui->cmb3->currentData().toString();

    // Проверяем, не используется ли выбранная стойка регистрации уже другим рейсом
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT flight_id FROM Flight WHERE counter_id = :counter_id");
    checkQuery.bindValue(":counter_id", cmbbb);
    if (checkQuery.exec() && checkQuery.next()) {
        // Стойка регистрации уже используется другим рейсом
        QMessageBox::critical(this, "Ошибка", "Выбранная стойка регистрации уже используется другим рейсом.");
        return;
    }

    // Подготавливаем SQL запрос для вставки данных
    QSqlQuery query;
    query.prepare("INSERT INTO Flight (number, date, gate, aircraft_id, destination_id, counter_id) "
                  "VALUES (:number, :date, :gate, :aircraft_id, :destination_id, :counter_id)");
    query.bindValue(":number", pu2);
    query.bindValue(":date", pu3);
    query.bindValue(":gate", pu4);
    query.bindValue(":aircraft_id", cmb);
    query.bindValue(":destination_id", cmbb);
    query.bindValue(":counter_id", cmbbb);

    // Выполняем SQL запрос для вставки данных
    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        ui->teResult->append("Error: " + query.lastError().text());
        return;
    }

    qDebug() << "Данные успешно добавлены в базу данных.";
    ui->teResult->append("Успех: данные успешно добавлены.");
    selectAll(); // Обновить таблицу, чтобы отобразить новые данные
}



void onb7::remove()
{
    // Проверяем, открыто ли соединение с базой данных
    if (!DatabaseHelper::isDatabaseConnected())
    {
        QMessageBox::critical(this, "Ошибка", "База данных не подключена.");
        return;
    }

    // Получаем текущую выбранную строку
    int curRow = ui->tw->currentRow();

    // Проверяем, что выбранная строка действительно существует
    if (curRow < 0)
    {
        QMessageBox::critical(this, "Ошибка", "Строка не выбрана!");
        return;
    }

    // Просим пользователя подтвердить удаление строки
    if (QMessageBox::question(this, "Удалить", "Удалить строку?", QMessageBox::Cancel, QMessageBox::Ok) == QMessageBox::Cancel)
        return;

    // Создаем объект запроса
    QSqlQuery query(DatabaseHelper::getDatabaseConnection());

    QString pu1 = ui->tw->item(curRow, 0)->text();

    // Подготавливаем строку запроса для удаления данных из таблицы
    QString sqlstr = "DELETE FROM Flight WHERE flight_id = :1";

    // Подготавливаем запрос к выполнению
    query.prepare(sqlstr);
    query.bindValue(":1", pu1);

    // Выполняем запрос
    if (!query.exec())
    {
        // Проверяем текст ошибки на наличие фразы "foreign key"
        if (query.lastError().text().contains("foreign key", Qt::CaseInsensitive))
        {
            QMessageBox::critical(this, "Ошибка", "Невозможно удалить, так как есть связанные записи.");
        }
        else
        {
            // Если запрос не выполнен по другой причине, выводим сообщение об ошибке
            QMessageBox::critical(this, "Ошибка", query.lastError().text());
        }
        return;
    }

    // Выводим сообщение об удалении строки
    ui->teResult->append(QString("Удалено %1 строк").arg(query.numRowsAffected()));

    // Обновляем содержимое компонента таблицы
    selectAll();
}

void onb7::edit()
{
    // Получить выбранные значения из выпадающих списков
    QString cmb = ui->cmb1->currentData().toString();
    QString cmbb = ui->cmb2->currentData().toString();
    QString cmbbb = ui->cmb3->currentData().toString();

    QString pu2 = ui->le2->text();
    QString pu3 = ui->le3->text();
    QString pu4 = ui->le4->text();

    // Получаем текущую выбранную строку
    int curRow = ui->tw->currentRow();

    // Проверяем, что строка действительно выбрана
    if (curRow < 0)
    {
        ui->teResult->append("Внимание: строка не выбрана!");
        return;
    }

    // Получаем ID рейса, который нужно обновить
    QString flightId = ui->tw->item(curRow, 0)->text();

    // Проверяем, не используется ли выбранная стойка регистрации уже другим рейсом
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT flight_id FROM Flight WHERE counter_id = :counter_id AND flight_id != :flight_id");
    checkQuery.bindValue(":counter_id", cmbbb);
    checkQuery.bindValue(":flight_id", flightId);
    if (checkQuery.exec() && checkQuery.next()) {
        // Стойка регистрации уже используется другим рейсом
        QMessageBox::critical(this, "Ошибка", "Выбранная стойка регистрации уже используется другим рейсом.");
        return;
    }

    // Подготавливаем SQL запрос для обновления данных рейса
    QSqlQuery query;
    query.prepare("UPDATE Flight SET number = :number, date = :date, gate = :gate, aircraft_id = :aircraft_id, destination_id = :destination_id, counter_id = :counter_id "
                  "WHERE flight_id = :flight_id");
    query.bindValue(":number", pu2);
    query.bindValue(":date", pu3);
    query.bindValue(":gate", pu4);
    query.bindValue(":aircraft_id", cmb);
    query.bindValue(":destination_id", cmbb);
    query.bindValue(":counter_id", cmbbb);
    query.bindValue(":flight_id", flightId);

    // Выполняем запрос на обновление данных рейса
    if (!query.exec()) {
        ui->teResult->append("Ошибка: " + query.lastError().text());
        return;
    }

    ui->teResult->append("Успех: данные успешно обновлены.");
    selectAll(); // Обновить таблицу после обновления
}


void onb7::search()
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
void onb7::generateReportRequest()
{
    // Вызываем функцию selectAll для получения данных о поставщиках из таблицы
    selectAll();

    // Создаем заголовок отчета
    QString reportText = "Отчет о Рейсах:\n\n";
    reportText += "Уважаемые коллеги,\n\n";
    reportText += "Подготовлен отчет о Рейсах:\n";

    // Добавляем информацию о каждом поставщике из таблицы
    for (int row = 0; row < ui->tw->rowCount(); ++row) {
        QString pu1 = ui->tw->item(row, 0)->text();
        QString pu2 = ui->tw->item(row, 1)->text();
        QString pu3 = ui->tw->item(row, 2)->text();
        QString pu4 = ui->tw->item(row, 3)->text();
        QString pu5 = ui->tw->item(row, 4)->text();
        QString pu6 = ui->tw->item(row, 5)->text();
        QString pu7 = ui->tw->item(row, 6)->text();


        reportText += "- ID " + pu1 + ", Номер " + pu2 + ", Дата " + pu3 + ", Выход " + pu4 + ", Самолет " + pu5 + ", Пункт Назначения " + pu6 + + ", Стойка " + pu7 + ";\n";
    }

    reportText += "\nС уважением,\n";
    reportText += "Ваше имя"; // Замените "Ваше имя" на ваше имя или название компании

    // Имя файла для сохранения отчета
    QString fileName = "/Users/ilinkonstantin/Desktop/Рейс_report.txt";

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << reportText;
        file.close();
        QMessageBox::information(this, "Успех", "Отчет о поставщиках успешно сохранен в файле " + fileName);
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать файл для сохранения отчета о поставщиках.");
    }
}


void onb7::on_tw_itemSelectionChanged()
{
    int curRow = ui->tw->currentRow();

    if (curRow < 0)
    {
        ui->cmb1->setCurrentIndex(0);
        ui->cmb2->setCurrentIndex(0);
        ui->cmb3->setCurrentIndex(0);

        ui->le1->clear();
        ui->le2->clear();
        ui->le3->clear();
        ui->le4->clear();

        return;
    }

    QString pu1 = ui->tw->item(curRow, 0)->text();
    QString pu2 = ui->tw->item(curRow, 1)->text();
    QString pu3 = ui->tw->item(curRow, 2)->text();
    QString pu4 = ui->tw->item(curRow, 3)->text();
    QString cmb = ui->tw->item(curRow, 4)->text();
    QString cmbb = ui->tw->item(curRow, 5)->text();
    QString cmbbb = ui->tw->item(curRow, 6)->text();


    ui->le1->setText(pu1);
    ui->le2->setText(pu2);
    ui->le3->setText(pu3);
    ui->le4->setText(pu4);
    ui->cmb1->setCurrentText(cmb);
    ui->cmb2->setCurrentText(cmbb);
    ui->cmb3->setCurrentText(cmbbb);


}


void onb7::clean()
{
    ui->cmb1->setCurrentIndex(0);
    ui->cmb2->setCurrentIndex(0);
    ui->cmb3->setCurrentIndex(0);

    ui->le1->clear();
    ui->le2->clear();
    ui->le3->clear();
    ui->le4->clear();

}

