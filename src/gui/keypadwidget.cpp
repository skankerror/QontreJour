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

KeypadWidget::KeypadWidget(QWidget *parent)
    : QWidget{parent}
{
  populateWidget();
}

void KeypadWidget::populateWidget()
{
  auto layout = new QGridLayout();

  auto timeButton = new QPushButton("time", this);
  layout->addWidget(timeButton, 0, 0);
  connect(timeButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Time); });

  auto cueButton = new QPushButton("cue", this);
  layout->addWidget(cueButton, 0, 1);
  connect(cueButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Cue); });

  auto groupButton = new QPushButton("group", this);
  layout->addWidget(groupButton, 0, 2);
  connect(groupButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Group); });

  auto recButton = new QPushButton("record", this);
  layout->addWidget(recButton, 0, 3);
  connect(recButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Record); });

  auto updateButton = new QPushButton("update", this);
  layout->addWidget(updateButton, 0, 4);
  connect(updateButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Update); });

  auto deleteButton = new QPushButton("delete", this);
  layout->addWidget(deleteButton, 0, 5);
  connect(deleteButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Delete); });

  auto patchButton = new QPushButton("patch", this);
  layout->addWidget(patchButton, 0, 6);
  connect(patchButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Patch); });

  /**********************************************/

  auto timeinButton = new QPushButton("in", this);
  layout->addWidget(timeinButton, 1, 0);
  connect(timeinButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Timein); });

  auto sevenButton = new QPushButton("7", this);
  layout->addWidget(sevenButton, 1, 1);
  connect(sevenButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Seven); });

  auto eightButton = new QPushButton("8", this);
  layout->addWidget(eightButton, 1, 2);
  connect(eightButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Eight); });

  auto nineButton = new QPushButton("9", this);
  layout->addWidget(nineButton, 1, 3);
  connect(nineButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Nine); });

  auto arobaseButton = new QPushButton("@", this);
  layout->addWidget(arobaseButton, 1, 4);
  connect(arobaseButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Arobase); });

  auto levelPlusButton = new QPushButton("+%", this);
  layout->addWidget(levelPlusButton, 1, 5);
  connect(levelPlusButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Pluspc); });

  auto unpatchButton = new QPushButton("unpatch", this);
  layout->addWidget(unpatchButton, 1, 6);
  connect(unpatchButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Unpatch); });

  /**********************************************/

  auto timeoutButton = new QPushButton("out", this);
  layout->addWidget(timeoutButton, 2, 0);
  connect(timeoutButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Timeout); });

  auto fourButton = new QPushButton("4", this);
  layout->addWidget(fourButton, 2, 1);
  connect(fourButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Four); });

  auto fiveButton = new QPushButton("5", this);
  layout->addWidget(fiveButton, 2, 2);
  connect(fiveButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Five); });

  auto sixButton = new QPushButton("6", this);
  layout->addWidget(sixButton, 2, 3);
  connect(sixButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Six); });

  auto channelButton = new QPushButton("CH", this);
  layout->addWidget(channelButton, 2, 4);
  connect(channelButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Channel); });

  auto levelMoinsButton = new QPushButton("-%", this);
  layout->addWidget(levelMoinsButton, 2, 5);
  connect(levelMoinsButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Moinspc); });

  auto outputButton = new QPushButton("output", this);
  layout->addWidget(outputButton, 2, 6);
  connect(outputButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Output); });

  /**********************************************/

  auto delayinButton = new QPushButton("d in", this);
  layout->addWidget(delayinButton, 3, 0);
  connect(delayinButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Delayin); });

  auto oneButton = new QPushButton("1", this);
  layout->addWidget(oneButton, 3, 1);
  connect(oneButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::One); });

  auto twoButton = new QPushButton("2", this);
  layout->addWidget(twoButton, 3, 2);
  connect(twoButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Two); });

  auto threeButton = new QPushButton("3", this);
  layout->addWidget(threeButton, 3, 3);
  connect(threeButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Three); });

  auto thruButton = new QPushButton("thru", this);
  layout->addWidget(thruButton, 3, 4);
  connect(thruButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Thru); });

  auto plusButton = new QPushButton("+", this);
  layout->addWidget(plusButton, 3, 5);
  connect(plusButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Plus); });

  auto enterButton = new QPushButton("ENTER", this);
  layout->addWidget(enterButton, 3, 6, 2, 1);
  connect(enterButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Enter); });

  /**********************************************/

  auto delayoutButton = new QPushButton("d out", this);
  layout->addWidget(delayoutButton, 4, 0);
  connect(delayoutButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Delayout); });

  auto dotButton = new QPushButton(".", this);
  layout->addWidget(dotButton, 4, 1);
  connect(dotButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Dot); });

  auto zeroButton = new QPushButton("0", this);
  layout->addWidget(zeroButton, 4, 2);
  connect(zeroButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Zero); });

  auto clearButton = new QPushButton("C", this);
  layout->addWidget(clearButton, 4, 3);
  connect(clearButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Clear); });

  auto allButton = new QPushButton("all", this);
  layout->addWidget(allButton, 4, 4);
  connect(allButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::All); });

  auto moinsButton = new QPushButton("-", this);
  layout->addWidget(moinsButton, 4, 5);
  connect(moinsButton, &QPushButton::clicked, [=]
          { emit buttonClicked(KeypadButton::Moins); });


  setLayout(layout);

}
