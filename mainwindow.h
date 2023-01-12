#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QActionGroup>
#include <QAction>
#include <ossia-cpp/ossia-cpp98.hpp>

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



private:

  QMenu *m_fileMenu;
//  QMenu *editMenu;
//  QMenu *formatMenu;
  QMenu *m_helpMenu;
//  QActionGroup *alignmentGroup;
  QAction *m_newAct;
  QAction *m_openAct;
  QAction *m_saveAct;
//  QAction *printAct;
  QAction *m_exitAct;
//  QAction *undoAct;
//  QAction *redoAct;
//  QAction *cutAct;
//  QAction *copyAct;
//  QAction *pasteAct;
//  QAction *boldAct;
//  QAction *italicAct;
//  QAction *leftAlignAct;
//  QAction *rightAlignAct;
//  QAction *justifyAct;
//  QAction *centerAct;
//  QAction *setLineSpacingAct;
//  QAction *setParagraphSpacingAct;
  QAction *m_aboutAct;
  QAction *m_aboutQtAct;

};
#endif // MAINWINDOW_H
