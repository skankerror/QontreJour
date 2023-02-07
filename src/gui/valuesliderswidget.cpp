/*
 * (c) 2023 Michaël Creusy -- creusy(.)michael(@)gmail(.)com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "valuesliderswidget.h"
#include "../qontrejour.h"
#include <QLabel>


ValueSlidersWidget::ValueSlidersWidget(QWidget *parent)
  : QWidget(parent),
  m_stackedLayout(new QStackedLayout()),
  m_changePageComboBox(new QComboBox(this))
{
auto totalLayout = new QVBoxLayout();
totalLayout->addWidget(m_changePageComboBox);
totalLayout->addLayout(m_stackedLayout);
setLayout(totalLayout);

setMinimumHeight(200);

connect(m_changePageComboBox,
        &QComboBox::activated,
        m_stackedLayout,
        &QStackedLayout::setCurrentIndex);
}

void ValueSlidersWidget::setL_sliders(const QList<DmxValueSlider *> &t_L_sliders)
{
  for (const auto &item : std::as_const(m_L_sliders))
  {
    item->deleteLater();
  }
  m_L_sliders.squeeze();

  m_L_sliders = t_L_sliders;

}

/**********************************************************************/

DirectChannelWidget::DirectChannelWidget(QWidget *parent)
  : ValueSlidersWidget(parent)
{}

void DirectChannelWidget::populateWidget()
{
  // on crée le 1er widget pour contenir 32 sliders
  int page_count = m_L_sliders.size() / SLIDERS_PER_PAGE;
  if (m_L_sliders.size() > page_count * SLIDERS_PER_PAGE)
    page_count++;

  // TODO : probably crashes with with non 512ch universe.
  for (int i = 0; i < page_count; i++) // for each page
  {
    auto widget = new QWidget(this);
    auto pageLayout = new QHBoxLayout();

    for (int j = 0; j < SLIDERS_PER_PAGE; j++) // for each slider
    {
      auto layout = new QVBoxLayout();
      auto label = new QLabel(QString::number(j + (i * SLIDERS_PER_PAGE) + 1),
                              widget);
      label->setAlignment(Qt::AlignHCenter);
      auto slider = m_L_sliders.at(j + (i * SLIDERS_PER_PAGE));

      layout->addWidget(slider);
      layout->addWidget(label);
      layout->setAlignment(slider,
                           Qt::AlignHCenter);
      pageLayout->addLayout(layout);
    }
    widget->setLayout(pageLayout);
    m_stackedLayout->addWidget(widget);
    m_changePageComboBox->addItem(tr("page %1 --> Ch %2 - %3")
                                  .arg(i + 1)
                                  .arg((i * SLIDERS_PER_PAGE) + 1)
                                  .arg((i + 1) * SLIDERS_PER_PAGE));
  }
}

/*************************************************************************/

SubmasterWidget::SubmasterWidget(QWidget *parent)
  : ValueSlidersWidget(parent)
{}

void SubmasterWidget::populateWidget()
{
  // on crée le 1er widget pour contenir 20 sliders
  int page_count = m_L_sliders.size() / 20;
//  qDebug() << "page count : " << page_count;
  if (m_L_sliders.size() > page_count * 20)
    page_count++;

  // TODO : probably crashes with with non 512ch universe.
  for (int i = 0; i < page_count; i++) // for each page
  {
    auto widget = new QWidget(this);
    auto pageLayout = new QHBoxLayout();

    for (int j = 0; j < 20; j++) // for each slider
    {
      auto layout = new QVBoxLayout();
      auto label = new QLabel(QString::number(j + (i * 20) + 1),
                              widget);
      label->setAlignment(Qt::AlignHCenter);
      auto slider = m_L_sliders.at(j + (i * 20));

      layout->addWidget(slider);
      layout->addWidget(label);
      layout->setAlignment(slider,
                           Qt::AlignHCenter);
      pageLayout->addLayout(layout);
    }
    widget->setLayout(pageLayout);
    m_stackedLayout->addWidget(widget);
    m_changePageComboBox->addItem(tr("page %1 --> Channel Group %2 - %3")
                                  .arg(i + 1)
                                  .arg((i * 20) + 1)
                                  .arg((i + 1) * 20));
  }
}

/**********************************************************************/

DmxValueSlider::DmxValueSlider(QWidget *parent)
  :QSlider(parent)
{
  setMinimum(0);
  setMaximum(255);
}

DmxValueSlider::DmxValueSlider(DmxValue *t_dmxValue,
                               QWidget *parent)
  : QSlider(parent),
    m_dmxValue(t_dmxValue)
{
  setMinimum(0);
  setMaximum(255);

  connect(this,
          SIGNAL(valueChanged(int)),
          this,
          SLOT(updateLevel(int)));

  connect(m_dmxValue,
          SIGNAL(levelChanged(DmxValue::SignalSenderType,dmx)),
          this,
          SLOT(onValueLevelChanged(DmxValue::SignalSenderType,dmx)));

}

DmxValueSlider::~DmxValueSlider()
{}

void DmxValueSlider::unMoveSlider(dmx t_level)
{
  // we disconnect to avoid connecting loop
  disconnect(this,
             SIGNAL(valueChanged(int)),
             this,
             SLOT(updateLevel(int)));

  this->setValue(t_level);

  // we reconnect
  connect(this,
          SIGNAL(valueChanged(int)),
          this,
          SLOT(updateLevel(int)));
}

void DmxValueSlider::updateLevel(int t_level)
{
  if (t_level < 0) t_level = 0;
  if (t_level > 255) t_level = 255;
  if (m_dmxValue->getLevel() == t_level) return;

  auto type = m_dmxValue->getType();

  if (type == DmxValue::Channel)
    m_dmxValue->setLevel(DmxValue::DirectChannelEditSender,
                         t_level);

  if (type == DmxValue::ChannelGroup)
    m_dmxValue->setLevel(DmxValue::SubmasterSliderSender,
                         t_level);
}

void DmxValueSlider::onValueLevelChanged(DmxValue::SignalSenderType t_type,
                                         dmx t_level)
{
  Q_UNUSED(t_type)

  // we disconnect to avoid connecting loop
  disconnect(this,
             SIGNAL(valueChanged(int)),
             this,
             SLOT(updateLevel(int)));

  this->setValue(t_level);

  // we reconnect
  connect(this,
          SIGNAL(valueChanged(int)),
          this,
          SLOT(updateLevel(int)));


}
