#include "submasterwidget.h"
#include <QLayout>

SubMasterWidget::SubMasterWidget(QWidget *parent)
  : QWidget{parent}
{
  auto slidersLayout = new QHBoxLayout();

  for (int i = 0; i < SUBMASTER_SLIDERS_COUNT; i++)
  {
    auto slider = new QSlider(this);
    l_m_sliders.append(slider);
    slidersLayout->addWidget(slider);
  }

  setLayout(slidersLayout);
}
