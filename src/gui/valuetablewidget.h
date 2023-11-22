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

#ifndef VALUETABLEWIDGET_H
#define VALUETABLEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QTableView>
#include <QAbstractTableModel>
#include <QStyledItemDelegate>
#include <QEvent>
#include "../core/dmxvalue.h"
#include "../core/channeldataengine.h"


class ValueTableView;
class ValueTableModel;
class ChannelDelegate;

class ValueTableWidget
    : public QWidget
{

  Q_OBJECT

public :

  explicit ValueTableWidget(QWidget *parent = nullptr);

  virtual ~ValueTableWidget();

  ValueTableModel *getModel() const{ return m_model; }

private :

  void setRootValue(RootValue *t_rootValue);

signals :

public slots :

//  void setL_channelData(QList<ChannelData *> t_L_channelData);
  void setChannelDataEngine(ChannelDataEngine *t_cdEngine);

protected slots :

  void repaintTableView();

protected :

  ValueTableView *m_tableView;
  ValueTableModel *m_model;
  ChannelDelegate *m_channelDelegate;

};

/************************* ValueTableView ******************************/

class ValueTableView
    : public QTableView
{

  Q_OBJECT

public :

  explicit ValueTableView(QWidget *parent = nullptr);

  virtual ~ValueTableView();

protected :

  virtual void mousePressEvent(QMouseEvent *event) override;
  virtual void mouseReleaseEvent(QMouseEvent *event) override;
  virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
  virtual void mouseMoveEvent(QMouseEvent *event) override;

signals :

  void beginMouseEditing(QModelIndex,
                         QPoint);
  void endMouseEditing();

protected :

  bool m_isEditing = false;
  QPoint m_originEditingPoint;

};


/************************* ValueTableModel ******************************/

class ValueTableModel
    : public QAbstractTableModel
{

  Q_OBJECT

public :

  explicit ValueTableModel(QObject *parent = nullptr);

  virtual ~ValueTableModel();

//  void recieveValueFromMouse(const QModelIndex &t_index,
//                             const int t_value);

public slots :

  void setRootValue(RootValue *t_rootValue){ m_rootValue = t_rootValue; }
  void onSelectionChanged(QList<id> L_id);

protected :

  int rowCount(const QModelIndex &parent) const override;
  int columnCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index,
                int role) const override;
  bool setData(const QModelIndex &index,
               const QVariant &value,
               int role) override;
  QVariant headerData(int section,
                      Qt::Orientation orientation,
                      int role) const override;
  bool setHeaderData(int section,
                     Qt::Orientation orientation,
                     const QVariant &value,
                     int role) override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;

protected :

  RootValue *m_rootValue;
};

/************************* ChannelDelegate ******************************/

class ChannelDelegate
    : public QStyledItemDelegate
{

  Q_OBJECT

public:

  explicit ChannelDelegate(QObject *parent = nullptr);

  RootValue *getRootValue() const{ return m_rootValue; }

  // QAbstractItemDelegate interface
  virtual void paint(QPainter *painter,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index) const override;
  virtual QSize sizeHint(const QStyleOptionViewItem &option,
                         const QModelIndex &index) const override;

  void recieveValueFromMouse(const int t_value);

  void setChannelDataEngine(ChannelDataEngine *t_channelDataEngine)
  { m_channelDataEngine = t_channelDataEngine; }

public slots :

  void setRootValue(RootValue *t_rootValue){ m_rootValue = t_rootValue; }

private :

  RootValue *m_rootValue;

  ChannelDataEngine *m_channelDataEngine;
};

#endif // VALUETABLEWIDGET_H
