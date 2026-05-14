#include "MainWindow.h"
#include "ui_minicad.h"
#include <QToolBar>
#include <QPushButton>
#include <QDockWidget>
#include <QLabel>
#include <QDebug>

#include "../Core/Line.h"
#include "../Core/Circle.h"
#include "../Core/GeometryCalc.h"
#include "../Logic/AddPrimitiveCommand.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->showMaximized();

    m_canvas = new Canvas(this);
    if (ui->graphicsView) {
        ui->graphicsView->setScene(m_canvas);
        this->setCentralWidget(ui->graphicsView);
    }

    m_propertyPanel = new PropertyPanel(this);
    QDockWidget* dock = new QDockWidget("Properties", this);
    dock->setWidget(m_propertyPanel);
    addDockWidget(Qt::RightDockWidgetArea, dock);

    QToolBar* bar = new QToolBar(this);
    addToolBar(bar);

    QPushButton* btnSel = new QPushButton("SELECT", this); bar->addWidget(btnSel);
    connect(btnSel, &QPushButton::clicked, [this]() { m_canvas->setTool(Canvas::Tool::Select); });

    QPushButton* btnLine = new QPushButton("LINE", this); bar->addWidget(btnLine);
    connect(btnLine, &QPushButton::clicked, [this]() { m_canvas->setTool(Canvas::Tool::Line); });

    QPushButton* btnCirc = new QPushButton("CIRCLE", this); bar->addWidget(btnCirc);
    connect(btnCirc, &QPushButton::clicked, [this]() { m_canvas->setTool(Canvas::Tool::Circle); });

    bar->addSeparator();

    QPushButton* btnUndo = new QPushButton("UNDO", this); bar->addWidget(btnUndo);
    connect(btnUndo, &QPushButton::clicked, this, &MainWindow::undoAction);

    connect(m_canvas, &Canvas::lineFinished, this, [this](Point s, Point e) {
        static int idLine = 1000;
        auto obj = std::make_unique<Line>(idLine++, m_layerManager.getActiveLayerId(), s, e);
        m_commandManager.executeCommand(std::make_unique<AddPrimitiveCommand>(m_primitives, std::move(obj)));
        m_canvas->updateScene(m_primitives, m_selectionManager.getSelectedIds());
        });

    connect(m_canvas, &Canvas::circleFinished, this, [this](Point c, double r) {
        static int idCirc = 5000;
        auto obj = std::make_unique<Circle>(idCirc++, m_layerManager.getActiveLayerId(), c, r);
        m_commandManager.executeCommand(std::make_unique<AddPrimitiveCommand>(m_primitives, std::move(obj)));
        m_canvas->updateScene(m_primitives, m_selectionManager.getSelectedIds());
        });

    connect(m_canvas, &Canvas::objectClicked, this, &MainWindow::handleSelection);
}

void MainWindow::handleSelection(Point p) {
    m_selectionManager.clear();
    Primitive* selected = nullptr;

    for (auto& obj : m_primitives) {
        bool hit = false;
        if (obj->getType() == Primitive::Type::Line) {
            Line* l = static_cast<Line*>(obj.get());
            hit = GeometryCalc::isPointNearLine(p, l->start(), l->end(), 7.0);
        }
        else if (obj->getType() == Primitive::Type::Circle) {
            Circle* c = static_cast<Circle*>(obj.get());
            hit = GeometryCalc::isPointInCircle(p, c->center(), c->radius());
        }

        if (hit) {
            m_selectionManager.select(obj->getId());
            selected = obj.get();
            break;
        }
    }

    m_propertyPanel->showProperties(selected);
    m_canvas->updateScene(m_primitives, m_selectionManager.getSelectedIds());
}

void MainWindow::undoAction() {
    m_commandManager.undo();
    m_selectionManager.clear();
    m_propertyPanel->clearProperties();
    m_canvas->updateScene(m_primitives, m_selectionManager.getSelectedIds());
}

MainWindow::~MainWindow() { delete ui; }