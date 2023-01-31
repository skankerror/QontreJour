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
#include <QMenu>
#include <QActionGroup>
#include <QAction>
#include <QList>
#include <QPushButton>
#include <ossia-cpp/ossia-cpp98.hpp>
#include "dmxmanagerwidget.h"
#include "grandmasterwidget.h"
#include "playbackwidget.h"
#include "sequencerwidget.h"
#include "submasterwidget.h"
#include "directchannelwidget.h"
#include "dmxchanneloutputwidget.h"

#include "../core/dmxscene.h"
//#include "../core/dmxsequence.h"


class MainWindow
    : public QMainWindow
{
  Q_OBJECT

public:

  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private:

  void createDockWidgets();
  void createDmxManagerContainerWidget();
  void createCentralWidget();

  void createConnections();

signals:

  void universeCountChanged(int);

private slots:

  void addDmxManagerWidget();
  void removeDmxManagerWidget();
  void setDirectChannelWidget(int t_universeID);
  void setSubmasterWidget();

private:

  QTabWidget *m_tabWidget;
  SubmasterWidget *m_submasterWidget;
  DirectChannelWidget *m_directChannelWidget;
  QWidget *m_dmxManagerContainerWidget;
  QPushButton *m_addDmxManagerWidgetButton;
  QPushButton *m_removeDmxManagerWidgetButton;
  QVBoxLayout *m_dmxManagerContainerLayout;
  QList<DmxManagerWidget *> m_L_dmxManagerWidget;

  GrandMasterWidget *m_grandMasterWidget;
  PlaybackWidget *m_playbackWidget;
  SequencerWidget *m_sequencerWidget;
  DmxChannelOutputWidget *m_dmxChannelOutputWidget;

  QDmxManager *m_dmxManager;

  int m_universeCount; //s
};
#endif // MAINWINDOW_H
