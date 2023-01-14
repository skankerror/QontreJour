#include <QLayout>
#include "grandmasterwidget.h"

GrandMasterWidget::GrandMasterWidget(QWidget *parent)
  : QWidget(parent),
    m_grandMasterslider(new QSlider),
    m_blackOutButton(new QPushButton),
    m_grandMasterLabel(new QLabel)
{
  auto layout = new QVBoxLayout();
  layout->addWidget(m_blackOutButton);
  layout->addWidget(m_grandMasterslider);
  layout->addWidget(m_grandMasterLabel);
  setLayout(layout);
}
