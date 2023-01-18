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

#include "dmxchanneloutputtabledelegate.h"
#include <QPainter>
#include "../qontrejour.h"

DmxChannelOutputTableDelegate::DmxChannelOutputTableDelegate(QObject *parent)
  : QStyledItemDelegate{parent}
{

}


void DmxChannelOutputTableDelegate::paint(QPainter *painter,
                                          const QStyleOptionViewItem &option,
                                          const QModelIndex &index) const
{
  painter->save();
  if (!index.isValid()) return;
  if (!(index.row()%2)) // ligne paire. channel ID
  {
    painter->setBackgroundMode(Qt::OpaqueMode);
    QTextOption textOption;
    textOption.setAlignment(Qt::AlignCenter);
    painter->fillRect(option.rect,
                      QBrush(QColor("#3C559A")));
    painter->drawText(option.rect,
                      QString::number(((index.row() / 2 ) * DMX_CHANNEL_OUTPUT_TABLE_MODEL_COLUMNS_COUNT_DEFAULT)
                                      + index.column() + 1),
                      textOption);
  }
  painter->restore();
  if (index.row()%2)
    QStyledItemDelegate::paint(painter, option, index);

}
