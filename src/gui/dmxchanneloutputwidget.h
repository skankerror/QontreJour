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
#include <QTableView>
#include <QAbstractTableModel>
#include <QStyledItemDelegate>
#include "../core/dmxchannel.h"

class DmxChannelOutputTableView;
class DmxChannelOutputTableModel;
class DmxChannelOutputTableDelegate;

class DmxChannelOutputWidget
    : public QWidget
{

  Q_OBJECT

public:

  explicit DmxChannelOutputWidget(QWidget *parent = nullptr);


signals:

public slots:

  void onUniverseCountChanged(int t_universeCount);
  void setModel(DmxChannelOutputTableModel *t_model);
  void setDelegate(DmxChannelOutputTableDelegate *t_delegate);

private slots :

  void repaintTableView();

private:

//  DmxChannelOutputTableView *m_tableView;
  QTableView *m_tableView;
  QSpinBox *m_universeSpinBox;
  int m_universeCount;

};

/********************************************************************/

//class DmxChannelOutputTableView
//    : public QTableView
//{

//  Q_OBJECT

//public:

//  DmxChannelOutputTableView(QWidget *parent);

//};

/********************************************************************/

class DmxChannelOutputTableModel
    : public QAbstractTableModel
{

  Q_OBJECT

//  friend class DmxChannelOutputTableDelegate;

public:

  explicit DmxChannelOutputTableModel(QObject *parent = nullptr);

  QList<DmxChannel *> getL_dmxChannel() const { return m_L_dmxChannel; }
  void setL_dmxChannel(const QList<DmxChannel *> &t_L_dmxChannel){ m_L_dmxChannel = t_L_dmxChannel; }
  int getUniverseID() const { return m_universeID; }
  void setUniverseID(int t_universeID) { m_universeID = t_universeID; }

  // overrides
  int rowCount(const QModelIndex &parent) const override;
  int columnCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  bool setData(const QModelIndex &index, const QVariant &value, int role) override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role) override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;

public slots :

  void onChannelLevelChanged() {/*update();*/}

private:

  QList<DmxChannel *> m_L_dmxChannel;
  int m_universeID;

};

/*********************************************************************/

class DmxChannelOutputTableDelegate
    : public QStyledItemDelegate
{

  Q_OBJECT

public:

  explicit DmxChannelOutputTableDelegate(QObject *parent = nullptr);
  void setL_dmxChannel(const QList<DmxChannel *> &t_L_dmxChannel){ m_L_dmxChannel = t_L_dmxChannel; }
  int getUniverseID() const { return m_universeID; }
  void setUniverseID(int t_universeID) { m_universeID = t_universeID; }

  // override
  void paint(QPainter *painter,
             const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;
  QWidget *createEditor(QWidget *parent,
                        const QStyleOptionViewItem &option,
                        const QModelIndex &index) const override;
  void setEditorData(QWidget *editor,
                     const QModelIndex &index) const override;
  void setModelData(QWidget *editor,
                    QAbstractItemModel *model,
                    const QModelIndex &index) const override;


private:

  QList<DmxChannel *> m_L_dmxChannel;
  int m_universeID;

};


#endif // DMXCHANNELOUTPUTWIDGET_H
