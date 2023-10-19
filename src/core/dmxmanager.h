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

#ifndef DMXMANAGER_H
#define DMXMANAGER_H

#include <QObject>
#include <QString>
#include "qdmxlib/QDmxManager"
#include "../qontrejour.h"
#include "dmxvalue.h"
#include "dmxscene.h"
#include "dmxuniverse.h"
#include "dmxengine.h"
#include "interpreter.h"

class DmxPatch;

class DmxManager
    : public QObject
{

  Q_OBJECT
  Q_ENUM(HwPortType)
  Q_ENUM(WidgetType)

public :

  static DmxManager *instance();

  ~DmxManager();

  // getters
  QStringList getAvailableDriversNames() const;
  QStringList getAvailableDevicesNames(const QString &t_driverString);
  int getUniverseCount() const{ return m_L_universe.size() ;}
  DmxOutput *getOutput(Uid_Id t_output_Uid_Id);
  RootValue *getRootChannel() const{ return m_rootChannel; }
  DmxChannel *getChannel(id t_channelId);
  int getChannelCount() const{ return m_rootChannel->getL_childValueSize(); }
  RootValue *getRootChannelGroup() const{ return m_rootChannelGroup; }
  DmxChannelGroup *getChannelGroup(id t_groupId);
  int getChannelGroupCount() const{ return m_rootChannelGroup->getL_childValueSize(); }

  // patch interface
  void setStraightPatch(const uid t_uid); // one universe
  void setStraightPatch(const QList<uid> t_L_uid); // several universes
  void setStraightPatch(); // all universes
  void clearPatch();
  void patchOutputToChannel(DmxChannel *t_channel,
                            DmxOutput *t_output);
  void patchOutputListToChannel(DmxChannel *t_channel,
                                QList<DmxOutput *> t_L_output);
  void unpatchOutputFromChannel(DmxChannel *t_channel,
                                DmxOutput *t_output);
  void unpatchOutputListFromChannel(DmxChannel *t_channel,
                                    QList<DmxOutput *> t_L_output);
  void unpatchOutput(DmxOutput *t_output);
  void unpatchOutputList(QList<DmxOutput *> t_L_output);
  void clearChannelPatch(DmxChannel *t_channel);
  void clearChannelListPatch(QList<DmxChannel *> t_L_channel);

  // hardware connection
  bool hwConnect(HwPortType t_type,
                 QString &t_driver,
                 QString &t_device,
                 quint8 t_port,
                 uid t_ID);
  bool hwDisconnect(uid t_ID);

  // widget connections
  // connect values with widget
  void connectValueToWidget(WidgetType t_widgetType,
                            int t_widgetID,
                            ValueType t_valueType,
                            id t_valueID);
  void testingMethod();

private :

  explicit DmxManager(QObject *parent = nullptr);

  // create everything we need
  bool createUniverse(uid t_universeID);
  DmxChannelGroup *createChannelGroup(QList<DmxChannel *> t_L_channel);

  QList<QDmxDriver *> getAvailableDrivers() const;
  QList<QDmxDevice *> getAvailableDevices(const QString &t_driverString);
  RootValue *getRootOutput(const uid t_uid) const;
  QList<RootValue *> getL_rootOutput() const;
  void connectOutputs();
  void connectInterpreter();

signals :

  void connectGroupToSubmasterSlider(int t_sliderID,
                                     id valueID);
  void disconnectGroupFromSubmasterSlider(int t_sliderID);

  void connectChannelToDirectChannelSlider(int t_sliderID,
                                           id valueID);
  void disconnectChannelToDirectChannelSlider(int t_sliderID);

  void ChannelSelectionChanged();

public slots :

  void submasterToEngine(id t_id,
                         dmx t_level);
  void directChannelToEngine(id t_id,
                             dmx t_level);
  void keypadToInterpreter(KeypadButton t_buttonType);

private slots :

  // connected to ?
  void onOutputRequest(uid t_uid,
                       id t_id,
                       dmx t_level);
  // connected to interpreter
  void onAddChannelSelection(QList<id> t_L_id);
  void onRemoveChannelSelection(QList<id> t_L_id);
  void onAddOutputSelection(QList<Uid_Id> t_L_Uid_Id);
  void onRemoveOutputSelection(QList<Uid_Id> t_L_Uid_Id);
  void onSelectAll();
  void onClearChannelSelection();
  void onClearOutputSelection();
  void onSetLevel(dmx t_level);
  void onSendError();
  void onSendError_NoValueSpecified();


private :

  QDmxManager *m_hwManager;
  DmxPatch *m_dmxPatch;
  DmxEngine *m_dmxEngine;
  Interpreter *m_interpreter;
  QList<DmxUniverse *> m_L_universe;
  RootValue *m_rootChannel;
  RootValue *m_rootChannelGroup;

  // members for interpreter
  QList<id> m_L_channelsIdSelection;
  QList<Uid_Id> m_L_outputUid_IdSelection;

};

#endif // DMXMANAGER_H
