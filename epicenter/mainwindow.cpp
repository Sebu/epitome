#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QGLWidget>
#include <opencv/cv.h>
#include <opencv/highgui.h>


#include "glwidget.h"
#include <epicore/patch.h>
#include <epicore/cv_ext.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    debugWidgetL =  new AlbumWidget(ui->imageWidget);
    debugWidgetR =  new AlbumWidget(ui->otherWidget);

    this->connect( ui->loadButton, SIGNAL(clicked()),         this, SLOT(changeImage()) );
    this->connect( ui->saveButton, SIGNAL(clicked()),         this, SLOT(saveImage()) );
    this->connect( ui->calcButton, SIGNAL(clicked()),         this, SLOT(calculate())  );
    this->connect( ui->stepButton, SIGNAL(clicked()),         this, SLOT(step())       );
    this->connect( ui->prevButton, SIGNAL(clicked()), debugWidgetL, SLOT(prev())        );
    this->connect( ui->nextButton, SIGNAL(clicked()), debugWidgetL, SLOT(next())        );



}



void MainWindow::changeImage() {

    fileName = QFileDialog::getOpenFileName(this,tr("Open Image"), "/home/seb/Bilder", tr("Image Files (*.png *.jpeg *.jpg *.bmp)"));
    if(fileName=="") return;


    image = cv::imread( fileName.toStdString() );
    debugWidgetL->fromIpl( image, "image" );

}

void MainWindow::step() {
    for (int i=0; i<ui->stepSpin->value(); i++)
        singleStep();
}

bool MainWindow::singleStep() {
    static int x = 0;
    static int y = 0;
    static int maxX, maxY;

    if(x==0 && y==0) {
        
        this->seedmap = new SeedMap( image, ui->blockSpin->value(), ui->blockSpin->value(), ui->seedSpin->value(), ui->seedSpin->value());
        seedmap->maxError = ui->errorSpin->value();
        
        int w = ui->blockSpin->value();
        int h = ui->blockSpin->value();

        maxX = (image.cols / w);
        maxY = (image.rows / h);
    }

    Patch* patch = seedmap->getPatch(x,y);
    seedmap->match(*patch);

    if (!patch->matches->empty()) {

        cv::Mat warped = seedmap->sourceImage.clone(); //transform->warp();
        cv::Mat warpInv = cv::Mat::eye(3,3,CV_64FC1);
        cv::Mat selection( warpInv, cv::Rect(0,0,3,2) );
        cv::Mat rotInv;
        // highlight block
        cv::rectangle(warped, cv::Point(patch->x_, patch->y_),
                      cv::Point(patch->x_+patch->w_, patch->y_+patch->h_),
                      cv::Scalar(0,255,0,100),2);

        for(uint i=0; i<patch->matches->size(); i++) {
            Transform* current = patch->matches->at(i);

            double points[4][2] = { {current->seed->x_, current->seed->y_},
                                    {current->seed->x_+current->seed->w_, current->seed->y_},
                                    {current->seed->x_+current->seed->w_, current->seed->y_+current->seed->h_},
                                    {current->seed->x_,    current->seed->y_+current->seed->h_}
            };

            cv::Point newPoints[4];

            invertAffineTransform(current->warpMat, selection);
            invertAffineTransform(current->rotMat, rotInv);

            for(int i=0; i<4; i++ ) {
                cv::Mat p = (cv::Mat_<double>(3,1) << points[i][0], points[i][1], 1);

                cv::Mat a =  rotInv * (warpInv * p);

                newPoints[i].x = a.at<double>(0,0);
                newPoints[i].y = a.at<double>(0,1);

            }
            // highlight match
            for(int i=0; i<4; i++){
                cv::line(warped, newPoints[i], newPoints[(i+1) % 4], cv::Scalar(0,0,255,100));
            }
            cv::line(warped, newPoints[0], newPoints[1], cv::Scalar(255,0,0,100));

        }
        debugWidgetR->fromIpl( warped, "preview" );
    }

    cv::Mat reconstruction(seedmap->debugReconstruction());

    debugWidgetL->fromIpl( reconstruction, "reconstruction" );

    debugWidgetL->updateGL();
    debugWidgetR->updateGL();



    x++;
    if(x>=maxX) { x=0; y++; }
    if(y>=maxY) { y=0; return false; }

    return true;
}

void MainWindow::calculate() {
    calculateTimed();
}

void MainWindow::calculateTimed() {


    while(singleStep()) {}

    // FANCY DEBUG outputs
    cv::Mat reconstruction(seedmap->debugReconstruction());
    cv::Mat error( image.cols, image.rows, CV_8UC1);
    error = image - reconstruction;

    debugWidgetL->fromIpl( error, "error");

    debugWidgetL->update();

}

void MainWindow::saveImage() {
    QString saveName = QFileDialog::getSaveFileName(this,tr("Save Image"), "reconstruction/", tr("Image Files (*.png *.jpeg *.jpg *.bmp)"));
    seedmap->saveReconstruction(saveName.toStdString());
}

MainWindow::~MainWindow()
{
    delete ui;
}