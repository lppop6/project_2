#ifndef INPUTYOURNAME_H
#define INPUTYOURNAME_H

#include <QDialog>

namespace Ui {
class inputYourName;
}

class inputYourName : public QDialog
{
    Q_OBJECT

public:
    explicit inputYourName(QWidget *parent = nullptr);
    ~inputYourName();
signals:
    void send_testingName(QString,QString,QString);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::inputYourName *ui;
};

#endif // INPUTYOURNAME_H
