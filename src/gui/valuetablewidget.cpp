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

#include <QLayout>
#include <QHeaderView>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include "../qontrejour.h"
#include "../core/dmxmanager.h"
#include "valueeditwidget.h"


ValueTableWidget::ValueTableWidget(QWidget *parent)
  : QWidget(parent),
    m_tableView(new ValueTableView(this)),
    m_model(new ValueTableModel(this)),
    m_channelDelegate(new ChannelDelegate(this))
{
  setL_channelData(MANAGER
                       ->getDmxEngine()
                       ->getChannelEngine()
                       ->getL_channelData());

  auto totalLayout = new QVBoxLayout();
  totalLayout->addWidget(m_tableView);

  setLayout(totalLayout);

  m_tableView->setSortingEnabled(false);
  m_tableView->setUpdatesEnabled(true);
  m_tableView->horizontalHeader()->setMinimumSectionSize(CHANNEL_TABLE_ITEM_SIZE);
  m_tableView->verticalHeader()->setMinimumSectionSize(CHANNEL_TABLE_ITEM_SIZE);
  m_tableView->horizontalHeader()->hide();
  m_tableView->verticalHeader()->hide();

  m_tableView->setModel(m_model);
  m_tableView->setItemDelegate(m_channelDelegate);

  m_tableView->resizeColumnsToContents();
  m_tableView->resizeRowsToContents();
}

ValueTableWidget::~ValueTableWidget()
{}

void ValueTableWidget::setRootValue(RootValue *t_rootValue)
{
  // we connect to update views
  auto L_dmxChannel = t_rootValue->getL_childValue();
  m_model->setRootValue(t_rootValue);
  m_channelDelegate->setRootValue(t_rootValue);

  for (const auto &item : std::as_const(L_dmxChannel))
  {
    DmxValue *value = item;

    connect(value,
            SIGNAL(levelChanged(id,dmx)),
            this,
            SLOT(repaintTableView()));
  }
}

void ValueTableWidget::setL_channelData(QList<ChannelData *> t_L_channelData)
{
  m_model->setL_channelData(t_L_channelData);
  m_channelDelegate->setL_channelData(t_L_channelData);
  setRootValue(MANAGER
                   ->getRootChannel());
}

void ValueTableWidget::repaintTableView()
{
  emit m_model->layoutChanged();
}


/************************* ValueTableView ******************************/

ValueTableView::ValueTableView(QWidget *parent)
  :QTableView(parent)
{}

ValueTableView::~ValueTableView()
{}

void ValueTableView::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
  {
    auto index = indexAt(event->pos());
    if (index.flags().testFlag(Qt::ItemIsEditable))
    {
      auto myModel = static_cast<ValueTableModel *>(model());
      auto indexList = myModel->getEditedIndexes();
      m_isEditing = true;
      m_originEditingPoint = event->pos();
      // NOTE : voir ça avec ctrl ou shift ?
//      if (indexList.indexOf(index) == -1) // si l'index n'est pas ds la selection
//      {
////        myModel->clearSelectionList(); // on clear
//        myModel->addEditedIndex(index); // on ajoute notre index
//      }
      return;
    }
  }
  QTableView::mousePressEvent(event);
}

void ValueTableView::mouseReleaseEvent(QMouseEvent *event)
{
  if (m_isEditing) m_isEditing = false;

  QTableView::mouseReleaseEvent(event);
}

void ValueTableView::mouseDoubleClickEvent(QMouseEvent *event)
{
  QTableView::mouseDoubleClickEvent(event);
}

void ValueTableView::mouseMoveEvent(QMouseEvent *event)
{
  if (m_isEditing)
  {
    auto myModel = static_cast<ValueTableModel *>(model());
    auto indexList = myModel->getEditedIndexes();

    for (const auto &item
         : std::as_const(indexList))
    {
      auto value = item.data().toInt();
      auto yValue = event->pos().y()
          - m_originEditingPoint.y();
      value -= yValue;
//      if (value > 255)
//      {
//        value = 255;
//      }
//      if (value < 0)
//      {
//        value = 0;
//      }
//      model()->setData(item,
//                       value,
//                       Qt::EditRole);
      myModel->recieveValueFromMouse(item,
                                     value);
    }
    m_originEditingPoint = event->pos();
    return;
  }
  QTableView::mouseMoveEvent(event);
}

/************************* ValueTableModel ******************************/

ValueTableModel::ValueTableModel(QObject *parent)
  : QAbstractTableModel(parent)
{
  auto channelEngine = MANAGER->getDmxEngine()->getChannelEngine();
  connect(channelEngine,
          &ChannelEngine::selectionChanged,
          this,
          &ValueTableModel::onSelectionChanged);
}

ValueTableModel::~ValueTableModel()
{}

void ValueTableModel::recieveValueFromMouse(const QModelIndex &t_index,
                                            const int t_value)
{
  int valueID = (((t_index.row() - 1)/2) * DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT)
                + t_index.column();
  overdmx over = 0;
  dmx value = 0;
  if (t_value > 255) value = 255;
  else if (t_value < 0) value = 0;
  else value = t_value;
  over = t_value - value;
  MANAGER->directChannelWidgetsToEngine(valueID,
                                        value,
                                        over);
}

void ValueTableModel::onSelectionChanged(QList<id> L_id)
{
  QModelIndexList indexlist = QModelIndexList();
  for (const auto &item
       : std::as_const(L_id))
  {
    auto index = getIndexFromValueId(item);
    indexlist.append(index);
  }
  m_editedIndexes = indexlist;
  editedIndexChanged();
}

//void ValueTableModel::setEditedIndexes(const QModelIndexList &t_editedIndexes)
//{
//  m_editedIndexes = t_editedIndexes;
//  editedIndexChanged();
//}

//void ValueTableModel::addEditedIndex(QModelIndex &t_editedIndexes)
//{
//  m_editedIndexes.append(t_editedIndexes);
//  editedIndexChanged();
//}

QModelIndex ValueTableModel::getIndexFromValue(const LeveledValue *t_value) const
{
  return getIndexFromValueId(t_value->getid());
}

QModelIndex ValueTableModel::getIndexFromValueId(const id &t_id) const
{
//  return index((2 * (t_id / DMX_VALUE_TABLE_MODEL_ROWS_COUNT_DEFAULT)) + 1,
//               t_id % DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT);
  return index(t_id / DMX_VALUE_TABLE_MODEL_ROWS_COUNT_DEFAULT,
               t_id % DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT);
}

QVariant ValueTableModel::oldDdata(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (!m_rootValue)
    return QVariant();

  if (index.flags().testFlag(Qt::ItemIsEditable))
  {
    int valueID = (((index.row() -1)/2) * DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT)
                  + index.column();
    if (valueID < 0 || valueID >= m_rootValue->getL_childValueSize())
      return QVariant();

    auto dmxValue = m_rootValue->getL_childValue().at(valueID);
    ChannelDataFlag flag = static_cast<DmxChannel *>(dmxValue)->getChannelDataFlag();

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
      case ChannelDataFlag::SelectedSceneFlag :
        return QBrush(QColor(LIGHT_GREEN_COLOR));
        break;
      case ChannelDataFlag::DirectChannelFlag :
        return QBrush(QColor(LIGHT_YELLOW_COLOR));
        break;
      case ChannelDataFlag::ChannelGroupFlag :
        return QBrush(QColor(LIGHT_BLUE_COLOR));
        break;
      case ChannelDataFlag::ParkedFlag :
        return QBrush(QColor(RED_COLOR));
        break;
      case ChannelDataFlag::IndependantFlag :
        return QBrush(QColor(PURPLE_COLOR));
        break;
      case ChannelDataFlag::UnknownFlag :
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

QVariant ValueTableModel::showAllData(const QModelIndex &index, int role) const
{
//  if (!index.isValid())
//    return QVariant();

  if (!m_rootValue)
  {
    qDebug() << "ValueTableModel::showAllData no rootvalue !";
    return QVariant();
  }

  if (!(index.flags().testFlag(Qt::ItemIsEnabled)))
    return QVariant();

  int valueID = ((index.row() * DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT)
                 + index.column());
  if (valueID < 0 || valueID >= m_L_channelData.size())
  {
    qDebug() << "ValueTableModel::showAllData id outta range !";
    return QVariant();
  }

//  auto dmxValue = m_rootValue->getL_childValue().at(valueID);
//  ChannelDataFlag flag = static_cast<DmxChannel *>(dmxValue)->getChannelDataFlag();
  auto channelData = m_L_channelData.at(valueID);
  ChannelDataFlag flag = channelData->getFlag();

  QVariant varRet;

  switch(role)
    {
    case Qt::DisplayRole :
//    case Qt::EditRole :
    varRet.setValue(channelData);
//    return QVariant::fromValue(channelData);
    return varRet;
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
      case ChannelDataFlag::SelectedSceneFlag :
        return QBrush(QColor(LIGHT_GREEN_COLOR));
        break;
      case ChannelDataFlag::DirectChannelFlag :
        return QBrush(QColor(LIGHT_YELLOW_COLOR));
        break;
      case ChannelDataFlag::ChannelGroupFlag :
        return QBrush(QColor(LIGHT_BLUE_COLOR));
        break;
      case ChannelDataFlag::ParkedFlag :
        return QBrush(QColor(RED_COLOR));
        break;
      case ChannelDataFlag::IndependantFlag :
        return QBrush(QColor(PURPLE_COLOR));
        break;
      case ChannelDataFlag::UnknownFlag :
        return QBrush(QColor(LIGHT_GREY_COLOR));
        break;
      default:
        break;
      }
    default:
      return QVariant();
      break;
    }
//    int ret = ((index.row() / 2 ) * DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT)
//              + index.column() + 1;
//    auto relatedIndex = QAbstractTableModel::index(index.row() + 1,
//                                                   index.column());
//    switch(role)
//    {
//    case Qt::DisplayRole :
//    case Qt::EditRole :
//      return ret > 512 ? QVariant() : ret;
//      break;
//    case Qt::TextAlignmentRole :
//      return Qt::AlignCenter;
//      break;
//    case Qt::BackgroundRole :
//      if ((m_editedIndexes.size() == 0)
//          || (m_editedIndexes.indexOf(relatedIndex) == -1))
//        return QBrush(LIGHT_BLUE_COLOR);
//      else
//        return QBrush(LIGHT_YELLOW_COLOR);
//      break;
//    default:
//      return QVariant();
//      break;
//    }
//  }
}

QVariant ValueTableModel::showFilteredData(const QModelIndex &index, int role) const
{

}

void ValueTableModel::editedIndexChanged()
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

int ValueTableModel::rowCount(const QModelIndex &parent) const
{
  return parent.isValid() ? 0 : DMX_VALUE_TABLE_MODEL_ROWS_COUNT_DEFAULT;
}

int ValueTableModel::columnCount(const QModelIndex &parent) const
{
  return parent.isValid() ? 0 : DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT;
}

QVariant ValueTableModel::data(const QModelIndex &index,
                               int role) const
{
//  return oldDdata(index,
//                  role);
//  return showAllData(index,
//                      role);
  return QVariant();
}

QVariant ValueTableModel::filterData(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (!m_rootValue)
    return QVariant();

  if (!(m_editedIndexes.size()))
    return QVariant();

  if (index.flags().testFlag(Qt::ItemIsEditable))
  {
    int valueID = (((index.row() -1)/2) * DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT)
        + index.column();
    if (valueID < 0 || valueID >= m_editedIndexes.size())
      return QVariant();

    auto dmxValue = getValueFromIndex(m_editedIndexes.at(valueID));
    ChannelDataFlag flag = static_cast<DmxChannel *>(dmxValue)->getChannelDataFlag();

    switch(role)
    {
    case Qt::DisplayRole :
    case Qt::EditRole :
      return static_cast<LeveledValue *>(dmxValue)->getLevel();
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
      case ChannelDataFlag::SelectedSceneFlag :
        return QBrush(QColor(LIGHT_GREEN_COLOR));
        break;
      case ChannelDataFlag::DirectChannelFlag :
        return QBrush(QColor(LIGHT_YELLOW_COLOR));
        break;
      case ChannelDataFlag::ChannelGroupFlag :
        return QBrush(QColor(LIGHT_BLUE_COLOR));
        break;
      case ChannelDataFlag::ParkedFlag :
        return QBrush(QColor(RED_COLOR));
        break;
      case ChannelDataFlag::IndependantFlag :
        return QBrush(QColor(PURPLE_COLOR));
        break;
      case ChannelDataFlag::UnknownFlag :
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
        + index.column()/* + 1*/;
    if (ret >= m_editedIndexes.size())
      return QVariant();
    auto index = m_editedIndexes.at(ret);
    auto value = getValueFromIndex(index);

    switch(role)
    {
    case Qt::DisplayRole :
    case Qt::EditRole :
      return value->getid() + 1;
      break;
    case Qt::TextAlignmentRole :
      return Qt::AlignCenter;
      break;
    case Qt::BackgroundRole :
        return QBrush(LIGHT_YELLOW_COLOR);
      break;
    default:
      return QVariant();
      break;
    }
  }

}

bool ValueTableModel::setData(const QModelIndex &index,
                              const QVariant &value,
                              int role)
{
//  Q_UNUSED(role)

//  if (!index.isValid() || !(index.flags().testFlag(Qt::ItemIsEditable)))
//    return false;

//  int valueID = (((index.row() - 1)/2) * DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT)
//      + index.column();
//  emit dataChanged(index,index);

//  return true;
  return false;
}

bool ValueTableModel::setFilterData(const QModelIndex &index, const QVariant &value, int role)
{
  if (!index.isValid() || !(index.flags().testFlag(Qt::ItemIsEditable)))
    return false;

  int valueID = (((index.row() - 1)/2) * DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT)
      + index.column();
  auto dmxValue = m_rootValue->getL_childValue().at(valueID);
  emit dataChanged(index,index);

  return true;

}

QVariant ValueTableModel::headerData(int section,
                                     Qt::Orientation orientation,
                                     int role) const
{
  return QVariant();
}

bool ValueTableModel::setHeaderData(int section,
                                    Qt::Orientation orientation,
                                    const QVariant &value,
                                    int role)
{
  return true;
}

Qt::ItemFlags ValueTableModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    return Qt::NoItemFlags;

  return Qt::ItemIsEnabled;

//  if (!(index.row()%2)) // ligne paire.
//    return Qt::ItemIsEnabled;
//  else // ligne impaire
//    return Qt::ItemIsEnabled
//        | Qt::ItemIsSelectable
//        | Qt::ItemIsEditable;

  return QAbstractTableModel::flags(index);

}

LeveledValue *ValueTableModel::getValueFromIndex(const QModelIndex &t_index) const
{
  int valueID = (((t_index.row() -1)/2) * DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT)
      + t_index.column();
  if (valueID < 0 || valueID >= m_rootValue->getL_childValueSize())
    return nullptr;

  return m_rootValue->getL_childValue().at(valueID);

}

QList<LeveledValue *> ValueTableModel::getValuesFromIndexList(const QModelIndexList &t_L_index) const
{
  auto L_channel = QList<LeveledValue *>();
  for (auto item : t_L_index)
  {
    auto channel = getValueFromIndex(item);
    if (channel)
      L_channel.append(channel);
  }
  return L_channel;
}

/************************* ChannelDelegate ******************************/

ChannelDelegate::ChannelDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{}



void ChannelDelegate::paint(QPainter *painter,
                            const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{
  int valueID = ((index.row() * DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT)
                 + index.column());
  if (valueID < 0 || valueID >= m_L_channelData.size())
  {
    qDebug() << "bluk !";
    return;
  }
  auto channelData = m_L_channelData.at(valueID);
  ChannelDataFlag flag = channelData->getFlag();
  QColor dmxColor;
  switch(flag)
  {
  case SelectedSceneFlag :
    dmxColor = QColor(LIGHT_GREEN_COLOR);
    break;
  case DirectChannelFlag :
    dmxColor = QColor(LIGHT_YELLOW_COLOR);
    break;
  case ChannelGroupFlag :
    dmxColor = QColor(LIGHT_BLUE_COLOR);
    break;
  case ParkedFlag :
    dmxColor = QColor(RED_COLOR);
    break;
  case IndependantFlag :
    dmxColor = QColor(PURPLE_COLOR);
    break;
  default :
    dmxColor = QColor(LIGHT_GREY_COLOR);
    break;
  }

  painter->save();
  painter->fillRect(option.rect, Qt::black);
  painter->restore();

  painter->save();
  QPen dmxPen(dmxColor);
  dmxPen.setWidth(3);
  painter->setPen(dmxPen);
  QTextOption textOption;
  textOption.setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
//  painter->drawRect(option.rect);
  painter->drawText(option.rect,
                    QString::number(channelData->getActual_Level()),
                    textOption);
  painter->restore();

  painter->save();
  QPen idPen(Qt::white);
  idPen.setWidth(3);
  painter->setPen(idPen);
  textOption.setAlignment(Qt::AlignTop | Qt::AlignHCenter);
  painter->drawText(option.rect,
                    QString::number(channelData->getChannelID() + 1),
                    textOption);
  painter->restore();

}

QSize ChannelDelegate::sizeHint(const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
  return QStyledItemDelegate::sizeHint(option,
                                       index);
}
