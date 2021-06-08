#include "subjecttesting.h"
#include "ui_subjecttesting.h"

SubjectTesting::SubjectTesting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SubjectTesting)
{
    ui->setupUi(this);
}

SubjectTesting::~SubjectTesting()
{
    delete ui;
}

void SubjectTesting::on_buttonBox_accepted()
{
    bool fluency_lack = false;
    bool joggle = false;
    bool disturb = false;
    bool masic = false;
    if(ui->cbx_distorb->isChecked()){
        disturb = true;
    }
    if(ui->cbx_joggle->isChecked()){
        joggle = true;
    }
    if(ui->cbx_lackFluency->isChecked()){
        fluency_lack = true;
    }
    if(ui->cbx_masic->isChecked()){
        masic = true;
    }
    emit send_SubjectResu(fluency_lack,joggle,disturb,masic);
}
