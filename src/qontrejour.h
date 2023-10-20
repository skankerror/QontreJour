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


#define UNIVERSE_OUTPUT_COUNT_DEFAULT 512
#define DEFAULT_CHANNEL_COUNT 512

#define SUBMASTER_SLIDERS_COUNT_PER_PAGE 20
#define SUBMASTER_SLIDERS_PAGE_COUNT 10

#define DMX_VALUE_TABLE_MODEL_ROWS_COUNT_DEFAULT 32
#define DMX_VALUE_TABLE_MODEL_COLUMNS_COUNT_DEFAULT 32

#define MAIN_WINDOW_WIDTH 1280
#define MAIN_WINDOW_HEIGHT 720

#define SLIDERS_PER_PAGE 32

#define DEFAULT_IN_TIME 5.0f
#define DEFAULT_OUT_TIME 5.0f

#define ORANGE_COLOR QRgb(0xffBA6D2B)
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
#define GET_CHANNEL_GROUP(x) DmxManager::instance()->getChannelGroup(x)
#define GET_CHANNEL(x) DmxManager::instance()->getChannel(x)
#define GET_CHANNEL_COUNT DmxManager::instance()->getChannelCount()
#define GET_OUTPUT(x,y) DmxManager::instance()->getOutput(Uid_Id(x,y))
#define GET_UNIVERSE_COUNT DmxManager::instance()->getUniverseCount()

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

typedef float sceneID_f;
typedef float time_f;

#define NO_ID -1
#define NO_UID -1
#define NULL_DMX 0
#define NULL_DMX_OFFSET 0
#define NULL_UID_ID Uid_Id(NO_UID,NO_ID)

#define DEFAULT_OUTPUT_NAME "OUT"
#define DEFAULT_CHANNEL_NAME "CH"
#define DEFAULT_GROUP_NAME "GROUP"

#define BUTTON_WIDTH_MAX 55

enum KeypadButton
{
  Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine, Dot,
  Time, Timein, Timeout, Delayin, Delayout,
  Channel, Output, Cue, Group,
  Record, Update, Delete,
  Patch, Unpatch,
  Plus, Moins, Clear, All, Thru,
  Pluspc, Moinspc, Arobase,
  Step, Goto,
  Help,
  UnknownButton
};

enum ValueType
{
  RootOutput, // stored in universe
  OutputType, // stored in root
  RootChannel, // stored in manager
  ChannelType, // stored in root
  RootChannelGroup, // stored in manager
  ChannelGroup, // stored in root
  SequenceType,
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

#endif // QONTREJOUR_H
