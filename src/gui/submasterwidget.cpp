#include "submasterwidget.h"
#include <QLayout>

SubMasterWidget::SubMasterWidget(QWidget *parent)
  : QWidget{parent}
{
  auto slidersLayout = new QHBoxLayout();

  for (int i = 0; i < SUBMASTER_SLIDERS_COUNT; i++)
  {
    auto slider = new QSlider(this);
    slider->setMaximum(255);
    slider->setMinimum(0);
//    auto DmxChannelGroup
    m_L_sliders.append(slider);
    slidersLayout->addWidget(slider);
  }

  setLayout(slidersLayout);
}

// TODO : c'est le bordel y faut sans doute sous-classer slider...
// et faire méthodes pour y assigner des channels existants.
