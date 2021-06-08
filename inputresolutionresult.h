#ifndef INPUTRESOLUTIONRESULT_H
#define INPUTRESOLUTIONRESULT_H

#include <QDialog>
#include <QDebug>

namespace Ui {
class InputResolutionResult;
}

class InputResolutionResult : public QDialog
{
    Q_OBJECT

public:
    explicit InputResolutionResult(QWidget *parent = 0);
    ~InputResolutionResult();

signals:
    void sendResolutionResult(double);
private slots:
    void on_buttonBox_accepted();

private:
    Ui::InputResolutionResult *ui;
};

#endif // INPUTRESOLUTIONRESULT_H
