#ifndef MGRAPHICS_H
#define MGRAPHICS_H

#include "single_area.h"

#include <memory>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QVector>
#include <QStack>
#include <QSlider>
#include <QLabel>
#include <QPoint>
#include <QString>
#include <QColor>
#include <QFileDialog>
#include <QDate>
#include <QTime>

enum files : int {Load, Save};

const QString Data_dir = QString("DATA/DATA ")
        + QDate::currentDate().toString(QString("dd_MM_yyyy")) + QString(" ")
            + QTime::currentTime().toString(QString("hh_mm_ss"));

static auto getFileName = [](auto type)->
auto {
    switch (type) {
    case files::Load: return QFileDialog::getOpenFileName
                (nullptr, "Select image", "", "*.jpg *.jpeg *.bmp *.png");
        break;
    case files::Save: return QFileDialog::getSaveFileName
                (nullptr,"SAVE DATA",Data_dir,"*.dat");
        break;
    default:
        return QString();
        break;
    }
};


class MGraphics : public QGraphicsView
{
    Q_OBJECT
    using PXitem = QGraphicsPixmapItem;
    using pItem = std::unique_ptr<PXitem>;
    using pSlider = std::unique_ptr<QSlider>;
    using pLabel = std::unique_ptr<QLabel>;
    using Ellipse = std::unique_ptr<QGraphicsEllipseItem>;
public:
    using Labels = QVector<QVector<quint64>>;
    using Labels_row = QVector<quint64>;
    using Objects = QVector<S_area>;
    MGraphics();

   static Labels data_01; // HERE LABELS DATA
   static Objects data_obj; // HERE OBJECTS

   const QImage& get_bin_img()const
   {//getter of threshold image
       return b_img;
   }
   void backward(); //ctrl_z
   void forward(); //ctrl_y
   void autoThreshold();//breadley-rot
   bool dataIsReady()const;//true if data was calc
   void setthread_ON_WORK(bool value);


protected:
  void mouseMoveEvent(QMouseEvent *event)override;
  void mousePressEvent(QMouseEvent *event)override;
  void mouseReleaseEvent(QMouseEvent *event)override;
  void drawBackground(QPainter *painter, const QRectF &rect)override;
  void keyPressEvent(QKeyEvent *event)override;
  void wheelEvent(QWheelEvent *event)override;

public slots:

  void load_from_file(const QString&);
  void setCursor_mode(char);
  void ObjectsColorChange(QColor);
  void RandomColorize();
  void setThickness(int);

private slots:

  void Slider_Change(int);
  void OSlider_Change(int);
  void Slider_Release();

private:
  bool on_img(qint32, qint32)const; //predicate : true if cursor on image
  bool on_img(QPoint)const;
  QPoint transform(QPoint)const; // transform coordinates (local need)
  QPoint drawCurve_andGetCenter(QImage&); //return centerMass of curve
  void ShowObjectUnderCursor(QMouseEvent*); //highlight objects
  bool decide_to_draw(QPoint)const;//local logic
  bool isCorrectRelease(QMouseEvent*);//logic
  bool PXtoNull(pItem&&);//delete item from scene safely
  void newPX(pItem&&,const QPixmap&);//add new item to scene smartly
  void newPX(pItem&&,const QImage&);//overload
  void newPX(pItem&&,QImage&&);//overload
  void recalculation();

  int thickness_pen; //>=1
  char cursor_mode; //0 - view(nothing), 1 - draw, 2 - erase
  bool drawingFlag;
  QColor ColorObj; // color of "highlighted" object
  bool thread_ON_WORK; // true is worker thread is not done

  pLabel txt; // show theshold value
  pLabel otxt; //show opacity value
  pSlider Slider; //threshold slider
  pSlider OSlider; //opacity slider
  QPixmap draw_pix; // icon of draw cursor
  QPixmap erase_pix; // icon of erase cursor

  QGraphicsScene scene;
  QPixmap pm; //SOURCE IMAGE
  QImage b_img; // contained threshold image in mem
  QStack<QImage> Ctrl_Z, Ctrl_Y;

  pItem sourceItem;
  pItem titem; // contained threshold image on scene
  pItem track_item; // contained temp image (highlight object)
  pItem randItem; // contained randomize-colored image

  Ellipse CircleOnCur;
};

#endif // MGRAPHICS_H
