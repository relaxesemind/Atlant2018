#include "mgraphics.h"
#include "single_area.h"
#include "core.h"

#include <QDebug>
#include <QList>
#include <QToolTip>
#include <QApplication>
#include <QImage>
#include <QLineF>
#include <QPointF>
#include <QVector2D>
#include <cmath>
#include <random>
#include <chrono>
#include <QThread>
#include <QFloat16>

//declaration of constants
//
const qint32 SLIDER_X_POS = 8;
const qint32 SLIDER_Y_POS = 8;
const qint32 SLIDER_WIDTH = 200;
const qint32 SLIDER_HEIGHT = 20;
const qint32 BIN_BLACK = 0x0;
const quint32 BIN_WHITE = 0xFFFFFF;
const quint32 ARGB_A = 0xFF000000;
const qreal zoomMultiple = 1.05;
const QRgb default_color = qRgb(0,145,218);
const QRgb black = qRgb(0,0,0);
//global variables for drawing logic
quint64 ID_onDraw; //current objects id on draw
QPoint prevPoint;
QPoint StartPoint;
QPoint EndPoint;
QStack<QLineF> lines;
QStack<QGraphicsLineItem*> line_items;
//
//--------------------------
//random colors factory
//
template
< class engine = std::default_random_engine,
  class distribution = std::uniform_int_distribution<uint>
> class GenColor
{
    const uint MIN = 0x1;
    const uint MAX = 0xFFFFFF;
    engine re;
    distribution dist;
public:
    GenColor():dist(MIN,MAX)
    {
        auto s = std::chrono::system_clock::now().time_since_epoch().count();
        re.seed(s);
    }

    operator QRgb()
    {
       return static_cast<QRgb> (*this());
    }

    auto operator()()
    {
       return dist(re);
    }
};
//

void MGraphics::recalculation()
{
    // if image exist
    // Create new thread and new Worker
    // Worker is class which forms vector of objects from bin image
//    // all is safe and auto self-destroying
//         if (b_img.isNull()) return;

//         this->setthread_ON_WORK(true);
//         QThread* thread = new QThread();
//         Core*    task = new Core(b_img);
//    //--------------------------------------------------------------------
//         task -> moveToThread(thread);
//    //--------------------------------------------------------------------
//         this->setEnabled(false);

//         connect(thread,&QThread::started,
//                 task, &Core::select,Qt::DirectConnection);
//         connect(task,&Core::SelectingFinished,
//                 thread,&QThread::quit,Qt::DirectConnection);
//         connect(task,&Core::SelectingFinished,
//                 this,[=](){
//             this->setEnabled(true);
//         });
//         //automatically delete thread and task object when work is done:
//         connect(thread,&QThread::finished,
//                 task,&Core::deleteLater,Qt::DirectConnection);
//         connect(thread,&QThread::finished,
//                 thread,&QThread::deleteLater,Qt::DirectConnection);
//         connect(thread,&QThread::finished,this,[=](){
//             this->setthread_ON_WORK(false);
//         });
//    //--------------------------------------------------------------------
//         thread->start();
}

MGraphics::MGraphics():thickness_pen(2), cursor_mode(0),drawingFlag(false),ColorObj(default_color)
{//geometry and logics of scene
    this->setAlignment(Qt::AlignCenter);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setMinimumHeight(100);
    this->setMinimumWidth(100);
    this->setCursor(Qt::ArrowCursor);
    this->setMouseTracking(true);
    QPainter p;
    this->drawBackground(&p,this->rect());
    this->setScene(&scene);

    Slider = std::make_unique<QSlider> (Qt::Horizontal,this);
    OSlider = std::make_unique<QSlider> (Qt::Horizontal,this);
    txt = std::make_unique<QLabel>("threshold level",this);
    otxt = std::make_unique<QLabel> ("Opacity level",this);

    OSlider->setMaximum(100);
    OSlider->setMinimum(0);
    OSlider->setValue(100);
    Slider->setMaximum(255);
    Slider->setMinimum(0);

    scene.addWidget(Slider.get());
    scene.addWidget(OSlider.get());
    scene.addWidget(txt.get());
    scene.addWidget(otxt.get());

    Slider->setGeometry(SLIDER_X_POS,SLIDER_Y_POS,SLIDER_WIDTH,SLIDER_HEIGHT);
    OSlider->setGeometry(SLIDER_X_POS,SLIDER_Y_POS + SLIDER_HEIGHT,SLIDER_WIDTH,SLIDER_HEIGHT);
    txt->setGeometry(SLIDER_X_POS + SLIDER_WIDTH + 3,SLIDER_Y_POS - 3,SLIDER_WIDTH - 50,SLIDER_HEIGHT);
    otxt->setGeometry(SLIDER_X_POS + SLIDER_WIDTH + 3,SLIDER_Y_POS - 3 + SLIDER_HEIGHT,SLIDER_WIDTH - 50,SLIDER_HEIGHT);
    txt->setStyleSheet("QLabel{color: rgba(0, 0, 0, 200); background-color : rgba(0, 0, 0, 50);}");
    otxt->setStyleSheet("QLabel{color: rgba(0, 0, 0, 200); background-color : rgba(0, 0, 0, 50);}");

    connect(Slider.get(),SIGNAL(valueChanged(int)),this,SLOT(Slider_Change(int)));
    connect(OSlider.get(),SIGNAL(valueChanged(int)),this,SLOT(OSlider_Change(int)));
    connect(Slider.get(),SIGNAL(sliderReleased()),this,SLOT(Slider_Release()));

    draw_pix.load(QCoreApplication::applicationDirPath() + "/res/draw_cur.png");
    erase_pix.load(QCoreApplication::applicationDirPath() + "/res/erase_cur.png");
}

void MGraphics::setCursor_mode(char mode)
{
    switch (mode) {
    case 0:
        this->setCursor(Qt::ArrowCursor);
        break;
    case 1:
        this->setCursor(QCursor(draw_pix,0,draw_pix.height()));
        break;
    case 2:
        this->setCursor(QCursor(erase_pix,0,erase_pix.height()));
        break;
    }
    cursor_mode = mode;
}

void MGraphics::load_from_file(const QString& path)
{//load image to scene
 //antistacking defs
 //image fits in center of scene
 //now source image contained in QPixmap pm
    PXtoNull(std::move(titem));
    PXtoNull(std::move(track_item));
    PXtoNull(std::move(randItem));
//
    pm.load(path);
//
    newPX(std::move(sourceItem),pm);
    MGraphics::centerOn(sourceItem.get());
    MGraphics::fitInView(sourceItem.get(),Qt::KeepAspectRatio);
    update();
}

void MGraphics::OSlider_Change(int value)
{//Opacity of threshold image
 //between 0.0 (transparent) and 1.0 (opaque)
    if (titem && titem->scene() == &scene)
    {
        qreal opacity = qreal(value) / 100;
        otxt->setText("Opacity level is " + QString::number(value));
        titem->setOpacity(opacity);
    }
}

uint toGrayScale(const QImage& image,qint32 x ,qint32 y)
{//I = 0.2125R + 0.7154G + 0.0721B
 //return GrayScale pixel(x,y) from image with half precision
 //mostly fast;
    QRgb q = image.pixel(x,y);
    qfloat16 half_precision = qRed(q) * 0.212F + qGreen(q) * 0.715F + qBlue(q) * 0.0721F;
    return static_cast<uint> (half_precision);
}

QImage Bradley_Rot(const QImage& src)
{//main idea from this : https://habrahabr.ru/post/278435
 //my own implementation

    const qint32 w = src.width();
    const qint32 h = src.height();
    QImage ret_img(w,h,src.format());
    const qint32 S = w / 8;
    const float t = 0.15F;
    qint32 s2 = S / 2;
    //get integral_image
    //S(x, y) = I(x, y) + S(x-1, y) + S(x, y-1) – S(x-1, y-1);
    MGraphics::Labels integral_image
            (h,MGraphics::Labels_row(w,0));
    for (qint32 x = 0; x < w; ++x) integral_image[0][x] = toGrayScale(src,x,0);
    for (qint32 y = 0; y < h; ++y) integral_image[y][0] = toGrayScale(src,0,y);

    for (qint32 y = 1; y < h; ++y)
    {
        for (qint32 x = 1; x < w; ++x)
        {
            integral_image[y][x] = toGrayScale(src,x,y) + integral_image[y][x-1]
                    + integral_image[y-1][x] - integral_image[y-1][x-1];
        }
    }

    for (qint32 y = 0; y < h; ++y)
    {
        for (qint32 x = 0; x < w; ++x)
        {
            qint32 x1 = x - s2;
            qint32 x2 = x + s2;
            qint32 y1 = y - s2;
            qint32 y2 = y + s2;

            if (x1 < 0) x1 = 0;
            if (x2 >= w) x2 = w - 1;
            if (y1 < 0) y1 = 0;
            if (y2 >= h) y2 = h-1;

            qint32 count = (x2-x1)*(y2-y1);
            //S(x, y) = S(A) + S(D) – S(B) – S(C)
            qint32 sum = integral_image[y2][x2] - integral_image[y1][x2]
                    - integral_image[y2][x1] + integral_image[y1][x1];
            if ((toGrayScale(src,x,y) * count)
                    < (sum * (1.0F - t)))
            {
                ret_img.setPixel(x,y,BIN_BLACK);
            }else
            {
                ret_img.setPixel(x,y,BIN_WHITE);
            }

        }
    }
    return ret_img;
}

QImage threshold_img(const QImage& source_img, qint32 threshold_value)
{//by threshold value
 //return new image
    qint32 const h = source_img.height();
    qint32 const w = source_img.width();
    QImage ret_img(w,h,source_img.format());

    for (qint32 i = 0;i < w; ++i)
      for(qint32 j = 0;j < h; ++j)
      {
         QRgb p = source_img.pixel(i,j);
         if (qRed(p) + qGreen(p) + qBlue(p)
                 >= threshold_value * 3)
          {
              ret_img.setPixel(i,j,BIN_WHITE);
          }else{
              ret_img.setPixel(i,j,BIN_BLACK);
          }
      }
    return ret_img; //is std::move(ret_img);
}

void MGraphics::Slider_Change(qint32 value)
{//Slider info
    txt->setText(QString("threshold level is ")
                 + QString::number(value));
}

void MGraphics::Slider_Release()
{//threshold by sliders move
    if (pm.isNull()) return;
    Ctrl_Z.clear();
    Ctrl_Y.clear();

    b_img = threshold_img(pm.toImage(),Slider->value());
    newPX(std::move(titem),b_img);
}

inline bool MGraphics::on_img(qint32 x,qint32 y) const
{//return true if (x,y) is pixmap point
    return bool (!((x < 0)||(y < 0)||
                   (x + 1 > pm.width())||
                   (y + 1 > pm.height())));
}
inline bool MGraphics::on_img(QPoint p) const
{
    return bool (!((p.x() < 0)||(p.y() < 0)||
                   (p.x() + 1 > pm.width())||
                   (p.y() + 1 > pm.height())));
}

void MGraphics::ObjectsColorChange(QColor col)
{//setter
    ColorObj = col;
}

bool MGraphics::dataIsReady() const
{
   return bool(!data_01.empty() && !data_obj.empty() && data_01.size() == pm.height());
}

void MGraphics::ShowObjectUnderCursor(QMouseEvent *event)
{//highlight
    QPoint p = transform(event->pos()); //current pos
    qint32 x = p.x();
    qint32 y = p.y();
    if (dataIsReady() && cursor_mode != 2)
    {
        auto id = data_01[y][x];
        if (id){
            //create temp empty image "track"
            QImage track(pm.width(),pm.height(),QImage::Format_ARGB32);
            track.fill(qRgba(0, 0, 0, 0));
            //fill "track" by object points
            for (const QPoint& p : data_obj[id - 1].Points) {
               track.setPixel(p,ColorObj.rgb());
             }
            newPX(std::move(track_item),std::move(track));
            //tooltip is info under cursor
            QToolTip::showText(event->globalPos(),QString("id = " + QString::number(data_01[y][x])));
        }else QToolTip::showText(event->globalPos(),QString("(" + QString::number(x) +
                                                            "," + QString::number(y) + ")"));
    }
}

bool MGraphics::PXtoNull(pItem&& item)
{//return true if item was 0 before call

    if (item)
    {
        item = nullptr;
        return false;
    } else return true;
}

void MGraphics::newPX(pItem&& item,const QPixmap& pix)
{
    item = std::make_unique<MGraphics::PXitem> (pix);
    scene.addItem(item.get());
    if (item == titem)
    {
        item->setOpacity(static_cast<qreal>(OSlider->value())/100);
    }
    update();
}

void MGraphics::newPX(pItem&& item,const QImage& img)
{
    item = std::make_unique<MGraphics::PXitem>
            (QPixmap::fromImage(img));
    scene.addItem(item.get());
    if (item == titem)
    {
        item->setOpacity(static_cast<qreal>(OSlider->value())/100);
    }
    update();
}

void MGraphics::setthread_ON_WORK(bool value)
{
    thread_ON_WORK = value;
}

void MGraphics::newPX(pItem&& item, QImage&& img)
{
    item = std::make_unique<MGraphics::PXitem>
            (QPixmap::fromImage(std::move(img)));
    scene.addItem(item.get());
    if (item == titem)
    {
        item->setOpacity(static_cast<qreal>(OSlider->value())/100);
    }
    update();
}

QPoint MGraphics::transform(QPoint pos) const
{//global cursor pos to local_scene coordinated
    QPointF l = mapToScene(pos.x(),pos.y());
    qint32 x = static_cast<qint32> (l.x());
    qint32 y = static_cast<qint32> (l.y());
    return QPoint(x,y);
}

void MGraphics::mouseMoveEvent(QMouseEvent *event)
{
    QPoint p = transform(event->pos());
    if (!on_img(p)){
        PXtoNull(std::move(track_item));
        return;
    }
    ShowObjectUnderCursor(event);
//----------------------------------------------------------------------------------------------------------------------

    if (drawingFlag && cursor_mode == 1)//draw
    {
       QLineF line(prevPoint.x(),prevPoint.y(),p.x(),p.y());
       line_items.push(scene.addLine(line,QPen(QBrush(ColorObj),
                       2*static_cast<qreal>(thickness_pen),Qt::SolidLine,Qt::RoundCap)));
       lines.push(line);
       prevPoint = p;
    }
    if (drawingFlag && cursor_mode == 2)//erase
    {
        QLineF line(prevPoint.x(),prevPoint.y(),p.x(),p.y());
        line_items.push(scene.addLine(line,
         QPen(QBrush(QColor(Qt::white)),2*static_cast<qreal>(thickness_pen),Qt::SolidLine,Qt::RoundCap)));
        lines.push(line);
        prevPoint = p;
    }
}

bool MGraphics::decide_to_draw(QPoint p) const
{
    bool result;
    switch (cursor_mode) {
    case 1:
        result = data_01[p.y()][p.x()] > 0 ? true : false;
        break;
    case 2:
        result = true; //data_01[p.y()][p.x()] > 0 ? false : true;
        break;
    default:
        result = false;
        break;
    }
    return result;
}

void MGraphics::mousePressEvent(QMouseEvent *event)
{
    QPoint p = transform(event->pos());
    if (!on_img(p) || !dataIsReady()) { return; }
//----------------------------------------------------------------------------------------------------------------------
    lines.clear();
    line_items.clear();
    PXtoNull(std::move(randItem));
    drawingFlag = decide_to_draw(p);
    if (drawingFlag)
    {
      //save start point
        ID_onDraw = data_01[p.y()][p.x()];
        StartPoint = prevPoint = p;
    }
}

auto get_prime(qint32 Radius)
{//getter min drawing primitive
   QVector<QPoint> res;
   for (qint32 y = -Radius; y < Radius; ++y)
       for (qint32 x = -Radius; x < Radius; ++x)
       {
           if (std::pow(x,2) + std::pow(y,2)
                   <= std::pow(Radius,2))
           {
               res.push_back(QPoint(x,y));
           }
       }
   return res;
}

class SecureDrawing
{//Class secure safely drawing
    qint32 w;
    qint32 h;
    QPoint pt;
    uint clr;
    QImage& ref;
    bool Valid(QPoint point)
    {
        return point.x() >= 0 && point.y() >= 0 &&
               point.x() < w && point.y() < h;
    }
public:
    explicit SecureDrawing(QImage& image, QPoint draw, uint color):
       w(image.width()), h(image.height()),pt(draw),clr(color), ref(image){}
    SecureDrawing() = default;
    ~SecureDrawing()
    {
        if (Valid(pt))
          ref.setPixel(pt,clr);
    }
};

void drawLineOnQImage(QImage& img,QPointF p1,QPointF p2, const uint color, qint32 thickness = 2)
{
    QVector2D n(p2 - p1);
    qint32 len = static_cast<qint32> (n.length());
    n.normalize();
    QVector2D v(p1);
    auto prime = get_prime(thickness);
    while (len--)
    {
        v += n;
        for (auto&& p : prime)
        {
           SecureDrawing pixel
                   (img,p + v.toPoint(),color);
        }
    }
}

inline bool isBlack(qint32 x, qint32 y, const QImage& im)
{
    return bool(im.pixel(x,y) == black);
}

void fill_area(QImage& img, QPoint Start_point)
{//fill closed area, start with Start_point

  QStack<QPoint> depth;
  depth.push(Start_point);
  const qint32 w = img.width();
  const qint32 h = img.height();

  while (!depth.empty())
  {
    QPoint t = depth.pop();
    qint32 x = t.x();
    qint32 y = t.y();
    img.setPixel(t,BIN_BLACK);

    if((x + 1 < w)&&(!isBlack(x+1,y,img)))
    {
        depth.push(QPoint(x+1,y));
    }
    if((x - 1> -1)&&(!isBlack(x-1,y,img)))
    {
        depth.push(QPoint(x-1,y));
    }
    if((y + 1< h)&&(!isBlack(x,y+1,img)))
    {
        depth.push(QPoint(x,y+1));
    }
    if((y - 1> -1)&&(!isBlack(x,y-1,img)))
    {
        depth.push(QPoint(x,y-1));
    }
  }
}

QPoint getStartPoint(const QImage& img, QPoint CenterMass)
{
   return CenterMass;
}

QPoint MGraphics::drawCurve_andGetCenter(QImage& img)
{
    quint32 N = 0;
    qreal avgX = 0.0; // average X coordinate
    qreal avgY = 0.0; // average Y coordinate
    //QVector<QPoint> ConvexPolygon;

    while (!line_items.empty())//size line_items == size lines
    {
        //reWrite bin image
        drawLineOnQImage(img,lines.top().p1(),lines.top().p2(),
                         cursor_mode == 1 ? BIN_BLACK : BIN_WHITE,thickness_pen);
        avgX += lines.top().p1().x();
        avgY += lines.top().p1().y();
        //remove temp lines from scene
       scene.removeItem(line_items.top());
       lines.pop(); line_items.pop();
       ++N;
    }
    N = N > 0 ? N : 1;
    avgX /= N;
    avgY /= N;
   return QPoint(static_cast<int>(avgX),static_cast<int>(avgY));
}

bool MGraphics::isCorrectRelease(QMouseEvent *event)
{// logic for draw and erase cursors
 //
    drawingFlag = false;
    QPoint p = transform(event->pos());
    qint32 x = p.x(); qint32 y = p.y();
    EndPoint = p;
    if (cursor_mode == 0) return false;
    if (!on_img(x,y) || !dataIsReady()) {
        while (!line_items.empty())
        {
          scene.removeItem(line_items.top());
          line_items.pop();
        }
        return false;
    }
    if (cursor_mode == 1 && ID_onDraw != data_01[y][x])
    {
        while (!line_items.empty())
        {
          scene.removeItem(line_items.top());
          line_items.pop();
        }

        return false;
    }
   return true;
}

void MGraphics::mouseReleaseEvent(QMouseEvent *event)
{
    if (!isCorrectRelease(event)) return;

    QImage red(std::move(b_img));
    Ctrl_Z.push(red);//push before drawing

    QPoint centerMass = drawCurve_andGetCenter(red);

    if (cursor_mode == 1){
    fill_area(red,
              getStartPoint(red,centerMass));
    }

    b_img = red;
    newPX(std::move(titem),std::move(red));
}

void MGraphics::drawBackground(QPainter *painter, const QRectF &rect)
{
    //119'136'153	#778899	Светлый синевато-серый	Lightslategray
    //230'230'250	#e6e6fa	Бледно-лиловый	Lavender
    //211'211'211	#d3d3d3	Светло-серый	Lightgray
   QColor col(211,211,211);
   painter->fillRect(rect,col);
}

void MGraphics::backward()
{
    if (Ctrl_Z.empty()) return;
    Ctrl_Y.push(b_img);
    b_img = Ctrl_Z.pop();
    newPX(std::move(titem),b_img);
}
void MGraphics::forward()
{
    if (Ctrl_Y.empty()) return;
    Ctrl_Z.push(Ctrl_Y.top());
    b_img = Ctrl_Y.pop();
    newPX(std::move(titem),b_img);
}

void MGraphics::autoThreshold()
{
    if (pm.isNull()) return;

    Ctrl_Z.clear();
    Ctrl_Y.clear();

    b_img = Bradley_Rot(pm.toImage());
    newPX(std::move(titem),b_img);
}


void MGraphics::keyPressEvent(QKeyEvent *e)
{
    if (e->key() && e->modifiers() & Qt::ControlModifier)
    {
        switch (e->key()) {
        case Qt::Key_Z: backward(); break;
        case Qt::Key_Y: forward(); break;
        case Qt::Key_T: autoThreshold(); break;
        case Qt::Key_O: MGraphics::load_from_file(getFileName(files::Load)); break;
        case Qt::Key_R: MGraphics::recalculation(); break;
        }
    }
}

void MGraphics::RandomColorize()
{
    if (!dataIsReady() || pm.isNull() || thread_ON_WORK) return;

    QImage mask(pm.width(),pm.height(),QImage::Format_ARGB32);
    mask.fill(qRgba(0, 0, 0, 0));
    GenColor<> gen;

    for (const S_area& obj : data_obj)
    {
        uint objColor = gen();
        for (const QPoint& p : obj.Points)
        {
            mask.setPixel(p,ARGB_A + objColor);
        }
        for (const QPoint& p : obj.CPoints)
        {
            mask.setPixel(p,ARGB_A + BIN_BLACK);
        }
    }
    newPX(std::move(randItem),std::move(mask));
}

void MGraphics::wheelEvent(QWheelEvent *event)
{//zooming by wheel
    //
    if (pm.isNull() || scene.items().empty()) return;

    QPoint numDegrees = event->angleDelta() / 8;
    if (numDegrees.y() > 0)
    {
        this->scale(zoomMultiple,zoomMultiple);
    }else
    {
        this->scale(1/zoomMultiple,1/zoomMultiple);
    }
}

void MGraphics::setThickness(qint32 number)
{
   thickness_pen = number;
}









