#ifndef PROPERTYPANEL_H
#define PROPERTYPANEL_H
#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include "../Core/Primitive.h"

class PropertyPanel : public QWidget {
    Q_OBJECT
public:
    explicit PropertyPanel(QWidget* parent = nullptr);
    void showProperties(Primitive* primitive);
    void clearProperties();
private:
    QVBoxLayout* m_layout;
    QLineEdit* m_idEdit;
    QLineEdit* m_typeEdit;
    QLineEdit* m_coordsEdit;
};
#endif