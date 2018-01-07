

#include "mainwindow.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsLineItem>

using namespace std;


constexpr double pix_per_b = 0.15;
constexpr uint64_t num_spots = 10000;

constexpr QPointF SIZEF = QPointF(bsize,bsize) / pix_per_b;
constexpr QPointF ZEROF = {0,0};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow())
{
    ui->setupUi(this);
    
    obj = new point_obj();
    
    screen = new QGraphicsScene;
    
    screen->setSceneRect(QRectF(ZEROF,SIZEF));
    
    view = new QGraphicsView(screen);
    
    setCentralWidget(view);
    
    oldpoint = QPointF(obj->prevp)/pix_per_b;
    make_link_chain();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete screen;
    delete view;
    delete obj;
}


void MainWindow::make_link_chain(){
    for(int i = 0; i < num_spots; i++){
        QPoint p = this->obj->find_next();
        this->obj->add_next(p);
        this->add_point(QPointF(p) / pix_per_b);
    }
}

void MainWindow::add_point(QPointF point){
    auto line = screen->addLine(QLineF(oldpoint,point),QPen(QBrush(Qt::black),1.0));
    oldpoint = point;
}
