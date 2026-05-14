#include "MainWindow.h"
#include "ui_minicad.h" 

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_canvas = new Canvas(this);

    if (ui->graphicsView) {
        ui->graphicsView->setScene(m_canvas);
    }

    m_propertyPanel = new PropertyPanel(this);
}

MainWindow::~MainWindow() {
    delete ui;
}