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
  auto channelEngine = MANAGER
                           ->getDmxEngine()
                           ->getChannelEngine();
  setL_channelData(channelEngine
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

  connect(channelEngine,
          &ChannelEngine::selectionChanged,
          this,
          &ValueTableWidget::repaintTableView);
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
  auto topLeft = index(0,0);
  auto bottomRight = index(DMX_VALUE_TABLE_MODEL_ROWS_COUNT_DEFAULT - 1,
                           DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT - 1);
  emit dataChanged(topLeft,
                   bottomRight);
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
  return QVariant();
}

bool ValueTableModel::setData(const QModelIndex &index,
                              const QVariant &value,
                              int role)
{
  Q_UNUSED(index)
  Q_UNUSED(value)
  Q_UNUSED(role)

  return false;
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

/************************* ChannelDelegate ******************************/

ChannelDelegate::ChannelDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
  auto channelEngine = MANAGER
                           ->getDmxEngine()
                           ->getChannelEngine();
  connect(channelEngine,
          &ChannelEngine::selectionChanged,
          this,
          &ChannelDelegate::onSelectionChanged);
}

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

  QColor backGroundColor(Qt::black);
  if (channelData->getIsSelected())
    backGroundColor = DARK_ORANGE_COLOR;

  painter->save();
  painter->fillRect(option.rect, /*Qt::black*/ backGroundColor);
  painter->restore();

  painter->save();
  QPen dmxPen(dmxColor);
  dmxPen.setWidth(3);
  painter->setPen(dmxPen);
  QTextOption textOption;
  textOption.setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
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

//QModelIndex ChannelDelegate::getIndexFromValueId(const id &t_id) const
//{
//  return QModelIndex(t_id / DMX_VALUE_TABLE_MODEL_ROWS_COUNT_DEFAULT,
//                     t_id % DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT,);
//}

void ChannelDelegate::onSelectionChanged(QList<id> L_id)
{
  m_L_directChannelId = L_id;
}

//void ChannelDelegate::editedIndexChanged()
//{

//}
