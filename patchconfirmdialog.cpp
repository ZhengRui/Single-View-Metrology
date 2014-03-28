#include "patchconfirmdialog.h"
#include "ui_patchconfirmdialog.h"


#include <QWheelEvent>
#include <QtWidgets>
#include <iostream>

using namespace std;

patchConfirmDialog::patchConfirmDialog(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::patchConfirmDialog)
{
    ui->setupUi(this);

    patchView = new QGraphicsView(this);
    patchScene = new QGraphicsScene(this);

    patchView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    patchScene->setBackgroundBrush(QBrush(Qt::darkGray, Qt::SolidPattern));
    patchView->setScene(patchScene);
    patchView->setDragMode(patchView->ScrollHandDrag);
    patchView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    setWindowTitle("Patch PreView");
    setCentralWidget(patchView);


    QPushButton *save_button = new QPushButton(this);
    save_button->setText(tr("SAVE"));
    save_button->setStyleSheet("background-color:rgba(0,0,0,50%); color:rgba(255,0,0,40%); font-weight:bold");
    save_button->setGeometry(10,10,70,25);


    QPushButton *cancel_button = new QPushButton(this);
    cancel_button->setText(tr("CANCEL"));
    cancel_button->setStyleSheet("background-color:rgba(0,0,0,50%); color:rgba(0,255,0,40%); font-weight:bold");
    cancel_button->setGeometry(90,10,70,25);

    connect(save_button, SIGNAL(clicked()), this, SLOT(savePatch()));
    connect(cancel_button, SIGNAL(clicked()), this, SLOT(cancel()));

}

patchConfirmDialog::~patchConfirmDialog()
{
    delete ui;
}


void patchConfirmDialog::wheelEvent(QWheelEvent *event)
{
    double scaleFactor = 1.15;
    if(event->delta() > 0)
    {
        this->patchView->scale(scaleFactor, scaleFactor);
    } else {
        this->patchView->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}

void patchConfirmDialog::popout()
{

    patchScene->clear();
    patchScene->addPixmap(QPixmap::fromImage(patchImg));
    this->show();
    this->raise();
    this->activateWindow();

}



void patchConfirmDialog::savePatch()
{
    QByteArray fileFormat("png");

    if(!QDir("patches").exists())    QDir().mkdir("patches");

    QString initialPath = QDir::currentPath() + "/patches/untitled." + fileFormat;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    initialPath,
                                                    tr("%1 Files (*.%2);;All Files (*)")
                                                    .arg(QString::fromLatin1(fileFormat.toUpper()))
                                                    .arg(QString::fromLatin1(fileFormat)));
    if (!fileName.isEmpty()) {
        patchImg.save(fileName, fileFormat.constData());
        this->close();
        cout << "\nSave patch: " << fileName.toStdString() << endl;
    }

    QStringList pieces = fileName.split("/");
    QString patchName = pieces.value(pieces.length() - 1);

    QFile file("patches/wrl.tmp");
    file.open(QIODevice::ReadWrite);
    QTextStream wrl(&file);
    if(wrl.readLine().startsWith("#VRML"))
    {
        int seekpos = file.size() - 7;
        file.seek(seekpos);
    } else {
        wrl << "#VRML V2.0 utf8\n\nCollision {\n\tcollide FALSE\n\tchildren [\n";
    }

    wrl << "Shape {\n";
    wrl << "    appearance Appearance {\n";
    wrl << "        texture ImageTexture {\n";
    wrl << "            url \"" << patchName << "\"\n";
    wrl << "        }\n";
    wrl << "    }\n";
    wrl << "    geometry IndexedFaceSet {\n";
    wrl << "        coord Coordinate {\n";
    wrl << "            point [\n";

    std::vector<Vector3d>::iterator iter3d;
    for(iter3d = scnPoints.begin(); iter3d != scnPoints.end(); iter3d++)
    {
        Vector3d scnPt = *iter3d;
        wrl << "                " << scnPt(0) << " " << scnPt(1) << " " << scnPt(2) << ",\n";
    }

    wrl << "            ]\n";
    wrl << "        }\n";
    wrl << "        coordIndex [\n";
    wrl << "            ";
    for(uint i=0; i<scnPoints.size(); i++)
        wrl << i << ", ";
    wrl << "-1,\n      ]\n";
    wrl << "        texCoord TextureCoordinate {\n";
    wrl << "            point [\n";

    std::vector<Vector2d>::iterator iter2d;
    for(iter2d = texPoints.begin(); iter2d != texPoints.end(); iter2d++)
    {
        Vector2d texPt = *iter2d;
        wrl << "            " << texPt(0) << " " << texPt(1) << " " << ",\n";
    }
    wrl << "            ]\n";
    wrl << "        }\n";
    wrl << "        texCoordIndex [\n";
    wrl << "            ";
    for(uint i=0; i<texPoints.size(); i++)
        wrl << i << ", ";
    wrl << "-1,\n      ]\n";
    wrl << "        solid FALSE\n";
    wrl << "    }\n";
    wrl << "}\n";
    wrl << "    ]\n";
    wrl << "}";
}

void patchConfirmDialog::cancel()
{
    this->close();
}
