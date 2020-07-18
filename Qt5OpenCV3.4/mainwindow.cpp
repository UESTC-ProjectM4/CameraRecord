#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //让级联分类器 打开OpenCV官方训练好的人脸识别的Haar特征分类器
    ccf.load("F:\\qtWP\\qt5.7.9\\testCamera\\haarcascade_frontalface_alt2.xml");

    capture.open(0);    //录制默认的视频设备，这里我用的自己的笔记本摄像头
    captureFace = true; //默认开启人脸识别

    //初始化计时器
    minRecord = 0;
    secRecord = 0;
    recordTimer = new QTimer();
    connect(recordTimer,SIGNAL(timeout()),this,SLOT(updateRecordTimer()));
    ui->minLCD->setDigitCount(2);
    ui->secLCD->setDigitCount(2);

    //视频播放控制器
    vedioTimer = new QTimer();
    connect(recordTimer,SIGNAL(timeout()),this,SLOT(updateImageTimer()));
    vedioSec = 0;

    //初始化录制机状态
    record_flag=false;
    replay_flag=0;
    replayfileopen_flag=false;
    ui->stateLabel->setText("state: preview");

    //不能用到按钮变灰
    ui->replayButton->setEnabled(false);
    ui->endButton->setEnabled(false);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    //是否处于录制状态
    if(replay_flag == 0)
    {
        capture.read(frame);                //摄像头中读取当前帧
        if(record_flag == true)
        {
            videoWriter.write(frame);
        }

    }
    else if(replay_flag == 1)   //回放状态
    {
        if(replayfileopen_flag==false) //回看文件打开标识
        {
            capture.open(saveVideoName);
            replayfileopen_flag=true;
        }
        else
        {
            capture >> frame;
            QThread::usleep(25000);
            if(frame.empty() == true)
            {
                replayfileopen_flag=false;
                ui->stateLabel->setText("state: preview");
                replay_flag=0;
                capture.open(0);    //录制默认的视频设备
                capture.read(frame);                //摄像头中读取当前帧
            }
        }
    }

    //是否开启人脸识别
    if(captureFace)
    {
        cvtColor(frame, img, CV_BGR2GRAY);  //转换成灰度图，因为harr特征需要从灰度图中提取
        //equalizeHist(img, img);             //直方图均衡行

        /* detectMultiScale函数它可以检测出图片中所有的人脸
         * 并将人脸用vector保存各个人脸的坐标、大小（用矩形表示），函数由分类器对象调用
         */
        ccf.detectMultiScale(img, faces, 1.1, 3, 0, Size(100, 100), Size(500, 500));
        for (std::vector<Rect>::const_iterator iter = faces.begin(); iter != faces.end(); iter++)
        {
            rectangle(frame, *iter, Scalar(0, 0, 255), 2, 8); //画出脸部矩形
        }

    }

    cvtColor(frame, frame, COLOR_BGR2RGB);//OpenCV抓取的BGR格式图像格式转换Qt用的RGB格式
    //让label显示图像
    disImage = QImage((const unsigned char*)(frame.data),frame.cols,frame.rows,QImage::Format_RGB888);
    ui->label->setPixmap(QPixmap::fromImage(disImage.scaled(ui->label->size(), Qt::KeepAspectRatio)));
}

void MainWindow::on_startButton_clicked()
{
    record_flag=true;   //录制标记打开

    //按钮变动
    ui->startButton->setEnabled(false);
    ui->endButton->setEnabled(true);
    ui->stateLabel->setText("state: recording...");

    //开始计时
    minRecord = 0;
    secRecord = 0;
    recordTimer->start(0);

    //打开录制视频
    videoWriter.open(saveVideoName,VideoWriter::fourcc('M', 'J', 'P', 'G'), 30.0, Size(640, 480), true);
}

void MainWindow::on_endButton_clicked()
{
    record_flag=false;
    replay_flag=0;
    replayfileopen_flag=false;

    ui->startButton->setEnabled(true);
    ui->endButton->setEnabled(false);
    ui->replayButton->setEnabled(true);

    ui->stateLabel->setText("state: stop record");
    recordTimer->stop();    //停止计时
}

void MainWindow::on_replayButton_clicked()
{
    ui->stateLabel->setText("state: replay...");
    replay_flag=1;

    //    //回放速率控制器
    //    vedioSec = 0;
    //    vedioTimer->start();
}

void MainWindow::on_faceRadioButton_clicked()
{
    captureFace = ui->faceRadioButton->isChecked();
}

/**
  * @date 20-7-14 @author marktlen
  * @brief 录制时钟刷新槽函数，超时触发
  */
void MainWindow::updateRecordTimer()
{
    secRecord++;
    if(secRecord >= 60*10)
    {
        secRecord = 0;
        minRecord++;
    }
    ui->minLCD->display(minRecord);
    ui->secLCD->display(secRecord/10);
}

void MainWindow::updateImageTimer()
{
    vedioSec++;

    if(captureFace)
    {
        capture >> frame;
    }
    else
    {
        if(vedioSec%100)
            capture >> frame;
    }

    if(frame.empty() == true)
    {
        replayfileopen_flag=false;
        ui->stateLabel->setText("state: preview");
        replay_flag=0;
        capture.open(0);    //录制默认的视频设备
        capture.read(frame);                //摄像头中读取当前帧
        vedioTimer->stop();
    }
}
