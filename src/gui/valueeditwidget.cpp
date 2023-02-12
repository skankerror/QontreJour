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

#include "valueeditwidget.h"
#include "valuesliderswidget.h"
#include "../qontrejour.h"
#include "../core/dmxmanager.h"
#include <QLabel>


ValueEditWidget::ValueEditWidget(DmxValue *t_rootValue,
                                 QWidget *parent)
  : QWidget(parent),
    m_rootValue(t_rootValue),
    m_layout(new QHBoxLayout())
{
  setWindowFlag(Qt::Window);
  setLayout(m_layout);

  auto valueType = m_rootValue->getType();

  switch(valueType)
  {
  case DmxValue::RootChannel :
    break;
  case DmxValue::RootOutput :
    break;
  case DmxValue::RootScene :
    break;
  case DmxValue::ChannelGroup :
    createGroupEditWidget();
    break;
  case DmxValue::Scene :
    break;
  default :
    break;
  }
}

ValueEditWidget::ValueEditWidget(DmxValue::ValueType t_type,
                                 QList<DmxValue *> t_L_value,
                                 QWidget *parent)
  : QWidget(parent),
    m_L_value(t_L_value),
    m_layout(new QHBoxLayout())
{
  setWindowFlag(Qt::Window);
  setLayout(m_layout);

  switch(t_type)
  {
  case DmxValue::RootChannel :
    break;
  case DmxValue::RootOutput :
    break;
  case DmxValue::RootScene :
    break;
  case DmxValue::ChannelGroup :
    createNewGroupWidget();
    break;
  case DmxValue::Scene :
    break;
  default :
    break;
  }

}

void ValueEditWidget::createGroupEditWidget()
{
  auto widget = new GroupEditWidget(m_rootValue,
                                    this);
  m_layout->addWidget(widget);

  connect(widget,
          SIGNAL(closeEditor()),
          this,
          SLOT(closeEditor()));
}

void ValueEditWidget::createNewGroupWidget()
{
//  auto newGroup = MANAGER->createChannelGroup(m_L_value);
  auto widget = new GroupEditWidget(m_L_value,
                                    this);
  m_layout->addWidget(widget);

  m_layout->addWidget(widget);
  connect(widget,
          SIGNAL(closeEditor()),
          this,
          SLOT(closeEditor()));
}

void ValueEditWidget::closeEditor()
{
  QWidget::close();
}

/***************************************************************************/

GroupEditWidget::GroupEditWidget(DmxValue *t_rootValue,
                                 QWidget *parent)
  : QWidget(parent),
    m_rootValue(t_rootValue),
    m_nameEdit(new QLineEdit(this)),
    m_slidersComboBox(new QComboBox(this)),
    m_tableWidget(new ValueTableWidget(this))
{
  createWidget();
  m_nameEdit->setText(m_rootValue->getName());

}

GroupEditWidget::GroupEditWidget(QList<DmxValue *> t_L_value,
                                 QWidget *parent)
  : QWidget(parent),
    m_L_value(t_L_value),
    m_nameEdit(new QLineEdit(this)),
    m_slidersComboBox(new QComboBox(this)),
    m_tableWidget(new ValueTableWidget(this))
{
  createWidget();
  m_nameEdit->setText("Channel Group");
}

void GroupEditWidget::createWidget()
{
  auto totalLayout = new QVBoxLayout();

  auto nameLayout = new QHBoxLayout();
  auto nameLabel = new QLabel("Channel Group Name -->", this);
  nameLayout->addWidget(nameLabel);
  nameLayout->addWidget(m_nameEdit);

  auto sliderLayout = new QHBoxLayout();
  auto sliderLabel = new QLabel("Choose Slider -->", this);
  auto L_sliderIDConnected = QList<id>();

  for (int i = 0;
       i <
       SUBMASTER_SLIDERS_COUNT_PER_PAGE * SUBMASTER_SLIDERS_PAGE_COUNT;
       i++)
  {
    auto value = MANAGER->getRootChannelGroup()->getChildValue(i);
    if (value)
    {
      auto widget = value->getAssignedWidget();
      if (widget)
      {
        auto slider = static_cast<ValueSlider *>(widget);
        auto sliderID = slider->getID();
        L_sliderIDConnected.append(sliderID);
      }
    }
  }

  for (int i = 0;
       i <
       SUBMASTER_SLIDERS_COUNT_PER_PAGE * SUBMASTER_SLIDERS_PAGE_COUNT;
       i++)
  {
    if (L_sliderIDConnected.contains(i))
      m_slidersComboBox->addItem(QString("%1 *").arg(i + 1), i + 1);
    else
      m_slidersComboBox->addItem(QString::number(i + 1), i + 1);
  }

  sliderLayout->addWidget(sliderLabel);
  sliderLayout->addWidget(m_slidersComboBox);

  m_tableWidget->setRootValue(MANAGER->getRootChannel(0)); // start with uid 1
  m_tableWidget->hideRecButtons();
  m_tableWidget->getModel()->selectAll();

  auto validationLayout = new QHBoxLayout();
  auto cancelButton = new QPushButton("Cancel", this);
  auto okButton = new QPushButton("Ok", this);
  connect(okButton,
          SIGNAL(clicked(bool)),
          this,
          SLOT(okSlot()));
  connect(cancelButton,
          SIGNAL(clicked(bool)),
          this,
          SLOT(cancelSlot()));
  validationLayout->addStretch();
  validationLayout->addWidget(cancelButton);
  validationLayout->addWidget(okButton);

  totalLayout->addLayout(nameLayout);
  totalLayout->addLayout(sliderLayout);
  totalLayout->addWidget(m_tableWidget);
  totalLayout->addLayout(validationLayout);
  setLayout(totalLayout);


}

void GroupEditWidget::okSlot()
{
  // TODO : comment updater les channels ? depuis la table ?
  if (!m_rootValue) // we must create new group
  {
    m_rootValue = MANAGER->createChannelGroup(m_L_value);
  }

  m_rootValue->setName(m_nameEdit->text());
  MANAGER->connectValueToWidget(DmxManager::DmxSlider,
                                m_slidersComboBox->currentIndex(),
                                DmxValue::ChannelGroup,
                                m_rootValue->getID());

  emit closeEditor();
  QWidget::close();
}

void GroupEditWidget::cancelSlot()
{
  emit closeEditor();
  QWidget::close();
}
