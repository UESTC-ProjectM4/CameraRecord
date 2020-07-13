#ifndef PROCESSIMAGE_H
#define PROCESSIMAGE_H

#include <QtGui>
#include "videodevice.h"

class ProcessImage : public QWidget
{
    Q_OBJECT
public:
    ProcessImage(QWidget *parent=0);
    ~ProcessImage();

private:
    QPainter *painter;
    QLabel *label,*stateLabel;
    QImage *frame;


    QPushButton *Startbutton,*Endbutton,*Replaybutton; //3 operation buttons next to videocapture
    QImage *replay_frame;

    //QPixmap *frame;
    QTimer *timer;
    int rs;
    uchar *pp; //
    uchar * p;


    uchar *replay_pp;
    uchar *replay_p;



    unsigned int len;
    int convert_yuv_to_rgb_pixel(int y, int u, int v);
    int convert_yuv_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height);
    VideoDevice *vd;

    FILE *fp,*fp_replay;
    QFile File_replay;
    bool record_flag,replayfileopen_flag;
    uchar replay_flag;

private slots:
    void paintEvent(QPaintEvent *);
    void display_error(QString err);
    void on_Startbutton_clicked();
    void on_Endbutton_clicked();
    void on_Replaybutton_clicked();


};

#endif
