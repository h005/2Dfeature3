#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>
#include <opencv.hpp>
#include <QString>
#include <QDir>
#include <QStringList>
#include <math.h>

using namespace std;
using namespace cv;

#define GLCM_DIS 3
#define GLCM_CLASS 16
#define NUM_MOMENTS 7
#define NUM_GLCM_FEATURES 4
#define NUM_GLCM_DIRECTONS 3

#define CoWidth 600
#define CoHeight 800

class Image
{
public:
    Image();

private:
    QString path;

    QString output;

    Mat *image,*gray;

    //hist
    MatND *hist;

    Mat pcaResult;
    // invariant Moments
    double **moments;

    double *salientArea;

    double **glcm;

    int NUM;

    double *numCorners;

    double *numBlobs;

    QStringList fileName; // absoluate path

private:
    Mat grade16(Mat gray);

    void readIn(int index);

    void readInGray(int index);

    //    t means the number of image
    //    index means the direction of GLCM
    //    glcmMatrix means the glcm matrix
    void setGLCMfeatures(int t,int index,double glcmMatrix[][GLCM_CLASS]);

    double abs0(double num);

    void setFileNameList(QString matrixFile);

public:

    Image(QString matrixFile // .matrix file
           ,QString path);

    void sethist();

    void pca();

    void setMoments();

    void setMomentsOpencv();

    void getfeature();

    void setSaliency();

    void setGLCM();

    void setNumCorners();

    void setNumBolbs();

    void print();

    void printFileNameList();

};

#endif // IMAGE_H
