#include "cellsselecter.h"

#include <QRgb>
#include <QStack>

//CellsSelecter::CellsSelecter(QObject *parent) : QObject(parent)
//{

//}

const QRgb black = qRgb(0,0,0);

inline bool inner(qint32 x, qint32 y, CellsSelecter::Labels& V)
{//return true if d is inner point
 //dangerous function. May cause out of range!
    return (V[y][x + 1])&&(V[y][x - 1])&&(V[y + 1][x])&&(V[y - 1][x]);
}

inline bool isBlack(qint32 x, qint32 y, const QImage& im)
{
    return im.pixel(x,y) == black;
}

void fill(const QImage& img, CellsSelecter::Labels& V, qint32 _x, qint32 _y, qint32 L)
{
  QPoint t;
  QStack<QPoint> depth;
  depth.push(QPoint(_x,_y));
  const qint32 w = img.width();
  const qint32 h = img.height();

  while (!depth.empty())
  {
    t = depth.pop();
    qint32 x = t.rx();
    qint32 y = t.ry();
    V[y][x] = L; // filling.

    if((x + 1 < w)&&(isBlack(x+1,y,img))&&(V[y][x + 1] == 0))
    {
        depth.push(QPoint(x+1,y));
    }
    if((x - 1> -1)&&(isBlack(x-1,y,img))&&(V[y][x - 1] == 0))
    {
        depth.push(QPoint(x-1,y));
    }
    if((y + 1< h)&&(isBlack(x,y+1,img))&&(V[y + 1][x] == 0))
    {
        depth.push(QPoint(x,y+1));
    }
    if((y - 1> -1)&&(isBlack(x,y-1,img))&&(V[y - 1][x] == 0))
    {
        depth.push(QPoint(x,y-1));
    }
  }
}

CellsSelecter::Objects CellsSelecter::select(const QImage &img)
{
    if (isWorking == true) return Objects();

    isWorking = true;
    qint32 L = 1;
    Labels array(img.height(), Labels_row(img.width(),0));

    for(qint32 y = 0; y < img.height(); ++y)
      for(qint32 x = 0; x < img.width(); ++x) {

          if ((!array[y][x]) and (isBlack(x,y,img))) {
              fill(img,array,x,y,L++);
          }
      }

    const qint32 size = --L; // size == num of objects
    Objects objects(size);

    for (int i = 0; i < size; ++i) {
        objects[i] = S_area(i);
    }

    qint32 const _h = img.height();
    qint32 const _w = img.width();

    if ((size > 0)&&(_h > 2)&&(_w > 2))
    {
    for(int y = 1; y < _h - 1; ++y)//general case
      for(int x = 1; x < _w - 1; ++x)//general case
      {
          auto id = array[y][x];
          if ((id > 0)&&(id < size + 1))
          {
            QPoint t(x,y);
            objects[id - 1].add_main(t);

            if (!inner(x,y,array))
            {
                objects[id - 1].add_cont(t);
            }
          }
      }
    for(int x = 1; x < _w - 1; ++x)//top case
     {
        auto id = array[0][x];
        if ((id > 0)&&(id < size + 1))
        {
           QPoint t(x,0);
           objects[id - 1].add_main(t);
           objects[id - 1].add_cont(t);
        }
     }
    for(int x = 1; x < _w - 1; ++x)//bottom case
     {
        auto id = array[_h - 1][x];
        if ((id > 0)&&(id < size + 1))
                {
                   QPoint t(x,_h - 1);
                   objects[id - 1].add_main(t);
                   objects[id - 1].add_cont(t);
                }
     }
    for(int y = 0; y < _h; ++y)//left case
     {
         auto id = array[y][0];
         if ((id > 0)&&(id < size + 1))
         {
            QPoint t(0,y);
            objects[id - 1].add_main(t);
            objects[id - 1].add_cont(t);
         }
     }
    for(int y = 0; y < _h; ++y)//right case
     {
         auto id = array[y][_w - 1];
         if ((id > 0)&&(id < size + 1))
         {
            QPoint t(_w - 1,y);
            objects[id - 1].add_main(t);
            objects[id - 1].add_cont(t);
         }
     }
   }

    emit selectingIsFinished();
    isWorking = false;
    return objects;
}
