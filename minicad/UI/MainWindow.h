#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <vector>
#include <memory>
#include "Canvas.h"
#include "PropertyPanel.h"
#include "../Core/Primitive.h"
#include "../Logic/CommandManager.h"
#include "../Logic/LayerManager.h"
#include "../Logic/SelectionManager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void undoAction();
    void handleSelection(Point p, bool ctrlPressed);
    void handleMove(double dx, double dy);
    void addLayer();
    void changeColor();
    void assignLayer();
    void updateCanvas();

    void saveDxf();
    void loadDxf();

private:
    Ui::MainWindow* ui;
    Canvas* m_canvas;
    PropertyPanel* m_propertyPanel;
    QComboBox* m_layerCombo;

    std::vector<std::unique_ptr<Primitive>> m_primitives;
    CommandManager m_commandManager;
    LayerManager m_layerManager;
    SelectionManager m_selectionManager;

    void refreshLayerList();
};
#endif