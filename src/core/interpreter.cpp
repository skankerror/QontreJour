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

#include "interpreter.h"
#include <QDebug>

Interpreter::Interpreter(QObject *parent)
    : QObject{parent}
{}

void Interpreter::recieveData(KeypadButton t_button)
{
  if (t_button == KeypadButton::All)
  {
    m_valueCount = 0;
    m_isValued = false;
    m_L_selectedChannels.squeeze();
    m_L_selectedChannels.clear();
    m_L_digits.squeeze();
    m_L_digits.clear();
    emit selectAll();
    return;
  }

  if (t_button == KeypadButton::Clear)
  {
    m_valueCount = 0;
    m_isValued = false;
    m_L_selectedChannels.squeeze();
    m_L_selectedChannels.clear();
    m_L_digits.squeeze();
    m_L_digits.clear();
    emit clearSelection();
    return;
  }

  if ((m_valueCount == 0)
      && (t_button > KeypadButton::Dot)) // no value yet and it's not value
  {
    emit sendError(); // syntax error
    return;
  }


  // if it's a value
  if (t_button <= KeypadButton::Dot)
  {
    // we add to list
    m_L_digits.append(t_button);
    return;
  }

  // it's not a value.
  switch (t_button)
  {
  case KeypadButton::Channel : calculateId(); selectId(); break;
  case KeypadButton::Output : calculateId(); selectId(); break;
  case KeypadButton::Thru : calculateId(); selectThru(); break;
  case KeypadButton::Plus : calculateId(); selectPlus(); break;
  case KeypadButton::Moins : calculateId(); selectMoins(); break;
  default : break;
  }
}

void Interpreter::calculateId()
{

}

void Interpreter::calculateFloat()
{

}

void Interpreter::selectId()
{

}

void Interpreter::selectThru()
{

}

void Interpreter::selectPlus()
{

}

void Interpreter::selectMoins()
{

}

