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

    this->setWindowTitle("Mini-CAD Engineering Shell v1.0");
    this->showMaximized();

    m_canvas = new Canvas(this);
    if (ui->graphicsView) {
        ui->graphicsView->setScene(m_canvas);
        this->setCentralWidget(ui->graphicsView);
    }

    QLabel* coordLabel = new QLabel("X: 0, Y: 0", this);
    ui->statusBar->addPermanentWidget(coordLabel);
    connect(m_canvas, &Canvas::mouseMoved, this, [coordLabel](Point p) {
        coordLabel->setText(QString("X: %1, Y: %2").arg(p.x).arg(p.y));
        });

    m_propertyPanel = new PropertyPanel(this);
    QDockWidget* dock = new QDockWidget("Object Properties", this);
    dock->setWidget(m_propertyPanel);
    addDockWidget(Qt::RightDockWidgetArea, dock);

    QToolBar* bar = new QToolBar("Main Toolbar", this);
    addToolBar(bar);

    QPushButton* btnSel = new QPushButton("SELECT", this); bar->addWidget(btnSel);
    connect(btnSel, &QPushButton::clicked, [this]() { m_canvas->setTool(Canvas::Tool::Select); });

    QPushButton* btnWall = new QPushButton("WALL", this); bar->addWidget(btnWall);
    connect(btnWall, &QPushButton::clicked, [this]() { m_canvas->setTool(Canvas::Tool::Wall); });

    QPushButton* btnCol = new QPushButton("COLUMN", this); bar->addWidget(btnCol);
    connect(btnCol, &QPushButton::clicked, [this]() { m_canvas->setTool(Canvas::Tool::Column); });

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
        if (index >= 0) m_layerManager.setActiveLayer(m_layerCombo->itemData(index).toInt());
        });

    QPushButton* btnAddLayer = new QPushButton("+ LAYER", this); bar->addWidget(btnAddLayer);
    connect(btnAddLayer, &QPushButton::clicked, this, &MainWindow::addLayer);

    bar->addSeparator();

    QPushButton* btnUndo = new QPushButton("UNDO", this); bar->addWidget(btnUndo);
    connect(btnUndo, &QPushButton::clicked, this, &MainWindow::undoAction);

    QShortcut* undoShortcut = new QShortcut(QKeySequence::Undo, this);
    connect(undoShortcut, &QShortcut::activated, this, &MainWindow::undoAction);

    connect(m_canvas, &Canvas::lineFinished, this, [this](Point s, Point e) {
        static int idL = 1000;
        auto obj = std::make_unique<Line>(idL++, m_layerManager.getActiveLayerId(), s, e);
        m_commandManager.executeCommand(std::make_unique<AddPrimitiveCommand>(m_primitives, std::move(obj)));
        updateCanvas();
        });

    connect(m_canvas, &Canvas::circleFinished, this, [this](Point c, double r) {
        static int idC = 5000;
        auto obj = std::make_unique<Circle>(idC++, m_layerManager.getActiveLayerId(), c, r);
        m_commandManager.executeCommand(std::make_unique<AddPrimitiveCommand>(m_primitives, std::move(obj)));
        updateCanvas();
        });

    connect(m_canvas, &Canvas::objectClicked, this, &MainWindow::handleSelection);
    connect(m_canvas, &Canvas::objectMoved, this, &MainWindow::handleMove);
}

void MainWindow::updateCanvas() {
    m_canvas->updateScene(m_primitives, m_selectionManager.getSelectedIds(), m_layerManager.getPriorityMap());
}

void MainWindow::handleSelection(Point p, bool ctrlPressed) {
    if (!ctrlPressed) m_selectionManager.clear();

    Primitive* lastFound = nullptr;
    for (auto& obj : m_primitives) {
        bool hit = false;
        if (obj->getType() == Primitive::Type::Line) {
            Line* l = static_cast<Line*>(obj.get());
            hit = GeometryCalc::isPointNearLine(p, l->start(), l->end(), 10.0);
        }
        else {
            Circle* c = static_cast<Circle*>(obj.get());
            hit = GeometryCalc::isPointInCircle(p, c->center(), c->radius());
        }

        if (hit) {
            m_selectionManager.select(obj->getId(), ctrlPressed);
            lastFound = obj.get();
            if (!ctrlPressed) break;
        }
    }
    m_propertyPanel->showProperties(lastFound, m_layerManager);
    updateCanvas();
}

void MainWindow::handleMove(double dx, double dy) {
    for (int id : m_selectionManager.getSelectedIds()) {
        for (auto& obj : m_primitives) {
            if (obj->getId() == id) {
                obj->move(dx, dy);
            }
        }
    }
    updateCanvas();
}

void MainWindow::addLayer() {
    bool ok;
    QString name = QInputDialog::getText(this, "New Layer", "Name:", QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty()) {
        m_layerManager.createLayer(name.toStdString());
        refreshLayerList();
    }
}

void MainWindow::refreshLayerList() {
    m_layerCombo->clear();
    for (auto const& [id, layer] : m_layerManager.getAllLayers()) {
        m_layerCombo->addItem(QString::fromStdString(layer.name), id);
    }
}

void MainWindow::changeColor() {
    if (m_selectionManager.isEmpty()) return;
    QColor color = QColorDialog::getColor(Qt::black, this, "Choose Color");
    if (color.isValid()) {
        for (int id : m_selectionManager.getSelectedIds()) {
            for (auto& obj : m_primitives) {
                if (obj->getId() == id) obj->setColor(color.rgba());
            }
        }
        updateCanvas();
    }
}

void MainWindow::assignLayer() {
    if (m_selectionManager.isEmpty()) return;
    int targetLayer = m_layerManager.getActiveLayerId();
    for (int id : m_selectionManager.getSelectedIds()) {
        for (auto& obj : m_primitives) {
            if (obj->getId() == id) obj->setLayerId(targetLayer);
        }
    }
    updateCanvas();
}

void MainWindow::undoAction() {
    m_commandManager.undo();
    m_selectionManager.clear();
    m_propertyPanel->clearProperties();
    updateCanvas();
}

MainWindow::~MainWindow() {
    delete ui;
}