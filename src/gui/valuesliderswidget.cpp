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
#include "../core/dmxmanager.h"
#include <QLabel>
#include <QDebug>


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

void ValueSlidersWidget::setRootValue(DmxValue *t_rootValue)
{
  m_rootValue = t_rootValue;
  populateWidget();
}

void ValueSlidersWidget::connectSlider(int t_sliderID,
                                       DmxValue *t_value)
{
  if (t_sliderID < 0
      || t_sliderID >= m_L_sliders.size()
      || (!t_value))
  {
    qDebug() << "problem in ValueSlidersWidget::connectSlider";
    return;
  }

  auto slider = m_L_sliders.at(t_sliderID);

  if (slider->getIsConnected())
  {
    disconnectSlider(t_sliderID);
  }

  slider->setDmxValue(t_value);

  connect(slider,
          SIGNAL(valueChanged(int)),
          slider,
          SLOT(updateLevel(int)));

  connect(t_value,
          SIGNAL(levelChanged(DmxValue::SignalSenderType,dmx)),
          slider,
          SLOT(onValueLevelChanged(DmxValue::SignalSenderType,dmx)));

  slider->setIsConnected(true);
  t_value->setAssignedWidget(slider);

}

void ValueSlidersWidget::connectSlider(int t_sliderID,
                                       id valueID)
{
  auto value = m_rootValue->getChildValue(valueID);
  if (value)
  {
    return connectSlider(t_sliderID,
                         value);
  }
}

void ValueSlidersWidget::disconnectSlider(int t_sliderID)
{
  if (t_sliderID < 0
      || t_sliderID >= m_L_sliders.size())
  {
    qDebug() << "problem in ValueSlidersWidget::disConnectSlider";
    return;
  }
  auto slider = m_L_sliders.at(t_sliderID);
  disconnect(slider,
             SIGNAL(valueChanged(int)),
             slider,
             SLOT(updateLevel(int)));

  auto value = slider->getDmxValue();

  disconnect(value,
             SIGNAL(levelChanged(DmxValue::SignalSenderType,dmx)),
             slider,
             SLOT(onValueLevelChanged(DmxValue::SignalSenderType,dmx)));

  slider->setIsConnected(false);
  value->setAssignedWidget(nullptr);

}

/**********************************************************************/

DirectChannelWidget::DirectChannelWidget(QWidget *parent)
  : ValueSlidersWidget(parent)
{}

void DirectChannelWidget::populateWidget()
{
  // on crée les sliders
  // TODO : vider avant ?
  auto L_dmxChannel = m_rootValue->getL_childValue();
  for (const auto &item : std::as_const(L_dmxChannel))
  {
    auto directChannelSlider = new ValueSlider(item,
                                               this);
    connect(item,
            SIGNAL(blockDirectChannelSlider(dmx)),
            directChannelSlider,
            SLOT(unMoveSlider(dmx)));

    m_L_sliders.append(directChannelSlider);
  }

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

void DirectChannelWidget::setDirectChannelUniverseID(uid t_uid)
{
  setRootValue(MANAGER->getRootChannel(t_uid));
//  TODO : changer le spinbox
}


/*************************************************************************/

SubmasterWidget::SubmasterWidget(QWidget *parent)
  : ValueSlidersWidget(parent)
{
  auto manager = MANAGER;
  setRootValue(manager->getRootChannelGroup());

  connect(manager,
          SIGNAL(connectGroupToSubmasterSlider(int,id)),
          this,
          SLOT(connectSlider(int,id)));

  connect(manager,
          SIGNAL(disconnectGroupFromSubmasterSlider(int)),
          this,
          SLOT(disconnectSlider(int)));

}

bool SubmasterWidget::getIsSLiderConnected(int t_sliderID)
{
  if (t_sliderID < 0
      || t_sliderID >= m_L_sliders.size())
  {
    qDebug() << "SubmasterWidget::getIsSLiderConnected";
    return false;
  }
  auto slider = m_L_sliders.at(t_sliderID);
  return slider->getIsConnected();

}

void SubmasterWidget::populateWidget()
{
  // on crée 200 sliders
  for (int i = 0;
       i < SUBMASTER_SLIDERS_COUNT_PER_PAGE * SUBMASTER_SLIDERS_PAGE_COUNT;
       i++)
  {
    auto submasterSlider = new ValueSlider(this);
    submasterSlider->setID(i);
    m_L_sliders.append(submasterSlider);
  }

  for (int i = 0;
       i < SUBMASTER_SLIDERS_PAGE_COUNT;
       i++) // for each page
  {
    auto widget = new QWidget(this);
    auto pageLayout = new QHBoxLayout();

    for (int j = 0; j < 20; j++) // for each slider
    {
      auto layout = new QVBoxLayout();
      auto label = new QLabel(QString::number(j + (i * SUBMASTER_SLIDERS_COUNT_PER_PAGE) + 1),
                              widget);
      label->setAlignment(Qt::AlignHCenter);
      auto slider = m_L_sliders.at(j + (i * SUBMASTER_SLIDERS_COUNT_PER_PAGE));

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
                                  .arg((i * SUBMASTER_SLIDERS_COUNT_PER_PAGE) + 1)
                                  .arg((i + 1) * SUBMASTER_SLIDERS_COUNT_PER_PAGE));
  }
}

/**********************************************************************/

ValueSlider::ValueSlider(QWidget *parent)
  :QSlider(parent),
    m_isConnected(false)
{
  setMinimum(0);
  setMaximum(255);
}

ValueSlider::ValueSlider(DmxValue *t_dmxValue,
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

  m_dmxValue->setAssignedWidget(this);
  m_isConnected = true;

}

ValueSlider::~ValueSlider()
{}

void ValueSlider::unMoveSlider(dmx t_level)
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

void ValueSlider::updateLevel(int t_level)
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

void ValueSlider::onValueLevelChanged(DmxValue::SignalSenderType t_type,
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
