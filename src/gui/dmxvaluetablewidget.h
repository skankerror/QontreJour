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

#ifndef DMXVALUETABLEWIDGET_H
#define DMXVALUETABLEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QTableView>
#include <QAbstractTableModel>
#include <QEvent>
#include "../core/dmxvalue.h"


class DmxValueTableView;
class DmxValueTableModel;

class DmxValueTableWidget
    : public QWidget
{

  Q_OBJECT

public:

  explicit DmxValueTableWidget(QWidget *parent = nullptr);

  virtual ~DmxValueTableWidget();

signals :

  void askForUniverseChanged(int); // universeID to ask to mainwindow

public slots:

  void onUniverseCountChanged(int t_universeCount);
  void setUniverseID(const int t_ID);
  void setRootValue(DmxValue *t_rootValue);

protected slots :

  void onSpinboxSelected(int t_universeID);
  void repaintTableView();

protected :

  DmxValueTableView *m_tableView;
  DmxValueTableModel *m_model;
  QSpinBox *m_universeSpinBox;
  QPushButton *m_selectAll;
  QPushButton *m_recGroup;
  QPushButton *m_recScene;
  QPushButton *m_clearSelectionButton;
  int m_universeCount;

};

/******************************************************************/

class DmxValueTableView
    : public QTableView
{

  Q_OBJECT

public :

  explicit DmxValueTableView(QWidget *parent = nullptr);

  virtual ~DmxValueTableView();

protected:

  virtual void mousePressEvent(QMouseEvent *event) override;
  virtual void mouseReleaseEvent(QMouseEvent *event) override;
  virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
  virtual void mouseMoveEvent(QMouseEvent *event) override;

signals :

  void beginMouseEditing(QModelIndex, QPoint);
  void endMouseEditing();

protected :

  bool m_isEditing = false;
  QPoint m_originEditingPoint;

};


/********************************************************************/

class DmxValueTableModel
    : public QAbstractTableModel
{

  Q_OBJECT

public:

  explicit DmxValueTableModel(QObject *parent = nullptr);

  virtual ~DmxValueTableModel();

  DmxValue *getRootValue() const{ return m_rootValue; }
  uid getUniverseID() const { return m_universeID; }
  QModelIndexList getEditedIndexes() const{ return m_editedIndexes; }

public slots:

  void setRootValue(DmxValue *t_rootValue){ m_rootValue = t_rootValue; }
  void setUniverseID(uid t_universeID) { m_universeID = t_universeID; }
  void setEditedIndexes(const QModelIndexList &t_editedIndexes);

  void addEditedIndex(QModelIndex &t_editedIndexes);

  void clearSelectionList();
  void selectAll();


private slots :

  void editedIndexChanged();

protected :

  int rowCount(const QModelIndex &parent) const override;
  int columnCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;
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

private :

  QModelIndex getIndexFromValue(const DmxValue *t_value ) const;
  QModelIndexList getNon0ValueIndexList() const;

protected :

  uid m_universeID;
  DmxValue *m_rootValue;
  QModelIndexList m_editedIndexes;

};

#endif // DMXVALUETABLEWIDGET_H
