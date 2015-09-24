#include "image.h"
#include <QDebug>

#define NoProject

Image::Image(QString matrixFile, // .matrix file
             QString path) // imgs folder
{
    this->path = path;

    output = matrixFile;

    setFileNameList(matrixFile);

    NUM = fileName.count();

    hist = new MatND[NUM];

    pcaResult = Mat();

    numCorners = new double[NUM];

    numBlobs = new double[NUM];

    salientArea = new double[NUM];

    moments = new double*[NUM];
    for(int i=0;i<NUM;i++)
        moments[i] = new double[NUM_MOMENTS];

    // four features of glcm and there is three directions
    glcm = new double*[NUM];
    for(int i=0;i<NUM;i++)
        glcm[i] = new double[NUM_GLCM_FEATURES*NUM_GLCM_DIRECTONS];

}



Mat Image::grade16(Mat gray)
{
    for(int i=0;i<gray.rows;i++)
        for(int j=0;j<gray.cols;j++)
            gray.at<uchar>(i,j) = (gray.at<uchar>(i,j)&0xF0)>>4;
    return gray;
}

void Image::readIn(int index)
{
    *image = imread(fileName.at(index).toStdString());
    *gray = imread(fileName.at(index).toStdString(),0);
}

void Image::readInGray(int index)
{
    gray = new Mat();
    *gray = imread(fileName.at(index).toStdString(),0);
}

void Image::setGLCMfeatures(int t, int index, double glcmMatrix[][GLCM_CLASS])
{
    for(int i=0;i<GLCM_CLASS;i++)
    {
        for(int j=0;j<GLCM_CLASS;j++)
        {
            // entropy
            if(glcmMatrix[i][j]>0)
                glcm[t][index*4] -= glcmMatrix[i][j]*log10(glcmMatrix[i][j]);
            // energy
            glcm[t][index*4+1] += glcmMatrix[i][j]*glcmMatrix[i][j];
            // contrast
            glcm[t][index*4+2] += (i-j)*(i-j)*glcmMatrix[i][j];
            // homogenity
            glcm[t][index*4+3] += 1.0/(1+(i-j)*(i-j))*glcmMatrix[i][j];
        }
    }
}

double Image::abs0(double num)
{
    return num>0?num:-num;
}

void Image::sethist()
{
    int channels[1];
    int histSize[1];
    float hranges[2];
    const float *ranges[1];
    channels[0] = 0;
    histSize[0] = 256;
    hranges[0] = 0.0;
    hranges[1] = 255.0;
    ranges[0] = hranges;
    for(int i=0;i<NUM;i++)
    {
        readInGray(i);
        calcHist(gray,
                 1,
                 channels,
                 Mat(),
                 hist[i],
                 1,
                 histSize,
                 ranges);
        delete gray;
//        for(int j=0;j<256;j++)
//            cout<<hist[i].at<float>(j)<<" ";
//        cout<<endl;
    }


    cout<<"hist...ok"<<endl;
}

void Image::pca()
{
    // the images' size must be the same!
    Mat pcaOriginal = Mat(CoWidth*CoHeight,NUM,CV_32FC1);
    for(int i=0;i<NUM;i++)
    {
        readInGray(i);
        cv::resize(*gray,*gray,cv::Size(CoWidth,CoHeight));
        Mat grayTmp = gray->reshape(1,gray->cols*gray->rows);

        Mat tmp0 = pcaOriginal.col(i);

        grayTmp.convertTo(tmp0,CV_32FC1,1.0/255);

        delete gray;
        delete &grayTmp;
        delete &tmp0;
    }

    PCA pca0(pcaOriginal,Mat(),CV_PCA_DATA_AS_COL,5);
    pcaResult = pca0.project(pcaOriginal);

//    for(int i=0;i<pcaResult.rows;i++)
//    {
//        for(int j=0;j<pcaResult.cols;j++)
//            cout<<pcaResult.at<float>(i,j)<<" ";
//        cout<<endl;
//    }

    cout<<"pca...ok"<<endl;


    delete &pcaOriginal;
}

void Image::setMoments()
{
    for(int t=0;t<NUM;t++)
    {
        readInGray(t);
        double m00=0,m01=0,m10=0;
        double m20=0,m02=0,m11=0;
        double m30=0,m12=0,m21=0,m03=0;
        double u10=0,u01=0;
        double y00=0,y10=0,y01=0;
        double y11=0,y20=0,y02=0;
        double y30=0,y12=0,y21=0,y03=0;
        double n20=0,n02=0,n11=0;
        double n30=0,n03=0,n12=0,n21=0;
//        for(int j=0;j<nl;j++)
//        {
//            uchar* data = image->ptr<uchar>(j);
//            for(int k=0;k<nc;k++)
//                m00+=data[i];
//        }
        for(int i=0;i<gray->rows;i++)
        {
            for(int j=0;j<gray->cols;j++)
            {
                m00 += gray->at<uchar>(i,j);
                m10 += i*gray->at<uchar>(i,j);
                m01 += j*gray->at<uchar>(i,j);
                m20 += i*i*gray->at<uchar>(i,j);
                m02 += j*j*gray->at<uchar>(i,j);
                m11 += i*j*gray->at<uchar>(i,j);
                m30 += i*i*i*gray->at<uchar>(i,j);
                m03 += j*j*j*gray->at<uchar>(i,j);
                m12 += i*j*j*gray->at<uchar>(i,j);
                m21 += i*i*j*gray->at<uchar>(i,j);
            }
        }
        u01 = m01/m00;
        u10 = m10/m00;
        y00 = m00;
        y10 = 0;
        y01 = 0;
        y11 = m11-u01*m10;
        y20 = m20-u10*m10;
        y02 = m02-u01*m01;
        y30 = m30-3*u10*m20+2*u10*u10*m10;
        y12 = m12-2*u01*m11-u10*m02+2*u01*u01*m10;
        y21 = m21-2*u10*m11-u01*m20+2*u10*u10*m01;
        y03 = m03-3*u01*m02+2*u01*u01*m01;

        n20 = y20/m00/m00;
        n02 = y02/m00/m00;
        n11 = y11/m00/m00;
        n30 = y30/pow(m00,2.5);
        n03 = y03/pow(m00,2.5);
        n12 = y12/pow(m00,2.5);
        n21 = y21/pow(m00,2.5);

        moments[t][0] = n20+n02;
        moments[t][1] = (n20-n02)*(n20-n02)+4*n11*n11;
        moments[t][2] = (n30-3*n12)*(n30-3*n12)+(3*n21-n03)*(3*n21-n03);
        moments[t][3] = (n30+n12)*(n30+n12)+(n21+n03)*(n21+n03);
        moments[t][4] = (n30-3*n12)*(n30+n12)*((n30+n12)*(n30+n12)-3*(n21+n03)*(n21+n03))+
                (3*n21-n03)*(n21+n03)*(3*(n30+n12)*(n30+n12)-(n21+n03)*(n21+n03));
        moments[t][5] = (n20-n02)*((n30+n12)*(n30+n12)-(n21+n03)*(n21+n03))+4*n11*(n30+n12)*(n30+n21);
        moments[t][6] = (3*n21-n03)*(n30+n12)*((n30+n12)*(n30+n12)-3*(n21+n03)*(n21+n03))+
                (3*n12-n30)*(n21+n03)*(3*(n30+n12)*(n30+n12)-(n21+n03)*(n21+n03));

        delete gray;

//        for(int i=0;i<7;i++)
//            cout<<moments[t][i]<<" ";
//        cout<<endl;

    }
}
    // ref http://www.cnblogs.com/ronny/p/3985810.html
void Image::setMomentsOpencv()
{
    for(int t=0;t<NUM;t++)
    {
        readInGray(t);
        Moments mts = cv::moments(*gray);
        HuMoments(mts,moments[t]);
//        qDebug()<<"moments ";
//        for(int i=0;i<7;i++)
//            qDebug()<< moments[t][i]<<" ";
//        qDebug() << endl;
        delete gray;
    }
    cout<<"set moments ... done"<<endl;
}

void Image::getfeature()
{

}

void Image::setSaliency()
{
    double thresh = 200;
    memset(salientArea,0,sizeof(double)*NUM);
    for(int t=0;t<NUM;t++)
    {
        double max = 0,min = 1e30;
        readInGray(t);
        double **salient;
        salient = new double*[gray->rows];
        for(int i=0;i<gray->rows;i++)
        {
            salient[i] = new double[gray->cols];
            memset(salient[i],0,sizeof(double)*gray->cols);
        }
        for(int i=0;i<gray->rows;i++)
            for(int j=0;j<gray->cols;j++)
            {
                for(int k=0;k<256;k++)
                    salient[i][j] += abs0((double)hist[t].at<float>(k)-
                                          (double)hist[t].at<float>((int)gray->at<uchar>(i,j)));
                max = max>salient[i][j]?max:salient[i][j];
                min = min<salient[i][j]?min:salient[i][j];
            }
        if(min==max)
        {
            salientArea[t] = 0;
            continue;
        }
        double a = 255.0/(max-min);
        double b = -255.0/(max-min)*min;
        for(int i=0;i<gray->rows;i++)
            for(int j=0;j<gray->cols;j++)
            {
                salient[i][j] = a*salient[i][j]+b;
                if(salient[i][j]>thresh)
                    salientArea[t]++;
            }

        delete gray;
//        delete salient;
        for(int i=0;i<gray->rows;i++)
            delete salient[i];

//        qDebug()<<"set salientArea ..."<<salientArea[t]<<endl;
    }
    cout<<"set salientArea...ok"<<endl;
}

//compute the GLCM of horizonal veritical and dialog
//and get the 4 features
//details in http://blog.csdn.net/carson2005/article/details/38442533
//the glcmMatirx has 16 values, so the size is 16*16
void Image::setGLCM()
{
    double glcmMatrix[GLCM_CLASS][GLCM_CLASS];

    for(int t=0;t<NUM;t++)
    {
        memset(glcm[t],0,sizeof(double)*12);
        readInGray(t);
        Mat *grade = new Mat();
        *grade = grade16(*gray);
        int width = grade->cols;
        int height = grade->rows;
//        namedWindow("grade");
//        imshow("grade",*grade);
//        horizontal
        memset(glcmMatrix,0,sizeof(glcmMatrix));
        for(int i=0;i<height;i++)
            for(int j=0;j<width;j++)
            {
                if((j+GLCM_DIS)<width)
                    glcmMatrix[grade->at<uchar>(i,j)][grade->at<uchar>(i,j+GLCM_DIS)]++;
                if((j-GLCM_DIS)>0)
                    glcmMatrix[grade->at<uchar>(i,j)][grade->at<uchar>(i,j-GLCM_DIS)]++;
            }
        setGLCMfeatures(t,0,glcmMatrix);
//        vertical
        memset(glcmMatrix,0,sizeof(glcmMatrix));
        for(int i=0;i<height;i++)
            for(int j=0;j<width;j++)
            {
                if((i+GLCM_DIS)<height)
                    glcmMatrix[grade->at<uchar>(i,j)][grade->at<uchar>(i+GLCM_DIS,j)]++;
                if((i-GLCM_DIS)>0)
                    glcmMatrix[grade->at<uchar>(i,j)][grade->at<uchar>(i-GLCM_DIS,j)]++;
            }
        setGLCMfeatures(t,1,glcmMatrix);
//        diagonal
        memset(glcmMatrix,0,sizeof(glcmMatrix));
        for(int i=0;i<height;i++)
            for(int j=0;j<width;j++)
            {
                if((i+GLCM_DIS)<height && (j+GLCM_DIS)<width)
                    glcmMatrix[grade->at<uchar>(i,j)][grade->at<uchar>(i+GLCM_DIS,j+GLCM_DIS)]++;
                if((i-GLCM_DIS)>0 && (j-GLCM_DIS)>0)
                    glcmMatrix[grade->at<uchar>(i,j)][grade->at<uchar>(i-GLCM_DIS,j-GLCM_DIS)]++;
            }
        setGLCMfeatures(t,2,glcmMatrix);
        delete gray;
        delete grade;
//        for(int i=0;i<12;i++)
//            cout<<glcm[t][i]<<" ";
//        cout<<endl;
    }
    cout<<"glcm...ok"<<endl;
}

void Image::setNumCorners()
{
    memset(numCorners,0,sizeof(double)*NUM);
    Mat *dst,*dstnorm;
    dst = new Mat();
    dstnorm = new Mat();
    int blockSize = 2;
    int apertureSize = 3;
    double k = 0.04;
    int thresh = 200;

    for(int t=0;t<NUM;t++)
    {
        readInGray(t);
        cornerHarris(*gray,*dst,blockSize,apertureSize,k,BORDER_DEFAULT);
//        normalizing
        normalize(*dst,*dstnorm,0,255,NORM_MINMAX,CV_32FC1,Mat());
//        count corners
        for(int i=0;i<dstnorm->rows;i++)
            for(int j=0;j<dstnorm->cols;j++)
                if((int) dstnorm->at<float>(i,j) >thresh)
                    numCorners[t]++;
        delete gray;
        //cout<<numCorners[t]<<endl;
    }
    delete dst;
    delete dstnorm;
    cout<<"corner...ok"<<endl;
}

void Image::setNumBolbs()
{
    memset(numBlobs,0,sizeof(double)*NUM);
    for(int t=0;t<NUM;t++)
    {
        readInGray(t);
        SimpleBlobDetector detector;
        vector<KeyPoint> keypoints;
        detector.detect(*gray,keypoints);
        numBlobs[t] = keypoints.size();
//        cout<<numBlobs[t]<<endl;
        delete gray;
    }
    cout<<"blob...ok"<<endl;
}

void Image::print()
{
    int pos = output.lastIndexOf('.');
    output.replace(pos+1,6,QString("2df"));
//    qDebug()<< output<< endl;
    freopen(output.toStdString().c_str(),"w",stdout);
    for(int i=0;i<NUM;i++)
    {
        printf("%s\n",fileName.at(i).toStdString().c_str());
        // hist
        for(int j=0;j<256;j++)
            printf("%f ",hist[i].at<float>(j));
        qDebug() << "hist...done" << endl;
        // moments
        for(int j=0;j<7;j++)
            printf("%lf ",moments[i][j]);
        qDebug() << "moments...done" << endl;
        // glcm
        for(int j=0;j<12;j++)
            printf("%lf ",glcm[i][j]);
        qDebug() << "glcm...done" << endl;
        // harris
        printf("%lf ",numCorners[i]);
        qDebug() << "harris...done" << endl;
        // blobs
        printf("%lf ",numBlobs[i]);
        qDebug() << "blobs...done" << endl;
        // salientArea
        printf("%lf ",salientArea[i]);
        qDebug() << "salientArea...done" <<endl;
        // pca
        for(int j=0;j<5;j++)
            printf("%f ",pcaResult.at<float>(i,j));
        qDebug() << "pca...done" <<endl;
        printf("\n");

    }
    fclose(stdout);
}

void Image::printFileNameList()
{
    for(int i=0;i<fileName.size();i++)
        cout<<fileName.at(i).toStdString()<<endl;
}

void Image::setFileNameList(QString matrixFile)
{
    freopen(matrixFile.toStdString().c_str(),"r",stdin);
    QString tmp;
    char tmpss[200];
    double tmpNum;
    while(scanf("%s",tmpss)!=EOF)
    {
        //  拼接路径，并消除路径中的../以及./
        QString tmpPath = path;
        tmp = QDir::cleanPath(tmpPath.append(QString(tmpss)));
        fileName.push_back(tmp);
#ifdef NoProject
        for(int i=0;i<16;i++)
            scanf("%lf",&tmpNum);
#else
        for(int i=0;i<32;i++)
            scanf*"%lf",&tmpNum);
#endif
    }
}
