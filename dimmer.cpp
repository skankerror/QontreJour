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

