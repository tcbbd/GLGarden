#ifndef SNOWDIALOG_H
#define SNOWDIALOG_H

#include <QDialog>

class Snow;

namespace Ui {
class SnowDialog;
}

class SnowDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SnowDialog(QWidget *parent = 0);
    SnowDialog(Snow *snow, QWidget *parent = 0);
    ~SnowDialog();

private slots:
    void on_change_wind_button_clicked();

    void on_snownum_slider_valueChanged(int value);

private:
    Ui::SnowDialog *ui;
    Snow *snow;
};

#endif // SNOWDIALOG_H
