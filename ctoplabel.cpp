#include "ctoplabel.h"

CTopLabel::CTopLabel(int id,QWidget *parent) :
    QLabel(parent)
{
    setLabelId(id);
    setAutoFillBackground(true);

    //当前状态设置为初始化
    currentState=initShot;

    //设置为正在截图
    isShot=true;

    //初始化
    beginPoint=QPoint(0,0);
    endPoint=QPoint(0,0);
    shotRect=QRect(0,0,0,0);

    //左键按下状态
    isPressed=false;

    //初始化窗口拖动点
    dragPosition=QPoint(-1,-1);

    //设置无边框，任务栏无图标，顶置
    setWindowFlags(Qt::FramelessWindowHint|Qt::Tool|Qt::WindowStaysOnTopHint);

    //抓取屏幕
    fullScreenPixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
}


void CTopLabel::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button()==Qt::LeftButton)
    {

        isPressed=true;
        if(isShot)
        {
            //正在截图时，改变状态及beginPoint
            beginPoint=ev->pos();
            currentState=beginShot;
        }
        else
        {
            //截图完毕时
            dragPosition=ev->globalPos()-this->pos();
        }
    }
}

void CTopLabel::mouseMoveEvent(QMouseEvent *ev)
{
    if(isPressed)
    {
        if(isShot)
        {
            //截屏拖动时
            //qDebug()<<"isShot";
            endPoint=ev->pos();
            currentState=onShot;
            update();

        }
        else
        {
            //窗口移动
            move(ev->globalPos()-dragPosition);
        }

    }
}

void CTopLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    if(ev->button()==Qt::LeftButton)
    {


        if(isShot)
        {
            endPoint=ev->pos();
            update();

            //截图区域图像
            resultPixmap=fullScreenPixmap.copy(shotRect);

            //截图结果窗口
            this->setGeometry(shotRect);


            //结果复制到剪切板
            QClipboard *clipboard=QApplication::clipboard();
            clipboard->setPixmap(resultPixmap);

            //改变状态
            currentState=finishShot;
            isShot=false;

            emit shotted();

        }
        else
        {
            move(ev->globalPos()-dragPosition);

        }
    }

    isPressed=false;
}



void CTopLabel::mouseDoubleClickEvent(QMouseEvent *e)
{
    if(e->button()==Qt::LeftButton)
    {
        //关闭当前
        emit closeMe(labelId);
    }
}

void CTopLabel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    //设置画笔
    painter.setPen(QPen(Qt::blue,1,Qt::SolidLine,Qt::FlatCap));

    //根据拖动位置，计算截图区域

    //左上->右下
    if(beginPoint.x()<endPoint.x()&&beginPoint.y()<endPoint.y())
    {
        shotRect=QRect(beginPoint,endPoint);
    }

    //左下->右上
    else if(beginPoint.x()<endPoint.x()&&beginPoint.y()>endPoint.y())
    {
        shotRect=QRect(beginPoint.x(),endPoint.y(),endPoint.x()-beginPoint.x(),beginPoint.y()-endPoint.y());
    }

    //右上->左下
    else if(beginPoint.x()>endPoint.x()&&beginPoint.y()<endPoint.y())
    {
        shotRect=QRect(endPoint.x(),beginPoint.y(),beginPoint.x()-endPoint.x(),endPoint.y()-beginPoint.y());
    }

    //右下->左上
    else
    {
        shotRect=QRect(endPoint,beginPoint);
    }

    //绘制全屏
    if(currentState!=finishShot)
    {
        painter.drawText(this->rect(),Qt::AlignHCenter,tr("拖动鼠标截图"));
        painter.drawPixmap(0,0,fullScreenPixmap);
    }

    //正在拖动截屏区域
    if(currentState==onShot)
    {
        painter.drawRect(shotRect);
    }

    //完成截屏
    if(currentState==finishShot)
    {
        painter.drawPixmap(0,0,resultPixmap);
        painter.setPen(QPen(Qt::gray,1,Qt::SolidLine,Qt::FlatCap));//设置画笔
        painter.drawRect(0,0,resultPixmap.size().width()-1,resultPixmap.size().height()-1);
    }

}