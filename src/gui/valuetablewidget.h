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

private :

  void setRootValue(RootValue *t_rootValue);

public slots :

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

  void setChannelDataEngine(ChannelDataEngine *t_channelDataEngine)
  { m_channelDataEngine = t_channelDataEngine; }

protected :

  virtual void mousePressEvent(QMouseEvent *event) override;
  virtual void mouseReleaseEvent(QMouseEvent *event) override;
  virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
  virtual void mouseMoveEvent(QMouseEvent *event) override;

private :

  int getChannelIdFromIndex(QModelIndex t_index);

signals :

  void beginMouseEditing(QModelIndex,
                         QPoint);
  void endMouseEditing();

protected :

  ChannelDataEngine *m_channelDataEngine;

  bool m_isEditing = false;
  QPoint m_originEditingPoint;
  id m_channelIdEdited = NO_ID;

};


/************************* ValueTableModel ******************************/

class ValueTableModel
    : public QAbstractTableModel
{

  Q_OBJECT

public :

  explicit ValueTableModel(QObject *parent = nullptr)
      : QAbstractTableModel(parent)
  {}

  virtual ~ValueTableModel(){}

protected :

  int rowCount(const QModelIndex &parent) const override
  { return parent.isValid() ? 0 : DMX_VALUE_TABLE_MODEL_ROWS_COUNT_DEFAULT; }
  int columnCount(const QModelIndex &parent) const override
  { return parent.isValid() ? 0 : DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT; }
  QVariant data(const QModelIndex &index,
                int role) const override
  { return QVariant(); }
  bool setData(const QModelIndex &index,
               const QVariant &value,
               int role) override
  { Q_UNUSED(index) Q_UNUSED(value) Q_UNUSED(role) return false; }

  QVariant headerData(int section,
                      Qt::Orientation orientation,
                      int role) const override
  { return QVariant(); }

  bool setHeaderData(int section,
                     Qt::Orientation orientation,
                     const QVariant &value,
                     int role) override
  { return false; }

  Qt::ItemFlags flags(const QModelIndex &index) const override
  { if (!index.isValid()) return Qt::NoItemFlags;
    return Qt::ItemIsEnabled;}
};

/************************* ChannelDelegate ******************************/

class ChannelDelegate
    : public QStyledItemDelegate
{

  Q_OBJECT

public:

  explicit ChannelDelegate(QObject *parent = nullptr);

  virtual void paint(QPainter *painter,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index) const override;
  virtual QSize sizeHint(const QStyleOptionViewItem &option,
                         const QModelIndex &index) const override;

  void recieveValuePlusFromMouse(const bool t_isPlus);

  void setChannelDataEngine(ChannelDataEngine *t_channelDataEngine)
  { m_channelDataEngine = t_channelDataEngine; }

private :

  ChannelDataEngine *m_channelDataEngine;
};

#endif // VALUETABLEWIDGET_H
