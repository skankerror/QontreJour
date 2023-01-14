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

#include "dimmer.h"

Dimmer::Dimmer(int uneAdresse, int unNbCanaux, QObject *parent) :
  QObject(parent),
  m_adresse(uneAdresse),
  m_nbCanaux(unNbCanaux)
{

}


Dimmer::Dimmer(const Dimmer &source): QObject()
{
  m_adresse = source.m_adresse;
  m_nbCanaux = source.m_nbCanaux;
  m_etat = source.m_etat;
}

void Dimmer::setAdresse(int uneAdresse)
{
  m_adresse = uneAdresse;
}

void Dimmer::setNbCanaux(int unNbCanaux)
{
  m_nbCanaux = unNbCanaux;
}

void Dimmer::initialiser()
{

}

void Dimmer::initialiser(int adresse, int nbCanaux)
{
  Q_UNUSED(adresse)
  Q_UNUSED(nbCanaux)
}

void Dimmer::commander()
{

}

