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

#ifndef DIMMER_H
#define DIMMER_H


#include <QObject>
#include <QString>
#include <QVector>
//#include <QMap>

//#include "interfacedmx.h"

class Dimmer : public QObject
{

  Q_OBJECT

public:

  explicit Dimmer(int uneAdresse = 0,
                 int unNbCanaux = 0,
                 QObject *parent = nullptr);
  ~Dimmer(){}
  Dimmer(const Dimmer &source);

  int getAdresse() const{ return m_adresse; }
  void setAdresse(int uneAdresse);
  int getNbCanaux() const { return m_nbCanaux; }
  void setNbCanaux(int unNbCanaux);
  void initialiser();
  void initialiser(int adresse, int nbCanaux);
  void commander();

private:

  int m_adresse;
  int m_nbCanaux;
  QVector<int> m_etat;


signals:

};



#endif // DIMMER_H
