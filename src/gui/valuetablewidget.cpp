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
  auto dmxEngine = MANAGER->getDmxEngine();
  auto channelDataEngine = dmxEngine->getChannelDataEngine();
  setChannelDataEngine(channelDataEngine);
  setRootValue(MANAGER->getRootChannel());

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

  connect(channelDataEngine,
          &ChannelDataEngine::sigToUpdateChannelView,
          this,
          &ValueTableWidget::repaintTableView);
}

ValueTableWidget::~ValueTableWidget()
{}

void ValueTableWidget::setRootValue(RootValue *t_rootValue)
{
  // we connect to update views
  auto L_dmxChannel = t_rootValue->getL_childValue();
  for (const auto &item
       : std::as_const(L_dmxChannel))
  {
    connect(item,
            SIGNAL(levelChanged(id,dmx)),
            this,
            SLOT(repaintTableView()));
  }
}

void ValueTableWidget::setChannelDataEngine(ChannelDataEngine *t_cdEngine)
{
  m_channelDelegate->setChannelDataEngine(t_cdEngine);
  m_tableView->setChannelDataEngine(t_cdEngine);
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
    m_isEditing = true;
    m_originEditingPoint = event->pos();
    return;
  }
  if (event->button() == Qt::RightButton)
  {
    int valueID = getChannelIdFromIndex(indexAt(event->pos()));
    if (valueID < 0 || valueID >= 512)
    {
      qDebug() << "bluk !";
      return;
    }
    else
    {
      auto channelData = m_channelDataEngine->getChannelData(valueID);
      if (channelData->getIsSelected())
        m_channelDataEngine->removeIdFromL_select(valueID);
      else
        m_channelDataEngine->addIdToL_select(valueID);
      return;
    }
  }
  QTableView::mousePressEvent(event);
}

void ValueTableView::mouseReleaseEvent(QMouseEvent *event)
{
  if (m_isEditing)
    m_isEditing = false;
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
    auto channelDelegate = static_cast<ChannelDelegate*>(itemDelegate());
    auto yValue = event->pos().y()
                  - m_originEditingPoint.y();
    if (yValue > 0)
      channelDelegate->recieveValuePlusFromMouse(false);
    else if (yValue < 0)
      channelDelegate->recieveValuePlusFromMouse(true);
    m_originEditingPoint = event->pos();
    return;
  }
  QTableView::mouseMoveEvent(event);
}

int ValueTableView::getChannelIdFromIndex(QModelIndex t_index)
{
  int valueID = ((t_index.row()
                  * DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT)
                 + t_index.column());
  return valueID;
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
  if (valueID < 0
      || valueID >= m_channelDataEngine
                        ->getChannelDataCount())
  {
    qDebug() << "bluk !";
    return;
  }
  auto channelData = m_channelDataEngine->getChannelData(valueID);
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
  painter->fillRect(option.rect,
                    backGroundColor);
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

void ChannelDelegate::recieveValuePlusFromMouse(const bool t_isPlus)
{
//  qDebug() << "yvalue" << t_isPlus;
//  auto
}

