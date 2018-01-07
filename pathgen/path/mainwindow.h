#pragma once

#include <QMainWindow>
#include "board.h"
#include "ui_mainwindow.h"

namespace Ui {
class MainWindow;
}
class QGraphicsView;
class QGraphicsScene;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    void make_link_chain();
    void add_point(QPointF point);
private:
    Ui::MainWindow * ui;
    QGraphicsView * view;
    QGraphicsScene * screen;
    QPointF oldpoint;
    point_obj * obj;
};
