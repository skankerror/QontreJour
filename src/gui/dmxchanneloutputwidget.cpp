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

#include "dmxchanneloutputwidget.h"
#include "../qontrejour.h"
#include <QLayout>
#include <QDebug>
#include <QLabel>
#include <QHeaderView>
#include <QPainter>
#include <QSlider>
#include <QMouseEvent>


DmxChannelOutputWidget::DmxChannelOutputWidget(QWidget *parent)
  : QWidget(parent),
//    m_tableView(new QTableView(parent)),
    m_tableView(new DmxChannelOutputTableView(this)),
    m_model(new DmxChannelOutputTableModel(this)),
    m_delegate(new DmxChannelOutputTableDelegate(this)),
    m_universeSpinBox(new QSpinBox(this))
{
  auto totalLayout = new QVBoxLayout();
  auto headerLayout = new QHBoxLayout();

  auto label = new QLabel("Dmx Channels output from universe :  ", this);

  // TODO : connect this spinbox to the world !
  m_universeSpinBox->setMaximum(1);// at the beginning ther's only one universe
  m_universeSpinBox->setMinimum(1);// there's always one universe

  headerLayout->addWidget(label);
  headerLayout->addWidget(m_universeSpinBox);

  totalLayout->addLayout(headerLayout);
  totalLayout->addWidget(m_tableView);

  setLayout(totalLayout);


  m_tableView->setSortingEnabled(false);
  m_tableView->horizontalHeader()->setMinimumSectionSize(49);
  m_tableView->verticalHeader()->setMinimumSectionSize(49);
  m_tableView->horizontalHeader()->hide();
  m_tableView->verticalHeader()->hide();
  m_tableView->resizeColumnsToContents();

  m_tableView->setModel(m_model);
  m_tableView->setItemDelegate(m_delegate);

  m_tableView->resizeColumnsToContents();

}

void DmxChannelOutputWidget::onUniverseCountChanged(int t_universeCount)
{
  m_universeCount = t_universeCount;
  m_universeSpinBox->setMaximum(t_universeCount);
  m_tableView->resizeColumnsToContents();

}

void DmxChannelOutputWidget::setL_dmxChannel(const QList<DmxChannel *> t_L_dmxChannel)
{
  m_model->setL_dmxChannel(t_L_dmxChannel);
  m_delegate->setL_dmxChannel(t_L_dmxChannel);
}

void DmxChannelOutputWidget::setUniverseID(const int t_ID)
{
  m_model->setUniverseID(t_ID);
  m_delegate->setUniverseID(t_ID);
}


void DmxChannelOutputWidget::repaintTableView()
{
  // NOTE : ugly workaround to refresh view when channel changed
  m_tableView->hide();
  m_tableView->show();
}

void DmxChannelOutputWidget::mousePressEvent(QMouseEvent *event)
{
//  if (event->button() == Qt::LeftButton)
//  {
//    if (m_tableView->rect().contains(event->position().toPoint()))
//    {
//      m_tableView->mousePressEvent(event);
//      return;
//    }
//    if (m_universeSpinBox->rect().contains(event->position().toPoint()))
//    {
//      m_universeSpinBox->mousePressEvent(event);
//      return;
//    }
//  }
  QWidget::mousePressEvent(event);
}

void DmxChannelOutputWidget::mouseReleaseEvent(QMouseEvent *event)
{
  QWidget::mouseReleaseEvent(event);
}

void DmxChannelOutputWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
  QWidget::mouseDoubleClickEvent(event);
}

void DmxChannelOutputWidget::mouseMoveEvent(QMouseEvent *event)
{
  QWidget::mouseMoveEvent(event);
}

/*********************************************************************/

DmxChannelOutputTableView::DmxChannelOutputTableView(QWidget *parent)
  :QTableView(parent)
{}

//void DmxChannelOutputTableView::mousePressEvent(QMouseEvent *event)
//{
//  if (event->button() == Qt::LeftButton)
//  {
//    auto position = event->position().toPoint();
//    auto index = indexAt(position);
//    if (index.isValid()
//        && index.flags() == Qt::ItemIsEditable)
//    {
//      isEditing = true;
//      // TODO :
//      // chopper le canal et l'éditer
//    }
//  }
//  QTableView::mousePressEvent(event);
//}

//void DmxChannelOutputTableView::mouseReleaseEvent(QMouseEvent *event)
//{
//  QTableView::mouseReleaseEvent(event);
//}

//void DmxChannelOutputTableView::mouseDoubleClickEvent(QMouseEvent *event)
//{
//  QTableView::mouseDoubleClickEvent(event);
//}

//void DmxChannelOutputTableView::mouseMoveEvent(QMouseEvent *event)
//{
//  QTableView::mouseMoveEvent(event);
//}
//bool DmxChannelOutputTableView::event(QEvent *event)
//{

//}


/**********************************************************************/

DmxChannelOutputTableModel::DmxChannelOutputTableModel(QObject *parent)
  : QAbstractTableModel(parent)
{}

int DmxChannelOutputTableModel::rowCount(const QModelIndex &parent) const
{
  return parent.isValid() ? 0 : DMX_CHANNEL_OUTPUT_TABLE_MODEL_ROWS_COUNT_DEFAULT;
}

int DmxChannelOutputTableModel::columnCount(const QModelIndex &parent) const
{
  return parent.isValid() ? 0 : DMX_CHANNEL_OUTPUT_TABLE_MODEL_COLUMNS_COUNT_DEFAULT;

}

QVariant DmxChannelOutputTableModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (index.flags().testFlag(Qt::ItemIsEditable))
  {
    int channelID = (((index.row() -1)/2) * DMX_CHANNEL_OUTPUT_TABLE_MODEL_COLUMNS_COUNT_DEFAULT)
        + index.column();
    if (channelID < 0 || channelID >= m_L_dmxChannel.size())
      return QVariant();

    auto dmxChannel = m_L_dmxChannel.at(channelID);

    switch(role)
    {
    case Qt::DisplayRole :
    case Qt::EditRole :
      return dmxChannel->getLevel();
      break;
    case Qt::TextAlignmentRole :
      return Qt::AlignCenter;
      break;
    case Qt::BackgroundRole:
      return QBrush(QColor(186, 109, 43));
      break;
    default:
      return QVariant();
      break;
    }

//    return dmxChannel->getChannelID(); // just to test it
  }
  else
  {
    int ret = ((index.row() / 2 ) * DMX_CHANNEL_OUTPUT_TABLE_MODEL_COLUMNS_COUNT_DEFAULT)
        + index.column() + 1;
    switch(role)
    {
    case Qt::DisplayRole :
    case Qt::EditRole :
      return ret > 512 ? QVariant() : ret;
      break;
    case Qt::TextAlignmentRole :
      return Qt::AlignCenter;
      break;
    case Qt::BackgroundRole:
      return QBrush(QColor(60, 85, 164));
      break;
    default:
      return QVariant();
      break;
    }
  }
}

bool DmxChannelOutputTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (!index.isValid() || !(index.flags().testFlag(Qt::ItemIsEditable)))
    return false;

  int channelID = (((index.row() - 1)/2) * DMX_CHANNEL_OUTPUT_TABLE_MODEL_COLUMNS_COUNT_DEFAULT)
      + index.column();
//  qDebug() << "channelID i, model : " << channelID;
  auto dmxChannel = m_L_dmxChannel.at(channelID);
  dmxChannel->setLevel(channelID,
                       value.toInt());
  return true;
}

QVariant DmxChannelOutputTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  return QVariant();
}

bool DmxChannelOutputTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
  return true;
}

Qt::ItemFlags DmxChannelOutputTableModel::flags(const QModelIndex &index) const
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

/****************************************************************************/

DmxChannelOutputTableDelegate::DmxChannelOutputTableDelegate(QObject *parent)
  : QStyledItemDelegate(parent)
{}


void DmxChannelOutputTableDelegate::paint(QPainter *painter,
                                          const QStyleOptionViewItem &option,
                                          const QModelIndex &index) const
{
  painter->save();
  if (!index.isValid())
  {
    painter->restore();
    QStyledItemDelegate::paint(painter, option, index);
    return;
  }
  if (!(index.row()%2)) // ligne paire. channel ID
  {
    painter->setBackgroundMode(Qt::OpaqueMode);
    QTextOption textOption;
    textOption.setAlignment(Qt::AlignCenter);
    painter->fillRect(option.rect,
                      QBrush(QColor(186, 109, 43)));
    painter->drawText(option.rect,
                      QString::number(((index.row() / 2 ) * DMX_CHANNEL_OUTPUT_TABLE_MODEL_COLUMNS_COUNT_DEFAULT)
                                      + index.column() + 1),
                      textOption);
  }
  if (index.row()%2) // ligne impaire, channel level
  {
    painter->setBackgroundMode(Qt::OpaqueMode);
    QTextOption textOption;
    textOption.setAlignment(Qt::AlignCenter);
    painter->fillRect(option.rect,
                      QBrush(QColor(60, 85, 164)));
    int channelID = (((index.row() -1)/2) * DMX_CHANNEL_OUTPUT_TABLE_MODEL_COLUMNS_COUNT_DEFAULT)
        + index.column();
    if (channelID < 0 || channelID >= m_L_dmxChannel.size())
      return;
    auto dmxChannel = m_L_dmxChannel.at(channelID);
    painter->drawText(option.rect,
                      QString::number(dmxChannel->getLevel()),
                      textOption);
  }
  painter->restore();

}


QWidget *DmxChannelOutputTableDelegate::createEditor(QWidget *parent,
                                                     const QStyleOptionViewItem &option,
                                                     const QModelIndex &index) const
{
  if (index.isValid())
  {
    auto slider = new QSlider(parent);
    slider->setMaximum(255);
    slider->setMinimum(0);
    slider->setValue(index.data().toInt()); // NOTE : dangerous !
    return slider;
  }
  return QStyledItemDelegate::createEditor(parent, option, index);
}

void DmxChannelOutputTableDelegate::setEditorData(QWidget *editor,
                                                  const QModelIndex &index) const
{
  if (index.isValid())
  {
    auto slider = qobject_cast<QSlider *>(editor);
    slider->setValue(index.data().toInt()); // NOTE : dangerous !
    return;
  }
  QStyledItemDelegate::setEditorData(editor, index);
}

void DmxChannelOutputTableDelegate::setModelData(QWidget *editor,
                                                 QAbstractItemModel *model,
                                                 const QModelIndex &index) const
{
  if (index.isValid())
  {
    auto slider = qobject_cast<QSlider *>(editor);
    model->setData(index, QVariant::fromValue(slider->value()));
    return;
  }
  QStyledItemDelegate::setModelData(editor, model, index);
}






