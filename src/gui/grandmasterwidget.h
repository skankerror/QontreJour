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

#ifndef GRANDMASTERWIDGET_H
#define GRANDMASTERWIDGET_H

#include <QWidget>
#include <QSlider>
#include <QPushButton>
#include <QLabel>

class GrandMasterWidget : public QWidget
{

  Q_OBJECT

public:

  explicit GrandMasterWidget(QWidget *parent = nullptr);

signals:

private:

  QSlider *m_grandMasterslider;
  QPushButton *m_blackOutButton;
  QLabel *m_grandMasterLabel;

};

#endif // GRANDMASTERWIDGET_H
