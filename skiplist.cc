/*

Tietorakenteiden harjoitustyö, syksy 2004, Jussi Jousimo
Ohjaaja: Janne Rinta-Mänty

Toteuttaa hyppylistan lähteessä [1] mainituilla algoritmeilla.

Lähteet:
[1] William Pugh. Skip Lists: Skip lists: A probabilistic alternative to
    balanced trees. Communications of the ACM, 33(6):668--676, June 1990,
    ftp://ftp.cs.umd.edu/pub/skipLists/skiplists.pdf.

*/

#include <iostream>
#include <csignal>
#include <vector>
#include "skiplist.h"
#include "rng.h"

using namespace std;

/* footer = listan päättösolmu
   level = solmun taso, 1 <= level <= maxLevel
   key = avain
   debug = 1=lausekattavuustulostus */
template<typename T> SkipListNode<T>::SkipListNode<T>(SkipListNode<T> *footer,
                                                      int level,
                                                      const T key,
                                                      int debug=0) :
  level(level), key(key), debug(debug) {
  if (level<1) {
    cerr << "SkipListNode<T>(): Invalid level." << endl;
    raise(SIGABRT);
    return;
  }
  forward=new SkipListNode<T> *[level];
  for (int i=0; i<level; i++) forward[i]=footer;
}

template<typename T> SkipListNode<T>::~SkipListNode<T>() {
  delete[] forward;
}

/* Palauttaa solmun avaimen. */
template<typename T> const T &SkipListNode<T>::getKey() { return key; }

/* Palauttaa solmun tason. */
template<typename T> int SkipListNode<T>::getLevel() { return level; }

/* Palauttaa seuraajaosoittimen.
   index = seuraajaosoittimen taso */
template<typename T> SkipListNode<T> *SkipListNode<T>::getForward(int index) {
  if (index<0 || index>level-1) {
    cerr << "getForward(): Invalid index." << endl;
    raise(SIGABRT);
    return NULL;
  }
  return forward[index];
}

/* Asettaa seuraajaosoittimen.
     index = seuraajaosoittimen taso
     node = seuraajaosoitin */
template<typename T> void SkipListNode<T>::setForward(int index,
                                                      SkipListNode<T> *node) {
  if (index<0 || index>level-1) {
    cerr << "setForward(): Invalid index." << endl;
    raise(SIGABRT);
    return;
  }
  forward[index]=node;
}

/* Tulostaa solmun tiedot. */
template<typename T> ostream &operator<<(ostream &os,
                                         SkipListNode<T> *node) {
  os << "address=" << reinterpret_cast<const void *>(node);
  os << ", key=" << node->key;
  os << ", level=" << node->level;
  os << ", forward=";
  for (int i=0; i<node->level-1; i++)
    cout << reinterpret_cast<const void *>(node->forward[i]) << " ";
  if (node->level>0)
    cout << reinterpret_cast<const void *>(node->forward[node->level-1]);
  return os;
}

/* Palauttaa satunnaisen tason uudelle solmulle. */
template<typename T> int SkipList<T>::randomLevel() {
  int lvl=1;
  while (double(random(100))/100<p && lvl<maxLevel) lvl++;
  return lvl;
}

/* maxLevel = listan solmujen maksimitaso
   p = todennäköisyys, jonka mukaan solmujen taso valitaan
   lastKey = suurin avain, jonka tulee olla suurempi kuin mikään listaan
             lisättävä avain
   compare = avainten vertailufunktio
   debug = 1=lausekattavuustulostus
   maxLevel voidaan lähteen [1] mukaan määritellä sopivaksi
   yhtälön 2^maxLevel = avainten_lkm eli
   maxLevel = log(avainten_lkm)/log(2) avulla. */
template<typename T> SkipList<T>::SkipList<T>(int maxLevel, double p,
                                              const T &lastKey,
  int (*const compare)(const T &, const T &),
                                              int debug=0) :
  maxLevel(maxLevel), p(p), lastKey(lastKey), compare(compare), level(1),
  debug(debug) {
  if (maxLevel<1 || p<0 || p>1) {
    cerr << "Level must be >= 1 and probability must be between 0 and 1."
         << endl;
    raise(SIGABRT);
    return;
  }
  footer=new SkipListNode<T>(NULL, maxLevel, lastKey);
  header=new SkipListNode<T>(footer, maxLevel, lastKey);
}

/* Tuhoaa listan. */
template<typename T> SkipList<T>::~SkipList<T>() {
  SkipListNode<T> *node=header;
  while (node!=NULL) {
    SkipListNode<T> *tmp=node->getForward(0);
    delete node;
    node=tmp;
  }
}

/* Etsii avaimen listasta ja palauttaa osoittimen avaimen solmuun. */
template<typename T> SkipListNode<T> *SkipList<T>::search(const T &key) {
  SkipListNode<T> *node=header;
  for (int i=level-1; i>=0; i--)
    while (compare(node->getForward(i)->getKey(), key)<0)
      node=node->getForward(i);
  node=node->getForward(0);
  if (compare(node->getKey(), key)==0) return node;
  else return NULL;
}

/* Lisää avaimen listaan. */
template<typename T> void SkipList<T>::insert(const T &key) {
  if (compare(key, lastKey)==0) {
    cerr << "Cannot insert the last key." << endl;
    raise(SIGABRT);
    return;
  }

  if (search(key)!=NULL) {
    cerr << "Insertion of multiple same keys unsupported." << endl;
    raise(SIGABRT);
    return;
  }

  SkipListNode<T> **update=new SkipListNode<T> *[maxLevel];
  SkipListNode<T> *node=header;

  // Etsitään paikka avaimelle aloittamalla ylimmältä tasolta
  // etummaisesta solmusta.
  for (int i=level-1; i>=0; i--) {
    while (compare(node->getForward(i)->getKey(), key)<0) {
      if (debug==1) cout << "insert(): 1" << endl;
      node=node->getForward(i);
    }

    // Tallennetaan kaikki ne solmut tasoittain, joiden kautta kuljetaan.
    update[i]=node;
  }

  int lvl=randomLevel();

  // Jos uuden solmun taso ylittää listan tason, korotetaan listan taso
  // samaksi.
  if (lvl>level) {
    for (int i=level; i<lvl; i++) {
      if (debug==1) cout << "insert(): 2" << endl;
      update[i]=header;
    }
    level=lvl;
  }

  node=new SkipListNode<T>(footer, lvl, key);
  // Asetetaan uuden solmun osoittimet.
  for (int i=0; i<lvl; i++) {
    if (debug==1) cout << "insert(): 3" << endl;
    node->setForward(i, update[i]->getForward(i));
    update[i]->setForward(i, node);
  }
}

/* Poistaa avaimen listasta. */
template<typename T> void SkipList<T>::remove(const T &key) {
  if (compare(key, lastKey)==0) return;

  SkipListNode<T> **update=new SkipListNode<T> *[maxLevel];
  SkipListNode<T> *node=header;

  // Etsitään solmu, jossa avain sijaitsee.
  for (int i=level-1; i>=0; i--) {
    while (compare(node->getForward(i)->getKey(), key)<0) {
      if (debug==1) cout << "remove(): 1" << endl;
      node=node->getForward(i);
    }
    // Tallennetaan kaikki ne solmut tasoittain, joiden kautta kuljetaan.
    update[i]=node;
  }

  node=node->getForward(0);
  if (compare(node->getKey(), key)==0) {
    for (int i=0; i<level; i++) {
      if (update[i]->getForward(i)!=node) {
        // Reitillä ollut solmu ei viittaa poistettavaan solmuun, joten
        // sitä ei tarvitse käsitellä eikä loppujakaan.
        if (debug==1) cout << "remove(): 2" << endl;
        break;
      }
      if (debug==1) cout << "remove(): 3" << endl;

      // Asetetaan reitillä ollut solmu osoittamaan ohi poistettavan solmun.
      update[i]->setForward(i, node->getForward(i));
    }
    delete node;

    // Asetetaan listan tason vastaamaan solmujen korkeinta tasoa.
    while (level>1 && header->getForward(level-1)==footer) {
      if (debug==1) cout << "remove(): 4" << endl;
      level--;
    }
  }
}

/* Tarkistaa, että lista täyttää hyppylistan vaatimukset. */
template<typename T> void SkipList<T>::validate(const vector<T> &keys) {
  vector<bool> checked(keys.size(), false);

  SkipListNode<T> *node=header;
  while (node!=NULL) {

    // Merkitään avain, jos se on listassa.
    for (unsigned int i=0; i<keys.size(); i++)
      if (compare(node->getKey(), keys[i])==0) {
        checked[i]=true;
        break;
      }

    // Tarkistetaan, että yhdenkään solmun taso ei ylitä listan tasoa.
    if (node!=header && node!=footer && node->getLevel()>level) {
      cerr << "VALIDATE: Invalid level in the node." << endl;
      raise(SIGABRT);
      return;
    }

    // Tarkistetaan, että seuraajasolmujen avaimet ovat suurempia kuin tämän
    // solmun.
    if (node!=header && node!=footer)
      for (int i=node->getLevel()-1; i>=0; i--)
        if (compare(node->getForward(i)->getKey(), node->getKey())<0) {
          cerr << "VALIDATE: Nodes not in order." << endl;
          raise(SIGABRT);
          return;
        }

    // Tarkistetaan, että ylemmän tason seuraajasolmun avain on suurempi kuin
    // alemman tason seuraajasolmun.
    if (node!=footer)
      for (int i=node->getLevel()-1; i>0; i--)
        if (compare(node->getForward(i)->getKey(),
                    node->getForward(i-1)->getKey())<0) {
          cerr << "VALIDATE: Forward pointers not in order." << endl;
          raise(SIGABRT);
          return;
        }

    node=node->getForward(0);
  }

  // Tarkistetaan, että kaikki avaimet on merkitty ja näin ollen listassa.
  for (unsigned int i=0; i<checked.size(); i++)
    if (checked[i]==false) {
      cerr << "VALIDATE: Missing key." << endl;
      raise(SIGABRT);
      return;
    }
}

/* Tulostaa listan nousevassa avainjärjestyksessä. */
template<typename T> void SkipList<T>::print() {
  cout << "list level=" << level << endl;
  SkipListNode<T> *node=header;
  while (node!=NULL) {
    cout << node << endl;
    node=node->getForward(0);
  }
}
