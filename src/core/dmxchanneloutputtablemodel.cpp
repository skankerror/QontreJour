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

#include "dmxchanneloutputtablemodel.h"
#include "../qontrejour.h"

DmxChannelOutputTableModel::DmxChannelOutputTableModel(QObject *parent)
  : QAbstractTableModel(parent)
{}

DmxChannelOutputTableModel::DmxChannelOutputTableModel(const QList<DmxChannel *> &t_L_dmxChannel
                                                       , QObject *parent)
  : QAbstractTableModel(parent),
    m_L_dmxChannel(t_L_dmxChannel)
{}


int DmxChannelOutputTableModel::rowCount(const QModelIndex &parent) const
{
  return parent.isValid() ? 0 : DMX_CHANNEL_OUTPUT_TABLE_MODEL_ROWS_COUNT_DEFAULT;
}

int DmxChannelOutputTableModel::columnCount(const QModelIndex &parent) const
{
  return parent.isValid() ? 0 : DMX_CHANNEL_OUTPUT_TABLE_MODEL_COLUMNS_COUNT_DEFAULT;

}

QVariant DmxChannelOutputTableModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (index.flags().testFlag(Qt::ItemIsEditable))
  {
    int channelID = (((index.row() -1)/2) * DMX_CHANNEL_OUTPUT_TABLE_MODEL_COLUMNS_COUNT_DEFAULT)
        + index.column();
    if (channelID < 0 || channelID >= m_L_dmxChannel.size())
      return QVariant();
    auto dmxChannel = m_L_dmxChannel.at(channelID);
    return dmxChannel->getLevel();
//    return dmxChannel->getChannelID(); // just to test it
  }
  else
  {
    int ret = ((index.row() / 2 ) * DMX_CHANNEL_OUTPUT_TABLE_MODEL_COLUMNS_COUNT_DEFAULT)
        + index.column() + 1;
    return ret > 512 ? QVariant() : ret;
  }
  // TODO : ajouter roles :
  //     case Qt::TextAlignmentRole: return Qt::AlignCenter;
  //     case Qt::DisplayRole: case Qt::EditRole:
//     case Qt::BackgroundRole:


}

bool DmxChannelOutputTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (!index.isValid() || !(index.flags().testFlag(Qt::ItemIsEditable)))
    return false;

  int channelID = ((index.row() - 1) * DMX_CHANNEL_OUTPUT_TABLE_MODEL_COLUMNS_COUNT_DEFAULT)
      + index.column();
  auto dmxChannel = m_L_dmxChannel.at(channelID);
  dmxChannel->setLevel(value.toInt());
  return true;

}

QVariant DmxChannelOutputTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  return QVariant();
}

bool DmxChannelOutputTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  return true;
}

Qt::ItemFlags DmxChannelOutputTableModel::flags(const QModelIndex &index) const
{
  // TODO : handle universe with less than 512 channel
  if (!index.isValid())
    return Qt::NoItemFlags;

  if (!(index.row()%2)) // ligne paire.
    return Qt::ItemIsEnabled;
  else // ligne impaire
    return Qt::ItemIsEnabled
        | Qt::ItemIsSelectable
        | Qt::ItemIsEditable;


  return QAbstractTableModel::flags(index);
}
