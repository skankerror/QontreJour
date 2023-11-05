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

#include "keypadwidget.h"
#include <QGridLayout>
#include <QPushButton>
#include <QDebug>

/************************ PushButton *****************************/

PushButton::PushButton(QWidget *parent)
    : QPushButton(parent)
{
  setMaximumWidth(BUTTON_WIDTH_MAX);
}

PushButton::PushButton(const QString &text,
                       QWidget *parent)
    : QPushButton(text,
                  parent)

{
  setMaximumWidth(BUTTON_WIDTH_MAX);
}

/************************ KeypadWidget *****************************/

KeypadWidget::KeypadWidget(QWidget *parent)
    : QWidget{parent}
{
  populateWidget();
}

void KeypadWidget::populateWidget()
{
  auto layout = new QGridLayout();

  auto patchButton = new PushButton("patch", this);
  layout->addWidget(patchButton, 0, 0);
  connect(patchButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Patch); });

  auto unpatchButton = new PushButton("unpat", this);
  layout->addWidget(unpatchButton, 0, 1);
  connect(unpatchButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Unpatch); });

  auto outputButton = new PushButton("outp", this);
  layout->addWidget(outputButton, 0, 2);
  connect(outputButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Output); });

  auto gotoButton = new PushButton("goto", this);
  layout->addWidget(gotoButton, 0, 3);
  connect(gotoButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Goto); });

  auto stepButton = new PushButton("Step", this);
  layout->addWidget(stepButton, 0, 4);
  connect(stepButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Step); });

  auto helpButton = new PushButton("?", this);
  layout->addWidget(helpButton, 0, 5);

  /**********************************************/

  auto timeButton = new PushButton("time", this);
  layout->addWidget(timeButton, 1, 0);
  connect(timeButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Time); });

  auto cueButton = new PushButton("cue", this);
  layout->addWidget(cueButton, 1, 1);
  connect(cueButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Cue); });

  auto groupButton = new PushButton("group", this);
  layout->addWidget(groupButton, 1, 2);
  connect(groupButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Group); });

  auto recButton = new PushButton("REC", this);
  layout->addWidget(recButton, 1, 3);
  connect(recButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Record); });

  auto updateButton = new PushButton("UPD", this);
  layout->addWidget(updateButton, 1, 4);
  connect(updateButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Update); });

  auto deleteButton = new PushButton("DEL", this);
  layout->addWidget(deleteButton, 1, 5);
  connect(deleteButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Delete); });

  /**********************************************/

  auto timeinButton = new PushButton("in", this);
  layout->addWidget(timeinButton, 2, 0);
  connect(timeinButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Timein); });

  auto sevenButton = new PushButton("7", this);
  layout->addWidget(sevenButton, 2, 1);
  connect(sevenButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Seven); });

  auto eightButton = new PushButton("8", this);
  layout->addWidget(eightButton, 2, 2);
  connect(eightButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Eight); });

  auto nineButton = new PushButton("9", this);
  layout->addWidget(nineButton, 2, 3);
  connect(nineButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Nine); });

  auto arobaseButton = new PushButton("@%", this);
  layout->addWidget(arobaseButton, 2, 4);
  connect(arobaseButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::ArobasePercent); });

  auto levelPlusButton = new PushButton("+%", this);
  layout->addWidget(levelPlusButton, 2, 5);
  connect(levelPlusButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Pluspc); });

  /**********************************************/

  auto timeoutButton = new PushButton("out", this);
  layout->addWidget(timeoutButton, 3, 0);
  connect(timeoutButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Timeout); });

  auto fourButton = new PushButton("4", this);
  layout->addWidget(fourButton, 3, 1);
  connect(fourButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Four); });

  auto fiveButton = new PushButton("5", this);
  layout->addWidget(fiveButton, 3, 2);
  connect(fiveButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Five); });

  auto sixButton = new PushButton("6", this);
  layout->addWidget(sixButton, 3, 3);
  connect(sixButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Six); });

  auto channelButton = new PushButton("CH", this);
  layout->addWidget(channelButton, 3, 4);
  connect(channelButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Channel); });

  auto levelMoinsButton = new PushButton("-%", this);
  layout->addWidget(levelMoinsButton, 3, 5);
  connect(levelMoinsButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Moinspc); });

  /**********************************************/

  auto delayinButton = new PushButton("d in", this);
  layout->addWidget(delayinButton, 4, 0);
  connect(delayinButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Delayin); });

  auto oneButton = new PushButton("1", this);
  layout->addWidget(oneButton, 4, 1);
  connect(oneButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::One); });

  auto twoButton = new PushButton("2", this);
  layout->addWidget(twoButton, 4, 2);
  connect(twoButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Two); });

  auto threeButton = new PushButton("3", this);
  layout->addWidget(threeButton, 4, 3);
  connect(threeButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Three); });

  auto thruButton = new PushButton("thru", this);
  layout->addWidget(thruButton, 4, 4);
  connect(thruButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Thru); });

  auto plusButton = new PushButton("+", this);
  layout->addWidget(plusButton, 4, 5);
  connect(plusButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Plus); });

  /**********************************************/

  auto delayoutButton = new PushButton("d out", this);
  layout->addWidget(delayoutButton, 5, 0);
  connect(delayoutButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Delayout); });

  auto dotButton = new PushButton(".", this);
  layout->addWidget(dotButton, 5, 1);
  connect(dotButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Dot); });

  auto zeroButton = new PushButton("0", this);
  layout->addWidget(zeroButton, 5, 2);
  connect(zeroButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Zero); });

  auto clearButton = new PushButton("C", this);
  layout->addWidget(clearButton, 5, 3);
  connect(clearButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Clear); });

  auto allButton = new PushButton("all", this);
  layout->addWidget(allButton, 5, 4);
  connect(allButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::All); });

  auto moinsButton = new PushButton("-", this);
  layout->addWidget(moinsButton, 5, 5);
  connect(moinsButton, &PushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Moins); });

  setLayout(layout);
  layout->setSizeConstraint(QLayout::SetMaximumSize);
}

/****************************************************************/

PlaybackWidget::PlaybackWidget(QWidget *parent)
    : QWidget(parent),
    m_ASlider(new QSlider(this)),
    m_ALabel(new QLabel("A", this)),
    m_BSlider(new QSlider(this)),
    m_BLabel(new QLabel("B", this)),
    m_seqInfButton(new PushButton(this)),
    m_seqSupButton(new PushButton(this)),
    m_pauseButton(new PushButton(this)),
    m_goBackButton(new PushButton(this)),
    m_goButton(new QPushButton(this))
{
  auto layout = new QGridLayout();

  m_seqInfButton->setText("seq-");
  m_seqSupButton->setText("seq+");
  m_pauseButton->setText("pause");
  m_goBackButton->setText("GoBack");
  m_goButton->setText("GO");
  m_ALabel->setAlignment(Qt::AlignHCenter);
  m_BLabel->setAlignment(Qt::AlignHCenter);

  layout->addWidget(m_ASlider, 0, 0, 8, 1);
  layout->addWidget(m_BSlider, 0, 1, 8, 1);
  layout->addWidget(m_ALabel, 8, 0, 1, 1);
  layout->addWidget(m_BLabel, 8, 1, 1, 1);
  layout->addWidget(m_seqInfButton, 9, 0, 1, 1);
  layout->addWidget(m_seqSupButton, 9, 1, 1, 1);
  layout->addWidget(m_pauseButton, 10, 0, 1, 1);
  layout->addWidget(m_goBackButton, 10, 1, 1, 1);
  layout->addWidget(m_goButton, 11, 0, 1, 2);

  layout->setAlignment(m_ASlider, Qt::AlignHCenter);
  layout->setAlignment(m_BSlider, Qt::AlignHCenter);

  setLayout(layout);
//  layout->setSizeConstraint(QLayout::SetMaximumSize);

}

/****************************************************************/

GrandMasterWidget::GrandMasterWidget(QWidget *parent)
    : QWidget(parent),
    m_grandMasterslider(new QSlider(this)),
    m_blackOutButton(new QPushButton(this)),
    m_grandMasterLabel(new QLabel("GM",this))
{
  auto layout = new QVBoxLayout();

  m_blackOutButton->setText("Black Out");
  m_grandMasterLabel->setAlignment(Qt::AlignHCenter);
  layout->addWidget(m_blackOutButton);
  layout->addWidget(m_grandMasterslider);
  layout->addWidget(m_grandMasterLabel);
  layout->setAlignment(m_grandMasterslider, Qt::AlignHCenter);
  setLayout(layout);
//  layout->setSizeConstraint(QLayout::SetMaximumSize);

}
