#ifndef PATCHCONFIRMDIALOG_H
#define PATCHCONFIRMDIALOG_H

#include <QGraphicsView>
#include <QMainWindow>
#include <vector>
#include <Eigen/Dense>
#include <QImage>

using namespace Eigen;

namespace Ui {
class patchConfirmDialog;
}

class patchConfirmDialog : public QMainWindow {
    Q_OBJECT

public:
    explicit patchConfirmDialog(QWidget *parent = 0);

    QGraphicsScene *patchScene;
    QGraphicsView *patchView;
    QImage patchImg;

    std::vector<Vector3d> scnPoints;
    std::vector<Vector2d> texPoints;

    void popout();

    ~patchConfirmDialog();


protected:
    virtual void wheelEvent(QWheelEvent *event);

private:
    Ui::patchConfirmDialog *ui;

private slots:
    void savePatch();
    void cancel();
};

#endif // PATCHCONFIRMDIALOG_H
