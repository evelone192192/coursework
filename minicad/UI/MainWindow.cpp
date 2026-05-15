#include "MainWindow.h"
#include "ui_minicad.h"
#include <QToolBar>
#include <QPushButton>
#include <QDockWidget>
#include <QColorDialog>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QShortcut>
#include <QLabel>

#include "../Core/Line.h"
#include "../Core/Circle.h"
#include "../Core/GeometryCalc.h"
#include "../Core/Primitive.h"
#include "../Logic/AddPrimitiveCommand.h"
#include "../IO/DXFConverter.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->setWindowTitle("Mini-CAD Engineering Shell");
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
    QDockWidget* dock = new QDockWidget("Inspector", this);
    dock->setWidget(m_propertyPanel);
    addDockWidget(Qt::RightDockWidgetArea, dock);

    QToolBar* bar = new QToolBar(this); addToolBar(bar);

    QPushButton* btnOpen = new QPushButton("OPEN DXF", this); bar->addWidget(btnOpen);
    connect(btnOpen, &QPushButton::clicked, this, &MainWindow::loadDxf);

    QPushButton* btnSave = new QPushButton("SAVE DXF", this); bar->addWidget(btnSave);
    connect(btnSave, &QPushButton::clicked, this, &MainWindow::saveDxf);

    bar->addSeparator();

    QPushButton* btnSel = new QPushButton("SELECT", this); bar->addWidget(btnSel);
    connect(btnSel, &QPushButton::clicked, [this]() { m_canvas->setTool(Canvas::Tool::Select); updateCanvas(); });

    QPushButton* btnEdit = new QPushButton("EDIT", this); bar->addWidget(btnEdit);
    connect(btnEdit, &QPushButton::clicked, [this]() { m_canvas->setTool(Canvas::Tool::Edit); updateCanvas(); });

    QPushButton* btnWall = new QPushButton("WALL", this); bar->addWidget(btnWall);
    connect(btnWall, &QPushButton::clicked, [this]() { m_canvas->setTool(Canvas::Tool::Wall); updateCanvas(); });

    QPushButton* btnCol = new QPushButton("COLUMN", this); bar->addWidget(btnCol);
    connect(btnCol, &QPushButton::clicked, [this]() { m_canvas->setTool(Canvas::Tool::Column); updateCanvas(); });

    bar->addSeparator();

    m_layerCombo = new QComboBox(this); bar->addWidget(m_layerCombo);
    refreshLayerList();
    connect(m_layerCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int i) {
        if (i >= 0) { m_layerManager.setActiveLayer(m_layerCombo->itemData(i).toInt()); updateCanvas(); }
        });

    QPushButton* btnAddL = new QPushButton("+L", this); bar->addWidget(btnAddL);
    connect(btnAddL, &QPushButton::clicked, this, &MainWindow::addLayer);

    bar->addSeparator();

    QPushButton* btnUndo = new QPushButton("UNDO", this); bar->addWidget(btnUndo);
    connect(btnUndo, &QPushButton::clicked, this, &MainWindow::undoAction);

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
    connect(m_canvas, &Canvas::vertexMoved, this, [this](int, int, Point p) {
        if (m_selectionManager.isEmpty()) return;
        int id = *m_selectionManager.getSelectedIds().begin();
        for (auto& o : m_primitives) {
            if (o->getId() == id && o->getLayerId() == m_layerManager.getActiveLayerId()) {
                if (o->getType() == Primitive::Type::Line) {
                    Line* l = static_cast<Line*>(o.get());
                    if (GeometryCalc::distance(p, l->start()) < GeometryCalc::distance(p, l->end())) l->setStart(p);
                    else l->setEnd(p);
                }
                else if (o->getType() == Primitive::Type::Circle) {
                    static_cast<Circle*>(o.get())->setRadius(GeometryCalc::distance(p, static_cast<Circle*>(o.get())->center()));
                }
            }
        }
        updateCanvas();
        });
}

void MainWindow::saveDxf() {
    QString path = QFileDialog::getSaveFileName(this, "Save DXF", "", "DXF Files (*.dxf)");
    if (!path.isEmpty()) {
        if (DXFConverter::saveToFile(path.toStdString(), m_primitives))
            QMessageBox::information(this, "Success", "File saved successfully!");
        else
            QMessageBox::critical(this, "Error", "Could not save file.");
    }
}

void MainWindow::loadDxf() {
    QString path = QFileDialog::getOpenFileName(this, "Open DXF", "", "DXF Files (*.dxf)");
    if (!path.isEmpty()) {
        auto loaded = DXFConverter::loadFromFile(path.toStdString());
        if (!loaded.empty()) {
            m_primitives.clear();
            for (auto& item : loaded) m_primitives.push_back(std::move(item));
            m_selectionManager.clear();
            updateCanvas();
            QMessageBox::information(this, "Success", "File loaded successfully!");
        }
        else {
            QMessageBox::warning(this, "Warning", "File is empty or corrupted.");
        }
    }
}

void MainWindow::updateCanvas() {
    m_canvas->updateScene(m_primitives, m_selectionManager.getSelectedIds(), m_layerManager.getPriorityMap(), m_layerManager.getActiveLayerId());
}

void MainWindow::handleSelection(Point p, bool ctrl) {
    if (!ctrl) m_selectionManager.clear();
    Primitive* last = nullptr;
    int activeId = m_layerManager.getActiveLayerId();
    for (auto& o : m_primitives) {
        if (o->getLayerId() != activeId) continue;
        bool hit = false;
        if (o->getType() == Primitive::Type::Line) hit = GeometryCalc::isPointNearLine(p, static_cast<Line*>(o.get())->start(), static_cast<Line*>(o.get())->end(), 10.0);
        else hit = GeometryCalc::isPointInCircle(p, static_cast<Circle*>(o.get())->center(), static_cast<Circle*>(o.get())->radius());
        if (hit) { m_selectionManager.select(o->getId(), ctrl); last = o.get(); if (!ctrl) break; }
    }
    m_propertyPanel->showProperties(last, m_layerManager);
    updateCanvas();
}

void MainWindow::handleMove(double dx, double dy) {
    int activeId = m_layerManager.getActiveLayerId();
    for (int id : m_selectionManager.getSelectedIds()) {
        for (auto& o : m_primitives) if (o->getId() == id && o->getLayerId() == activeId) o->move(dx, dy);
    }
    updateCanvas();
}

void MainWindow::addLayer() {
    bool ok; QString n = QInputDialog::getText(this, "Layer", "Name:", QLineEdit::Normal, "", &ok);
    if (ok && !n.isEmpty()) { m_layerManager.createLayer(n.toStdString()); refreshLayerList(); }
}

void MainWindow::refreshLayerList() {
    m_layerCombo->clear();
    for (auto const& [id, l] : m_layerManager.getAllLayers()) m_layerCombo->addItem(QString::fromStdString(l.name), id);
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

void MainWindow::undoAction() { m_commandManager.undo(); m_selectionManager.clear(); updateCanvas(); }

MainWindow::~MainWindow() { delete ui; }