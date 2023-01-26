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

#include "dmxvaluetablewidget.h"
#include "../qontrejour.h"
#include <QLayout>
#include <QHeaderView>
#include <QPainter>
//#include <QSlider>
#include <QMouseEvent>
//#include <QWheelEvent>
#include <QDebug>


DmxValueTableWidget::DmxValueTableWidget(QWidget *parent)
  : QWidget(parent),
    m_tableView(new DmxValueTableView(this)),
    m_model(new DmxValueTableModel(this)),
    m_label(new QLabel(this)),
    m_universeSpinBox(new QSpinBox(this))
{
  auto totalLayout = new QVBoxLayout();
  auto headerLayout = new QHBoxLayout();

  // TODO : connect this spinbox to the world !
  m_universeSpinBox->setMaximum(1);// at the beginning ther's only one universe
  m_universeSpinBox->setMinimum(1);// there's always one universe

  headerLayout->addStretch();
  headerLayout->addWidget(m_label);
  headerLayout->addWidget(m_universeSpinBox);

  totalLayout->addLayout(headerLayout);
  totalLayout->addWidget(m_tableView);

  setLayout(totalLayout);


  m_tableView->setSortingEnabled(false);
  m_tableView->setUpdatesEnabled(true);
  m_tableView->horizontalHeader()->setMinimumSectionSize(30);
  m_tableView->verticalHeader()->setMinimumSectionSize(20);
  m_tableView->horizontalHeader()->hide();
  m_tableView->verticalHeader()->hide();
  m_tableView->resizeColumnsToContents();

  m_tableView->setModel(m_model);

  m_tableView->resizeColumnsToContents();

  connect(m_universeSpinBox,
          SIGNAL(valueChanged(int)),
          this,
          SLOT(onSpinboxSelcted(int)));

}

DmxValueTableWidget::~DmxValueTableWidget()
{}

void DmxValueTableWidget::onUniverseCountChanged(int t_universeCount)
{
  m_universeCount = t_universeCount;
  m_universeSpinBox->setMaximum(t_universeCount);
  m_tableView->resizeColumnsToContents();

}

void DmxValueTableWidget::setL_dmxValue(const QList<DmxValue *> t_L_dmxValue)
{
  m_model->setL_dmxValue(t_L_dmxValue);

}

void DmxValueTableWidget::setUniverseID(const int t_ID)
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

void DmxValueTableWidget::onSpinboxSelected(int t_universeID)
{
  emit askForUniverseChanged(t_universeID - 1);
}

void DmxValueTableWidget::repaintTableView()
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
    qDebug() << "position : " << index;
    if (index.flags().testFlag(Qt::ItemIsEditable))
    {
      m_isEditing = true;
//      qDebug() << "isEditing : " << m_isEditing;
      m_originEditingPoint = event->pos();
//      qDebug() << "position" << m_originEditingPoint;
      m_editedIndex = index;
      return;
    }
  }
  QTableView::mousePressEvent(event);
}

void DmxValueTableView::mouseReleaseEvent(QMouseEvent *event)
{
  if (m_isEditing)
  {
    m_isEditing = false;
  }
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
    auto value = m_editedIndex.data().toInt();
    auto yValue = event->pos().y() - m_originEditingPoint.y();
//    qDebug() << "y value in DmxValueTableView::mouseMoveEvent " << yValue;
    value -= yValue;
    if (value > 255) value = 255;
    if (value < 0) value = 0;
    model()->setData(m_editedIndex, value, Qt::EditRole);
    m_originEditingPoint = event->pos();
    return;
  }
  QTableView::mouseMoveEvent(event);
}

//void DmxValueTableView::wheelEvent(QWheelEvent *event)
//{
//  QTableView::wheelEvent(event);
//}

/************************************************************************/

DmxValueTableModel::DmxValueTableModel(QObject *parent)
  : QAbstractTableModel(parent)
{}

DmxValueTableModel::~DmxValueTableModel()
{}

int DmxValueTableModel::rowCount(const QModelIndex &parent) const
{
  return parent.isValid() ? 0 : DMX_VALUE_TABLE_MODEL_ROWS_COUNT_DEFAULT;
}

int DmxValueTableModel::columnCount(const QModelIndex &parent) const
{
  return parent.isValid() ? 0 : DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT;
}

QVariant DmxValueTableModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (index.flags().testFlag(Qt::ItemIsEditable))
  {
    int valueID = (((index.row() -1)/2) * DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT)
        + index.column();
    if (valueID < 0 || valueID >= m_L_dmxValue.size())
      return QVariant();

    auto dmxValue = m_L_dmxValue.at(valueID);

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
    int ret = ((index.row() / 2 ) * DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT)
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

bool DmxValueTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (!index.isValid() || !(index.flags().testFlag(Qt::ItemIsEditable)))
    return false;

  int valueID = (((index.row() - 1)/2) * DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT)
      + index.column();
  //  qDebug() << "channelID i, model : " << channelID;
  auto dmxValue = m_L_dmxValue.at(valueID);
  dmxValue->setLevel(valueID,
                     value.toInt());

  emit dataChanged(index,index);

  return true;
}

QVariant DmxValueTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  return QVariant();
}

bool DmxValueTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
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


