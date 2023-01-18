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

#ifndef DMXCHANNELOUTPUTTABLEMODEL_H
#define DMXCHANNELOUTPUTTABLEMODEL_H

#include <QAbstractTableModel>
#include "dmxchannel.h"

class DmxChannelOutputTableModel
    : public QAbstractTableModel
{

  Q_OBJECT

public:

  explicit DmxChannelOutputTableModel(QObject *parent = nullptr);
  DmxChannelOutputTableModel(const QList<DmxChannel *> &t_L_dmxChannel,
                             QObject *parent = nullptr);

  QList<DmxChannel *> getL_dmxChannel() const { return m_L_dmxChannel; }
  void setL_dmxChannel(const QList<DmxChannel *> &t_L_dmxChannel){ m_L_dmxChannel = t_L_dmxChannel; }

  // overrides
  int rowCount(const QModelIndex &parent) const override;
  int columnCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  bool setData(const QModelIndex &index, const QVariant &value, int role) override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role) override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;


private:

  QList<DmxChannel *> m_L_dmxChannel;

};

#endif // DMXCHANNELOUTPUTTABLEMODEL_H
