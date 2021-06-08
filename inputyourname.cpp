#include "inputyourname.h"
#include "ui_inputyourname.h"

inputYourName::inputYourName(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::inputYourName)
{
    ui->setupUi(this);
}

inputYourName::~inputYourName()
{
    delete ui;
}

void inputYourName::on_buttonBox_accepted()
{
    QString s1 = ui->let_model->text();
    QString s2 = ui->let_Tester->text();
    QString s3 = ui->let_beTesting->text();
    emit send_testingName(s3,s2,s1);
}
