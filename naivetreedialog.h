#ifndef NAIVETREEDIALOG_H
#define NAIVETREEDIALOG_H

#include "naivetree.h"

#include <QDialog>

namespace Ui {
class NaiveTreeDialog;
}

class NaiveTreeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NaiveTreeDialog(QWidget *parent = 0);
    ~NaiveTreeDialog();
    void setTree(NaiveTree *tree, bool exist);
    bool isExist();
    void recover();

private slots:
    void on_rootR_slider_valueChanged(int value);

    void on_rootG_slider_valueChanged(int value);

    void on_rootB_slider_valueChanged(int value);

    void on_leafR_slider_valueChanged(int value);

    void on_leafG_slider_valueChanged(int value);

    void on_leafB_slider_valueChanged(int value);

    void on_base_radius_slider_valueChanged(int value);

    void on_height_slider_valueChanged(int value);

    void on_leaf_size_slider_valueChanged(int value);

private:
    Ui::NaiveTreeDialog *ui;
    NaiveTree *tree;
    bool exist;

    NaiveTreeInfo info;
};

#endif // NAIVETREEDIALOG_H
