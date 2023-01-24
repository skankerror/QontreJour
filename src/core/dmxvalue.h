#ifndef DMXVALUE_H
#define DMXVALUE_H

#include <QObject>

class DmxValue : public QObject
{

  Q_OBJECT

public:

  explicit DmxValue(int t_ID,
                    QObject *parent = nullptr);

  virtual ~DmxValue()/* = 0*/;

// getters
  quint8 getLevel() const{  return m_level; }
  int getID() const { return m_ID; }

  // setters
  void setID(int t_ID);
  void setPropertyLevel(int t_level); // setter different from the slot, needed by property

signals:

  void levelChanged(int,      //m_ID
                    quint8);  //m_level

public slots:

  void setLevel(int t_ID,
                quint8 t_level);

protected :

  quint8 m_level;
  int m_ID;

//private:

  Q_PROPERTY(quint8 level
             READ getLevel
             WRITE setPropertyLevel
             NOTIFY levelChanged)
};

#endif // DMXVALUE_H
