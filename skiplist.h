/*

Tietorakenteiden harjoitusty�, syksy 2004, Jussi Jousimo
Ohjaaja: Janne Rinta-M�nty

Toteuttaa hyppylistan l�hteess� [1] mainituilla algoritmeilla.

L�hteet:
[1] William Pugh. Skip Lists: Skip lists: A probabilistic alternative to
    balanced trees. Communications of the ACM, 33(6):668--676, June 1990,
    ftp://ftp.cs.umd.edu/pub/skipLists/skiplists.pdf.

*/

#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <iostream>
#include <vector>
#include "rng.h"

/* Hyppylistan solmun toteuttava luokka. */
template<typename T> class SkipListNode {
  int level;
  T key;
  SkipListNode<T> **forward;
  const int debug;

public:
  /* footer = listan p��tt�solmu
     level = solmun taso, 1 <= level <= maxLevel
     key = avain
     debug = 1=lausekattavuustulostus */
  SkipListNode<T>(SkipListNode<T> *footer, int level, const T key, int debug);
  ~SkipListNode<T>();

  /* Palauttaa solmun avaimen. */
  const T &getKey();

  /* Palauttaa solmun tason. */
  int getLevel();

  /* Palauttaa seuraajaosoittimen.
     index = seuraajaosoittimen taso */
  SkipListNode<T> *getForward(int index);

  /* Asettaa seuraajaosoittimen.
     index = seuraajaosoittimen taso
     node = seuraajaosoitin */
  void setForward(int index, SkipListNode<T> *node);

  /* Tulostaa solmun tiedot. */
  friend std::ostream &operator<<<T>(std::ostream &os, SkipListNode<T> *node);
};

/* Hyppylistan toteuttava luokka. */
template<typename T> class SkipList {
  const int maxLevel;
  const double p;
  const int lastKey;
  int (*const compare)(const T &, const T &);
  SkipListNode<T> *header, *footer;
  int level;
  RandomNumberGenerator random;
  const int debug;

protected:
  /* Palauttaa satunnaisen tason uudelle solmulle. */
  int randomLevel();

public:
  /* maxLevel = listan solmujen maksimitaso
     p = todenn�k�isyys, jonka mukaan solmujen taso valitaan
     lastKey = suurin avain, jonka tulee olla suurempi kuin mik��n listaan
               lis�tt�v� avain
     compare = avainten vertailufunktio
     debug = 1=lausekattavuustulostus
     maxLevel voidaan l�hteen [1] mukaan m��ritell� sopivaksi
     yht�l�n 2^maxLevel = avainten_lkm eli
     maxLevel = log(avainten_lkm)/log(2) avulla. */
  SkipList<T>(int maxLevel, double p, const T &lastKey,
              int (*const compare)(const T &, const T &), int debug);

  /* Tuhoaa listan. */
  ~SkipList<T>();

  /* Etsii avaimen listasta ja palauttaa osoittimen avaimen solmuun. */
  SkipListNode<T> *search(const T &key);

  /* Lis�� avaimen listaan. */
  void insert(const T &key);

  /* Poistaa avaimen listasta. */
  void remove(const T &key);

  /* Tarkistaa, ett� lista t�ytt�� hyppylistan vaatimukset.
     keys = avaimet, jotka pit�isi olla listassa */
  void validate(const std::vector<T> &keys);

  /* Tulostaa listan nousevassa avainj�rjestyksess�. */
  void print();
};

#endif
