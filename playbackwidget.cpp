#include <QLayout>
#include "playbackwidget.h"

PlaybackWidget::PlaybackWidget(QWidget *parent)
  : QWidget(parent),
    m_ASlider(new QSlider(this)),
    m_BSlider(new QSlider(this)),
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

  layout->addWidget(m_ASlider, 0, 0, 4, 1);
  layout->addWidget(m_BSlider, 0, 1, 4, 1);
  layout->addWidget(m_seqInfButton, 4, 0, 1, 1);
  layout->addWidget(m_seqSupButton, 4, 1, 1, 1);
  layout->addWidget(m_pauseButton, 5, 0, 1, 1);
  layout->addWidget(m_goBackButton, 5, 1, 1, 1);
  layout->addWidget(m_goButton, 6, 0, 1, 2);

  setLayout(layout);
}
