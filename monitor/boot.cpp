#include "boot.h"
#include "ui_boot.h"

Boot::Boot(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Boot)
{
    ui->setupUi(this);
    this->setWindowTitle("导航页——列车微环境检测");

    //<设置背景图片
    this->setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(QPixmap("../monitor/background/boot.jpg"))); //
    this->setPalette(palette);
    //>

    //<设置实数数据按钮图标
    QIcon dataButton_icon;
    dataButton_icon.addFile("../monitor/icon/Line_chart.png");
    ui->pushButton_livedata->setIcon(dataButton_icon);
    //>
    //<设置数据查找按钮图标
    QIcon queryButton_icon;
    queryButton_icon.addFile("../monitor/icon/search.png");
    ui->pushButton_query->setIcon(queryButton_icon);
    //>
    //<设置数据分析按钮图标
    QIcon analysisButton_icon;
    analysisButton_icon.addFile("../monitor/icon/Pie_Graph.png");
    ui->pushButton_analysis->setIcon(analysisButton_icon);
    //>
    //<设置配置按钮图标;
    QIcon settingButton_icon;
    settingButton_icon.addFile("../monitor/icon/config_set.png");
    ui->pushButton_setting->setIcon(settingButton_icon);
    //>
}

Boot::~Boot()
{
    delete ui;
}

void Boot::on_pushButton_livedata_clicked()
{
    emit currentDataPage();
    this->close();
}

void Boot::on_pushButton_query_clicked()
{
    emit queryPage();
    this->close();
}

void Boot::on_pushButton_analysis_clicked()
{
    emit analysisPage();
    this->close();
}

void Boot::on_pushButton_setting_clicked()
{
    emit settingPage();
    this->close();
}
