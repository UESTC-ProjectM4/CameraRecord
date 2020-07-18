#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QTimer>
#include <QDateTime>

#include <QThread>
#include <QPalette>

#include "opencv2/opencv.hpp"//OpenCV文件包含
using namespace cv;          //OpenCV命名空间
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //摄像头信息
    VideoCapture capture;   //视频录制驱动
    Mat frame, img;         //cv矩阵，用于存放图像信息
    QImage disImage;        //Qt图片格式
    VideoWriter videoWriter;      //视频输出器

    //人脸识别相关
    CascadeClassifier ccf;  //级联分类器
    vector<Rect> faces; //创建一个容器保存检测出来的脸
    bool captureFace;   //识别开关

    //录屏计时器
    QTimer *recordTimer; //视频录制时长计算器
    int minRecord;
    int secRecord;

    //录制状态标签
    bool record_flag;           //录制状态
    bool replayfileopen_flag;   //回放文件状态
    short replay_flag;          //回放状态

    //当前系统时间
    QDateTime datetime;
    QString strDatetime;        //设置显示日期时间字段
    QString recordDatetime;


    QPainter *painter;



private slots:

    void paintEvent(QPaintEvent *);

    void on_startButton_clicked();

    void on_endButton_clicked();

    void on_replayButton_clicked();

    void on_faceRadioButton_clicked();

    void updateRecordTimer();           //计时器刷新内容

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
