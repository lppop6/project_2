#ifndef INPUTGRAYSCALERESULT_H
#define INPUTGRAYSCALERESULT_H

#include <QDialog>

namespace Ui {
class InputGrayScaleResult;
}

class InputGrayScaleResult : public QDialog
{
    Q_OBJECT

public:
    explicit InputGrayScaleResult(QWidget *parent = 0);
    ~InputGrayScaleResult();

private slots:
    void on_buttonBox_accepted();
signals:
    void sendGrayScaleResult(double);

private:
    Ui::InputGrayScaleResult *ui;
};

#endif // INPUTGRAYSCALERESULT_H
