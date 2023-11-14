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
#include <QDebug>

/************************** ValueSlidersWidget ************************/

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

void ValueSlidersWidget::setRootValue(RootValue *t_rootValue)
{
  m_rootValue = t_rootValue;
  populateWidget();
  connectSliders();
}

void ValueSlidersWidget::connectSliders()
{
  for (int i = 0;
       i < m_L_sliders.size()
       && i < m_rootValue->getL_childValueSize();
       i++)
  {
    connectSlider(i,i);
  }
}

void ValueSlidersWidget::connectSlider(int t_sliderID,
                                       LeveledValue *t_value)
{
  if (t_sliderID < 0
      || t_sliderID >= m_L_sliders.size()
      || (!t_value))
  {
    qDebug() << "problem in ValueSlidersWidget::connectSlider";
    return;
  }

  auto slider = m_L_sliders.at(t_sliderID);
  auto label = m_L_nameLabels.at(t_sliderID);

  if (slider->getIsConnected())
  {
    disconnectSlider(t_sliderID);
  }

  label->setText(t_value->getName());
  slider->setDmxValue(t_value);

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
  if (t_sliderID < m_L_nameLabels.size())
  {
    auto label = m_L_nameLabels.at(t_sliderID);
    label->setText("");
  }

  disconnect(slider,
             SIGNAL(valueChanged(int)),
             slider,
             SLOT(updateLevel(int)));

  auto value = slider->getDmxValue();

  disconnect(value,
             SIGNAL(levelChanged(id,dmx)),
             slider,
             SLOT(onValueLevelChanged(id,dmx)));

  slider->setIsConnected(false);
  value->setAssignedWidget(nullptr);

}

/************************** DirectChannelWidget ************************/

DirectChannelWidget::DirectChannelWidget(QWidget *parent)
  : ValueSlidersWidget(parent)
{
  auto manager = MANAGER;
  setRootValue(manager->getRootChannel());
  connect(manager,
          SIGNAL(connectChannelToDirectChannelSlider(int,id)),
          this,
          SLOT(connectSlider(int,id)));

  connect(manager,
          SIGNAL(disconnectChannelToDirectChannelSlider(int)),
          this,
          SLOT(disconnectSlider(int)));

}

void DirectChannelWidget::populateWidget()
{
  // on crée les sliders
  // TODO : vider avant ?

  auto L_dmxChannel = m_rootValue->getL_childValue();
  for (const auto &item
       : std::as_const(L_dmxChannel))
  {
    auto directChannelSlider = new ValueSlider(item,
                                               this);

    connect(directChannelSlider,
            SIGNAL(valueSliderMoved(id,dmx,overdmx)),
            MANAGER,
            SLOT(directChannelWidgetsToEngine(id,dmx,overdmx)));

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
      auto nameLabel = new QLabel("", this);
      nameLabel->setAlignment(Qt::AlignHCenter);
      nameLabel->setWordWrap(true);
      m_L_nameLabels.append(nameLabel);

      auto layout = new QVBoxLayout();
      auto label = new QLabel(QString::number(j + (i * SLIDERS_PER_PAGE) + 1),
                              widget);
      label->setAlignment(Qt::AlignHCenter);
      auto slider = m_L_sliders.at(j + (i * SLIDERS_PER_PAGE));
      slider->setID(j + (i * SLIDERS_PER_PAGE));

      layout->addWidget(slider);
      layout->addWidget(label);
      layout->addWidget(nameLabel);
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


/************************** SubmasterWidget ************************/

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
  for (int i = 0;
       i < SUBMASTER_SLIDERS_PAGE_COUNT;
       i++) // for each page
  {
    auto widget = new QWidget(this);
    auto pageLayout = new QHBoxLayout();

    for (int j = 0;
         j < SUBMASTER_SLIDERS_COUNT_PER_PAGE;
         j++) // for each slider
    {
      auto submasterSlider = new ValueSlider(this);
      submasterSlider->setID(i);
      m_L_sliders.append(submasterSlider);

      connect(submasterSlider,
              SIGNAL(valueSliderMoved(id,dmx)),
              MANAGER,
              SLOT(submasterSliderToEngine(id,dmx)));


      auto nameLabel = new QLabel("", this);
      nameLabel->setAlignment(Qt::AlignHCenter);
      nameLabel->setWordWrap(true);
      m_L_nameLabels.append(nameLabel);

      auto layout = new QVBoxLayout();
      auto idLabel = new QLabel(QString::number(j + (i * SUBMASTER_SLIDERS_COUNT_PER_PAGE) + 1),
                                widget);
      idLabel->setAlignment(Qt::AlignHCenter);

      layout->addWidget(submasterSlider);
      layout->addWidget(idLabel);
      layout->addWidget(nameLabel);
      layout->setAlignment(submasterSlider,
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

/************************** ValueSlider ************************/

ValueSlider::ValueSlider(QWidget *parent)
  :QSlider(parent),
    m_isConnected(false)
{
  setMinimum(0);
  setMaximum(255);
}

ValueSlider::ValueSlider(LeveledValue *t_dmxValue,
                         QWidget *parent)
  : QSlider(parent)
{
  setMinimum(0);
  setMaximum(255);
  setTickInterval(10);
  setTickPosition(QSlider::TicksBothSides);
  setDmxValue(t_dmxValue);
}

ValueSlider::~ValueSlider()
{}

void ValueSlider::setDmxValue(LeveledValue *t_dmxValue)
{
  m_dmxValue = t_dmxValue;
  connect(this,
          SIGNAL(valueChanged(int)),
          this,
          SLOT(updateLevel(int)));

  connect(m_dmxValue,
          SIGNAL(levelChanged(id,dmx)),
          this,
          SLOT(onValueLevelChanged(id,dmx)));

  m_dmxValue->setAssignedWidget(this);
  m_isConnected = true;
}

void ValueSlider::updateLevel(int t_level)
{
  if (t_level < 0) t_level = 0;
  if (t_level > 255) t_level = 255;
  emit valueSliderMoved(m_ID,
                        t_level);
}

void ValueSlider::onValueLevelChanged(id t_id,
                                      dmx t_level)
{
  Q_UNUSED(t_id)
  blockSignals(true);
  this->setValue(t_level);
  blockSignals(false);
}
