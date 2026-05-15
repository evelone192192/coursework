#include "MainWindow.h"
#include "ui_minicad.h"
#include <QToolBar>
#include <QPushButton>
#include <QDockWidget>
#include <QColorDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QShortcut>
#include <QKeySequence>
#include <QLabel>
#include <QDebug>

#include "../Core/Line.h"
#include "../Core/Circle.h"
#include "../Core/GeometryCalc.h"
#include "../Logic/AddPrimitiveCommand.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->setWindowTitle("Mini-CAD Engineering Shell v1.1");
    this->showMaximized();

    m_canvas = new Canvas(this);
    if (ui->graphicsView) {
        ui->graphicsView->setScene(m_canvas);
        this->setCentralWidget(ui->graphicsView);
    }

    QLabel* coordLabel = new QLabel("X: 0.0, Y: 0.0", this);
    coordLabel->setMinimumWidth(150);
    ui->statusBar->addPermanentWidget(coordLabel);

    connect(m_canvas, &Canvas::mouseMoved, this, [coordLabel](Point p) {
        coordLabel->setText(QString("CURSOR X: %1, Y: %2").arg(p.x).arg(p.y));
        });

    m_propertyPanel = new PropertyPanel(this);
    QDockWidget* dock = new QDockWidget("Object Inspector", this);
    dock->setWidget(m_propertyPanel);
    addDockWidget(Qt::RightDockWidgetArea, dock);

    QToolBar* bar = new QToolBar("Toolbox", this);
    addToolBar(bar);

    QPushButton* btnSel = new QPushButton("SELECT", this); bar->addWidget(btnSel);
    connect(btnSel, &QPushButton::clicked, [this]() { m_canvas->setTool(Canvas::Tool::Select); updateCanvas(); });

    QPushButton* btnEdit = new QPushButton("EDIT NODES", this); bar->addWidget(btnEdit);
    connect(btnEdit, &QPushButton::clicked, [this]() { m_canvas->setTool(Canvas::Tool::Edit); updateCanvas(); });

    QPushButton* btnWall = new QPushButton("WALL", this); bar->addWidget(btnWall);
    connect(btnWall, &QPushButton::clicked, [this]() { m_canvas->setTool(Canvas::Tool::Wall); updateCanvas(); });

    QPushButton* btnCol = new QPushButton("COLUMN", this); bar->addWidget(btnCol);
    connect(btnCol, &QPushButton::clicked, [this]() { m_canvas->setTool(Canvas::Tool::Column); updateCanvas(); });

    bar->addSeparator();
    QPushButton* btnColor = new QPushButton("COLOR", this); bar->addWidget(btnColor);
    connect(btnColor, &QPushButton::clicked, this, &MainWindow::changeColor);

    QPushButton* btnAssign = new QPushButton("SET LAYER", this); bar->addWidget(btnAssign);
    connect(btnAssign, &QPushButton::clicked, this, &MainWindow::assignLayer);

    bar->addSeparator();
    m_layerCombo = new QComboBox(this);
    bar->addWidget(m_layerCombo);
    refreshLayerList();
    connect(m_layerCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
        if (index >= 0) {
            m_layerManager.setActiveLayer(m_layerCombo->itemData(index).toInt());
            updateCanvas();
        }
        });

    QPushButton* btnAddLayer = new QPushButton("+ LAYER", this); bar->addWidget(btnAddLayer);
    connect(btnAddLayer, &QPushButton::clicked, this, &MainWindow::addLayer);

    bar->addSeparator();
    QShortcut* undoShortcut = new QShortcut(QKeySequence::Undo, this);
    connect(undoShortcut, &QShortcut::activated, this, &MainWindow::undoAction);

    connect(m_canvas, &Canvas::lineFinished, this, [this](Point s, Point e) {
        auto obj = Primitive::create(Primitive::Type::Line,
            1000 + m_primitives.size(),
            m_layerManager.getActiveLayerId(),
            s, e, 0);

        if (obj) {
            m_commandManager.executeCommand(std::make_unique<AddPrimitiveCommand>(m_primitives, std::move(obj)));
            updateCanvas();
        }
        });

    connect(m_canvas, &Canvas::circleFinished, this, [this](Point c, double r) {
        auto obj = Primitive::create(Primitive::Type::Circle,
            5000 + m_primitives.size(),
            m_layerManager.getActiveLayerId(),
            c, Point(), r);

        if (obj) {
            m_commandManager.executeCommand(std::make_unique<AddPrimitiveCommand>(m_primitives, std::move(obj)));
            updateCanvas();
        }
        });

    connect(m_canvas, &Canvas::objectClicked, this, &MainWindow::handleSelection);
    connect(m_canvas, &Canvas::objectMoved, this, &MainWindow::handleMove);

    connect(m_canvas, &Canvas::vertexMoved, this, [this](int, int, Point newPos) {
        if (m_selectionManager.isEmpty()) return;
        int activeId = m_layerManager.getActiveLayerId();
        int selId = *m_selectionManager.getSelectedIds().begin();
        for (auto& obj : m_primitives) {
            if (obj->getId() == selId && obj->getLayerId() == activeId) {
                if (obj->getType() == Primitive::Type::Line) {
                    Line* l = static_cast<Line*>(obj.get());
                    if (GeometryCalc::distance(newPos, l->start()) < GeometryCalc::distance(newPos, l->end()))
                        l->setStart(newPos);
                    else l->setEnd(newPos);
                }
                else if (obj->getType() == Primitive::Type::Circle) {
                    Circle* c = static_cast<Circle*>(obj.get());
                    c->setRadius(GeometryCalc::distance(newPos, c->center()));
                }
            }
        }
        updateCanvas();
        });
}

void MainWindow::updateCanvas() {
    m_canvas->updateScene(m_primitives, m_selectionManager.getSelectedIds(), m_layerManager.getPriorityMap(), m_layerManager.getActiveLayerId());
}

void MainWindow::handleSelection(Point p, bool ctrlPressed) {
    if (!ctrlPressed) m_selectionManager.clear();
    Primitive* lastFound = nullptr;
    int activeLayerId = m_layerManager.getActiveLayerId();
    for (auto& obj : m_primitives) {
        if (obj->getLayerId() != activeLayerId) continue;
        bool hit = false;
        if (obj->getType() == Primitive::Type::Line) {
            Line* l = static_cast<Line*>(obj.get());
            hit = GeometryCalc::isPointNearLine(p, l->start(), l->end(), 10.0);
        }
        else {
            Circle* c = static_cast<Circle*>(obj.get());
            hit = GeometryCalc::isPointInCircle(p, c->center(), c->radius());
        }
        if (hit) { m_selectionManager.select(obj->getId(), ctrlPressed); lastFound = obj.get(); if (!ctrlPressed) break; }
    }
    m_propertyPanel->showProperties(lastFound, m_layerManager);
    updateCanvas();
}

void MainWindow::handleMove(double dx, double dy) {
    int activeLayerId = m_layerManager.getActiveLayerId();
    for (int id : m_selectionManager.getSelectedIds()) {
        for (auto& obj : m_primitives) {
            if (obj->getId() == id && obj->getLayerId() == activeLayerId) obj->move(dx, dy);
        }
    }
    updateCanvas();
}

void MainWindow::addLayer() {
    bool ok;
    QString name = QInputDialog::getText(this, "New Layer", "Layer Name:", QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) { m_layerManager.createLayer(name.toStdString()); refreshLayerList(); }
}

void MainWindow::refreshLayerList() {
    m_layerCombo->clear();
    for (auto const& [id, layer] : m_layerManager.getAllLayers()) m_layerCombo->addItem(QString::fromStdString(layer.name), id);
}

void MainWindow::changeColor() {
    if (m_selectionManager.isEmpty()) return;
    QColor color = QColorDialog::getColor(Qt::black, this, "Select Color");
    if (color.isValid()) {
        int activeId = m_layerManager.getActiveLayerId();
        for (int id : m_selectionManager.getSelectedIds()) {
            for (auto& obj : m_primitives) {
                if (obj->getId() == id && obj->getLayerId() == activeId) obj->setColor(color.rgba());
            }
        }
        updateCanvas();
    }
}

void MainWindow::assignLayer() {
    if (m_selectionManager.isEmpty()) return;
    int targetLayer = m_layerManager.getActiveLayerId();
    for (int id : m_selectionManager.getSelectedIds()) {
        for (auto& obj : m_primitives) if (obj->getId() == id) obj->setLayerId(targetLayer);
    }
    updateCanvas();
}

void MainWindow::undoAction() { m_commandManager.undo(); m_selectionManager.clear(); m_propertyPanel->clearProperties(); updateCanvas(); }

MainWindow::~MainWindow() { delete ui; }