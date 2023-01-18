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

#include "dmxchanneloutputwidget.h"
#include "../qontrejour.h"
#include <QLayout>
#include <QDebug>
#include <QLabel>

DmxChannelOutputWidget::DmxChannelOutputWidget(QWidget *parent)
  : QWidget(parent),
    m_tableView(new DmxChannelOutputTableView(this)),
    m_delegate(new DmxChannelOutputTableDelegate(this)),
    m_universeSpinBox(new QSpinBox(this))
{
  auto totalLayout = new QVBoxLayout();
  auto headerLayout = new QHBoxLayout();

  auto label = new QLabel("Dmx Channels output from universe :  ", this);

  m_universeSpinBox->setMaximum(STATIC_UNIVERSE_COUNT);
  m_universeSpinBox->setMinimum(1);// there's always one universe

  m_tableView->setItemDelegate(m_delegate);


  headerLayout->addWidget(label);
  headerLayout->addWidget(m_universeSpinBox);

  totalLayout->addLayout(headerLayout);
  totalLayout->addWidget(m_tableView);

  setLayout(totalLayout);
  m_tableView->resizeColumnsToContents();
}

void DmxChannelOutputWidget::onUniverseCountChanged(int t_universeCount)
{
  m_universeSpinBox->setMaximum(t_universeCount);
  m_tableView->resizeColumnsToContents();

}

void DmxChannelOutputWidget::setModel(DmxChannelOutputTableModel *t_model)
{
  m_tableView->setModel(t_model);
  m_tableView->resizeColumnsToContents();

}



