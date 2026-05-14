#include "PropertyPanel.h"

PropertyPanel::PropertyPanel(QWidget* parent) : QWidget(parent) {
    m_layout = new QVBoxLayout(this);
    m_idEdit = new QLineEdit(this); m_idEdit->setReadOnly(true);
    m_typeEdit = new QLineEdit(this); m_typeEdit->setReadOnly(true);
    m_coordsEdit = new QLineEdit(this); m_coordsEdit->setReadOnly(true);
    m_layerEdit = new QLineEdit(this); m_layerEdit->setReadOnly(true);

    m_layout->addWidget(new QLabel("Object ID:")); m_layout->addWidget(m_idEdit);
    m_layout->addWidget(new QLabel("Type:")); m_layout->addWidget(m_typeEdit);
    m_layout->addWidget(new QLabel("Layer:")); m_layout->addWidget(m_layerEdit);
    m_layout->addWidget(new QLabel("Info:")); m_layout->addWidget(m_coordsEdit);
    m_layout->addStretch();
}

void PropertyPanel::showProperties(Primitive* p, LayerManager& lm) {
    if (!p) { clearProperties(); return; }
    m_idEdit->setText(QString::number(p->getId()));
    m_typeEdit->setText(p->getType() == Primitive::Type::Line ? "LINE" : "CIRCLE");
    m_coordsEdit->setText(QString::fromStdString(p->getInfo()));

    Layer* l = lm.getLayer(p->getLayerId());
    m_layerEdit->setText(l ? QString::fromStdString(l->name) : "None");
}

void PropertyPanel::clearProperties() {
    m_idEdit->clear(); m_typeEdit->clear(); m_coordsEdit->clear(); m_layerEdit->clear();
}