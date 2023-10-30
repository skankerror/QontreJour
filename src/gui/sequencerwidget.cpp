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

#include "sequencerwidget.h"
#include <QLayout>
#include <QDebug>

/********************** SequencerWidget ****************************/

SequencerWidget::SequencerWidget(Sequence *t_seq,
                                 QWidget *parent)
  : QWidget(parent),
    m_treeViewTop(new SequencerTreeView(this)),
    m_timeWidget(new SequencerTimeWidget(this)),
    m_treeViewBottom(new SequencerTreeView(this)),
    m_model(new SequencerTreeModel(t_seq))
{
  auto layout = new QVBoxLayout();

  m_treeViewTop->setModel(m_model);
  m_treeViewBottom->setModel(m_model);
  m_treeViewBottom->setHeaderHidden(true);

  updateTableViews();

  layout->addWidget(m_treeViewTop);
  layout->addStretch();
  layout->addWidget(m_timeWidget);
  layout->addWidget(m_treeViewBottom);
  setLayout(layout);

  connect(t_seq,
          &Sequence::seqSizeChanged,
          this,
          &SequencerWidget::updateTableViews);
}

SequencerWidget::~SequencerWidget()
{}

void SequencerWidget::updateTableViews()
{
  qsizetype seqSize = m_model->getRootItem()->getL_childScene().size();
  for (qsizetype i = 1;
       i < seqSize;
       i++)
  {
    m_treeViewTop->setRowHidden(i,
                                QModelIndex(),
                                true);
  }

  m_treeViewBottom->setRowHidden(0,
                                 QModelIndex(),
                                 true);

}

/********************** SequencerTimeWidget**************************/

SequencerTimeWidget::SequencerTimeWidget(QWidget *parent,
                                         const Qt::WindowFlags &f)
    : QWidget(parent,
              f)
{}

/********************** SequencerTreeView ****************************/

SequencerTreeView::SequencerTreeView(QWidget *parent)
  : QTreeView(parent)
{}

SequencerTreeView::~SequencerTreeView()
{}

/********************* SequencerTreeModel **************************/

SequencerTreeModel::SequencerTreeModel(Sequence *t_seq,
                                       QAbstractItemModel *parent)
  : QAbstractItemModel(parent),
    m_rootItem(t_seq)
{}

SequencerTreeModel::~SequencerTreeModel()
{
  m_rootItem->deleteLater();
}

DmxValue *SequencerTreeModel::getDmxValue(const QModelIndex &index) const
{
  if (!index.isValid()) return m_rootItem;

  auto scene = static_cast<DmxScene *>(index.internalPointer());
  if (scene) return scene;
  return nullptr;
}


QModelIndex SequencerTreeModel::index(int row, int column, const QModelIndex &parent) const
{
//  if (!parent.isValid()) return QModelIndex();

  // TODO check 1ère ligne
  if (parent.isValid() && parent.column() !=0) return QModelIndex();

  auto parentValue = getDmxValue(parent);
  if (!parentValue) return QModelIndex();
  // si c'est le rootitem
  auto rootItem = qobject_cast<Sequence *>(parentValue);
  if (rootItem)
  {
    auto childScene = m_rootItem->getScene((id)row);
    return createIndex(row,
                       column,
                       childScene);
  }
  // si c'est une main scene
  auto parentScene = qobject_cast<DmxScene *>(parentValue);
  if (parentScene)
  {
    auto childScene = parentScene->getL_subScene().at(row);
    if (childScene)
      return createIndex(row,
                         column,
                         childScene);

  }
  return QModelIndex();
}

QModelIndex SequencerTreeModel::parent(const QModelIndex &child) const
{
  if (!child.isValid()) return QModelIndex();

  auto childValue = getDmxValue(child);

  // si c'est une scene
  auto mainScene = qobject_cast<DmxScene *>(childValue);
  if (mainScene) return QModelIndex();

  // si c'est une subscene
  auto subScene = qobject_cast<SubScene *>(childValue);
  if (subScene)
  {
    auto parentScene = subScene->getParentScene();
    return createIndex(parentScene->getStepNumber(),
                       0,
                       parentScene);
  }
  return QModelIndex();
}

int SequencerTreeModel::rowCount(const QModelIndex &parent) const
{
  auto parentValue = getDmxValue(parent);
  auto rootItem = qobject_cast<Sequence *>(parentValue);
  if (rootItem)
  {
    qsizetype size = rootItem->getSize();
    return size;
  }

  auto parentScene = qobject_cast<DmxScene *>(parentValue);
  if (parentScene) return parentScene->getL_subScene().size();

  return 0;
}

int SequencerTreeModel::columnCount(const QModelIndex &parent) const
{
  return HeaderFieldCount;
}

QVariant SequencerTreeModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role != Qt::DisplayRole && role != Qt::EditRole)
    return QVariant();

  auto scene = qobject_cast<DmxScene *>(getDmxValue(index));

  if (scene)
  {
    int col = index.column();
    switch(col)
    {
    case StepField : return scene->getStepNumber(); break;
    case IDField : return scene->getSceneID(); break;
    case NameField : return scene->getName(); break;
    case NoteField : return scene->getNotes(); break;
    case InField : return scene->getTimeIn(); break;
    case OutField : return scene->getTimeOut(); break;
    case DelayInField : return scene->getDelayIn(); break;
    case DelayOutField : return scene->getDelayOut(); break;
    default : break;
    }
  }
  return QVariant();

}

bool SequencerTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (role != Qt::EditRole)
      return false;

  if (!index.isValid() || !(index.flags().testFlag(Qt::ItemIsEditable)))
    return false;

  auto scene = qobject_cast<DmxScene *>(getDmxValue(index));
  if (!scene) return false;

  int col = index.column();
  switch(col)
  {
  case IDField : scene->setSceneID(value.toInt()); emit dataChanged(index,index); return true; break;
  case NameField : scene->setName(value.toString()); emit dataChanged(index,index); return true; break;
  case NoteField : scene->setNotes(value.toString()); emit dataChanged(index,index); return true; break;
  case InField : scene->setTimeIn(value.toFloat()); emit dataChanged(index,index); return true; break;
  case OutField : scene->setTimeOut(value.toFloat()); emit dataChanged(index,index); return true; break;
  case DelayInField : scene->setDelayIn(value.toFloat()); emit dataChanged(index,index); return true; break;
  case DelayOutField : scene->setDelayOut(value.toFloat()); emit dataChanged(index,index); return true; break;
  default : break;
  }

  return false;

}

QVariant SequencerTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole)
  {
    if (orientation == Qt::Horizontal)
    {
      switch(section)
      {
      case StepField : return QString("Step"); break;
      case IDField : return QString("N°"); break;
      case NameField : return QString("Name"); break;
      case NoteField : return QString("Note"); break;
      case InField : return QString("IN"); break;
      case OutField : return QString("OUT"); break;
      case DelayInField : return QString("delay IN"); break;
      case DelayOutField : return QString("delay OUT"); break;
      default : break;
      }
      return QVariant();
    }
  }
  if (role == Qt::TextAlignmentRole
      && orientation == Qt::Horizontal)
    return Qt::AlignCenter;

  return QVariant();
}

bool SequencerTreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  Q_UNUSED(section)
  Q_UNUSED(orientation)
  Q_UNUSED(value)
  Q_UNUSED(role)
  return false;
}

//bool SequencerTreeModel::insertRows(int row, int count, const QModelIndex &parent)
//{
//  return false;
//}

//bool SequencerTreeModel::removeRows(int row, int count, const QModelIndex &parent)
//{
//  return false;
//}

//bool SequencerTreeModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
//{
//  return false;
//}

Qt::ItemFlags SequencerTreeModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    return Qt::NoItemFlags;

  auto scene = qobject_cast<DmxScene *>(getDmxValue(index));
  if (scene)
    if (scene->getStepNumber() == 0)
      return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  if (index.column() == 0)
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  else return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;

  return QAbstractItemModel::flags(index);
}

