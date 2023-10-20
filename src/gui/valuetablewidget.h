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
#include <QEvent>
#include "../core/dmxvalue.h"


class ValueTableView;
class ValueTableModel;

class ValueTableWidget
    : public QWidget
{

  Q_OBJECT

public :

  explicit ValueTableWidget(QWidget *parent = nullptr);

  virtual ~ValueTableWidget();

  ValueTableModel *getModel() const{ return m_model; }

signals :


public slots :

  void setRootValue(RootValue *t_rootValue);

protected slots :

  void repaintTableView();

protected :

  ValueTableView *m_tableView;
  ValueTableModel *m_model;

};

/******************************************************************/

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


/********************************************************************/

class ValueTableModel
    : public QAbstractTableModel
{

  Q_OBJECT

public :

  explicit ValueTableModel(QObject *parent = nullptr);

  virtual ~ValueTableModel();

  DmxValue *getRootValue() const{ return m_rootValue; }
  uid getUniverseID() const { return m_universeID; }
  QModelIndexList getEditedIndexes() const{ return m_editedIndexes; }

public slots :

  void setRootValue(RootValue *t_rootValue){ m_rootValue = t_rootValue; }
  void setUniverseID(uid t_universeID) { m_universeID = t_universeID; }
  void setEditedIndexes(const QModelIndexList &t_editedIndexes);

  void addEditedIndex(QModelIndex &t_editedIndexes);

  void clearSelectionList();
  void selectAll();

  //public because we need this edit widgets
  void editedIndexChanged();

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

  // to show only non 0 values
  QVariant filterData(const QModelIndex &index,
                      int role) const;
  bool setFilterData(const QModelIndex &index,
                     const QVariant &value,
                     int role);


private :

  DmxValue *getValueFromIndex(const QModelIndex &t_index) const;
  QList<DmxValue *> getValuesFromIndexList(const QModelIndexList & t_L_index) const;
  QModelIndex getIndexFromValue(const DmxValue *t_value ) const;
  QModelIndexList getNon0ValueIndexList() const;

protected :

  uid m_universeID;
  RootValue *m_rootValue;
  QModelIndexList m_editedIndexes;

};



#endif // VALUETABLEWIDGET_H
