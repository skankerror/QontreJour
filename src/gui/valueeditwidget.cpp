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

ValueEditWidget::ValueEditWidget(DmxValue *t_rootValue,
                                 QWidget *parent)
  : QWidget(parent),
    m_rootValue(t_rootValue),
    m_layout(new QHBoxLayout())
{
  setWindowFlag(Qt::Window);

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
  setLayout(m_layout);
}

void ValueEditWidget::createGroupEditWidget()
{
  auto widget = new GroupEditWidget(m_rootValue,
                                    this);
  m_layout->addWidget(widget);
}

/***************************************************************************/

GroupEditWidget::GroupEditWidget(DmxValue *t_rootValue,
                                 QWidget *parent)
  : QWidget(parent),
    m_rootValue(t_rootValue)
{

}
