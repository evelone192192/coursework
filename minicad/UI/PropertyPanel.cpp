#include "PropertyPanel.h"

PropertyPanel::PropertyPanel(QWidget* parent) : QWidget(parent) {
    m_layout = new QVBoxLayout(this);

    m_idEdit = new QLineEdit(this); m_idEdit->setReadOnly(true);
    m_typeEdit = new QLineEdit(this); m_typeEdit->setReadOnly(true);
    m_coordsEdit = new QLineEdit(this); m_coordsEdit->setReadOnly(true);

    m_layout->addWidget(new QLabel("Object ID:"));
    m_layout->addWidget(m_idEdit);
    m_layout->addWidget(new QLabel("Object Type:"));
    m_layout->addWidget(m_typeEdit);
    m_layout->addWidget(new QLabel("Geometry Info:"));
    m_layout->addWidget(m_coordsEdit);

    m_layout->addStretch();
}

void PropertyPanel::showProperties(Primitive* p) {
    if (!p) {
        clearProperties();
        return;
    }

    m_idEdit->setText(QString::number(p->getId()));
    m_typeEdit->setText(p->getType() == Primitive::Type::Line ? "LINE" : "CIRCLE");
    m_coordsEdit->setText(QString::fromStdString(p->getInfo()));
}

void PropertyPanel::clearProperties() {
    m_idEdit->clear();
    m_typeEdit->clear();
    m_coordsEdit->clear();
}