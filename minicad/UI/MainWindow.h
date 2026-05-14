#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <vector>
#include <memory>
#include "Canvas.h"
#include "PropertyPanel.h"
#include "../Core/Primitive.h"
#include "../Logic/CommandManager.h"
#include "../Logic/LayerManager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
private:
    Ui::MainWindow* ui;
    Canvas* m_canvas;
    PropertyPanel* m_propertyPanel;
    std::vector<std::unique_ptr<Primitive>> m_primitives;
    CommandManager m_commandManager;
    LayerManager m_layerManager;
};
#endif