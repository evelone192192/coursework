#include "PropertyPanel.h"

PropertyPanel::PropertyPanel(QWidget* parent) : QWidget(parent) {
    m_layout = new QVBoxLayout(this);

    m_layout->addWidget(new QLabel("Object ID:"));
    m_idEdit = new QLineEdit(this);
    m_layout->addWidget(m_idEdit);

    m_layout->addWidget(new QLabel("Type:"));
    m_typeEdit = new QLineEdit(this);
    m_layout->addWidget(m_typeEdit);

    m_layout->addWidget(new QLabel("Data:"));
    m_coordsEdit = new QLineEdit(this);
    m_layout->addWidget(m_coordsEdit);

    m_layout->addStretch();
}

void PropertyPanel::showProperties(Primitive* primitive) {
    if (!primitive) return;
    m_idEdit->setText(QString::number(primitive->getId()));
    m_typeEdit->setText(primitive->getType() == Primitive::Type::Line ? "Line" : "Other");
}

void PropertyPanel::clearProperties() {
    m_idEdit->clear();
    m_typeEdit->clear();
    m_coordsEdit->clear();
}