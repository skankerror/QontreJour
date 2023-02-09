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

#include "valuetablewidget.h"
#include "../qontrejour.h"
#include <QLayout>
#include <QHeaderView>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include "../core/dmxmanager.h"


ValueTableWidget::ValueTableWidget(QWidget *parent)
  : QWidget(parent),
    m_tableView(new DmxValueTableView(this)),
    m_model(new DmxValueTableModel(this)),
    m_universeSpinBox(new QSpinBox(this)),
    m_selectAll(new QPushButton("All", this)),
    m_recGroup(new QPushButton("Rec Group", this)),
    m_recScene(new QPushButton("Rec Scene", this)),
    m_clearSelectionButton(new QPushButton("C", this))
{
  m_model->setRootValue(nullptr);
  auto totalLayout = new QVBoxLayout();
  auto headerLayout = new QHBoxLayout();
  auto bottomLayout = new QHBoxLayout();

  auto label = new QLabel("Channels on Universe :", this);

  m_universeSpinBox->setMaximum(1);// at the beginning ther's only one universe
  m_universeSpinBox->setMinimum(1);// there's always at least one universe

  headerLayout->addStretch();
  headerLayout->addWidget(label);
  headerLayout->addWidget(m_universeSpinBox);
  bottomLayout->addStretch();
  bottomLayout->addWidget(m_selectAll);
  bottomLayout->addWidget(m_recGroup);
  bottomLayout->addWidget(m_recScene);
  bottomLayout->addWidget(m_clearSelectionButton);

  totalLayout->addLayout(headerLayout);
  totalLayout->addWidget(m_tableView);
  totalLayout->addLayout(bottomLayout);

  setLayout(totalLayout);

  m_tableView->setSortingEnabled(false);
  m_tableView->setUpdatesEnabled(true);
  m_tableView->horizontalHeader()->setMinimumSectionSize(38);
  m_tableView->horizontalHeader()->hide();
  m_tableView->verticalHeader()->hide();

  m_tableView->setModel(m_model);

  m_tableView->resizeColumnsToContents();
  m_tableView->resizeRowsToContents();

  connect(m_universeSpinBox,
          SIGNAL(valueChanged(int)),
          this,
          SLOT(onSpinboxSelected(int)));

  connect (m_clearSelectionButton,
           SIGNAL(clicked(bool)),
           m_model,
           SLOT(clearSelectionList()));

  connect(m_selectAll,
          SIGNAL(clicked(bool)),
          m_model,
          SLOT(selectAll()));

  connect(m_recGroup,
          SIGNAL(clicked(bool)),
          m_model,
          SLOT(recordGroup()));

}

ValueTableWidget::~ValueTableWidget()
{}

void ValueTableWidget::onUniverseCountChanged(int t_universeCount)
{
  m_universeCount = t_universeCount;
  m_universeSpinBox->setMaximum(t_universeCount);
  m_tableView->resizeColumnsToContents();
  m_tableView->resizeRowsToContents();

}

void ValueTableWidget::setUniverseID(const uid t_ID)
{
  m_model->setUniverseID(t_ID);

  disconnect(m_universeSpinBox,
             SIGNAL(valueChanged(int)),
             this,
             SLOT(onSpinboxSelected(int)));

  m_universeSpinBox->setValue(t_ID + 1);

  connect(m_universeSpinBox,
          SIGNAL(valueChanged(int)),
          this,
          SLOT(onSpinboxSelected(int)));

}

void ValueTableWidget::setRootValue(DmxValue *t_rootValue)
{
  // we connect to update views
  auto L_dmxChannel = t_rootValue->getL_childValue();
  m_model->setRootValue(t_rootValue);

  for (const auto &item : std::as_const(L_dmxChannel))
  {
    DmxValue *value = item;

    connect(value,
            SIGNAL(levelChanged(DmxValue::SignalSenderType,dmx)),
            this,
            SLOT(repaintTableView()));
  }
}

void ValueTableWidget::onSpinboxSelected(int t_universeID)
{
  emit askForUniverseChanged(t_universeID - 1);
}

void ValueTableWidget::repaintTableView()
{
  emit m_model->layoutChanged();
}


/*************************************************************************/

DmxValueTableView::DmxValueTableView(QWidget *parent)
  :QTableView(parent)
{}

DmxValueTableView::~DmxValueTableView()
{}

void DmxValueTableView::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
  {
    auto index = indexAt(event->pos());
    if (index.flags().testFlag(Qt::ItemIsEditable))
    {
      auto myModel = static_cast<DmxValueTableModel *>(model());
      auto indexList = myModel->getEditedIndexes();
      m_isEditing = true;
      m_originEditingPoint = event->pos();
      if (indexList.indexOf(index) == -1) // si l'index n'est pas ds la selection
      {
        myModel->clearSelectionList(); // on clear
        myModel->addEditedIndex(index); // on ajoute notre index
      }
      return;
    }
  }
  QTableView::mousePressEvent(event);
}

void DmxValueTableView::mouseReleaseEvent(QMouseEvent *event)
{
  if (m_isEditing) m_isEditing = false;

  QTableView::mouseReleaseEvent(event);
}

void DmxValueTableView::mouseDoubleClickEvent(QMouseEvent *event)
{
  QTableView::mouseDoubleClickEvent(event);
}

void DmxValueTableView::mouseMoveEvent(QMouseEvent *event)
{
  if (m_isEditing)
  {
    auto myModel = static_cast<DmxValueTableModel *>(model());
    auto indexList = myModel->getEditedIndexes();

    for (const auto &item
         : std::as_const(indexList))
    {
      auto value = item.data().toInt();
      auto yValue = event->pos().y()
          - m_originEditingPoint.y();
      value -= yValue;
      if (value > 255) value = 255;
      if (value < 0) value = 0;
      model()->setData(item,
                       value,
                       Qt::EditRole);
    }
    m_originEditingPoint = event->pos();
    return;
  }
  QTableView::mouseMoveEvent(event);
}

/************************************************************************/

DmxValueTableModel::DmxValueTableModel(QObject *parent)
  : QAbstractTableModel(parent)
{}

DmxValueTableModel::~DmxValueTableModel()
{}

void DmxValueTableModel::setEditedIndexes(const QModelIndexList &t_editedIndexes)
{
  m_editedIndexes = t_editedIndexes;
  editedIndexChanged();
}

void DmxValueTableModel::addEditedIndex(QModelIndex &t_editedIndexes)
{
  m_editedIndexes.append(t_editedIndexes);
  editedIndexChanged();
}

QModelIndexList DmxValueTableModel::getNon0ValueIndexList() const
{
  auto listRet = QModelIndexList();
  auto L_childValue = m_rootValue->getL_childValue();
  for(const auto &item
      : std::as_const(L_childValue))
  {
    if (item->getLevel() > 0) // TODO : problem with parked independant
    {
      auto index = getIndexFromValue(item);
      listRet.append(index);
    }
  }
  return listRet;
}

QModelIndex DmxValueTableModel::getIndexFromValue(const DmxValue *t_value) const
{
  auto valueID = t_value->getID();
  // determine row et column for the value
  auto row =  (2 * (valueID / DMX_VALUE_TABLE_MODEL_ROWS_COUNT_DEFAULT)) + 1;
  auto column = valueID % DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT;

  auto indexRet = index(row,
                        column);
  return indexRet;
}

void DmxValueTableModel::clearSelectionList()
{
  m_editedIndexes.clear();
//  editedIndexChanged();
  emit dataChanged(index(0,0),index(31,31)); // NOTE : BEURK !!
}

void DmxValueTableModel::selectAll()
{
  m_editedIndexes = getNon0ValueIndexList();
  editedIndexChanged();
}

void DmxValueTableModel::recordGroup()
{
//  auto L_channel = QList<DmxValue *>();

  DmxManager::instance()
      ->createChannelGroup(getValuesFromIndexList(
                             getNon0ValueIndexList()));
}

void DmxValueTableModel::editedIndexChanged()
{
  // we try to change the background of selected channels
  for (const auto &item
       : std::as_const(m_editedIndexes))
  {
    auto myIndex = index(item.row() - 1,
                         item.column());
    emit dataChanged(myIndex,
                     myIndex);
  }
}

int DmxValueTableModel::rowCount(const QModelIndex &parent) const
{
  return parent.isValid() ? 0 : DMX_VALUE_TABLE_MODEL_ROWS_COUNT_DEFAULT;
}

int DmxValueTableModel::columnCount(const QModelIndex &parent) const
{
  return parent.isValid() ? 0 : DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT;
}

QVariant DmxValueTableModel::data(const QModelIndex &index,
                                  int role) const
{
  if (!index.isValid())
    return QVariant();

  if (!m_rootValue)
    return QVariant();

  if (index.flags().testFlag(Qt::ItemIsEditable))
  {
    int valueID = (((index.row() -1)/2) * DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT)
        + index.column();
    if (valueID < 0 || valueID >= m_rootValue->getL_ChildValueSize())
      return QVariant();

    auto dmxValue = m_rootValue->getL_childValue().at(valueID);
    DmxValue::ChannelFlag flag = dmxValue->getChannelFlag();

    switch(role)
    {
    case Qt::DisplayRole :
    case Qt::EditRole :
      return dmxValue->getLevel();
      break;
    case Qt::TextAlignmentRole :
      return Qt::AlignCenter;
      break;
    case Qt::BackgroundRole:
      return QBrush(QColor(BLACK_COLOR));
      break;
    case Qt::ForegroundRole :
      switch(flag)
      {
      case DmxValue::SelectedSceneFlag :
        return QBrush(QColor(LIGHT_GREEN_COLOR));
        break;
      case DmxValue::DirectChannelFlag :
        return QBrush(QColor(LIGHT_YELLOW_COLOR));
        break;
      case DmxValue::ChannelGroupFlag :
        return QBrush(QColor(LIGHT_BLUE_COLOR));
        break;
      case DmxValue::ParkedFlag :
        return QBrush(QColor(RED_COLOR));
        break;
      case DmxValue::IndependantFlag :
        return QBrush(QColor(PURPLE_COLOR));
        break;
      case DmxValue::UnknownFlag :
        return QBrush(QColor(LIGHT_GREY_COLOR));
        break;
      default:
        break;
      }
    default:
      return QVariant();
      break;
    }
  }
  else
  {
    int ret = ((index.row() / 2 ) * DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT)
        + index.column() + 1;
    auto relatedIndex = QAbstractTableModel::index(index.row() + 1,
                                                   index.column());
    switch(role)
    {
    case Qt::DisplayRole :
    case Qt::EditRole :
      return ret > 512 ? QVariant() : ret;
      break;
    case Qt::TextAlignmentRole :
      return Qt::AlignCenter;
      break;
    case Qt::BackgroundRole :
      if ((m_editedIndexes.size() == 0)
          || (m_editedIndexes.indexOf(relatedIndex) == -1))
        return QBrush(LIGHT_BLUE_COLOR);
      else
        return QBrush(LIGHT_YELLOW_COLOR);
      break;
    default:
      return QVariant();
      break;
    }
  }
}

bool DmxValueTableModel::setData(const QModelIndex &index,
                                 const QVariant &value,
                                 int role)
{
  if (!index.isValid() || !(index.flags().testFlag(Qt::ItemIsEditable)))
    return false;

  int valueID = (((index.row() - 1)/2) * DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT)
      + index.column();
  auto dmxValue = m_rootValue->getL_childValue().at(valueID);
  // NOTE : it's ok for the moment, but if we create widget xith channelgroup ?
  dmxValue->setLevel(DmxValue::DirectChannelEditSender,
                     value.toInt());

  emit dataChanged(index,index);

  return true;
}

QVariant DmxValueTableModel::headerData(int section,
                                        Qt::Orientation orientation,
                                        int role) const
{
  return QVariant();
}

bool DmxValueTableModel::setHeaderData(int section,
                                       Qt::Orientation orientation,
                                       const QVariant &value,
                                       int role)
{
  return true;
}

Qt::ItemFlags DmxValueTableModel::flags(const QModelIndex &index) const
{
  // TODO : handle universe with less than 512 channel
  if (!index.isValid())
    return Qt::NoItemFlags;

  if (!(index.row()%2)) // ligne paire.
    return Qt::ItemIsEnabled;
  else // ligne impaire
    return Qt::ItemIsEnabled
        | Qt::ItemIsSelectable
        | Qt::ItemIsEditable;

  return QAbstractTableModel::flags(index);

}

DmxValue *DmxValueTableModel::getValueFromIndex(const QModelIndex &t_index) const
{
  int valueID = (((t_index.row() -1)/2) * DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT)
      + t_index.column();
  if (valueID < 0 || valueID >= m_rootValue->getL_ChildValueSize())
    return nullptr;

  return m_rootValue->getL_childValue().at(valueID);

}

QList<DmxValue *> DmxValueTableModel::getValuesFromIndexList(const QModelIndexList &t_L_index) const
{
  auto L_channel = QList<DmxValue *>();
  for (auto item : t_L_index)
  {
    auto channel = getValueFromIndex(item);
    if (channel)
      L_channel.append(channel);
  }
  return L_channel;
}

