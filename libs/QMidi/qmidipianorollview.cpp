/*
 * (c) 2021 Michaël Creusy -- creusy(.)michael(@)gmail(.)com
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

#include "qmidipianorollview.h"
#include <QDebug>


QMidiPianoRollView::QMidiPianoRollView(QWidget *parent) :
  QGraphicsView(parent),
  m_scene(new QGraphicsScene()),
  m_keyWidth(KEY_WIDTH),
  m_keyHeight(KEY_HEIGHT)
{
  float x = 0;
  for(int i = 0; i < 128; i++)
  {
    int position = i % 12;
    QGraphicsRectItem *rect;
    if(position == 0 || position == 2 || position == 4 || position == 5 || position == 7 || position == 9 || position == 11)
    {
      rect = new QGraphicsRectItem(x,0,m_keyWidth,m_keyHeight);
      m_scene->addItem(rect);
      if(position != 4 && position != 11 )
        x += m_keyWidth/2+1;
      else
        x += m_keyWidth+1;
    }
    else
    {
      rect = new QGraphicsRectItem(x,0,m_keyWidth,m_keyHeight/2);
      rect->setZValue(2);
      QBrush brush;
      QPen pen;
      brush.setStyle(Qt::SolidPattern);
      pen.setColor(Qt::black);
      brush.setColor(Qt::black);
      rect->setBrush(brush);
      rect->setPen(pen);
      m_scene->addItem(rect);
      x += m_keyWidth/2+1;
    }
    m_keys.push_back(rect);
  }
  setScene(m_scene);
}

void QMidiPianoRollView::mousePressEvent(QMouseEvent *t_event)
{
  if (auto item = itemAt(t_event->pos()))
  {
    auto item2 = static_cast<QGraphicsRectItem *>(item);
    auto messageOn = new QMidiMessage(this);
    messageOn->setStatus(NOTE_ON);
    messageOn->setChannel(1);
    messageOn->setPitch(m_keys.indexOf(item2));
    messageOn->setVelocity(64);
    onMidiReceive(messageOn);
    m_isKeyClicked = true;
    m_keyClicked = item2;
    messageOn->deleteLater();
  }
  QGraphicsView::mousePressEvent(t_event);
}

void QMidiPianoRollView::mouseDoubleClickEvent(QMouseEvent *t_event)
{
  QMidiPianoRollView::mousePressEvent(t_event);
}

// BUG : click et sort du cadre revient, ça plante. à débugger
void QMidiPianoRollView::mouseMoveEvent(QMouseEvent *t_event)
{
  if (m_isKeyClicked)
  {
    if (auto item = itemAt(t_event->pos())) // move is on an item
    {
      auto item2 = static_cast<QGraphicsRectItem *>(item);
      if (item2 == m_keyClicked)
        return;
      else
      {
        auto messageOff = new QMidiMessage(this);
        messageOff->setStatus(NOTE_OFF);
        messageOff->setChannel(1);
        messageOff->setPitch(m_keys.indexOf(m_keyClicked));
        messageOff->setVelocity(0);
        onMidiReceive(messageOff);
        m_isKeyClicked = false;
        m_keyClicked = nullptr;
        messageOff->deleteLater();

        auto messageOn = new QMidiMessage(this);
        messageOn->setStatus(NOTE_ON);
        messageOn->setChannel(1);
        messageOn->setPitch(m_keys.indexOf(item2));
        messageOn->setVelocity(64);
        onMidiReceive(messageOn);
        m_isKeyClicked = true;
        m_keyClicked = item2;
        messageOn->deleteLater();
      }
    }
    else // move is not on an item
    {
      auto messageOff = new QMidiMessage(this);
      messageOff->setStatus(NOTE_OFF);
      messageOff->setChannel(1);
      messageOff->setPitch(m_keys.indexOf(m_keyClicked));
      messageOff->setVelocity(0);
      onMidiReceive(messageOff);
      m_isKeyClicked = false;
      m_keyClicked = nullptr;
      messageOff->deleteLater();
    }
  }
  QGraphicsView::mouseMoveEvent(t_event);
}

void QMidiPianoRollView::mouseReleaseEvent(QMouseEvent *t_event)
{
  if (QGraphicsItem *item = itemAt(t_event->pos()))
  {
    auto messageOff = new QMidiMessage(this);
    messageOff->setStatus(NOTE_OFF);
    messageOff->setChannel(1);
    messageOff->setPitch(m_keys.indexOf(m_keyClicked));
    messageOff->setVelocity(0);
    onMidiReceive(messageOff);
    m_isKeyClicked = false;
    m_keyClicked = nullptr;
    messageOff->deleteLater();
  }
  QGraphicsView::mouseReleaseEvent(t_event);
}

bool QMidiPianoRollView::isBlackKey(int t_pitch)
{
  int position = t_pitch %12;
  if(position == 0 || position == 2 || position == 4 || position == 5 || position == 7 || position == 9 || position == 11)
    return false;
  else
    return true;
}

bool QMidiPianoRollView::isBlackKey(QGraphicsRectItem *t_item)
{
  int position = m_keys.indexOf(t_item) % 12;
  if(position == 0 || position == 2 || position == 4 || position == 5 || position == 7 || position == 9 || position == 11)
    return false;
  else
    return true;
}

void QMidiPianoRollView::onMidiReceive(QMidiMessage *t_message)
{

  switch(t_message->status())
  {
  case NOTE_ON:
  {
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(QColor(0,0,200, t_message->velocity()*2));
    m_keys[t_message->pitch()]->setBrush(brush);
    break;
  }
  case NOTE_OFF:
  {
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    QColor color;
    if(isBlackKey(t_message->pitch()))
      color = QColor(Qt::black);
    else
      color = QColor(Qt::white);
    brush.setColor(color);
    m_keys[t_message->pitch()]->setBrush(brush);
    break;
  }
  default: break;
  }
  emit sigKeyPressed(t_message);
}

