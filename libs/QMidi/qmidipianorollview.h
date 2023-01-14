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

#ifndef QMIDIPIANOROLLVIEW_H
#define QMIDIPIANOROLLVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QMouseEvent>
#include "qmidimessage.h"


class QMidiPianoRollView :
    public QGraphicsView
{

  Q_OBJECT

public:

  explicit QMidiPianoRollView(QWidget *parent = nullptr);

protected:

  void mousePressEvent(QMouseEvent *t_event) override;
  void mouseDoubleClickEvent(QMouseEvent *t_event) override;
  void mouseMoveEvent(QMouseEvent *t_event) override;
  void mouseReleaseEvent(QMouseEvent *t_event) override;

private:

  bool isBlackKey(int t_pitch);
  bool isBlackKey(QGraphicsRectItem *t_item);

signals:

  void sigKeyPressed(QMidiMessage *);

public slots:

  void onMidiReceive(QMidiMessage *t_message);

private:

  QGraphicsScene *m_scene;
  int m_keyWidth;
  int m_keyHeight;
  QVector<QGraphicsRectItem *> m_keys;

  bool m_isKeyClicked = false;
  QGraphicsRectItem *m_keyClicked = nullptr;

};

#endif // QMIDIPIANOROLLVIEW_H
