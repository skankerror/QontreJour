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

SequencerWidget::SequencerWidget(QWidget *parent)
  : QWidget(parent),
    m_treeView(new SequencerTreeView(this)),
    m_model(new SequencerTreeModel())/*,
    m_dmxSequence(t_dmxSequence)*/
{
  auto layout = new QVBoxLayout();

//  m_model->setSequence(m_dmxSequence);
  m_treeView->setModel(m_model);

  layout->addWidget(m_treeView);
  setLayout(layout);
}

SequencerWidget::~SequencerWidget()
{}

/************************************************************/

SequencerTreeView::SequencerTreeView(QWidget *parent)
  : QTreeView(parent)
{}

SequencerTreeView::~SequencerTreeView()
{}

/*********************************************************/

SequencerTreeModel::SequencerTreeModel(QAbstractItemModel *parent)
  : QAbstractItemModel(parent)
{
  QString name = "Root Item";
  m_rootItem = new DmxScene(0, name);
  m_rootItem->setType(DmxScene::RootScene);

  // test
  name = QString("accueil public");
  auto scene = new DmxScene(10, name, this);
  scene->setTimeIn(5.0);
  scene->setTimeOut(5.0);
  scene->setDelayIn(0);
  scene->setDelayOut(0);
  scene->setType(DmxScene::MainScene);
  scene->setParentSCene(m_rootItem);
  scene->setStepNumber(0);
  m_rootItem->addScene(scene);

//  m_sequence->addScene(scene);
}

SequencerTreeModel::~SequencerTreeModel()
{
  m_rootItem->deleteLater();
}

DmxScene *SequencerTreeModel::getScene(const QModelIndex &index) const
{
  if (index.isValid())
  {
    auto scene = static_cast<DmxScene *>(index.internalPointer());
    if (scene) return scene;
  }
  return m_rootItem;
}


QModelIndex SequencerTreeModel::index(int row, int column, const QModelIndex &parent) const
{
  if (parent.isValid() && parent.column() !=0) return QModelIndex();

  auto parentScene = getScene(parent);
  if (!parentScene) return QModelIndex();
  /*DmxScene *childScene;
  if (parentScene == m_rootItem) childScene = m_sequence->getScene(row);
  else*/auto childScene = parentScene->getSubscene(row);
  if (childScene)
    return createIndex(row,
                       column,
                       childScene);

  return QModelIndex();
//  if (!parent.isValid()) return QModelIndex();
//  if (parent.column() !=0) return QModelIndex();
//  auto parentScene = static_cast<DmxScene *>(parent.internalPointer());
////  if (!parentScene) return QModelIndex();
//  if (!parentScene) return createIndex(0,
//                                       0,
//                                       m_rootItem);
//  auto L_subScene = parentScene->getL_subScene();
//  auto size = L_subScene.size();
//  if ((size == 0)
//      || (row < (size - 1))) return QModelIndex();
//  auto scene = L_subScene.at(row);
//  if (!scene) return QModelIndex();
//  return createIndex(row,
//                     column,
//                     scene);
}

QModelIndex SequencerTreeModel::parent(const QModelIndex &child) const
{
  if (!child.isValid()) return QModelIndex();

  auto childScene = getScene(child);
  auto parentScene = childScene
      ? childScene->getParentSCene()
      : nullptr;

  if (parentScene == m_rootItem
      || !parentScene)
    return QModelIndex();

  return createIndex(parentScene->getStepNumber(),
                     0,
                     parentScene);


//  if (!child.isValid()) return QModelIndex();
//  auto childScene = static_cast<DmxScene *>(child.internalPointer());
//  auto type = childScene->DmxScene::getType();
//  if (type == DmxScene::RootScene)
//    return QModelIndex();
//  if (type == DmxScene::MainScene)
//    return createIndex(0,
//                       0,
//                       m_rootItem);
//  if (type == DmxScene::SubScene)
//    return createIndex(childScene->getStepNumber(),
//                       0,
//                       childScene->getParentSCene());

//  return QModelIndex();

}

int SequencerTreeModel::rowCount(const QModelIndex &parent) const
{
  auto parentScene = getScene(parent);
  if (parentScene == m_rootItem)
  {
    qDebug() << "rowCount for rootItem : "
             << m_rootItem->getSize();
    return m_rootItem->getSize();
  }
  int ret = parentScene ? parentScene->getSize() : 0;
  qDebug() << "rowCount for : "
           << parentScene
//           << "type : "
//           << parentScene->getType()
           << " : "
           << ret;

  return ret;
//  return parentScene ? parentScene->getSize() : 0;
//  auto parentScene = static_cast<DmxScene *>(parent.internalPointer());
//  if (!parentScene) parentScene = m_rootItem;
//  return parentScene ? parentScene->getSize() : 0;

//  if (!parentScene) return 0;
////  if (parentScene == m_rootItem) return m_sequence->getSize();
//  auto parentType = parentScene->DmxScene::getType();
//  if (parentType == DmxScene::RootScene) return m_sequence->getSize();
//  if (parentType == DmxScene::MainScene) return parentScene->getSize();
//  if (parentType == DmxScene::SubScene) return 0;

//  return 0;
}

int SequencerTreeModel::columnCount(const QModelIndex &parent) const
{
  return HeaderFieldCount;
}

//bool SequencerTreeModel::hasChildren(const QModelIndex &parent) const
//{
////  if (!parentScene) return false;
////  if (parentScene == m_rootItem) return true;
//  if (!parent.isValid()) return false;
//  int row = parent.row();
//  int col = parent.column();
//  if (row == 0
//      && col == 0)
//    return true;
//  auto parentScene = static_cast<DmxScene *>(parent.internalPointer());

//  auto parentType = parentScene->DmxScene::getType();
//  if (parentType == DmxScene::RootScene) return true;
//  if (parentType == DmxScene::SubScene) return false;
//  if (parentType == DmxScene::MainScene) return parentScene->getSize();
//  return false;
//}

QVariant SequencerTreeModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role != Qt::DisplayRole && role != Qt::EditRole)
    return QVariant();

//  auto scene = static_cast<DmxScene *>(index.internalPointer());
  auto scene = getScene(index);
  int col = index.column();
  switch(col)
  {
  case StepField : return scene->getStepNumber(); break;
  case IDField : return scene->getID(); break;
  case NameField : return scene->getName(); break;
  case NoteField : return scene->getNotes(); break;
  case InField : return scene->getTimeIn(); break;
  case OutField : return scene->getTimeOut(); break;
  case DelayInField : return scene->getDelayIn(); break;
  case DelayOutField : return scene->getDelayOut(); break;
  default : break;
  }
  return QVariant();

}

bool SequencerTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (role != Qt::EditRole)
      return false;

  if (!index.isValid() || !(index.flags().testFlag(Qt::ItemIsEditable)))
    return false;

//  auto scene = static_cast<DmxScene *>(index.internalPointer());
  auto scene = getScene(index);
  if (!scene) return false;
  int col = index.column();
  switch(col)
  {
  case IDField : scene->setID(value.toInt()); emit dataChanged(index,index); return true; break;
  case NameField : scene->setName(value.toString()); emit dataChanged(index,index); return true; break;
  case NoteField : scene->setNotes(value.toString()); emit dataChanged(index,index); return true; break;
  case InField : scene->setTimeIn(value.toDouble()); emit dataChanged(index,index); return true; break;
  case OutField : scene->setTimeOut(value.toDouble()); emit dataChanged(index,index); return true; break;
  case DelayInField : scene->setDelayIn(value.toDouble()); emit dataChanged(index,index); return true; break;
  case DelayOutField : scene->setDelayOut(value.toDouble()); emit dataChanged(index,index); return true; break;
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
  if (!index.isValid()) return Qt::NoItemFlags;
  if (index.column() == 0) return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  else return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;

  return QAbstractItemModel::flags(index);
}
