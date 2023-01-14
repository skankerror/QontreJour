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
