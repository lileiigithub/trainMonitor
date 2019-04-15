#ifndef BOOT_H
#define BOOT_H

#include <QMainWindow>

namespace Ui {
class Boot;
}

class Boot : public QMainWindow
{
    Q_OBJECT

public:
    explicit Boot(QWidget *parent = 0);
    ~Boot();
signals:
    void currentDataPage();
    void queryPage();
    void analysisPage();
    void settingPage();

private slots:
    void on_pushButton_livedata_clicked();

    void on_pushButton_query_clicked();

    void on_pushButton_analysis_clicked();

    void on_pushButton_setting_clicked();

private:
    Ui::Boot *ui;
};

#endif // BOOT_H
