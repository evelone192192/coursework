#ifndef PROPERTYPANEL_H
#define PROPERTYPANEL_H
#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include "../Core/Primitive.h"
#include "../Logic/LayerManager.h"

class PropertyPanel : public QWidget {
    Q_OBJECT
public:
    explicit PropertyPanel(QWidget* parent = nullptr);
    void showProperties(Primitive* p, LayerManager& lm);
    void clearProperties();
private:
    QVBoxLayout* m_layout;
    QLineEdit* m_idEdit, * m_typeEdit, * m_coordsEdit, * m_layerEdit;
};
#endif