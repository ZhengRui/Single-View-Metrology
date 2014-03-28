#ifndef SVMGRAPHICSVIEW_H
#define SVMGRAPHICSVIEW_H

#include <QGraphicsView>
#include <vector>
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <Eigen/LU>
#include "patchconfirmdialog.h"

using namespace Eigen;

struct imgPoint {
    double x;
    double y;
    double w;
};

struct imgLine {
    imgPoint p0;
    imgPoint p1;
};

struct scnPoint {
    double x;
    double y;
    double z;
    double w;
};

struct connPoint {
    imgPoint imgPt;
    scnPoint scnPt;
//    double** homoMatrix;
    Matrix3d homoMatrixEig3;
};


enum stateSign {_START, _XLines, _YLines, _ZLines, _RPlane, _RHeight, _PtPool, _Polygon};


class SVMGraphicsView : public QGraphicsView
{

public:
    SVMGraphicsView(QWidget *parent = 0);
    void openAct();
    void rescaleAct();
    void pickXLines();
    void pickYLines();
    void pickZLines();
    bool isLinesReady();
    void pickRefPlane();
    void pickRefHeightPoint();
    void pickPolygon();


    void calXVP();
    void calYVP();
    void calZVP();
    void calHomography_Public();
    void calGamma();
    void calPatch();
    void scnMapToImg();

    void make3DPtPool();
    void saveModel();


protected:
    virtual void wheelEvent(QWheelEvent* event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);

private:
    bool _pan, _hasimage, _xVP_exist, _yVP_exist, _zVP_exist, _refHomo_exist, _getGammaZ;
    int _panStartX, _panStartY, _extraRPt, _method, _textureMethod;
    double gap;
    stateSign state;

    std::vector<imgLine> XLines, YLines, ZLines;
    std::vector<connPoint> RPoints;
    std::vector<connPoint> RHpoints;
    std::vector<connPoint> PtPool;
    std::vector<connPoint> PolyPool;
    connPoint RHpt_cache;

    QGraphicsScene* Scene;
    QImage image;

    imgPoint* pointCache;
    connPoint* poolCache;

    imgPoint* xVP;
    imgPoint* yVP;
    imgPoint* zVP;
    Matrix3d refHomoI2S;
    Matrix3d refHomoS2I;
    Matrix<double, 3, 4> Proj;
    double gammaZ;

    patchConfirmDialog* patchConfirmDlg;

    void pushLines(std::vector<imgLine> &lineContainer, double x, double y);
    void pushRefPoints(double x, double y);
    bool popRefInDialog(double x, double y);
    void popMoveDialog();
    void popIfLoadVPDialog();
    void pushPtPool(double x, double y);

    void pushPolyPool(double x, double y);
    void reWindDel(std::vector<imgLine> &lineConatiner);

    imgPoint* calVP(std::vector<imgLine> &lineContainer);
    imgPoint* crossProduct(imgPoint p1, imgPoint p2);
    void normaLize(imgPoint* pt);
    void homoFy(imgPoint* pt);

    void eigenJacobianTest();
    void eigen3Test();
    double* luSolver(double** A, double *b, int n);

    Vector3d rotate(double x, double y, double z, Vector3d R, double theta);

    void calHomography(std::vector<connPoint> &connPtContainer);
    void calHomographyEig3(std::vector<connPoint> &connPtContainer);
    bool planeCheck(std::vector<connPoint> &connPtContainer);
    void makePatch();
    Vector3i bilinear(QImage image, double x, double y);

    Vector3d imgPointToVector3d(imgPoint imgPt);
    Vector3d scnPointToVector3d(scnPoint scnPt);

    void drawPoint(double x, double y, int size, const QPen & pen, const QBrush & brush);
    void drawScene();

    void svmInit();



};

#endif // SVMGRAPHICSVIEW_H
