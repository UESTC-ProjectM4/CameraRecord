#include "widget.h"
#include "ui_widget.h"

#include <QtGui>
#include "widget.h"
#include "videodevice.h"

/**
  * @date 20-7-13 @author lzh
  */
#include <QDebug>
#include <QIODevice>
#include <QDataStream>


extern "C"
{
#include <stdio.h>
#include <stdlib.h>
}
ProcessImage::ProcessImage(QWidget *parent):QWidget(parent)
        //ui(new Ui::Widget)
{
    /**
      * @author anonymous
      * @brief get buffer size(uchar*) from QWidget->geometry
      * @note image must also be 32-bit aligned
      */
    pp = (unsigned char *)malloc(320 * 240/*QWidget::width()*QWidget::height()*/* 3 * sizeof(char));
    replay_pp = (unsigned char *)malloc(320 * 240/*QWidget::width()*QWidget::height()*/* 3 * sizeof(char));
    painter = new QPainter(this);   //Constructs a painter that begins painting the paint device immediately.

    /**
      * @date 20-7-13 @author lzh
      * @brief add 3 operation buttons next to videocapture
      */
    Startbutton = new QPushButton(this);
    Endbutton = new QPushButton(this);
    Replaybutton = new QPushButton(this);

    //type conversion from uchar* to QImage
    frame = new QImage(pp,320,240,QImage::Format_RGB888);

    replay_frame = new QImage(replay_pp,320,240,QImage::Format_RGB888);

    /**
      * @author anonymous
      * @brief connect camera and message warning
      * @note The label display video
      */
    label = new QLabel();
    vd = new VideoDevice(tr("/dev/video0"));

    connect(vd, SIGNAL(display_error(QString)), this,SLOT(display_error(QString)));
    rs = vd->open_device();
    if(-1==rs)
    {
        QMessageBox::warning(this,tr("error"),tr("open /dev/dsp error"),QMessageBox::Yes);
        vd->close_device();
    }

    rs = vd->init_device();
    if(-1==rs)
    {
        QMessageBox::warning(this,tr("error"),tr("init failed"),QMessageBox::Yes);
        vd->close_device();
    }

    rs = vd->start_capturing();
    if(-1==rs)
    {
        QMessageBox::warning(this,tr("error"),tr("start capture failed"),QMessageBox::Yes);
        vd->close_device();
    }

    if(-1==rs)
    {
        QMessageBox::warning(this,tr("error"),tr("get frame failed"),QMessageBox::Yes);
        vd->stop_capturing();
    }

    //mark Timestamp
//    timer = new QTimer(this);
//    connect(timer,SIGNAL(timeout()),this,SLOT(update()));
//    timer->start(30);

    //insert the label in lines up widgets horizontally
    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(label);

    /**
      * @date 20-7-14 @author marktlen
      * @brief 录制时间计算
      */
    recordTimer = new QTimer(this);
    connect(recordTimer,SIGNAL(timeout()),this,SLOT(updateRecordTimer()));

    minLCD = new QLCDNumber(2); //只显示两位
    secLCD = new QLCDNumber(2);
    minRecord = 0;              //初始化时间
    secRecord = 0;

    QLabel *timepoint = new QLabel(":");
    QLabel *timeTitle = new QLabel("record time:");

    QHBoxLayout *timerHLayout = new QHBoxLayout();
    timerHLayout->addWidget(timeTitle);
    timerHLayout->addWidget(minLCD);
    timerHLayout->addWidget(timepoint);
    timerHLayout->addWidget(secLCD);

    /**
      * @date 20-7-13 @author lzh marktlen
      * @brief set UI and lines up buttons and laber
      */

    //insert 3 operation buttons in lines up widgets horizontally
    QVBoxLayout *vLayout = new QVBoxLayout();
    //insert state laber
    stateLabel = new QLabel();
    vLayout->addWidget(stateLabel);
    vLayout->addLayout(timerHLayout);
    vLayout->addWidget(Startbutton);
    vLayout->addWidget(Endbutton);
    vLayout->addWidget(Replaybutton);


    //lines up laber and buttons
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addLayout(hLayout);
    mainLayout->addLayout(vLayout);

    /**
      * @author anonymous
      * @brief set layout and window title
      * @note QString QObject::tr   返回源文本的翻译版本，可以选择基于歧义消除字符串和包含复数的字符串的n值；
      *                             否则，如果没有合适的翻译字符串，则返回sourceText本身。
      */
    setLayout(mainLayout);
    setWindowTitle(tr("Capture"));

    /**
      * @date 20-7-13 @author lzh
      * @brief set button.text and connect slot
      */
    Startbutton->setText("Record");
    Endbutton->setText("End");
    Replaybutton->setText("Replay");

    connect(Startbutton,SIGNAL(clicked()),this,SLOT(on_Startbutton_clicked()));
    connect(Endbutton,SIGNAL(clicked()),this,SLOT(on_Endbutton_clicked()));
    connect(Replaybutton,SIGNAL(clicked()),this,SLOT(on_Replaybutton_clicked()));

    //初始化录制机状态
    record_flag=false;
    replay_flag=0;
    replayfileopen_flag=false;
    stateLabel->setText("state: preview");

    //不能用到按钮变灰
    Replaybutton->setEnabled(false);
    Endbutton->setEnabled(false);
}

ProcessImage::~ProcessImage()
{
    rs = vd->stop_capturing();
    rs = vd->uninit_device();
    rs = vd->close_device();
}

void ProcessImage::paintEvent(QPaintEvent *)
{
    /**
      * @date 20-7-13 @author lzh
      * @brief 录制逻辑
      */
    if(replay_flag==0)
    {
        rs = vd->get_frame((void **)&p,&len);   //采集帧
//        qDebug("Image len=%d\n",len);           //打印帧大小
        if(record_flag==true){
            fwrite(p,len,1,fp);
        }
        //色彩空间转换
        convert_yuv_to_rgb_buffer(p,pp,320,240/*QWidget::width(),QWidget::height()*/);
        //从给定二进制数据的前len个字节加载图像,自动匹配图片格式
        frame->loadFromData((uchar *)pp,/*len*/320 * 240 * 3 * sizeof(char));

        label->setPixmap(QPixmap::fromImage(*frame,Qt::AutoColor)); //label 打印图片
        rs = vd->unget_frame();                                     //关闭采集帧
    }
    else if(replay_flag==1)
    {
        if(replayfileopen_flag==false) //回看文件打开标识
        {
            fp_replay=fopen("record.yuv","r");
            File_replay.open(fp_replay,QIODevice::ReadOnly);
            replayfileopen_flag=true;
        }
        else
        {
            QByteArray block;
            QDataStream out(&block,QIODevice::WriteOnly);
            if(!File_replay.atEnd())
            {
                out<<File_replay.read(153600);
                char *temp_char;
                temp_char=block.data();
                replay_p=(uchar *)temp_char;

                convert_yuv_to_rgb_buffer(replay_p,replay_pp,320,240);
                replay_frame->loadFromData((uchar *)replay_pp,320*240*3*sizeof(char));
                label->setPixmap(QPixmap::fromImage(*replay_frame,Qt::AutoColor));
                usleep(100000);
            }
            else
            {
                QMessageBox::warning(this,tr("Replay End"),tr("File replay is end!"),QMessageBox::Yes);
                File_replay.close();

                /**
                  * @date 20-7-14 @author marktlen
                  * @brief 录制放完后继续预览，再次点击可再次回看
                  */
                replayfileopen_flag=false;
                stateLabel->setText("state: preview");
                replay_flag=0;
            }

        }


    }
}

void ProcessImage::display_error(QString err)
{
    QMessageBox::warning(this,tr("error"), err,QMessageBox::Yes);
}

/*yuv格式转换为rgb格式*/
int ProcessImage::convert_yuv_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height)
{
    unsigned int in, out = 0;
    unsigned int pixel_16;
    unsigned char pixel_24[3];
    unsigned int pixel32;
    int y0, u, y1, v;
    for(in = 0; in < width * height * 2; in += 4) {
        pixel_16 =
                yuv[in + 3] << 24 |
                yuv[in + 2] << 16 |
                yuv[in + 1] <<  8 |
                yuv[in + 0];
        y0 = (pixel_16 & 0x000000ff);
        u  = (pixel_16 & 0x0000ff00) >>  8;
        y1 = (pixel_16 & 0x00ff0000) >> 16;
        v  = (pixel_16 & 0xff000000) >> 24;
        pixel32 = convert_yuv_to_rgb_pixel(y0, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
        pixel32 = convert_yuv_to_rgb_pixel(y1, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
    }
    return 0;
}

int ProcessImage::convert_yuv_to_rgb_pixel(int y, int u, int v)
{
    unsigned int pixel32 = 0;
    unsigned char *pixel = (unsigned char *)&pixel32;
    int r, g, b;
    r = y + (1.370705 * (v-128));
    g = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
    b = y + (1.732446 * (u-128));
    if(r > 255) r = 255;
    if(g > 255) g = 255;
    if(b > 255) b = 255;
    if(r < 0) r = 0;
    if(g < 0) g = 0;
    if(b < 0) b = 0;
    pixel[0] = r * 220 / 256;
    pixel[1] = g * 220 / 256;
    pixel[2] = b * 220 / 256;
    return pixel32;
}
/*yuv格式转换为rgb格式*/

/**
  * @date 20-7-14 @author marktlen
  * @brief 录制时钟刷新槽函数，超时触发
  */
void ProcessImage::updateRecordTimer()
{
    secRecord++;
    if(secRecord >= 60*10)
    {
        secRecord = 0;
        minRecord++;
    }
    minLCD->display(minRecord);
    secLCD->display(secRecord/10);
}

void ProcessImage::on_Startbutton_clicked()
{
    fp=fopen("record.yuv","wa+");
    record_flag=true;

    Startbutton->setEnabled(false);
    Endbutton->setEnabled(true);
    Replaybutton->setEnabled(true);
    stateLabel->setText("state: recording...");

    recordTimer->start(0);   //开始计时
    minRecord = 0;
    secRecord = 0;
}
void ProcessImage::on_Endbutton_clicked()
{
    record_flag=false;
    replay_flag=0;
    replayfileopen_flag=false;

    Startbutton->setEnabled(true);
    Endbutton->setEnabled(false);
    stateLabel->setText("state: stop record");
    recordTimer->stop();    //停止计时
}
void ProcessImage::on_Replaybutton_clicked()
{
    stateLabel->setText("state: replay...");
    replay_flag=1;
}
