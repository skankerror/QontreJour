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
#include <QLayout>

SubMasterWidget::SubMasterWidget(QWidget *parent)
  : QWidget(parent)
{
  auto slidersLayout = new QHBoxLayout();

  for (int i = 0; i < SUBMASTER_SLIDERS_COUNT; i++)
  {
    auto slider = new SubMasterSlider(this);
    slider->setMaximum(255);
    slider->setMinimum(0);
    m_L_sliders.append(slider);
    slidersLayout->addWidget(slider);
  }

  setLayout(slidersLayout);
}

// TODO : c'est le bordel y faut sans doute sous-classer slider...
// et faire méthodes pour y assigner des channels existants.
