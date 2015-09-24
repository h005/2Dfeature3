#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    image = NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_open_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                      tr("Open"),".",
                      tr("matrix Files(*.matrix)"));
    if(fileName == NULL)
        return ;
    QFileInfo fileInfo(fileName);
    QString path = fileInfo.absoluteDir().absolutePath().append("/");

    image = new Image(fileName,path);

    ui->path->resize(200,10);

    ui->info->resize(200,200);

    ui->path->setText(path);

    ui->info->setText(fileName);

    image->printFileNameList();
}

void MainWindow::on_pushButton_clicked()
{
    QString info = ui->info->text();
    image->sethist();
    info.append("set hist done\n\n");
    ui->info->setText(info);

////    image->setMoments();
    image->setMomentsOpencv();
    info.append("set Moments done\n\n");
    ui->info->setText(info);

    image->pca();
    info.append("set pca done\n\n");
    ui->info->setText(info);

    image->setGLCM();
    info.append("set GLCM done\n\n");
    ui->info->setText(info);

    image->setNumCorners();
    info.append("set num Corners done\n\n");
    ui->info->setText(info);

    image->setNumBolbs();
    info.append("set num Bolbs done\n\n");
    ui->info->setText(info);

    image->setSaliency();
    info.append("set saliency done\n\n");
    ui->info->setText(info);

    image->print();
}
