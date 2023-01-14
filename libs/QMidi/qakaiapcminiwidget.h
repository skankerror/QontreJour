#ifndef QAKAIAPCMINIWIDGET_H
#define QAKAIAPCMINIWIDGET_H

#include "qabstractmidicontrolerwidget.h"

class QAkaiAPCMiniWidget :
    public QAbstractMidiControlerWidget
{

  Q_OBJECT

public :

  explicit QAkaiAPCMiniWidget(QWidget *parent = nullptr);
  ~QAkaiAPCMiniWidget();

//private :

//  void drawWidget();

protected:

  void drawWidget() override;
};

#endif // QAKAIAPCMINIWIDGET_H
