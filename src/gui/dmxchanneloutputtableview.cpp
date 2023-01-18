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

#include "dmxchanneloutputtableview.h"
#include <QHeaderView>
#include "../qontrejour.h"

DmxChannelOutputTableView::DmxChannelOutputTableView(QWidget *parent)
  : QTableView(parent)
{
  setSortingEnabled(false);
  horizontalHeader()->setMinimumSectionSize(49);
//  horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  verticalHeader()->setMinimumSectionSize(49);
  horizontalHeader()->hide();
  verticalHeader()->hide();
//  for (int i = 0; i < DMX_CHANNEL_OUTPUT_TABLE_MODEL_COLUMNS_COUNT_DEFAULT; i++)
//  {
//    setColumnWidth(i, 10);
//  }
  resizeColumnsToContents();

}
