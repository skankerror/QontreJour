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

#include <QLayout>
#include "playbackwidget.h"

PlaybackWidget::PlaybackWidget(QWidget *parent)
  : QWidget(parent),
    m_ASlider(new QSlider(this)),
    m_ALabel(new QLabel("A", this)),
    m_BSlider(new QSlider(this)),
    m_BLabel(new QLabel("B", this)),
    m_seqInfButton(new QPushButton(this)),
    m_seqSupButton(new QPushButton(this)),
    m_pauseButton(new QPushButton(this)),
    m_goBackButton(new QPushButton(this)),
    m_goButton(new QPushButton(this))
{
  auto layout = new QGridLayout();

  m_seqInfButton->setText("seq-");
  m_seqSupButton->setText("seq+");
  m_pauseButton->setText("pause");
  m_goBackButton->setText("Go Back");
  m_goButton->setText("GO");
  m_ALabel->setAlignment(Qt::AlignHCenter);
  m_BLabel->setAlignment(Qt::AlignHCenter);

  layout->addWidget(m_ASlider, 0, 0, 8, 1);
  layout->addWidget(m_BSlider, 0, 1, 8, 1);
  layout->addWidget(m_ALabel, 8, 0, 1, 1);
  layout->addWidget(m_BLabel, 8, 1, 1, 1);
  layout->addWidget(m_seqInfButton, 9, 0, 1, 1);
  layout->addWidget(m_seqSupButton, 9, 1, 1, 1);
  layout->addWidget(m_pauseButton, 10, 0, 1, 1);
  layout->addWidget(m_goBackButton, 10, 1, 1, 1);
  layout->addWidget(m_goButton, 11, 0, 1, 2);

  layout->setAlignment(m_ASlider, Qt::AlignHCenter);
  layout->setAlignment(m_BSlider, Qt::AlignHCenter);

  setLayout(layout);
}
