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
#include <ossia-cpp/ossia-cpp98.hpp>
#include "dmxmanagerwidget.h"
#include "grandmasterwidget.h"
#include "playbackwidget.h"
#include "sequencerwidget.h"
#include "dmxoutputwidget.h"
#include "submasterwidget.h"

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:

  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private:

  void CreateActions();
  void CreateMenubar();
  void CreateToolBars();
  void CreateDockWidgets();
  void CreateCentralWidget();
  void CreateStatusBar();

  void TestingZone();


private:

  QMenu *m_fileMenu;
  QMenu *m_helpMenu;
  QAction *m_newAct;
  QAction *m_openAct;
  QAction *m_saveAct;
  QAction *m_exitAct;
  QAction *m_aboutAct;
  QAction *m_aboutQtAct;

  QTabWidget *m_tabWidget;
  SubMasterWidget *m_submasterWidget;
  DMXManagerWidget *m_dmxManagerWidget;

  GrandMasterWidget *m_grandMasterWidget;
  PlaybackWidget *m_playbackWidget;
  SequencerWidget *m_sequencerWidget;
  DmxOutputWidget *m_dmxOutputWidget;

  QDmxManager *m_dmxManager;

};
#endif // MAINWINDOW_H
