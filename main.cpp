#include "mainwindow.h"
#include <QApplication>

#include <opencv2/opencv.hpp>

int main(int argc, char *argv[])
{
    cv::Mat b = imread("C:\\Users\\mzd\\Desktop\\a.png");
    cv::imshow("nihao", b);
    cv::waitKey(0);
    return 0;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
