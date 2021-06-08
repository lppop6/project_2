#ifndef DELAYTESTING_H
#define DELAYTESTING_H

#include <QDialog>
#include <QDebug>
#include <QMessageBox>

namespace Ui {
class delayTesting;
}

class delayTesting : public QDialog
{
    Q_OBJECT

public:
    explicit delayTesting(QWidget *parent = 0);
    ~delayTesting();
signals:
    void send_DelayResult(int);
public slots:

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::delayTesting *ui;
};

#endif // DELAYTESTING_H
