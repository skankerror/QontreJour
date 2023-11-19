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

#ifndef QONTREJOUR_H
#define QONTREJOUR_H

#include <QObject>

#define UNIVERSE_OUTPUT_COUNT_DEFAULT 512
#define DEFAULT_CHANNEL_COUNT 512

#define SUBMASTER_SLIDERS_COUNT_PER_PAGE 20
#define SUBMASTER_SLIDERS_PAGE_COUNT 10

//#define DMX_VALUE_TABLE_MODEL_ROWS_COUNT_DEFAULT 32
#define DMX_VALUE_TABLE_MODEL_ROWS_COUNT_DEFAULT 16
#define DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT 32

#define MAIN_WINDOW_WIDTH 1280
#define MAIN_WINDOW_HEIGHT 720

#define SLIDERS_PER_PAGE 32

#define CHANNEL_TABLE_ITEM_SIZE 43

#define DEFAULT_IN_TIME 5.0f
#define DEFAULT_OUT_TIME 5.0f

#define ORANGE_COLOR QRgb(0xffBA6D2B)
#define DARK_ORANGE_COLOR QRgb(0xff5C3514)
#define BROWN_COLOR QRgb(0xff58371B)
#define LIGHT_BROWN_COLOR QRgb(0xffBF783B)
#define DEEP_BLUE_COLOR QRgb(0xff3C55A4)
#define LIGHT_BLUE_COLOR QRgb(0xff7DA7FF)
#define LIGHT_GREEN_COLOR QRgb(0xff4FC761)
#define LIGHT_YELLOW_COLOR QRgb(0xffB8BF7E)
#define LIGHT_GREY_COLOR QRgb(0xff5B6772)
#define RED_COLOR QRgb(0xff960710)
#define PURPLE_COLOR QRgb(0xff9D0F96)
#define GREY_COLOR QRgb(0xffBF783B)
#define BLACK_COLOR QRgb(0xff000000)


#define NULL_CH_ID_DMX Ch_Id_Dmx(NO_ID,NULL_DMX)
#define NULL_GR_ID_DMX Gr_Id_Dmx(NO_ID,NULL_DMX)

#define MANAGER DmxManager::instance()

// 0 to 255, value for dmx levels
typedef quint8 dmx ;

// needed when editing channel group level
// when one channel is at 0 or 255, the editing group can still change
// when need to keep decay between unchanged values, and those who are still changing
typedef qint16 overdmx;

// id for every output, channel, channel group, etc...
// not unsigned cos it can be -1 to specify no id
typedef qint16 id;

// id for universe. signed cos it can be -1 for channel group, scene, etc...
typedef qint16 uid;

typedef qreal sceneID_f;
typedef qreal time_f;

typedef quint8 percent;

#define NO_ID -1
#define NO_UID -1
#define NULL_DMX 0
#define NULL_DMX_OFFSET 0
#define NULL_UID_ID Uid_Id(NO_UID,NO_ID)
#define MAX_DMX 255

#define DEFAULT_OUTPUT_NAME "OUT"
#define DEFAULT_CHANNEL_NAME "CH"
#define DEFAULT_GROUP_NAME "GROUP"

#define BUTTON_WIDTH_MAX 55

#define PLUS_PERCENT 5
#define MOINS_PERCENT 5
#define PLUS_DMX 13
#define MOINS_DMX 13

enum KeypadButton
{
  Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine, Dot,
  Time, Timein, Timeout, Delayin, Delayout,
  Channel, Output, Cue, Group,
  Record, Update, Delete,
  Patch, Unpatch,
  Plus, Moins, Clear, All, Thru,
  Pluspc, Moinspc, ArobaseDmx, ArobasePercent,
  Step, Goto,
  Help,
  UnknownButton
};

enum PlayBackButton
{
  GoButton,
  GoBackButton,
  PauseButton,
  SeqPlusButton,
  SeqMoinsButton
};

enum ValueType
{
  RootOutputType, // stored in universe
  OutputType, // stored in root
  RootChannel, // stored in manager
  ChannelType, // stored in root
  RootChannelGroup, // stored in manager
  ChannelGroup, // stored in root
  SequenceType,
  Scene0,
  MainScene,
  SubSceneType,
  UnknownValueType
};

enum ChannelDataFlag
{
  SelectedSceneFlag,
  DirectChannelFlag,
  ChannelGroupFlag,
  ParkedFlag,
  IndependantFlag,
  UnknownFlag
};

enum HwPortType
{
  HwInput,
  HwOutput
};

enum WidgetType
{
  DmxSlider,
  DmxTableView,
  UnknownWidgetType
};

enum SelectionType
{
  ChannelSelectionType,
  OutputSelectionType,
  UnknownSelectionType
};

enum HeaderField
{
  StepField,
  IDField,
  NameField,
  NoteField,
  InField,
  OutField,
  DelayInField,
  DelayOutField,
  HeaderFieldCount
};

enum CueRole
{
  OutingCue,
  InningCue,
  NewSelectRole,
  UnknownRole
};

/******************************** Uid_Id *************************************/

//class DmxOutput;

class Uid_Id
{

public :

  explicit Uid_Id(const uid t_uid = NO_UID,
                  const id t_id = NO_ID)
      : m_universeID(t_uid),
      m_outputID(t_id)
  {}

  ~Uid_Id(){}

  bool operator==(const Uid_Id t_Uid_Id) const
  {
    return ((t_Uid_Id.getUniverseID() == m_universeID)
            && (t_Uid_Id.getOutputID() == m_outputID));
  }

  uid getUniverseID() const{ return m_universeID; }
  id getOutputID() const{ return m_outputID; }

  void setUniverseID(const uid t_universeID){ m_universeID = t_universeID; }
  void setOutputID(const id t_outputID){ m_outputID = t_outputID; }

  QString toString() const
  {
    return QString("%1.%2")
        .arg(m_universeID)
        .arg(m_outputID);
  }

  static QString UidtoString(const Uid_Id t_uid_id)
  {
    return QString("%1.%2")
        .arg(t_uid_id.getUniverseID())
        .arg(t_uid_id.getOutputID());
  }

private :

  uid m_universeID = NO_UID;
  id m_outputID = NO_ID;

};

class IdedValue
{

public :

  IdedValue(id t_id = NO_ID)
      : m_id(t_id)
  {}

  id getid() const{ return m_id; }
  void setid(const id t_id){ m_id = t_id; }

protected :

  id m_id = NO_ID;
};

class UniversedValue
{

public :

  UniversedValue(uid t_uid = NO_UID)
      : m_uid(t_uid)
  {}

  id getuid() const{ return m_uid; }
  void setuid(const id t_uid){ m_uid = t_uid; }

protected :

  uid m_uid = NO_UID;
};

/****************************** Ch_Id_Dmx ********************************/

class Ch_Id_Dmx
{

public :

  explicit Ch_Id_Dmx(const id t_ID = NO_ID,
                     const dmx t_level = NULL_DMX)
      : m_ID(t_ID),
      m_level(t_level)
  {}

  virtual ~Ch_Id_Dmx(){}

  bool operator==(const Ch_Id_Dmx t_id_dmx) const
  { return ((t_id_dmx.getid() == m_ID)
            && (t_id_dmx.getLevel() == m_level)); }
  virtual bool isBrother(const Ch_Id_Dmx t_id_dmx) const
  { return (m_ID == t_id_dmx.getid()); }

  // NOTE : use isBrother() before these operators
  bool operator<(const Ch_Id_Dmx t_id_dmx) const
  { return (m_level < t_id_dmx.getLevel()); }
  bool operator>(const Ch_Id_Dmx t_id_dmx) const
  { return (m_level > t_id_dmx.getLevel()); }
  bool operator<=(const Ch_Id_Dmx t_id_dmx) const
  { return (m_level <= t_id_dmx.getLevel()); }
  bool operator>=(const Ch_Id_Dmx t_id_dmx) const
  { return (m_level >= t_id_dmx.getLevel()); }


  id getid() const{ return m_ID; }
  dmx getLevel() const{ return m_level; }

  void setID(const id t_ID){ m_ID = t_ID; }
  void setLevel(const dmx t_level){ m_level = t_level; }

  bool isValid() const{ return (m_ID > NO_ID); }

private :

  id m_ID = NO_ID;
  dmx m_level = NULL_DMX;

};

/****************************** Gr_Id_Dmx ********************************/

class Gr_Id_Dmx
    : public Ch_Id_Dmx
{

public :

  explicit Gr_Id_Dmx(const id t_ID = NO_ID,
                     const dmx t_level = NULL_DMX)
      : Ch_Id_Dmx(t_ID,
                  t_level)
  {}

  ~Gr_Id_Dmx(){}

  bool isBrother(const Ch_Id_Dmx t_id_dmx) const override
  { return false; }
};

/****************************** Sceneid_Id_Dmx ********************************/

class Sceneid_Dmx
{

public :

  explicit Sceneid_Dmx(const sceneID_f t_ID = 0,
                       const dmx t_level = NULL_DMX)
      : m_ID(t_ID),
      m_level(t_level)
  {}

  ~Sceneid_Dmx(){}

  bool operator==(const Sceneid_Dmx t_id_dmx) const
  { return ((t_id_dmx.getid() == m_ID)
            && (t_id_dmx.getLevel() == m_level)); }
  virtual bool isBrother(const Sceneid_Dmx t_id_dmx) const
  { return (m_ID == t_id_dmx.getid()); }

  // NOTE : use isBrother() before these operators
  bool operator<(const Sceneid_Dmx t_id_dmx) const
  { return (m_level < t_id_dmx.getLevel()); }
  bool operator>(const Sceneid_Dmx t_id_dmx) const
  { return (m_level > t_id_dmx.getLevel()); }
  bool operator<=(const Sceneid_Dmx t_id_dmx) const
  { return (m_level <= t_id_dmx.getLevel()); }
  bool operator>=(const Sceneid_Dmx t_id_dmx) const
  { return (m_level >= t_id_dmx.getLevel()); }


  sceneID_f getid() const{ return m_ID; }
  dmx getLevel() const{ return m_level; }

  void setID(const sceneID_f t_ID){ m_ID = t_ID; }
  void setLevel(const dmx t_level){ m_level = t_level; }

  bool isValid() const{ return (m_ID > 0); }

private :

  sceneID_f m_ID = 0;
  dmx m_level = NULL_DMX;

};

/****************************** SeqId_SceneId ********************************/

class SeqId_SceneId
{
public :

  explicit SeqId_SceneId(id t_seqid = NO_ID,
                         sceneID_f t_sceneid = 0.0f)
      : m_seqId(t_seqid),
      m_sceneId(t_sceneid)
  {}

  bool operator==(const SeqId_SceneId t_id_sceneid) const
  { return ((t_id_sceneid.getid() == m_seqId)
            && (t_id_sceneid.getSceneId() == m_sceneId)); }

  virtual bool isBrother(const SeqId_SceneId t_id_sceneid) const
  { return (m_seqId == t_id_sceneid.getid()); }

  // NOTE : use isBrother() before these operators
  bool operator<(const SeqId_SceneId t_id_sceneid) const
  { return (m_sceneId < t_id_sceneid.getSceneId()); }
  bool operator>(const SeqId_SceneId t_id_sceneid) const
  { return (m_sceneId > t_id_sceneid.getSceneId()); }
  bool operator<=(const SeqId_SceneId t_id_sceneid) const
  { return (m_sceneId <= t_id_sceneid.getSceneId()); }
  bool operator>=(const SeqId_SceneId t_id_sceneid) const
  { return (m_sceneId >= t_id_sceneid.getSceneId()); }

  id getid() const{ return m_seqId; }
  void setid(id t_seqId){ m_seqId = t_seqId; }

  sceneID_f getSceneId() const{ return m_sceneId; }
  void setSceneId(sceneID_f t_sceneId){ m_sceneId = t_sceneId; }

private :

  id m_seqId;
  sceneID_f m_sceneId;
};

/******************************* ChannelData ***********************/

class ChannelData
    : public QObject
{

  Q_OBJECT

public :

  explicit ChannelData(id t_id = NO_ID,
                       dmx t_channelGroupLevel = NULL_DMX,
                       dmx t_directChannelLevel = NULL_DMX,
                       overdmx t_directChannelOffset = NULL_DMX_OFFSET,
                       dmx t_sceneLevel = NULL_DMX,
                       dmx t_nextSceneLevel = NULL_DMX,
                       QObject *parent = nullptr)
      : QObject(parent),
      m_channelID(t_id),
      m_channelGroupLevel(t_channelGroupLevel),
      m_directChannelLevel(t_directChannelLevel),
      m_directChannelOffset(t_directChannelOffset),
      m_sceneLevel(t_sceneLevel),
      m_nextSceneLevel(t_nextSceneLevel)
  {}

  ChannelData(const ChannelData &channel)
      : QObject(channel.parent())/*,
      m_channelID(channel.getChannelID()),
      m_channelGroupLevel(channel.getChannelGroupLevel()),
      m_directChannelLevel(channel.getDirectChannelLevel()),
      m_directChannelOffset(channel.getDirectChannelOffset()),
      m_sceneLevel(channel.getSceneLevel()),
      m_nextSceneLevel(channel.getNextSceneLevel()),
      m_flag(channel.getFlag()),
      m_actual_Level(channel.getActual_Level()),
      m_isDirectChannel(channel.getIsDirectChannel()),
      m_isSelected(channel.getIsSelected())*/
  {
    m_channelID = channel.getChannelID();
    m_channelGroupLevel = channel.getChannelGroupLevel();
    m_directChannelLevel = channel.getDirectChannelLevel();
    m_directChannelOffset = channel.getDirectChannelOffset();
    m_sceneLevel = channel.getSceneLevel();
    m_nextSceneLevel = channel.getNextSceneLevel();
    m_flag = channel.getFlag();
    m_actual_Level = channel.getActual_Level();
    m_isDirectChannel = channel.getIsDirectChannel();
    m_isSelected = channel.getIsSelected();
  }

  ~ChannelData(){}

  dmx getChannelGroupLevel() const{ return m_channelGroupLevel; }
  dmx getDirectChannelLevel() const{ return m_directChannelLevel; }
  overdmx getDirectChannelOffset() const{ return m_directChannelOffset; }
  dmx getSceneLevel() const{ return m_sceneLevel; }
  dmx getNextSceneLevel() const{ return m_nextSceneLevel; }
  dmx getActual_Level() const{ return m_actual_Level; }
  ChannelDataFlag getFlag() const{ return m_flag; }
  id getChannelID() const{ return m_channelID; }
  bool getIsSelected() const{ return m_isSelected; }
  bool getIsDirectChannel() const{ return m_isDirectChannel; }

  void setChannelGroupLevel(dmx t_channelGroupLevel)
  { m_channelGroupLevel = t_channelGroupLevel; }
  void setDirectChannelLevel(dmx t_directChannelLevel)
  { m_directChannelLevel = t_directChannelLevel; }
  void setDirectChannelOffset(overdmx t_directChannelOffset)
  { m_directChannelOffset = t_directChannelOffset; }
  void setSceneLevel(dmx t_sceneLevel){ m_sceneLevel = t_sceneLevel; }
  void setNextSceneLevel(dmx t_nextSceneLevel)
  { m_nextSceneLevel = t_nextSceneLevel; }
  void setChannelID(id t_channelID){ m_channelID = t_channelID; }
  void setActual_Level(dmx t_actual_Level){ m_actual_Level = t_actual_Level; }
  void setIsSelected(bool t_isSelected)
  { m_isSelected = t_isSelected;
    if (!t_isSelected) clearOverdmx(); }
  void setIsDirectChannel(bool t_isDirectChannel)
  { m_isDirectChannel = t_isDirectChannel; }

  void clearChannel()
  {
    m_directChannelLevel = NULL_DMX;
    m_directChannelOffset = NULL_DMX_OFFSET;
    m_sceneLevel = NULL_DMX;
    m_nextSceneLevel = NULL_DMX;
    // TODO : gerer le flag
  }

  void clearDirectChannel()
  {
    m_directChannelLevel = NULL_DMX;
    m_directChannelOffset = NULL_DMX_OFFSET;
    m_isDirectChannel = false;
//    m_sceneLevel = NULL_DMX;
  }

  void clearOverdmx(){ m_directChannelOffset = NULL_DMX_OFFSET; }
  void update()
  {
    setFlag(ChannelDataFlag::UnknownFlag);
    if (m_channelGroupLevel >= m_directChannelLevel)
    {
      m_actual_Level = m_channelGroupLevel;
      if (m_actual_Level)
        setFlag(ChannelDataFlag::ChannelGroupFlag);
      if (m_isDirectChannel) return;// on s'en fout de scene level
      if (m_sceneLevel >= m_actual_Level)
      {
        m_actual_Level = m_sceneLevel;
        if (m_actual_Level)
          setFlag(ChannelDataFlag::SelectedSceneFlag);
      }
    }
    else
    {
      m_actual_Level = m_directChannelLevel;
      setFlag(ChannelDataFlag::DirectChannelFlag);
      if (m_isDirectChannel) return;// on s'en fout de scene level
      if (m_sceneLevel >= m_actual_Level)
      {
        m_actual_Level = m_sceneLevel;
        setFlag(ChannelDataFlag::SelectedSceneFlag);
      }
    }
    // TODO : développer, gérer l'offset,etc...
  }

private :

  void setFlag(ChannelDataFlag t_flag){ m_flag = t_flag; }

private :

  id m_channelID = NO_ID;
  dmx m_channelGroupLevel = NULL_DMX;
  dmx m_directChannelLevel = NULL_DMX;
  overdmx m_directChannelOffset = NULL_DMX_OFFSET;
  dmx m_sceneLevel = NULL_DMX;
  dmx m_nextSceneLevel = NULL_DMX;
  ChannelDataFlag m_flag = UnknownFlag;

  dmx m_actual_Level = NULL_DMX;
  bool m_isSelected = false;
  bool m_isDirectChannel = false;
};
Q_DECLARE_METATYPE(ChannelData)


#endif // QONTREJOUR_H
