#ifndef TREEDIALOG_H
#define TREEDIALOG_H

#include "tree.h"

#include <QDialog>

class Tree;

namespace Ui {
class TreeDialog;
}

class TreeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TreeDialog(QWidget *parent = 0);
    ~TreeDialog();
    void setTree(Tree *tree, bool exist);
    bool isExist();
    void recover();

signals:

private slots:
    void on_base_radius_slider_valueChanged(int value);

    void on_height_slider_valueChanged(int value);

    void on_theta_slider_valueChanged(int value);

    void on_phi_slider_valueChanged(int value);

    void on_leaf_Rslider_valueChanged(int value);

    void on_leaf_Gslider_valueChanged(int value);

    void on_leaf_Bslider_valueChanged(int value);

    void on_leaf_size_slider_valueChanged(int value);

    void on_leaf_width_slider_valueChanged(int value);

private:
    Ui::TreeDialog *ui;
    Tree *tree;
    bool exist;

    TreeInfo info;
};

#endif // TREEDIALOG_H
