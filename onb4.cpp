#include "onb4.h"
#include "ui_onb4.h"
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
onb4::onb4(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::onb4)
{
    ui->setupUi(this);


    // Подключение сигналов к слотам
    connect(ui->btn1, SIGNAL(clicked(bool)), this, SLOT(add()));
    connect(ui->btn2, SIGNAL(clicked(bool)), this, SLOT(edit()));
    connect(ui->btn3, SIGNAL(clicked(bool)), this, SLOT(remove()));
    connect(ui->btn4, SIGNAL(clicked(bool)), this, SLOT(clean()));
    connect(ui->btnSearch, &QPushButton::clicked, this, &onb4::search);
    connect(ui->btnReport, &QPushButton::clicked, this, &onb4::generateReportRequest);
    // Вызываем метод selectAll() через таймер с нулевой задержкой, чтобы он выполнился после инициализации интерфейса
    QTimer::singleShot(0, this, &onb4::selectAll);

    // Задаем количество столбцов в компоненте таблицы
    ui->tw->setColumnCount(6);

    // Задаем заголовки столбцов таблицы
    ui->tw->setHorizontalHeaderItem(0, new QTableWidgetItem("ID"));
    ui->tw->setHorizontalHeaderItem(1, new QTableWidgetItem("Дата"));
    ui->tw->setHorizontalHeaderItem(2, new QTableWidgetItem("Рейс"));
    ui->tw->setHorizontalHeaderItem(3, new QTableWidgetItem("Багаж"));
    ui->tw->setHorizontalHeaderItem(4, new QTableWidgetItem("Выход"));
    ui->tw->setHorizontalHeaderItem(5, new QTableWidgetItem("Пасажир"));



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
}

onb4::~onb4()
{
    delete ui;
}


void onb4::on_btnExit_clicked()
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



void onb4::populate1ComboBox()
{
    ui->cmb1->clear();
    QSqlQuery query("SELECT number, flight_id FROM Flight ORDER BY number");
    while (query.next()) {
        QString pu1 = query.value("flight_id").toString();
        QString pu2 = query.value("number").toString();
        ui->cmb1->addItem(pu2, pu1);
    }
}

void onb4::populate2ComboBox()
{
    ui->cmb2->clear();
    QSqlQuery query("SELECT passenger_id, full_name FROM Passenger ORDER BY full_name");
    while (query.next()) {
        QString pu1 = query.value("passenger_id").toString();
        QString pu2 = query.value("full_name").toString();
        ui->cmb2->addItem(pu2, pu1);
    }
}

void onb4::selectAll()
{
    // Проверяем, открыто ли соединение с базой данных
    if (!DatabaseHelper::isDatabaseConnected())
    {
        QMessageBox::critical(this, "Ошибка", "База данных не подключена.");
        return;
    }

    QSqlQuery query(DatabaseHelper::getDatabaseConnection());

    QString sqlstr = "SELECT ticket_id, "
                     "date, "
                     "(SELECT number FROM Flight WHERE flight_id = Ticket.flight_id) AS flight_id, "
                     "luggage, "
                     "gate, "
                     "(SELECT full_name FROM Passenger WHERE passenger_id = Ticket.passenger_id) AS pass_name "
                     "FROM Ticket "
                     "ORDER BY ticket_id, flight_id, pass_name";

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
void onb4::add()
{
    QString pu2 = ui->le2->text();
    QString cmb = ui->cmb1->currentData().toString();
    QString pu3 = ui->le3->text();
    QString pu4 = ui->le4->text();
    QString cmbb = ui->cmb2->currentData().toString();

    // Подготавливаем SQL запрос для вставки данных
    QSqlQuery query;
    query.prepare("INSERT INTO Ticket (date, flight_id, luggage, gate, passenger_id) "
                  "VALUES (:1, :2, :3, :4, :5)");

    // Привязываем значения к параметрам запроса
    query.bindValue(":1", pu2);
    query.bindValue(":2", cmb);
    query.bindValue(":3", pu3);
    query.bindValue(":4", pu4);
    query.bindValue(":5", cmbb);

    // Выполняем SQL запрос для вставки данных
    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
        ui->teResult->append("Error: " + query.lastError().text());
        return;
    }

    qDebug() << "Данные успешно добавлены в базу данных.";

    ui->teResult->append("Успех: данные успешно добавлены.");

    // После успешного добавления билета, генерируем отчет по этому билету
    generateReportRequest(query.lastInsertId().toInt()); // Передаем ID только что добавленного билета
    selectAll(); // Обновить таблицу, чтобы отобразить новые данные
}
void onb4::remove()
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
    QString sqlstr = "DELETE FROM Ticket WHERE ticket_id = :1";

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

void onb4::edit()
{
    // Получить выбранные значения из выпадающих списков
    QString cmb = ui->cmb1->currentData().toString();
    QString cmbb = ui->cmb2->currentData().toString();
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

    // Получаем ID организации
    QString pupu = ui->tw->item(curRow, 0)->text();

    // Подготавливаем SQL запрос для обновления данных
    QSqlQuery query;
    query.prepare("UPDATE Ticket SET date = :1, flight_id = :2, luggage = :3, gate = :4, passenger_id = :5 "
                  "WHERE ticket_id = :6");
    query.bindValue(":1", pu2);
    query.bindValue(":2", cmb);
    query.bindValue(":3", pu3);
    query.bindValue(":4", pu4);
    query.bindValue(":5", cmbb);
    query.bindValue(":6", pupu);

    // Выполняем запрос
    if (!query.exec()) {
        ui->teResult->append("Ошибка: " + query.lastError().text());
        return;
    }

    ui->teResult->append("Успех: данные успешно обновлены.");
    selectAll(); // Обновить таблицу после обновления
}


void onb4::search()
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

void onb4::generateReportRequest(int ticketId)
{
    // Формируем текст отчета на основе данных о билете
    QString reportText = "Отчет о билете:\n\n";
    reportText += "ID билета: " + QString::number(ticketId) + "\n";
    reportText += "Дата: " + ui->le2->text() + "\n";
    reportText += "Рейс: " + ui->cmb1->currentText() + "\n";
    reportText += "Багаж: " + ui->le3->text() + "\n";
    reportText += "Выход: " + ui->le4->text() + "\n";
    reportText += "Пассажир: " + ui->cmb2->currentText() + "\n\n";
    reportText += "С уважением,\n";
    reportText += "Ваше имя"; // Замените "Ваше имя" на ваше имя или название компании

    // Имя файла для сохранения отчета
    QString fileName = "/Users/ilinkonstantin/Desktop/отчет_о_билете_" + QString::number(ticketId) + ".txt";

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << reportText;
        file.close();
        QMessageBox::information(this, "Успех", "Отчет о билете успешно сохранен в файле " + fileName);
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать файл для сохранения отчета о билете.");
    }
}

void onb4::on_tw_itemSelectionChanged()
{
    int curRow = ui->tw->currentRow();

    if (curRow < 0)
    {
        ui->cmb1->setCurrentIndex(0);
        ui->cmb2->setCurrentIndex(0);
        ui->le1->clear();
        ui->le2->clear();
        ui->le3->clear();
        ui->le4->clear();

        return;
    }

    QString pu1 = ui->tw->item(curRow, 0)->text();
    QString pu2 = ui->tw->item(curRow, 1)->text();
    QString cmb = ui->tw->item(curRow, 2)->text();
    QString pu3 = ui->tw->item(curRow, 3)->text();
    QString pu4 = ui->tw->item(curRow, 4)->text();
    QString cmbb = ui->tw->item(curRow, 5)->text();

    ui->le1->setText(pu1);
    ui->le2->setText(pu2);
    ui->cmb1->setCurrentText(cmb);
    ui->le3->setText(pu3);
    ui->le4->setText(pu4);
    ui->cmb2->setCurrentText(cmbb);

}


void onb4::clean()
{
    ui->cmb1->setCurrentIndex(0);
    ui->cmb2->setCurrentIndex(0);
    ui->le1->clear();
    ui->le2->clear();
    ui->le3->clear();
    ui->le4->clear();

}

