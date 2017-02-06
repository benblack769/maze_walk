#pragma once

#include <QMainWindow>
#include <vector>
#include "ui_mainwindow.h"
#include "point.h"
#include "farray2d.h"

using namespace std;

namespace Ui {
class MainWindow;
}
class QGraphicsView;
class QGraphicsScene;
class QGraphicsPixmapItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(string filename, QWidget *parent = 0);
    ~MainWindow() = default;
    
    void draw_maze(FArray2d<char> maze);
    void draw_connected_pts(vector<QPointF> points,QPen pen);
private:
    Ui::MainWindow * ui;
    QGraphicsView * view;
    QGraphicsScene * screen;
    QGraphicsPixmapItem * mazepix;
};
