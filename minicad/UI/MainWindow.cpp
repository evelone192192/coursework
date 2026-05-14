#include "MainWindow.h"
#include "ui_minicad.h"
#include <QDebug>
#include "../Core/Line.h"
#include "../Logic/AddPrimitiveCommand.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_canvas = new Canvas(this);
    if (ui->graphicsView) ui->graphicsView->setScene(m_canvas);
    m_propertyPanel = new PropertyPanel(this);

    connect(m_canvas, &Canvas::lineFinished, this, [this](Point s, Point e) {
        static int nextId = 1;
        auto newLine = std::make_unique<Line>(nextId++, m_layerManager.getActiveLayerId(), s, e);
        auto cmd = std::make_unique<AddPrimitiveCommand>(m_primitives, std::move(newLine));
        m_commandManager.executeCommand(std::move(cmd));

        m_canvas->addLine(s.x, s.y, e.x, e.y, QPen(Qt::black, 2));
        qDebug() << "Object added. Total:" << m_primitives.size();
        });
}

MainWindow::~MainWindow() { delete ui; }