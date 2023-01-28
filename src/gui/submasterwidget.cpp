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

#include "submasterwidget.h"
#include "../qontrejour.h"
#include <QLabel>

// sliders are created in mainwindow and pass to widget

SubmasterWidget::SubmasterWidget(QWidget *parent)
  : QWidget(parent),
    m_stackedLayout(new QStackedLayout()),
    m_changePageComboBox(new QComboBox(this))
{
  auto totalLayout = new QVBoxLayout();
  totalLayout->addWidget(m_changePageComboBox);
  totalLayout->addLayout(m_stackedLayout);
  setLayout(totalLayout);

}

void SubmasterWidget::setL_sliders(const QList<SubmasterSlider *> &t_L_sliders)
{
  m_L_sliders = t_L_sliders;

}

void SubmasterWidget::populateWidget()
{
  // on crée le 1er widget pour contenir 20 sliders
  int page_count = m_L_sliders.size() / 20;
//  qDebug() << "page count : " << page_count;
  if (m_L_sliders.size() > page_count * 20)
    page_count++;

  // TODO : probably crashes with with non 512ch universe.
  for (int i = 0; i < page_count; i++) // for each page
  {
    auto widget = new QWidget(this);
    auto pageLayout = new QHBoxLayout();

    for (int j = 0; j < 20; j++) // for each slider
    {
      auto layout = new QVBoxLayout();
      auto label = new QLabel(QString::number(j + (i * 20) + 1),
                              widget);
      label->setAlignment(Qt::AlignHCenter);
      auto slider = m_L_sliders.at(j + (i * 20));

      layout->addWidget(slider);
      layout->addWidget(label);
      layout->setAlignment(slider,
                           Qt::AlignHCenter);
      pageLayout->addLayout(layout);
    }
    widget->setLayout(pageLayout);
    m_stackedLayout->addWidget(widget);
    m_changePageComboBox->addItem(tr("page %1 --> Channel Group %2 - %3")
                                  .arg(i + 1)
                                  .arg((i * 20) + 1)
                                  .arg((i + 1) * 20));
  }
  connect(m_changePageComboBox,
          &QComboBox::activated,
          m_stackedLayout,
          &QStackedLayout::setCurrentIndex);

}
