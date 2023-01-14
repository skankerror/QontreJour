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

#ifndef PLAYBACKWIDGET_H
#define PLAYBACKWIDGET_H

#include <QWidget>
#include <QSlider>
#include <QPushButton>

class PlaybackWidget : public QWidget
{

  Q_OBJECT

public:

  explicit PlaybackWidget(QWidget *parent = nullptr);

signals:

private:

  QSlider *m_ASlider;
  QSlider *m_BSlider;
  QPushButton *m_seqInfButton;
  QPushButton *m_seqSupButton;
  QPushButton *m_pauseButton;
  QPushButton *m_goBackButton;
  QPushButton *m_goButton;

};

#endif // PLAYBACKWIDGET_H
