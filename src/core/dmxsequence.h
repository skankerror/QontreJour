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

#ifndef DMXSEQUENCE_H
#define DMXSEQUENCE_H

#include <QObject>
#include "dmxscene.h"

class DmxSequence
    : public QObject
{
  Q_OBJECT

public :

  explicit DmxSequence(QObject *parent = nullptr);

  // getter
  DmxScene *getSelectedScene();
  DmxScene *getNextScene();
  DmxScene *getScene(int t_step);

  void addScene(DmxScene *t_scene);
  void removeScene(DmxScene *t_scene);
  void removeScene(int t_step);

signals:

private :

  // TODO : always keep a void root scene;
  QList<DmxScene *> m_L_dmxScene;
  int m_selectedScene;
};

#endif // DMXSEQUENCE_H
