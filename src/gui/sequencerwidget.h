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

#ifndef SEQUENCERWIDGET_H
#define SEQUENCERWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QTreeView>
#include <QAbstractItemModel>
#include "../core/dmxscene.h"


class SequencerTreeView;
class SequencerTreeModel;


class SequencerWidget
    : public QWidget
{

  Q_OBJECT

public:

  explicit SequencerWidget(QWidget *parent = nullptr);

  virtual ~SequencerWidget();

signals:

protected :

  SequencerTreeView *m_treeView;
  SequencerTreeModel *m_model;

};

/******************************************************************/

class SequencerTreeView
    : public QTreeView
{

  Q_OBJECT

public:

  explicit SequencerTreeView(QWidget *parent = nullptr);

  virtual ~SequencerTreeView();

};

/***************************************************************/

class SequencerTreeModel
    : public QAbstractItemModel
{

  Q_OBJECT

public:

  enum HeaderField
  {
    StepField,
    IDField,
    NameField,
    NoteField,
    InField,
    OutField,
    DelayInField,
    DelayOutField,
    HeaderFieldCount
  };
  Q_ENUM(HeaderField)

  explicit SequencerTreeModel(QAbstractItemModel *parent = nullptr);

  virtual ~SequencerTreeModel();

  DmxScene *getScene(const QModelIndex &index) const;

protected :

  virtual QModelIndex index(int row, int column, const QModelIndex &parent) const override;
  virtual QModelIndex parent(const QModelIndex &child) const override;
  virtual int rowCount(const QModelIndex &parent) const override;
  virtual int columnCount(const QModelIndex &parent) const override;
//  virtual bool hasChildren(const QModelIndex &parent) const override;
  virtual QVariant data(const QModelIndex &index, int role) const override;
  virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role) override;
//  virtual bool insertRows(int row, int count, const QModelIndex &parent) override;
//  virtual bool removeRows(int row, int count, const QModelIndex &parent) override;
//  virtual bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;
  virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

protected :

  DmxScene *m_rootItem;

};

#endif // SEQUENCERWIDGET_H
