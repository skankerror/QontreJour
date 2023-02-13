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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QPushButton>
#include "universewidget.h"
#include "playbackwidget.h"
#include "sequencerwidget.h"
#include "valuesliderswidget.h"
#include "valuetablewidget.h"


class MainWindow
    : public QMainWindow
{

  Q_OBJECT

public :

  explicit MainWindow(QWidget *parent = nullptr);

  ~MainWindow();

private :

  void createCentralWidget();
  QWidget *createUniverseContainerWidget();
  void createDockWidgets();

private slots :

  void addUniverseWidget();
  void removeUniverseWidget();

private :

  DirectChannelWidget *m_directChannelWidget;
  QVBoxLayout *m_universeWidgetContainerLayout;
  QList<UniverseWidget *> m_L_universeWidget;

  ValueTableWidget *m_channelTableWidget;

};
#endif // MAINWINDOW_H
