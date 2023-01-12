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
