#include "qakaiapcminiwidget.h"
#include <QGridLayout>

#define X_RANGE 9
#define Y_RANGE 9

#define CREATE_BUTTON(x,y) auto button##x##y = new QPushButton(this); \
  layoutTotal->addWidget(button##x##y,x,y);


#define CREATE_SLIDER(y) auto slider##y = new QSlider(this); \
  layoutTotal->addWidget(slider##y,X_RANGE,y); \
  layoutTotal->setAlignment(slider##y, Qt::AlignHCenter);

// lighting apc mini buttons, 2nd data byte value
#define LIGHT_OFF 0;
// for rounded buttons
#define SINGLE_LIGHT_ON 1
#define SINGLE_LIGHT_BLINK 2
// for square buttons (except shift one)
#define GREEN_LIGHT_ON 1
#define GREEN_LIGHT_BLINK 2
#define RED_LIGHT_ON 3
#define RED_LIGHT_BLINK 4
#define YELLOW_LIGHT_ON 5
#define YELLOW_LIGHT_BLINK 6

QAkaiAPCMiniWidget::QAkaiAPCMiniWidget(QWidget *parent) :
  QAbstractMidiControlerWidget(parent)
{
  this->QAkaiAPCMiniWidget::drawWidget();
}

QAkaiAPCMiniWidget::~QAkaiAPCMiniWidget()
{}

void QAkaiAPCMiniWidget::drawWidget()
{
  auto layoutTotal = new QGridLayout();

  for (int i = 0; i < X_RANGE; i++)
  {
    for (int j = 0; j < Y_RANGE; j++)
    {
      CREATE_BUTTON(i,j)
    }
  }
  for (int j = 0; j < Y_RANGE; j++)
  {
    CREATE_SLIDER(j)
  }

  setLayout(layoutTotal);
}

