#include "dmxvalue.h"

DmxValue::DmxValue(int t_ID,
                   QObject *parent)
  : QObject(parent)
{}

DmxValue::~DmxValue()
{}

void DmxValue::setLevel(int t_ID,
                        quint8 t_level)
{
  Q_UNUSED(t_ID)
  if (m_level == t_level
      || (t_level < 0)
      || (t_level > 255))
    return;

  m_level = t_level;

  emit levelChanged(m_ID,
                    m_level);
}

void DmxValue::setID(int t_ID)
{
  if (m_ID == t_ID)
    return;
  m_ID = t_ID;
}

void DmxValue::setPropertyLevel(int t_level)
{
  if (m_level == t_level
      || (t_level < 0)
      || (t_level > 255))
    return;

  m_level = t_level;

  emit levelChanged(m_ID,
                    m_level);

}
