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

#ifndef DMXCHANNELOUTPUTWIDGET_H
#define DMXCHANNELOUTPUTWIDGET_H

#include <QWidget>
#include <QSpinBox>
#include "dmxchanneloutputtableview.h"
#include "../core/dmxchanneloutputtablemodel.h"
#include "dmxchanneloutputtabledelegate.h"


class DmxChannelOutputWidget
    : public QWidget
{
  Q_OBJECT

public:

  explicit DmxChannelOutputWidget(QWidget *parent = nullptr);

//  DmxChannelOutputTableView *getTableView() const { return m_tableView; }

signals:

public slots:

  void onUniverseCountChanged(int t_universeCount);
  void setModel(DmxChannelOutputTableModel *t_model);

private:

  DmxChannelOutputTableView *m_tableView;
  DmxChannelOutputTableDelegate *m_delegate;
  QSpinBox *m_universeSpinBox;

};

#endif // DMXCHANNELOUTPUTWIDGET_H
