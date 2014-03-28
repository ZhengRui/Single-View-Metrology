#include "svmgraphicsview.h"

#include <QtWidgets>
#include <QFileDialog>
#include <iostream>
#include <cmath>
#include "eigen.h"


using namespace std;
using namespace Eigen;

SVMGraphicsView::SVMGraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    Scene = new QGraphicsScene(this);
    Scene->setBackgroundBrush(QBrush(Qt::darkGray, Qt::SolidPattern));
    setScene(Scene);

    setDragMode(ScrollHandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    viewport()->setCursor(Qt::ArrowCursor);
    _pan = false;
    _hasimage = false;

}

void SVMGraphicsView::wheelEvent(QWheelEvent *event)
{
    double scaleFactor = 1.15;
    if(event->delta() > 0)
    {
        scale(scaleFactor, scaleFactor);
    } else {
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}

void SVMGraphicsView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        _pan = true;
        _panStartX = event->x();
        _panStartY = event->y();
        viewport()->setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    } else if (event->button() == Qt::LeftButton)
    {
        if (_hasimage)
        {
            QPointF imgCursorPos = mapToScene(event->pos());
            double x = imgCursorPos.x();
            double y = imgCursorPos.y();

            QPoint imgPix = imgCursorPos.toPoint();
            if (imgPix.x() >=0 && imgPix.x() < image.size().width() && imgPix.y() >=0 && imgPix.y() < image.size().height())
            {

                if (QApplication::keyboardModifiers() && Qt::ControlModifier)
                {
                    QGraphicsItem* itemAtCur = Scene->itemAt(imgCursorPos, QGraphicsView::transform());
                    if (itemAtCur->type() == QGraphicsEllipseItem::Type)
                    {
                        x = itemAtCur->boundingRect().center().x();
                        y = itemAtCur->boundingRect().center().y();
                    }
                //    cout << itemAtCur->type() << endl;
                }

                switch(state)
                {
                case _START:
                    cout << "Not Start Yet ?" << endl;
                    break;
                case _XLines:
                    pushLines(XLines, x, y);
                    break;
                case _YLines:
                    pushLines(YLines, x, y);
                    break;
                case _ZLines:
                    pushLines(ZLines, x, y);
                    break;
                case _RPlane:
                    pushRefPoints(x, y);
                    break;
                case _RHeight:
                    if (RHpoints.size() < 2)
                        pushRefPoints(x, y);
                    break;
                case _Polygon:
                    pushPolyPool(x, y);
                    break;
                case _PtPool:
                    pushPtPool(x, y);
                    break;
                default:
                    cout << "Already Finished ?" << endl;
                    break;
                }
            }
        }

    }
    event->ignore();
}

void SVMGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        _pan = false;
        viewport()->setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }
    event->ignore();
}

void SVMGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if (_pan)
    {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (event->x() - _panStartX));
        verticalScrollBar()->setValue(verticalScrollBar()->value() - (event->y() - _panStartY));
        _panStartX = event->x();
        _panStartY = event->y();
        event->accept();
        return;
    }

    QGraphicsView::mouseMoveEvent(event);

    if (_hasimage)
    {
        QPointF imgCursorPos = mapToScene(event->pos());
        QPoint imgPix = imgCursorPos.toPoint();
        if(pointCache != NULL)
        {
            drawScene();
            if (imgPix.x() >=0 && imgPix.x() < image.size().width() && imgPix.y() >=0 && imgPix.y() < image.size().height())
            {
                double endx = pointCache->x + 0.9 * (imgCursorPos.x() - pointCache->x);
                double endy = pointCache->y + 0.9 * (imgCursorPos.y() - pointCache->y);
                Scene->addLine(pointCache->x, pointCache->y, endx, endy, QPen(Qt::yellow, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            }
        }
//        cout << imgCursorPos.x() << ", " << imgCursorPos.y() << endl;

        if(_zVP_exist && (state == _RHeight || state == _PtPool) && imgPix.x() >=0 && imgPix.x() < image.size().width() && imgPix.y() >=0 && imgPix.y() < image.size().height())
        {
            if(pointCache == NULL)
                drawScene();

            double endx = zVP->x + (1-gap) * (imgCursorPos.x() - zVP->x);
            double endy = zVP->y + (1-gap) * (imgCursorPos.y() - zVP->y);
            Scene->addLine(zVP->x, zVP->y, endx, endy, QPen(Qt::white, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            endx = zVP->x + (1+gap) * (imgCursorPos.x() - zVP->x);
            endy = zVP->y + (1+gap) * (imgCursorPos.y() - zVP->y);
            double endx2 = zVP->x + 1.1 * (imgCursorPos.x() - zVP->x);
            double endy2 = zVP->y + 1.1 * (imgCursorPos.y() - zVP->y);
            Scene->addLine(endx, endy, endx2, endy2, QPen(Qt::white, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        }
    }
    event->ignore();
}

void SVMGraphicsView::svmInit()
{
    XLines.clear();
    YLines.clear();
    ZLines.clear();
    RPoints.clear();
    RHpoints.clear();
    PtPool.clear();
    PolyPool.clear();
    Scene->clear();
    state = _START;
    pointCache = NULL;
    poolCache = NULL;
    xVP = NULL;
    yVP = NULL;
    zVP = NULL;
    _xVP_exist = false;
    _yVP_exist = false;
    _zVP_exist = false;
    _refHomo_exist = false;
    _getGammaZ = false;
    _method = 0;
    _textureMethod = 0;
    gap = 0.005;
    refHomoI2S << 0,0,0,
                  0,0,0,
                  0,0,0;
    refHomoS2I << 0,0,0,
                  0,0,0,
                  0,0,0;
    Proj << 0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0;

    patchConfirmDlg = new patchConfirmDialog(this);
}

void SVMGraphicsView::openAct()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());

    if (!fileName.isEmpty())
    {
        image = QImage(fileName);
        if (image.isNull()) {
            QMessageBox::information(this, tr("Single View Modeling"), tr("Cannot load %1.").arg(fileName));
            return;
        }
        svmInit();
        Scene->addPixmap(QPixmap::fromImage(image));
        resetTransform();
        horizontalScrollBar()->setValue(0);
        verticalScrollBar()->setValue(0);
        _pan = false;
        _hasimage = true;
    }
}

void SVMGraphicsView::rescaleAct()
{
    if (_hasimage)
    {
        setTransform(QTransform::fromScale(1, 1));
    }
}

void SVMGraphicsView::pickXLines()
{
    if (_hasimage)
    {
        cout << "Please pick up endpoints of lines pointing in X direction" << endl;
        state = _XLines;
        if (pointCache != NULL)
        {
            pointCache = NULL;
            drawScene();
        }
    } else {
        QMessageBox::warning(this, tr("Warning"), tr("Please load a picture first !"));
    }
}

void SVMGraphicsView::pickYLines()
{
    if (_hasimage)
    {
        cout << "Please pick up endpoints of lines pointing in Y direction" << endl;
        state = _YLines;
        if (pointCache != NULL)
        {
            pointCache = NULL;
            drawScene();
        }
    } else {
        QMessageBox::warning(this, tr("Warning"), tr("Please load a picture first !"));
    }
}

void SVMGraphicsView::pickZLines()
{
    if (_hasimage)
    {
        cout << "Please pick up endpoints of lines pointing in Z direction" << endl;
        state = _ZLines;
        if (pointCache != NULL)
        {
            pointCache = NULL;
            drawScene();
        }
    } else {
        QMessageBox::warning(this, tr("Warning"), tr("Please load a picture first !"));
    }
}

void SVMGraphicsView::pickRefPlane()
{
    if (_hasimage)
    {
        pointCache = NULL;
        drawScene();
        //    cout << "Please pick up at least 2 points P1 and P2 on Z=0 plane which makes Vx, P1, P2 non-colinear and Vy, P1, P2 non-colinear" << endl;
        cout << "Please specify 4 or more points on Z=0 plane (including vanishing points on this plane's vanishing line if they can be calculated and " \
             << "will be taken into account) and make sure these points could form a polygon" << endl;
        state = _RPlane;
    } else {
        QMessageBox::warning(this, tr("Warning"), tr("Please load a picture first !"));
    }
}

void SVMGraphicsView::pickRefHeightPoint()
{
    if (_hasimage)
    {
        pointCache = NULL;
        drawScene();
        cout << "Please specify an off-plane reference point ( Z!=0 ) and give its 3D coordinates: " << endl;
        state = _RHeight;
    } else {
        QMessageBox::warning(this, tr("Warning"), tr("Please load a picture first !"));
    }
}

bool SVMGraphicsView::isLinesReady()
{
    if (_hasimage)
    {
        if (XLines.size() < 2)
        {
            QMessageBox::warning(this, tr("Warning"), tr("Please specify more lines in X direction so i can calculate X vanish point :)"));
        } else if (YLines.size() < 2) {
            QMessageBox::warning(this, tr("Warning"), tr("Please specify more lines in Y direction so i can calculate Y vanish point :)"));
        } else if (ZLines.size() < 2) {
            QMessageBox::warning(this, tr("Warning"), tr("Please specify more lines in Z direction so i can calculate Z vanish point :)"));
        } else {
            return true;
        }
    }
    return false;
}

void SVMGraphicsView::pushLines(std::vector<imgLine> &lineContainer, double x, double y)
{
    if (pointCache == NULL)
    {
        pointCache = new imgPoint;
        pointCache->x = x;
        pointCache->y = y;
        pointCache->w = 1;
    } else {
        imgLine newLine;
        newLine.p0 = *pointCache;
        newLine.p1.x = x;
        newLine.p1.y = y;
        newLine.p1.w = 1;
        lineContainer.push_back(newLine);
        pointCache = NULL;
    }
    drawScene();
}

void SVMGraphicsView::pushRefPoints(double x, double y)
{
    QGraphicsEllipseItem* cursorRPT;
    if (state == _RPlane)
        cursorRPT = Scene->addEllipse(x-3, y-3, 6, 6, QPen(Qt::black), QBrush(Qt::black, Qt::SolidPattern));
    else
        cursorRPT = Scene->addEllipse(x-3, y-3, 6, 6, QPen(QColor(163, 3, 168,255)), QBrush(QColor(163, 3, 168,255), Qt::SolidPattern));

    if(popRefInDialog(x, y))
    {
        drawScene();      
        if (_refHomo_exist && RHpoints.size()>=1)
        {
            Vector3d dualRHpt = refHomoS2I * scnPointToVector3d(RHpt_cache.scnPt);
            dualRHpt /= dualRHpt(2);

            QGraphicsEllipseItem* cursorRPT;
            cursorRPT = Scene->addEllipse(dualRHpt(0)-3, dualRHpt(1)-3, 6, 6, QPen(QColor(163, 3, 168,255)), QBrush(QColor(163, 3, 168,255), Qt::SolidPattern));
            QGraphicsOpacityEffect* dual_Eff = new QGraphicsOpacityEffect(this);
            dual_Eff->setOpacity(0.5);
            cursorRPT->setGraphicsEffect(dual_Eff);
        }
    } else {
        Scene->removeItem(cursorRPT);
    }
}

bool SVMGraphicsView::popRefInDialog(double x, double y)
{
    QDialog dialog(this);
    // Use a layout allowing to have a label next to each field
    QFormLayout form(&dialog);

    // Add some text above the fields
    form.addRow(new QLabel("Please input the 3D coordinates:"));

    // Add the lineEdits with their respective labels
    QList<QLineEdit *> fields;
    for(int i = 0; i < 3; ++i) {
        QLineEdit *lineEdit = new QLineEdit(&dialog);
        QString label;
        switch(i)
        {
            case 0:
                label = QString("X: ");
                lineEdit->setValidator(new QDoubleValidator());
                break;
            case 1:
                label = QString("Y: ");
                lineEdit->setValidator(new QDoubleValidator());
                break;
            default:
                label = QString("Z: ");
                if (state == _RPlane)
                {
                    lineEdit->setText("0");
                    lineEdit->setReadOnly(true);
                    lineEdit->setStyleSheet(QString( "background-color: gray"));
                } else {
                    lineEdit->setValidator(new QDoubleValidator());
                }
                break;
        }
        form.addRow(label, lineEdit);
        fields << lineEdit;
    }

    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // Show the dialog as modal
    if (dialog.exec() == QDialog::Accepted) {

        connPoint newRefPt;
        newRefPt.imgPt.w = 1;
        newRefPt.imgPt.x = x;
        newRefPt.imgPt.y = y;
        newRefPt.scnPt.w = 1;
        newRefPt.scnPt.z = 0;
 //       newRefPt.homoMatrix = NULL;
        newRefPt.homoMatrixEig3 << 0, 0, 0,
                                   0, 0, 0,
                                   0, 0, 0;

        int i = 0;
        // If the user didn't dismiss the dialog, do something with the fields
        foreach(QLineEdit * lineEdit, fields) {
            switch(i)
            {
                case 0:
                    newRefPt.scnPt.x = lineEdit->text().toDouble();
                    i++;
                    break;
                case 1:
                    newRefPt.scnPt.y = lineEdit->text().toDouble();
                    i++;
                    break;
                case 2:
                    newRefPt.scnPt.z = lineEdit->text().toDouble();
                    i++;
                    break;
                default:
                    break;
            }
        }

        switch(state)
        {
            case _RPlane:
                RPoints.push_back(newRefPt);
                break;
            case _RHeight:
                RHpt_cache = newRefPt;
                RHpoints.push_back(RHpt_cache);
                break;
            default:
                break;
        }

        return true;
    }
    return false;
}

void SVMGraphicsView::drawPoint(double x, double y, int size, const QPen & pen, const QBrush & brush)
{
    QGraphicsEllipseItem* pt = new QGraphicsEllipseItem;
    pt->setPen(pen);
    pt->setBrush(brush);
    pt->setRect(x-size/2, y-size/2, size, size);
    Scene->addItem(pt);
}



void SVMGraphicsView::drawScene()
{
    Scene->clear();
    Scene->addPixmap(QPixmap::fromImage(image));

    std::vector<imgLine>::iterator iterLINE;
    for(iterLINE = XLines.begin(); iterLINE != XLines.end(); iterLINE++)
    {
        imgLine line = *iterLINE;

        Scene->addLine(line.p0.x, line.p0.y, line.p1.x, line.p1.y, QPen(Qt::red, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        drawPoint(line.p0.x, line.p0.y, 4, QPen(Qt::red), QBrush(Qt::red, Qt::SolidPattern));
        drawPoint(line.p1.x, line.p1.y, 4, QPen(Qt::red), QBrush(Qt::red, Qt::SolidPattern));

    }

    for(iterLINE = YLines.begin(); iterLINE != YLines.end(); iterLINE++)
    {
        imgLine line = *iterLINE;

        Scene->addLine(line.p0.x, line.p0.y, line.p1.x, line.p1.y, QPen(Qt::green, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        drawPoint(line.p0.x, line.p0.y, 4, QPen(Qt::green), QBrush(Qt::green, Qt::SolidPattern));
        drawPoint(line.p1.x, line.p1.y, 4, QPen(Qt::green), QBrush(Qt::green, Qt::SolidPattern));

    }

    for(iterLINE = ZLines.begin(); iterLINE != ZLines.end(); iterLINE++)
    {
        imgLine line = *iterLINE;

        Scene->addLine(line.p0.x, line.p0.y, line.p1.x, line.p1.y, QPen(Qt::blue, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        drawPoint(line.p0.x, line.p0.y, 4, QPen(Qt::blue), QBrush(Qt::blue, Qt::SolidPattern));
        drawPoint(line.p1.x, line.p1.y, 4, QPen(Qt::blue), QBrush(Qt::blue, Qt::SolidPattern));

    }

    std::vector<connPoint>::iterator iterRPLANE;
    for(iterRPLANE = RPoints.begin(); iterRPLANE != RPoints.end(); iterRPLANE++)
    {
        connPoint connPt = *iterRPLANE;

        drawPoint(connPt.imgPt.x, connPt.imgPt.y, 6, QPen(Qt::black), QBrush(Qt::black, Qt::SolidPattern));
    }

    if (pointCache != NULL)
    {   
        drawPoint(pointCache->x, pointCache->y, 4, QPen(Qt::magenta), QBrush(Qt::magenta, Qt::SolidPattern));
    }

    if (xVP != NULL)
    {
        drawPoint(xVP->x, xVP->y, 4, QPen(QColor(205, 79, 86,255)), QBrush(QColor(205, 79, 86,255), Qt::SolidPattern));
    }

    if (yVP != NULL)
    {
        drawPoint(yVP->x, yVP->y, 4, QPen(QColor(205, 79, 86,255)), QBrush(QColor(205, 79, 86,255), Qt::SolidPattern));
    }

    if (zVP != NULL)
    {
        drawPoint(zVP->x, zVP->y, 4, QPen(QColor(205, 79, 86,255)), QBrush(QColor(205, 79, 86,255), Qt::SolidPattern));
    }

    std::vector<connPoint>::iterator iterRHpt;
    for(iterRHpt = RHpoints.begin(); iterRHpt != RHpoints.end(); iterRHpt++)
    {
        connPoint connPt = *iterRHpt;
        drawPoint(connPt.imgPt.x, connPt.imgPt.y, 6, QPen(QColor(163, 3, 168,255)), QBrush(QColor(163, 3, 168,255), Qt::SolidPattern));
    }

    std::vector<connPoint>::iterator iterPoolpt;
    for(iterPoolpt = PtPool.begin(); iterPoolpt != PtPool.end(); iterPoolpt++)
    {
        connPoint connPt = *iterPoolpt;
        drawPoint(connPt.imgPt.x, connPt.imgPt.y, 6, QPen(QColor(5, 179, 250,255)), QBrush(QColor(5, 179, 250,255), Qt::SolidPattern));
    }

    if (state == _Polygon)
    {
        for(iterPoolpt =  PolyPool.begin(); iterPoolpt != PolyPool.end(); iterPoolpt++)
        {
            QGraphicsEllipseItem* polyPt;
            connPoint connPt = *iterPoolpt;
            polyPt = Scene->addEllipse(connPt.imgPt.x-10, connPt.imgPt.y-10, 20, 20, QPen(QColor(5, 179, 250,255)), QBrush(QColor(5, 179, 250,255), Qt::SolidPattern));
            QGraphicsOpacityEffect* dual_Eff = new QGraphicsOpacityEffect(this);
            dual_Eff->setOpacity(0.5);
            polyPt->setGraphicsEffect(dual_Eff);
        }
    }

    if (poolCache != NULL)
    {
        QGraphicsEllipseItem* prePoolpt;
        if (!_method)
        {
            prePoolpt = Scene->addEllipse(poolCache->imgPt.x-10, poolCache->imgPt.y-10, 20, 20, QPen(QColor(5, 179, 250,255)), QBrush(QColor(5, 179, 250,255), Qt::SolidPattern));
        } else {
            prePoolpt = Scene->addEllipse(poolCache->imgPt.x-10, poolCache->imgPt.y-10, 20, 20, QPen(Qt::white), QBrush(Qt::white, Qt::SolidPattern));
        }
        QGraphicsOpacityEffect* dual_Eff = new QGraphicsOpacityEffect(this);
        dual_Eff->setOpacity(0.5);
        prePoolpt->setGraphicsEffect(dual_Eff);
    }
}

void SVMGraphicsView::keyPressEvent(QKeyEvent *event)
{
    if (_hasimage)
    {
        if (QApplication::keyboardModifiers() && Qt::AltModifier)
        {
            if (event->key() == Qt::Key_D)
            {
                switch(state)
                {
                    case _XLines:
                        reWindDel(XLines);
                        break;
                    case _YLines:
                        reWindDel(YLines);
                        break;
                    case _ZLines:
                        reWindDel(ZLines);
                        break;
                    case _RPlane:
                        if(RPoints.size())
                        {
                            RPoints.pop_back();
                            drawScene();
                        }
                        break;
                    case _RHeight:
                        if(RHpoints.size())
                        {
                            RHpoints.pop_back();
                            drawScene();
                        }
                        break;
                    case _PtPool:
                        if(PtPool.size())
                        {
                            connPoint lstPlPt = PtPool.back();
                            if(poolCache != NULL && poolCache->imgPt.x == lstPlPt.imgPt.x && poolCache->imgPt.y == lstPlPt.imgPt.y)
                            {
                                poolCache = NULL;
                            }
                            PtPool.pop_back();
                            drawScene();
                        }
                        break;
                    case _Polygon:
                        if(PolyPool.size())
                        {
                            PolyPool.pop_back();
                            drawScene();
                        }
                        break;
                    default:
                        break;
                }
            }
            if (event->key() == Qt::Key_M)
            {
                popMoveDialog();
            }
            if (event->key() == Qt::Key_S)
            {
                if (state == _PtPool)
                {
                    _method = 1 - _method;
                    drawScene();
                } else if (state == _Polygon) {
                    _textureMethod = 1 - _textureMethod;
                }
            }
            if (event->key() == Qt::Key_P)
            {
                calPatch();
            }
        }
        if (event->key() == Qt::Key_Equal)
        {
            gap *= 1.1;
        }
        if (event->key() == Qt::Key_Minus)
        {
            gap /= 1.1;
        }
    }
}


void SVMGraphicsView::popMoveDialog()
{

    QDialog dialog(this);
    // Use a layout allowing to have a label next to each field
    QFormLayout form(&dialog);

    // Add some text above the fields
    form.addRow(new QLabel("Cursor's new coordinate:"));

    // Add the lineEdits with their respective labels
    QList<QLineEdit *> fields;
    for(int i = 0; i < 2; ++i) {
        QLineEdit *lineEdit = new QLineEdit(&dialog);
        QString label;
        switch(i)
        {
            case 0:
                label = QString("X: ");
                lineEdit->setValidator(new QDoubleValidator());
                break;
            case 1:
                label = QString("Y: ");
                lineEdit->setValidator(new QDoubleValidator());
                break;
        }
        form.addRow(label, lineEdit);
        fields << lineEdit;
    }

    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // Show the dialog as modal
    double newx, newy;
    if (dialog.exec() == QDialog::Accepted) {
        int i = 0;
        // If the user didn't dismiss the dialog, do something with the fields
        foreach(QLineEdit * lineEdit, fields)
        {
            switch(i)
            {
                case 0:
                    newx = lineEdit->text().toDouble();
                    i++;
                    break;
                case 1:
                    newy = lineEdit->text().toDouble();
                    break;
            }
        }
        this->cursor().setPos(mapToGlobal(mapFromScene(newx, newy)));
    }
}

void SVMGraphicsView::reWindDel(std::vector<imgLine> &lineConatiner)
{
    if (pointCache != NULL)
    {
        pointCache = NULL;
    } else {
        int size = lineConatiner.size();
        if (size)
        {
            imgLine lstLine = lineConatiner[size-1];
            imgPoint newPtCache = lstLine.p0;
            pointCache = new imgPoint(newPtCache);
            lineConatiner.pop_back();
        }
    }
    drawScene();
}

void SVMGraphicsView::calXVP()
{
    if (_hasimage)
    {
        xVP = calVP(XLines);
        _xVP_exist = true;
        drawScene();
    } else {
        QMessageBox::warning(this, tr("Warning"), tr("Please load a picture and specify more than 2 lines in X direction !"));
    }
}

void SVMGraphicsView::calYVP()
{
    if (_hasimage)
    {
        yVP = calVP(YLines);
        _yVP_exist = true;
        drawScene();
    } else {
        QMessageBox::warning(this, tr("Warning"), tr("Please load a picture and specify more than 2 lines in Y direction !"));
    }
}

void SVMGraphicsView::calZVP()
{
    if (_hasimage)
    {
        zVP = calVP(ZLines);
        _zVP_exist = true;
        drawScene();
    } else {
        QMessageBox::warning(this, tr("Warning"), tr("Please load a picture and specify more than 2 lines in Z direction !"));
    }
}


imgPoint* SVMGraphicsView::calVP(std::vector<imgLine> &lineConatiner)
{
    int size = lineConatiner.size();
    if (size >= 2)
    {
        imgPoint* VP = new imgPoint;

        double *eig_val = (double *) calloc (3,sizeof(double));
        double **eig_vec = (double**) calloc (3,sizeof(double*));
        double **m = (double**) calloc (3,sizeof(double*));
        for (int i=0;i<3;i++) {
            m[i] = (double *) calloc (3,sizeof(double));
            eig_vec[i] = (double *) calloc (3, sizeof(double));
        }

        std::vector<imgLine>::iterator iterLINE;
        for(iterLINE = lineConatiner.begin(); iterLINE != lineConatiner.end(); iterLINE++)
        {
            imgLine line = *iterLINE;
            imgPoint *linePt = new imgPoint;
            linePt = crossProduct(line.p0, line.p1);
            double ptArray[3];
            ptArray[0] = linePt->x;
            ptArray[1] = linePt->y;
            ptArray[2] = linePt->w;
            for (int i=0; i<3; i++)
            {
                for (int j=0; j<3; j++)
                {
                    m[i][j] += ptArray[i] * ptArray[j];
                }
            }
        }

        cout << endl << "------------------------------------------------" << endl;
        for (int i=0; i<3; i++)
        {
            for (int j=0; j<3; j++)
            {
                cout << m[i][j] << "    ";
            }
            cout << endl;
        }

// --------------  Use eigen package from course webpage  ---------------------

//        eig_sys(3, m, eig_vec, eig_val);

//        cout <<  endl <<"eigenvalues: " << endl;
//        for (int i=0;i<3;i++) cout << eig_val[i] << " ";
//        cout << endl;
//        cout << endl << "eigenvectors: ";
//        for (int i=0;i<3;i++) {
//            cout << endl;
//            for (int j=0;j<3;j++)
//                cout << eig_vec[i][j] << " " ;
//        }
//        cout << endl;

//        VP->x = eig_vec[2][0]/eig_vec[2][2];
//        VP->y = eig_vec[2][1]/eig_vec[2][2];
//        VP->w = 1;

// ----------------------------------------------------------------------------


// ---------------- Use Eigen3 package ----------------------------------------

        Matrix3d mEig3;
        for (int i=0; i<3; i++)
            for (int j=0; j<3; j++)
                mEig3(i, j) = m[i][j];

        SelfAdjointEigenSolver<Matrix3d> es;
        es.compute(mEig3);
        cout << "\n Eigen Values: \n" << es.eigenvalues() << endl;
        cout << "\n Eigen Vectors: \n" << es.eigenvectors() << endl;

        Vector3d v = es.eigenvectors().col(0);
        VP->x = v(0)/v(2);
        VP->y = v(1)/v(2);
        VP->w = 1;

// ----------------------------------------------------------------------------


        for (int i=0;i<3;i++) {
            free(m[i]);
            free(eig_vec[i]);
        }
        free(m);
        free(eig_vec);
        free(eig_val);

        cout << endl << "Vanishing point position: (" << VP->x << ", " << VP->y << ")" << endl << "------------------------------------------------" << endl << endl;
        return VP;
    } else {
        QMessageBox::warning(this, tr("Warning"), tr("Please specify more than 2 lines in this direction !"));
    }
    return NULL;
}

imgPoint* SVMGraphicsView::crossProduct(imgPoint p0, imgPoint p1)
{
    imgPoint* lnPoint = new imgPoint;
    lnPoint->x = p0.y * p1.w - p0.w * p1.y;
    lnPoint->y = p1.x * p0.w - p0.x * p1.w;
    lnPoint->w = p0.x * p1.y - p0.y * p1.x;
    homoFy(lnPoint);
//    cout << p0.x << ", "<< p0.y << ", " << p0.w << ", " << p1.x << ", " << p1.y << ", " << p1.w << ", " << lnPoint->x << ", " << lnPoint->y << ", " << lnPoint->w << endl << "+++++++" << endl;
    return lnPoint;
}

void SVMGraphicsView::normaLize(imgPoint* pt)
{
    double mod = sqrt(pt->x * pt->x + pt->y * pt->y + pt->w * pt->w);
    pt->x /= mod;
    pt->y /= mod;
    pt->w /= mod;
}

void SVMGraphicsView::homoFy(imgPoint* pt)
{
    pt->x /= pt->w;
    pt->y /= pt->w;
    pt->w = 1;
}

void SVMGraphicsView::eigenJacobianTest()
{
    double *eig_val = (double *) calloc (3,sizeof(double));
    double **eig_vec = (double**) calloc (3,sizeof(double*));
    double **m = (double**) calloc (3,sizeof(double*));
    for (int i=0;i<3;i++) {
        m[i] = (double *) calloc (3,sizeof(double));
        eig_vec[i] = (double *) calloc (3, sizeof(double));
    }
    m[0][0] = 2; m[0][1] = 1; m[0][2] = 1;
    m[1][0] = 1; m[1][1] = 2; m[1][2] = 1;
    m[2][0] = 1; m[2][1] = 1; m[2][2] = 2;
    eig_sys (3,m,eig_vec,eig_val);
    cout << "eigenvalues " ;
    for (int i=0;i<3;i++) cout << eig_val[i] << " ";
    cout << endl;
    cout << "eigenvectors";
    for (int i=0;i<3;i++) {
        cout << endl;
        for (int j=0;j<3;j++)
            cout << eig_vec[i][j] << " " ;
    }
    cout << endl;

    for (int i=0;i<3;i++) {
        free(m[i]);
        free(eig_vec[i]);
    }
    free(m);
    free(eig_vec);
    free(eig_val);
}

void SVMGraphicsView::eigen3Test()
{
    MatrixXd A = MatrixXd::Random(100,100);
    MatrixXd b = MatrixXd::Random(100,50);
    MatrixXd x = A.fullPivLu().solve(b);
    double relative_error = (A*x - b).norm() / b.norm(); // norm() is L2 norm
    cout << "The relative error is:\n" << relative_error << endl;
}


void SVMGraphicsView::calHomography_Public()
{
    if(_hasimage)
    {
        popIfLoadVPDialog();
        // calHomography(RPoints);   // use self programmed LU solver, performance not good
        calHomographyEig3(RPoints); // use eigen3 package, very good performance
    } else {
        QMessageBox::warning(this, tr("Warning"), tr("Please load a picture and specify a plane first!"));
    }
}

void SVMGraphicsView::popIfLoadVPDialog()
{

    QDialog dialog(this);
    // Use a layout allowing to have a label next to each field
    QFormLayout form(&dialog);

    // Add some text above the fields
    form.addRow(new QLabel("Take X and Y vanishing points into consideration at this step if they can be and already been calculated?"));

    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    _extraRPt = 0;
    if (dialog.exec() == QDialog::Accepted) {
        if (_xVP_exist) _extraRPt++;
        if (_yVP_exist) _extraRPt++;
    }

}

void SVMGraphicsView::calHomographyEig3(std::vector<connPoint> &planePts)
{
    int refPtN =  planePts.size() + _extraRPt;
    if (refPtN < 4)
    {
        QMessageBox::warning(this, tr("Warning"), tr("Please specify at least 4 points (including vanishing points on this plane) of a POLYGON on this plane !"));
    } else {
        MatrixXd A(2*refPtN, 8);
        VectorXd b(2*refPtN);
        std::vector<connPoint>::iterator iterRPt;
        int i = 0;
        for (iterRPt=planePts.begin(); iterRPt!=planePts.end(); iterRPt++, i++)
        {
            connPoint refPt = *iterRPt;
            b(2*i) = refPt.scnPt.x;
            b(2*i+1) = refPt.scnPt.y;

            A(2*i, 0) = refPt.imgPt.x;
            A(2*i, 1) = refPt.imgPt.y;
            A(2*i, 2) = 1;
            A(2*i, 3) = 0;
            A(2*i, 4) = 0;
            A(2*i, 5) = 0;
            A(2*i, 6) = -refPt.imgPt.x * refPt.scnPt.x;
            A(2*i, 7) = -refPt.imgPt.y * refPt.scnPt.x;

            A(2*i+1, 0) = 0;
            A(2*i+1, 1) = 0;
            A(2*i+1, 2) = 0;
            A(2*i+1, 3) = refPt.imgPt.x;
            A(2*i+1, 4) = refPt.imgPt.y;
            A(2*i+1, 5) = 1;
            A(2*i+1, 6) = -refPt.imgPt.x * refPt.scnPt.y;
            A(2*i+1, 7) = -refPt.imgPt.y * refPt.scnPt.y;
        }


        if (_extraRPt)
        {
            if (_xVP_exist)
            {
                b(2*i) = 0;
                b(2*i+1) = -1;

                A(2*i, 0) = 0;
                A(2*i, 1) = 0;
                A(2*i, 2) = 0;
                A(2*i, 3) = xVP->x;
                A(2*i, 4) = xVP->y;
                A(2*i, 5) = 1;
                A(2*i, 6) = 0;
                A(2*i, 7) = 0;

                A(2*i+1, 0) = 0;
                A(2*i+1, 1) = 0;
                A(2*i+1, 2) = 0;
                A(2*i+1, 3) = 0;
                A(2*i+1, 4) = 0;
                A(2*i+1, 5) = 0;
                A(2*i+1, 6) = xVP->x;
                A(2*i+1, 7) = xVP->y;
            }

            i++;

            if (_yVP_exist)
            {
                b(2*i) = 0;
                b(2*i+1) = -1;

                A(2*i, 0) = yVP->x;
                A(2*i, 1) = yVP->y;
                A(2*i, 2) = 1;
                A(2*i, 3) = 0;
                A(2*i, 4) = 0;
                A(2*i, 5) = 0;
                A(2*i, 6) = 0;
                A(2*i, 7) = 0;

                A(2*i+1, 0) = 0;
                A(2*i+1, 1) = 0;
                A(2*i+1, 2) = 0;
                A(2*i+1, 3) = 0;
                A(2*i+1, 4) = 0;
                A(2*i+1, 5) = 0;
                A(2*i+1, 6) = yVP->x;
                A(2*i+1, 7) = yVP->y;
            }
        }



        VectorXd x = A.fullPivLu().solve(b);
        cout << "\n A: \n" << A << endl;
        cout << "\n b: \n" << b << endl;
        Eigen::MatrixXd Ab(2*refPtN, 9);

        for (int i=0; i<2*refPtN; i++)
        {
            for (int j=0; j<8; j++)
                Ab(i, j) = A(i, j);
            Ab(i, 8) = b(i);
        }

        FullPivLU<MatrixXd> lu(Ab);
        lu.setThreshold(1e-5);
        cout << "\n Rank of [A b]: \n" << lu.rank() << endl;
        cout << "\n x: \n" << x << endl;
        cout << "\n A*x: \n" << A*x << endl;

        Matrix3d homo;
        for (int i=0; i<8; i++) homo(i/3, i%3) = x(i);
        homo(2,2) = 1;
        refHomoI2S = homo;
        refHomoS2I = homo.inverse();
        _refHomo_exist = true;

        cout << "\n Homography matrix (from image to scene): \n" << refHomoI2S << endl;

        cout << "\n Homography matrix (from scene to image): \n" << refHomoS2I << endl;

        i = 1;
        for (iterRPt=planePts.begin(); iterRPt!=planePts.end(); iterRPt++, i++)
        {
            (*iterRPt).homoMatrixEig3 = homo;
            Vector3d imgPtVec = imgPointToVector3d((*iterRPt).imgPt);
            cout << "------------Point " << i <<" ------------- \n ImageCoord: \n" << imgPtVec << endl;
            Vector3d scnPtFromImg = homo * imgPtVec;
            scnPtFromImg /= scnPtFromImg(2);
            cout << "\n HomoMat * ImageCoord: \n" << scnPtFromImg << endl;
            cout << "\n SceneCoord: \n" << scnPointToVector3d((*iterRPt).scnPt) << endl;
        }
        if (_extraRPt)
        {
            if (_xVP_exist)
            {
                Vector3d imgPtVec = imgPointToVector3d(*xVP);
                cout << "------------Point " << i <<" ------------- \n ImageCoord: \n" << imgPtVec << endl;
                Vector3d scnPtFromImg = homo * imgPtVec;
            //    scnPtFromImg /= scnPtFromImg(2);
                cout << "\n HomoMat * ImageCoord: \n" << scnPtFromImg << endl;
                Vector3d xVPScn;
                xVPScn << 1, 0, 0;
                cout << "\n SceneCoord: \n" << xVPScn << endl;
            }
            i++;
            if (_yVP_exist)
            {
                Vector3d imgPtVec = imgPointToVector3d(*yVP);
                cout << "------------Point " << i <<" ------------- \n ImageCoord: \n" << imgPtVec << endl;
                Vector3d scnPtFromImg = homo * imgPtVec;
            //    scnPtFromImg /= scnPtFromImg(2);
                cout << "\n HomoMat * ImageCoord: \n" << scnPtFromImg << endl;
                Vector3d yVPScn;
                yVPScn << 0, 1, 0;
                cout << "\n SceneCoord: \n" << yVPScn << endl;
            }
        }

    }
}


Vector3d SVMGraphicsView::imgPointToVector3d(imgPoint imgPt)
{
    Vector3d vecPt;
    vecPt(0) = imgPt.x;
    vecPt(1) = imgPt.y;
    vecPt(2) = imgPt.w;
    return vecPt;
}

Vector3d SVMGraphicsView::scnPointToVector3d(scnPoint scnPt)
{
    Vector3d vecPt;
    vecPt(0) = scnPt.x;
    vecPt(1) = scnPt.y;
    vecPt(2) = scnPt.w;
    return vecPt;
}

/*
void SVMGraphicsView::calHomography(std::vector<connPoint> &planePts)
{

    int refPtN =  planePts.size();
    if (refPtN < 4)
    {
        QMessageBox::warning(this, tr("Warning"), tr("Please specify at least 4 points of a polygon on this plane !"));
    } else {
        double *b = (double *) calloc (2*refPtN, sizeof(double));
        double **A = (double **) calloc (2*refPtN, sizeof(double));
        for (int i=0; i<2*refPtN; i++)
        {
            A[i] = (double *) calloc (8, sizeof(double));
        }

        int i = 0;
        std::vector<connPoint>::iterator iterRPt;
        for (iterRPt=planePts.begin(); iterRPt!=planePts.end(); iterRPt++, i++)
        {
            connPoint refPt = *iterRPt;
            b[2*i] = refPt.scnPt.x;
            b[2*i+1] = refPt.scnPt.y;

            A[2*i][0] = refPt.imgPt.x;
            A[2*i][1] = refPt.imgPt.y;
            A[2*i][2] = 1;
            A[2*i][3] = 0;
            A[2*i][4] = 0;
            A[2*i][5] = 0;
            A[2*i][6] = -refPt.imgPt.x * refPt.scnPt.x;
            A[2*i][7] = -refPt.imgPt.y * refPt.scnPt.x;

            A[2*i+1][0] = 0;
            A[2*i+1][1] = 0;
            A[2*i+1][2] = 0;
            A[2*i+1][3] = refPt.imgPt.x;
            A[2*i+1][4] = refPt.imgPt.y;
            A[2*i+1][5] = 1;
            A[2*i+1][6] = -refPt.imgPt.x * refPt.scnPt.y;
            A[2*i+1][7] = -refPt.imgPt.y * refPt.scnPt.y;
        }

        cout << endl << "*******" << endl;
        for (int i=0; i<2*refPtN; i++)
        {
            for (int j=0; j<8; j++) cout << A[i][j] << ",  ";
            cout << b[i] << ";" <<endl;
        }
        cout << "*****" << endl;

        double *ATb = (double *) calloc (8, sizeof(double));
        double **ATA = (double **) calloc (8, sizeof(double));

        for (int i=0; i<8; i++)
        {
            ATA[i] = (double *) calloc (8, sizeof(double));
        }

        for (int i=0; i<8; i++)
        {
            for (int j=i; j<8; j++)
            {
                ATA[i][j] = 0;
                for (int k=0; k<2*refPtN; k++)
                {
                    ATA[i][j] += A[k][i] * A[k][j];
                }
                ATA[j][i] = ATA[i][j];
            }

            ATb[i] = 0;
            for (int k=0; k<2*refPtN; k++)
            {
                ATb[i] += A[k][i] * b[k];
            }
        }

        cout << endl << "*******" << endl;
        for (int i=0; i<8; i++)
        {
            for (int j=0; j<8; j++) cout << ATA[i][j] << ",  ";
            cout << ATb[i] << ";" << endl;
        }
        cout << "*****" << endl;



        double *homoMat1D;
        double **homoMat2D;
        homoMat1D = (double *) calloc (8, sizeof(double));
        homoMat2D = (double **) calloc (3, sizeof(double));
        for (int i=0; i<3; i++)
        {
            homoMat2D[i] = (double *) calloc (3, sizeof(double));
        }

        homoMat1D = luSolver(ATA, ATb, 8);

        homoMat2D[0][0] = homoMat1D[0]; homoMat2D[0][1] = homoMat1D[1]; homoMat2D[0][2] = homoMat1D[2];
        homoMat2D[1][0] = homoMat1D[3]; homoMat2D[1][1] = homoMat1D[4]; homoMat2D[1][2] = homoMat1D[5];
        homoMat2D[2][0] = homoMat1D[6]; homoMat2D[2][1] = homoMat1D[7]; homoMat2D[2][2] = 1;


        for (iterRPt=planePts.begin(); iterRPt!=planePts.end(); iterRPt++)
        {
            (*iterRPt).homoMatrix = homoMat2D;
        }

        for (int i=0; i<2*refPtN; i++)
        {
            free(A[i]);
        }
        for (int i=0; i<8; i++)
        {
            free(ATA[i]);
        }
        free(A);
        free(b);
        free(ATA);
        free(ATb);
        free(homoMat1D);
    }

}

double* SVMGraphicsView::luSolver(double **A, double *b, int n)
{
    double  **l;
    double  **u;
    double **p;
    double **Aprime;
    double *z;
    double *x;
    double *bprime;

    z = (double *) calloc (n, sizeof(double));
    x = (double *) calloc (n, sizeof(double));
    bprime = (double *) calloc (n, sizeof(double));
    l = (double **) calloc (n, sizeof(double));
    u = (double **) calloc (n, sizeof(double));
    p = (double **) calloc (n, sizeof(double));
    Aprime = (double **) calloc (n, sizeof(double));

    for (int i=0; i<n; i++)
    {
        l[i] = (double *) calloc (n, sizeof(double));
        u[i] = (double *) calloc (n, sizeof(double));
        p[i] = (double *) calloc (n, sizeof(double));
        Aprime[i] = (double *) calloc (n, sizeof(double));
    }

    for (int i=0; i<n; i++)
    {
        z[i] = 0;
        bprime[i] = b[i];
        for (int j=0; j<n; j++)
        {
            l[i][j] = (i == j);
            u[i][j] = 0;
            p[i][j] = (i == j);
            Aprime[i][j] = 0;
        }
    }



    for (int i=0; i<n; i++)
    {
        int max_j = i;
        for(int j=i; j<n; j++)
        {
            if (fabs(A[j][i]) > fabs(A[max_j][i])) max_j = j;
        }
        if (max_j != i)
        {
            for (int k=0; k<n; k++)
            {
                double tmp = p[i][k];
                p[i][k] = p[max_j][k];
                p[max_j][k] = tmp;
            }
            bprime[max_j] = b[i];
            bprime[i] = b[max_j];
        }
    }

    for (int i=0; i<n; i++)
    {
       for (int j=0; j<n; j++)
       {
           for (int k=0; k<n; k++)
           {
               Aprime[i][j] += p[i][k] * A[k][j];
           }
       }
    }

    for (int i=0; i<n; i++)
    {
        for (int j=0; j<n; j++)
        {
            double s;
            if (j <= i)
            {
                s = 0;
                for(int k=0; k<j; k++) s += l[j][k] * u[k][i];
                u[j][i] = Aprime[j][i] - s;
            }
            if (j >= i)
            {
                s = 0;
                for(int k=0; k<i; k++) s += l[j][k] * u[k][i];
                l[j][i] = (Aprime[j][i] - s) / u[i][i];
            }
        }
    }

    cout << endl << "***  L  ***" << endl;
    for (int i=0; i<n; i++)
    {
        for (int j=0; j<n; j++)
        {
            cout << l[i][j];
            if (j<n-1) cout << ", ";
            else cout << ";" << endl;
        }
    }
    cout << "***  L ***" << endl;

    cout << endl << "***  U  ***" << endl;
    for (int i=0; i<n; i++)
    {
        for (int j=0; j<n; j++)
        {
            cout << u[i][j];
            if (j<n-1) cout << ", ";
            else cout << ";" << endl;
        }
    }
    cout << "*** U ***" << endl;

    cout << endl << "***  P  ***" << endl;
    for (int i=0; i<n; i++)
    {
        for (int j=0; j<n; j++)
        {
            cout << p[i][j];
            if (j<n-1) cout << ", ";
            else cout << ";" << endl;
        }
    }
    cout << "***  P ***" << endl;


    for (int i=0; i<n; i++)
    {
        double s = 0;
        for (int j=0; j<i; j++) s += l[i][j] * z[j];
        z[i] = (bprime[i] - s) / l[i][i];
    }

    cout<< endl << "*** Z ***" << endl;
    for(int i=0;i<n;i++)    cout << z[i] << " ";
    cout << endl;


    for (int i=n-1; i>=0; i--)
    {
        double s = 0;
        for (int j=n-1; j>i; j--) s += u[i][j] * x[j];
        x[i] = (z[i] - s) / u[i][i];
    }

    cout<< endl << "Set of solution is" << endl;
    for(int i=0;i<n;i++)    cout << x[i] << " ";
    cout << endl;

    for (int i=0;i<n;i++)
    {
        free(l[i]);
        free(u[i]);
        free(p[i]);
        free(Aprime[i]);
    }
    free(l);
    free(u);
    free(p);
    free(Aprime);
    free(bprime);
    free(z);

    return x;
}
*/


void SVMGraphicsView::calGamma()
{
    if (_hasimage)
    {
        if (_refHomo_exist && (RHpoints.size() == 2) && _zVP_exist)
        {
            Vector3d P1, P2, O, Vz, b, t;
            P1 = refHomoS2I.col(0);
            P2 = refHomoS2I.col(1);
            O = refHomoS2I.col(2);
            Vz = imgPointToVector3d(*zVP);

            connPoint bConn = RHpoints[1];
            b = imgPointToVector3d(bConn.imgPt);
            connPoint tConn = RHpoints[0];
            t = imgPointToVector3d(tConn.imgPt);
            double z = bConn.scnPt.z;
            double deltaz = tConn.scnPt.z - z;

            cout << "++++++++++++ All 4 Check ++++++++++++++";
            cout << "\n P1: \n" << P1 << "\n\n P2: \n" << P2 << "\n\n O: \n" << O << "\n\n Vz: \n" << Vz << "\n\n b: \n" << b << "\n\n t: \n" << t << "\n\n z: " << z << " deltaz: " << deltaz << endl;

            int sign = 1;
            if ((b.cross(t).dot(Vz.cross(t))) > 0) sign = -1;
            gammaZ = sign * ((P1.cross(P2)).dot(O)) * (b.cross(t)).norm() / (deltaz * ((P1.cross(P2)).dot(b)) * (Vz.cross(t)).norm() + z * ((P1.cross(P2)).dot(Vz)) * (b.cross(t)).norm());
            gammaZ *= sign;

            cout << "\n gammaZ: \n " << gammaZ << endl;
            _getGammaZ = true;

            Proj << P1, P2, gammaZ * Vz, O;
            cout << "\n Projection Matrix from scene to image: \n" << Proj << endl;

        } else {
            QMessageBox::warning(this, tr("Warning"), tr("Please first calculate Z vanishing point, homography matrix of XY plane and give a reference off-plane point (x, y ,z) or 2 reference z-colinear points with only z !"));
        }
    } else {
        QMessageBox::warning(this, tr("Warning"), tr("Please load a picture, specify a plane and an off-plane height first!"));
    }
}


void SVMGraphicsView::scnMapToImg()
{
    if (_hasimage)
    {
        if (_getGammaZ)
        {
            drawScene();

            QDialog dialog(this);
            // Use a layout allowing to have a label next to each field
            QFormLayout form(&dialog);

            // Add some text above the fields
            form.addRow(new QLabel("Please input the Image coordinates:"));

            // Add the lineEdits with their respective labels
            QList<QLineEdit *> fields;
            for(int i = 0; i < 3; ++i) {
                QLineEdit *lineEdit = new QLineEdit(&dialog);
                QString label;
                switch(i)
                {
                    case 0:
                        label = QString("X: ");
                        lineEdit->setValidator(new QDoubleValidator());
                        break;
                    case 1:
                        label = QString("Y: ");
                        lineEdit->setValidator(new QDoubleValidator());
                        break;
                    default:
                        label = QString("Z: ");
                        lineEdit->setValidator(new QDoubleValidator());
                        break;
                }
                form.addRow(label, lineEdit);
                fields << lineEdit;
            }

            // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
            QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                       Qt::Horizontal, &dialog);
            form.addRow(&buttonBox);
            QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
            QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

            // Show the dialog as modal
            if (dialog.exec() == QDialog::Accepted) {
                Vector4d scnCord;

                int i = 0;
                foreach(QLineEdit * lineEdit, fields)
                {
                    scnCord(i) = lineEdit->text().toDouble();
                    i++;
                }
                scnCord(i) = 1;
                Vector3d imgCord = Proj * scnCord;
                imgCord /= imgCord(2);
                cout << "\n Scene coordinates: \n" << scnCord << "\n Image coordinates: \n" << imgCord << endl;

                QGraphicsEllipseItem* imgTarget;
                imgTarget = Scene->addEllipse(imgCord(0)-3, imgCord(1)-3, 6, 6, QPen(QColor(5, 179, 250,255)), QBrush(QColor(5, 179, 250,255), Qt::SolidPattern));
                QGraphicsOpacityEffect* dual_Eff = new QGraphicsOpacityEffect(this);
                dual_Eff->setOpacity(0.5);
                imgTarget->setGraphicsEffect(dual_Eff);
            }
        } else {
            QMessageBox::warning(this, tr("Warning"), tr("Please calculate all the staffs first!"));
        }
    } else {
        QMessageBox::warning(this, tr("Warning"), tr("Please load a picture and calculate all the staffs first!"));
    }
}


void SVMGraphicsView::make3DPtPool()
{
    if (_hasimage)
    {
        if (_getGammaZ)
        {
            state = _PtPool;
            drawScene();
            cout << "\n Please pick up all the points which will be used to form polygons later, start from one point in the reference plane. \n"
                 << "You can change method state afterwards. Begin with co-plane methods. \"Alt + S\" could switch between co-plane and vertical methods." << endl;
            poolCache = NULL;
            _method = 0;
        } else {
            QMessageBox::warning(this, tr("Warning"), tr("Please calculate all the staffs first!"));
        }
    } else {
        QMessageBox::warning(this, tr("Warning"), tr("Please load a picture first !"));
    }
}

void SVMGraphicsView::pushPtPool(double x, double y)
{
    connPoint* newPollPt;
    newPollPt = new connPoint;
    newPollPt->imgPt.x = x;
    newPollPt->imgPt.y = y;
    newPollPt->imgPt.w = 1;
    newPollPt->scnPt.w = 1;

    if (!PtPool.size())
    {
        newPollPt->homoMatrixEig3 = refHomoI2S;
        newPollPt->scnPt.z = 0;
        Vector3d scnCord = newPollPt->homoMatrixEig3 * imgPointToVector3d(newPollPt->imgPt);
        scnCord /= scnCord(2);
        newPollPt->scnPt.x = scnCord(0);
        newPollPt->scnPt.y = scnCord(1);

        cout << "Scene Coordinates: (" << newPollPt->scnPt.x << ", " << newPollPt->scnPt.y << ", " << 0 << ")" << endl;
        PtPool.push_back(*newPollPt);
        poolCache = newPollPt;
    } else {
        if (poolCache == NULL)
        {
            std::vector<connPoint>::iterator iterPoolpt;
            for(iterPoolpt = PtPool.begin(); iterPoolpt != PtPool.end(); iterPoolpt++)
            {
                connPoint connPt = *iterPoolpt;
                if (x == connPt.imgPt.x && y == connPt.imgPt.y)
                {
                    newPollPt->scnPt.x = connPt.scnPt.x;
                    newPollPt->scnPt.y = connPt.scnPt.y;
                    newPollPt->scnPt.z = connPt.scnPt.z;
                    newPollPt->homoMatrixEig3 = connPt.homoMatrixEig3;
                    poolCache = newPollPt;
                    break;
                }
            }
            cout<< "Please select a previous point (Ctrl + LClick) first !" << endl;
        } else {
            if (!_method)    // 0 coplane 1 vertical
            {
                newPollPt->scnPt.z = poolCache->scnPt.z;
                newPollPt->homoMatrixEig3 = poolCache->homoMatrixEig3;
                Vector3d scnCord = newPollPt->homoMatrixEig3 * imgPointToVector3d(newPollPt->imgPt);
                scnCord /= scnCord(2);
                newPollPt->scnPt.x = scnCord(0);
                newPollPt->scnPt.y = scnCord(1);
            } else {
                newPollPt->scnPt.x = poolCache->scnPt.x;
                newPollPt->scnPt.y = poolCache->scnPt.y;

                Vector3d P1, P2, O, Vz, b, bscn, t;
                P1 = refHomoS2I.col(0);
                P2 = refHomoS2I.col(1);
                O = refHomoS2I.col(2);
                Vz = imgPointToVector3d(*zVP);
                bscn << newPollPt->scnPt.x, newPollPt->scnPt.y, 1;
                b = refHomoS2I * bscn;
                b /= b(2);
                t = imgPointToVector3d(newPollPt->imgPt);
                int sign = 1;
                if ((b.cross(t).dot(Vz.cross(t))) > 0) sign = -1;
                double z;

                z = sign * ((P1.cross(P2)).dot(O)) * (b.cross(t)).norm() / (gammaZ * ((P1.cross(P2)).dot(b)) * (Vz.cross(t)).norm());

                newPollPt->scnPt.z = z;

                Matrix3d P;
                P << P1, P2, O + gammaZ * Vz * z;
                Matrix3d zHomo = P.inverse();
                newPollPt->homoMatrixEig3 = zHomo;

                _method = 0;
            }
            cout << "Scene Coordinates: (" << newPollPt->scnPt.x << ", " << newPollPt->scnPt.y << ", " << newPollPt->scnPt.z << ")" << endl;
            PtPool.push_back(*newPollPt);
            poolCache = newPollPt;
        }
    }

    drawScene();
}

void SVMGraphicsView::pickPolygon()
{
    if (_hasimage)
    {
        if (_getGammaZ && PtPool.size() >= 3)
        {
            poolCache = NULL;
            drawScene();
            cout << "\n Please select at least 3 points for each plane and make patch by press \"Alt + P\" or click \"Texture\" in \"Calculation\" menu, you can change TextureMethod (BoundingBox method or Polygon method) with \"Alt + S\", default: BoundingBox method" << endl;
            state = _Polygon;
            _textureMethod = 0;
            PolyPool.clear();
        } else {
            QMessageBox::warning(this, tr("Warning"), tr("Please calculate all the staffs and make point pool ready (at least 3 points) !"));
        }
    } else {
        QMessageBox::warning(this, tr("Warning"), tr("Please load a picture first !"));
    }

}

void SVMGraphicsView::pushPolyPool(double x, double y)
{
    std::vector<connPoint>::iterator iterPoolpt;
    for(iterPoolpt = PtPool.begin(); iterPoolpt != PtPool.end(); iterPoolpt++)
    {
        connPoint connPt = *iterPoolpt;
        if (x == connPt.imgPt.x && y == connPt.imgPt.y)
        {
            PolyPool.push_back(connPt);
            drawScene();
            break;
        }
    }
}


void SVMGraphicsView::calPatch()
{
    if (_hasimage)
    {
        if (PolyPool.size() >= 3)
        {
//            if (planeCheck(PolyPool))
//            {
                makePatch();
//            }
        } else {
            QMessageBox::warning(this, tr("Warning"), tr("To make patch, please calculate Alpha, pick up at least 3 points on a plane first !"));
        }
    } else {
        QMessageBox::warning(this, tr("Warning"), tr("Please load a picture, calculate all the stuff and pick up some points on a plane first!"));
    }
}

bool SVMGraphicsView::planeCheck(std::vector<connPoint> &polyPool)
{
    MatrixXd A(3, polyPool.size());
    Vector3d V1, V2, V3;

    connPoint pt = polyPool.back();
    double x0 = pt.scnPt.x;
    double y0 = pt.scnPt.y;
    double z0 = pt.scnPt.z;

    int i = 0;
    std::vector<connPoint>::iterator iter;
    for(iter = polyPool.begin(); iter != polyPool.end(); iter++, i++)
    {
        pt = *iter;
        A(0, i) = pt.scnPt.x - x0;
        A(1, i) = pt.scnPt.y - y0;
        A(2, i) = pt.scnPt.z - z0;
        x0 = pt.scnPt.x;
        y0 = pt.scnPt.y;
        z0 = pt.scnPt.z;
    }

    V1 << A.col(0);
    V2 << A.col(1);
//    V1.normalize();   // shorter vector should have less contribution of the check function, so comment it
//    V2.normalize();


    for(i = 2; i < int(polyPool.size()); i++)
    {
        V3 << A.col(i);
//        V3.normalize();
        double error = abs((V1.cross(V2)).dot(V3) / V3.norm());
        if(error > 5e-2)
        {
            cout << "\n Co-plane check failed, error: " << error << endl;
            QMessageBox::warning(this, tr("Warning"), tr("Points are not in the same plane, please check!"));
            return false;
        }
        V1 = V2;
        V2 = V3;
//        V1.normalize();
//        V2.normalize();
    }

    cout << "\nPass co-plane check..." << endl;
    return true;
}

void SVMGraphicsView::makePatch()
{
    drawScene();
    MatrixXd A(PolyPool.size()+1, 4);
    VectorXd I(PolyPool.size()+1);
    patchConfirmDlg->scnPoints.clear();
    patchConfirmDlg->texPoints.clear();

    int i = 0;
    int j;
    std::vector<connPoint>::iterator iter;
    for(iter = PolyPool.begin(); iter != PolyPool.end(); iter++, i++)
    {
        connPoint pt = *iter;
        A(i, 0) = pt.scnPt.x;
        A(i, 1) = pt.scnPt.y;
        A(i, 2) = pt.scnPt.z;
        A(i, 3) = -1;
        I(i) = 0;
        Vector3d pt3DCord;
        pt3DCord << pt.scnPt.x, pt.scnPt.y, pt.scnPt.z;
        patchConfirmDlg->scnPoints.push_back(pt3DCord);
    }
    A(i, 0) = 1;
    A(i, 1) = 1;
    A(i, 2) = 1;
    A(i, 3) = 1;
    I(i) = 1;

    MatrixXd x = A.fullPivLu().solve(I);
    cout << "\n Plane coefficients: \n" << x << endl;

    Vector3d V;
    V << x(0), x(1), x(2) ;
    V.normalize();
    Vector3d z;
    z << 0, 0, 1;

    double theta = acos(z.dot(V));
    Vector3d R;
    R = V.cross(z);
    if (theta > 1e-4)
        R.normalize();
    int sign = 1;
    if (z.dot(V) * x(3) * x(2) < 0) sign = -1;

    QPolygonF region;
    double bBoxXmin, bBoxXmax, bBoxYmin, bBoxYmax;
    std::vector<Vector3d> rotatedPts;
    for(iter = PolyPool.begin(); iter != PolyPool.end(); iter++)
    {
        connPoint pt = *iter;
        Vector3d rPt;
        rPt = rotate(pt.scnPt.x, pt.scnPt.y, pt.scnPt.z, R, theta);
        rotatedPts.push_back(rPt);

        QPointF qpt(rPt(0), rPt(1));
        region << qpt;

        if(iter == PolyPool.begin())
        {
            bBoxXmin = rPt(0);
            bBoxXmax = rPt(0);
            bBoxYmin = rPt(1);
            bBoxYmax = rPt(1);
        } else {
            bBoxXmin = min(bBoxXmin, rPt(0));
            bBoxXmax = max(bBoxXmax, rPt(0));
            bBoxYmin = min(bBoxYmin, rPt(1));
            bBoxYmax = max(bBoxYmax, rPt(1));
        }
    }

    double scale = 0.01;
    int oX = floor(bBoxXmin/scale);
    int oY = floor(bBoxYmin/scale);
    int width = ceil(bBoxXmax/scale) - oX + 1;
    int height = ceil(bBoxYmax/scale) - oY + 1;

    std::vector<Vector3d>::iterator iterR;
    for(iterR = rotatedPts.begin(); iterR != rotatedPts.end(); iterR++)
    {
        Vector3d patchPt = *iterR;
        patchPt(0) = (patchPt(0) - oX * scale) / ((width - 1) * scale);
        patchPt(1) = (patchPt(1) - oY * scale) / ((height - 1) * scale);
        Vector2d texCord;
        texCord << patchPt(0), patchPt(1);
        patchConfirmDlg->texPoints.push_back(texCord);
    }



    QImage patchImg(width, height, QImage::Format_ARGB32_Premultiplied);
    patchImg.fill(Qt::transparent);

    for(i=0; i<width; i++)
    {
        for(j=0; j<height; j++)
        {
            double rotatedX = (oX + i) * scale;
            double rotatedY = (oY + j) * scale;
            if(_textureMethod)
            {
                if (region.containsPoint(QPointF(rotatedX, rotatedY), Qt::OddEvenFill))
                {
                    Vector3d trueCord = rotate(rotatedX, rotatedY, sign * abs(x(3)) / sqrt(x(0) * x(0) + x(1) * x(1) + x(2) * x(2)), R, -theta);
                    Vector4d scnCord;
                    scnCord(0) = trueCord(0);
                    scnCord(1) = trueCord(1);
                    scnCord(2) = trueCord(2);
                    scnCord(3) = 1;
                    Vector3d imgCord = Proj * scnCord;
                    imgCord /= imgCord(2);

                    QGraphicsEllipseItem* imgTarget;
                    imgTarget = Scene->addEllipse(imgCord(0)-1, imgCord(1)-1, 2, 2, QPen(Qt::green), QBrush(Qt::green, Qt::SolidPattern));
                    QGraphicsOpacityEffect* dual_Eff = new QGraphicsOpacityEffect(this);
                    dual_Eff->setOpacity(0.5);
                    imgTarget->setGraphicsEffect(dual_Eff);
                    if(imgCord(0) >= 0 && imgCord(0) <= image.size().width()-1 && imgCord(1) >=0 && imgCord(1) <= image.size().height()-1)
                    {
                        Vector3i pixColor = bilinear(image, imgCord(0), imgCord(1));
                        //  patchImg.setPixel(i, height-j-1, qRgb(255,0,0));
                        patchImg.setPixel(i, height-j-1, qRgb(pixColor(0), pixColor(1), pixColor(2)));
                    }


                }
            } else {
                Vector3d trueCord = rotate(rotatedX, rotatedY, sign * x(3) / sqrt(x(0) * x(0) + x(1) * x(1) + x(2) * x(2)), R, -theta);
                Vector4d scnCord;
                scnCord(0) = trueCord(0);
                scnCord(1) = trueCord(1);
                scnCord(2) = trueCord(2);
                scnCord(3) = 1;
                Vector3d imgCord = Proj * scnCord;
                imgCord /= imgCord(2);

                QGraphicsEllipseItem* imgTarget;
                imgTarget = Scene->addEllipse(imgCord(0)-1, imgCord(1)-1, 2, 2, QPen(Qt::green), QBrush(Qt::green, Qt::SolidPattern));
                QGraphicsOpacityEffect* dual_Eff = new QGraphicsOpacityEffect(this);
                dual_Eff->setOpacity(0.5);
                imgTarget->setGraphicsEffect(dual_Eff);
                if(imgCord(0) >= 0 && imgCord(0) <= image.size().width()-1 && imgCord(1) >=0 && imgCord(1) <= image.size().height()-1)
                {
                    Vector3i pixColor = bilinear(image, imgCord(0), imgCord(1));
                    //  patchImg.setPixel(i, height-j-1, qRgb(255,0,0));
                    patchImg.setPixel(i, height-j-1, qRgb(pixColor(0), pixColor(1), pixColor(2)));
                }
            }
        }
    }

    patchConfirmDlg->patchImg = patchImg;
    patchConfirmDlg->popout();
}

Vector3d SVMGraphicsView::rotate(double x, double y, double z, Vector3d R, double theta)
{
    Vector3d newV;
    double u, v, w;
    u = R(0);
    v = R(1);
    w = R(2);

    newV << u * (u * x + v * y + w * z) * (1 - cos(theta)) + x * cos(theta) + (-w * y + v * z) * sin(theta),
            v * (u * x + v * y + w * z) * (1 - cos(theta)) + y * cos(theta) + (w * x - u * z) * sin(theta),
            w * (u * x + v * y + w * z) * (1 - cos(theta)) + z * cos(theta) + (-v * x + u * y) * sin(theta);
    return newV;
}

Vector3i SVMGraphicsView::bilinear(QImage image, double x, double y)
{
    int xL = floor(x);
    int xR = ceil(x);
    int yU = floor(y);
    int yL = ceil(y);

    Vector3i pLU, pRU, pLL, pRL, pXY;
    QColor pixelColor;
    pixelColor = QColor(image.pixel(xL, yU));
    pLU(0) = pixelColor.red();
    pLU(1) = pixelColor.green();
    pLU(2) = pixelColor.blue();
    pixelColor = QColor(image.pixel(xR, yU));
    pRU(0) = pixelColor.red();
    pRU(1) = pixelColor.green();
    pRU(2) = pixelColor.blue();
    pixelColor = QColor(image.pixel(xL, yL));
    pLL(0) = pixelColor.red();
    pLL(1) = pixelColor.green();
    pLL(2) = pixelColor.blue();
    pixelColor = QColor(image.pixel(xR, yL));
    pRL(0) = pixelColor.red();
    pRL(1) = pixelColor.green();
    pRL(2) = pixelColor.blue();

    double dx = x - xL;
    double dy = y - yU;
    RowVector2d l;
    Vector2d r;
    l << 1-dx, dx;
    r << 1-dy, dy;

    Matrix2d F;
    F << pLU(0), pLL(0), pRU(0), pRL(0);
    pXY(0) = l*F*r;
    F << pLU(1), pLL(1), pRU(1), pRL(1);
    pXY(1) = l*F*r;
    F << pLU(2), pLL(2), pRU(2), pRL(2);
    pXY(2) = l*F*r;

    return pXY;
}

void SVMGraphicsView::saveModel()
{
    if(_hasimage)
    {
        QString sourcePath = QDir::currentPath() + "/patches/wrl.tmp";
        if(QFile::exists(sourcePath))
        {

            QByteArray fileFormat("wrl");

            QString destPath = QDir::currentPath() + "/patches/untitled." + fileFormat;

            QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                            destPath,
                                                            tr("%1 Files (*.%2);;")
                                                            .arg(QString::fromLatin1(fileFormat.toUpper()))
                                                            .arg(QString::fromLatin1(fileFormat)));
            if (!fileName.isEmpty()) {
                QFile::copy(sourcePath, destPath);
                cout << "\nSave model: " << fileName.toStdString() << endl;
            }
        } else {
            QMessageBox::warning(this, tr("Warning"), tr("Please build all the patches first!"));
        }
    } else {
        QMessageBox::warning(this, tr("Warning"), tr("Please load a picture first!"));
    }
}
