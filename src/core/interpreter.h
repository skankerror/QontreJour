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

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <QObject>
#include "dmxvalue.h"
#include <QList>

class Interpreter
    : public QObject
{

  Q_OBJECT

public :

  explicit Interpreter(QObject *parent = nullptr);

  void addDigit(KeypadButton t_digit);
  void addDot();

signals :

  void setSelection(QList<id> t_L_id);
  void setLevel(dmx t_level);


public slots :


private :



};

#endif // INTERPRETER_H
