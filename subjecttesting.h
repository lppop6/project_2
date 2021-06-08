#ifndef SUBJECTTESTING_H
#define SUBJECTTESTING_H

#include <QDialog>

namespace Ui {
class SubjectTesting;
}

class SubjectTesting : public QDialog
{
    Q_OBJECT

public:
    explicit SubjectTesting(QWidget *parent = 0);
    ~SubjectTesting();
signals:
    void send_SubjectResu(bool,bool,bool,bool);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::SubjectTesting *ui;
};

#endif // SUBJECTTESTING_H
