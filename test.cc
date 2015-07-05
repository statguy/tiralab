/*

Tietorakenteiden harjoitustyö, syksy 2004, Jussi Jousimo
Ohjaaja: Janne Rinta-Mänty

*/

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <csignal>
#include "btree.h"
#include "skiplist.h"
#include "rng.h"

// http://www.parashift.com/c++-faq-lite/containers-and-templates.html#faq-34.12
#include "btree.cc"
#include "skiplist.cc"

using namespace std;

/* Lukee tiedostosta jokaisen rivin vektoriin muuttaen rivin tyypiksi T. */
template<typename T> void readKeys(const char *fileName, vector<T> &keys) {
  ifstream ifs(fileName);
  if (!ifs) {
    cerr << "Could not open input file '" << fileName << "'." << endl;
    raise(SIGABRT);
  }

  int line=1;
  string buffer;
  while (getline(ifs, buffer)) {
    stringstream ss(buffer);
    T key;
    if (!(ss >> key)) {
      cerr << "Invalid item in input file on line " << line << "." << endl;
      raise(SIGABRT);
      return;
    }
    keys.push_back(key);
    line++;
  }

  ifs.close();
}

/* Testaa puun avainten lisäys- ja poisto-operaatioita sekä mittaa
   operaatioihin kuluvan ajan. */
template<typename T> void testBTree(int degree, int iterations,
                                    vector<T> &keys,
                                    int (*compare)(const T &, const T &),
                                    int debug) {
  if (debug<0 || debug>4) {
    cerr << "Invalid debug level." << endl;
    raise(SIGABRT);
    return;
  }

  RandomNumberGenerator random;

  // Luo metsän, johon kaikki puut lisätään.
  BTree<T> **forest=new (BTree<T> *)[iterations];
  for (int i=0; i<iterations; i++)
    forest[i]=new BTree<T>(degree, compare, debug==4 ? 1 : 0);

  if (debug>0) {
    cout << "degree=" << degree << ", iterations=" << iterations
         << ", keys=" << keys.size() << endl;
    cout << "Validating insertions..." << endl;
  }
  else
    cout << degree << "," << iterations << "," << keys.size() << ","
         << flush;

  clock_t start, end;

  start=clock();

  // Lisää jokaiseen puuhun avaimet satunnaisessa järjestyksessä
  for (int i=0; i<iterations; i++) {
    vector<T> validateKeys;
    random_shuffle(keys.begin(), keys.end(), random);
    for (unsigned int j=0; j<keys.size(); j++) {
      forest[i]->insert(keys[j]);

      if (debug==3) {
        cout << "insert(" << keys[j] << ") " << j+1 << "/" << keys.size() << endl;
        forest[i]->printDebug();
        cout << "---" << endl;
      }
      else if (debug==2)
        cout << "insert(" << keys[j] << ") " << j+1 << "/" << keys.size() << endl;
      if (debug>0) {
        validateKeys.push_back(keys[j]);
        forest[i]->validate(validateKeys);
      }
    }

    if (debug==2) {
      forest[i]->print();
      cout << "---" << endl;
    }
  }

  end=clock();

  if (debug>0)
    cout << "Validating removals..." << endl;
  else
    cout << (end-start)/(double)CLOCKS_PER_SEC << "," << flush;

  start=clock();

  // Poistaa jokaisesta puusta avaimet satunnaisessa järjestyksessä
  for (int i=0; i<iterations; i++) {
    vector<T> validateKeys(keys);
    random_shuffle(keys.begin(), keys.end(), random);
    for (unsigned int j=0; j<keys.size(); j++) {
      forest[i]->remove(keys[j]);

      if (debug==3) {
        cout << "remove(" << keys[j] << ") " << j+1 << "/" << keys.size() << endl;
        forest[i]->printDebug();
        cout << "---" << endl;
      }
      else if (debug==2)
        cout << "remove(" << keys[j] << ") " << j+1 << "/" << keys.size() << endl;
      if (debug>0) {
        for (unsigned int k=0; k<validateKeys.size(); k++)
          if (compare(keys[j], validateKeys[k])==0) {
            validateKeys.erase(validateKeys.begin()+k);
            break;
          }
        forest[i]->validate(validateKeys);
      }
    }

    if (debug==2) {
      forest[i]->print();
      cout << "---" << endl;
    }
  }

  end=clock();

  if (debug==0)
    cout << (end-start)/(double)CLOCKS_PER_SEC << endl;

  // Tuhoaa metsän.
  for (int i=0; i<iterations; i++) delete forest[i];
  delete[] forest;
}

template<typename T> void testSkipList(int level, double probability,
                                       const T &lastKey, int iterations,
                                       vector<T> &keys,
                                       int (*compare)(const T &, const T &),
                                       int debug) {
  if (debug<0 || debug>4) {
    cerr << "Invalid debug level." << endl;
    raise(SIGABRT);
    return;
  }

  RandomNumberGenerator random;
  SkipList<T> **forest=new (SkipList<T> *)[iterations];
  for (int i=0; i<iterations; i++)
    forest[i]=new SkipList<T>(level, probability, lastKey, compare,
                              debug==4 ? 1 : 0);

  if (debug>0) {
    cout << "level=" << level << ", probability=" << probability
         << ", lastKey=" << lastKey << ", iterations=" << iterations << endl;
    cout << "Validating insertions..." << endl;
  }
  else
    cout << level << "," << probability << "," << iterations << ","
         << keys.size() << "," << flush;

  clock_t start, end;
  start=clock();

  // Lisää jokaiseen listaan avaimet satunnaisessa järjestyksessä.
  for (int i=0; i<iterations; i++) {
    vector<T> validateKeys;
    random_shuffle(keys.begin(), keys.end(), random);
    for (unsigned int j=0; j<keys.size(); j++) {
      forest[i]->insert(keys[j]);

      if (debug==3) {
        cout << "insert(" << keys[j] << ") " << j+1 << "/" << keys.size() << endl;
        forest[i]->print();
        cout << "---" << endl;
      }
      else if (debug==2)
        cout << "insert(" << keys[j] << ") " << j+1 << "/" << keys.size() << endl;
      if (debug>0) {
        //cout << "insert(" << keys[j] << ")" << endl;
        validateKeys.push_back(keys[j]);
        forest[i]->validate(validateKeys);
      }
    }

    if (debug==2) {
      forest[i]->print();
      cout << "---" << endl;
    }
  }

  end=clock();

  if (debug>0)
    cout << "Validating removals..." << endl;
  else
    cout << (end-start)/(double)CLOCKS_PER_SEC << "," << flush;

  start=clock();

  // Poistaa avaimet listasta satunnaisessa järjestyksessä
  for (int i=0; i<iterations; i++) {
    vector<T> validateKeys(keys);
    random_shuffle(keys.begin(), keys.end(), random);
    for (unsigned int j=0; j<keys.size(); j++) {
      forest[i]->remove(keys[j]);

      if (debug==3) {
        cout << "remove(" << keys[j] << ") " << j+1 << "/" << keys.size() << endl;
        forest[i]->print();
        cout << "---" << endl;
      }
      else if (debug==2)
        cout << "remove(" << keys[j] << ") " << j+1 << "/" << keys.size() << endl;
      if (debug>0) {
        for (unsigned int k=0; k<validateKeys.size(); k++)
          if (compare(keys[j], validateKeys[k])==0) {
            validateKeys.erase(validateKeys.begin()+k);
            break;
          }
        forest[i]->validate(validateKeys);
      }
    }

    if (debug==2) {
      forest[i]->print();
      cout << "---" << endl;
    }
  }

  end=clock();

  if (debug==0)
    cout << (end-start)/(double)CLOCKS_PER_SEC << endl;

  for (int i=0; i<iterations; i++) delete forest[i];
  delete[] forest;
}

/* Vertailee kokonaislukuavaimia. */
inline int intCompare(const int &a, const int &b) {
  if (a<b) return -1;
  if (a>b) return 1;
  return 0;
}

/* Vertailee merkkiavaimia. */
inline int charCompare(const char &a, const char &b) {
  if (a<b) return -1;
  if (a>b) return 1;
  return 0;
}

/*
  btree = testaa b-puuta
  skiplist = testaa hyppylistaa
  selftest

  degree = b-puun aste. oltava >=2
  iterations = luotavien puiden/listojen määrä (=iteraatioiden määrä)
  keys_file = tiedosto, josta avaimet luetaan
  level = hyppylistan maksimitaso
  probability = todennäköisyys, jolla solmujen taso valitaan
  debug_level = 0=ei debug-tulostusta,
                1=tulostaa rakenteet kaikkien avainten lisäysten ja poistojen
                  jälkeen
                2=tulostaa rakenteet jokaisen avaimen lisäyksen ja poiston
                  jälkeen
                3=lausekattavuustulostus
*/
void usage(const char *self) {
  cerr << "Usage: " << self << " btree <degree> <iterations>"
       << " <keys_file> <debug_level>" << endl;
  cerr << "       " << self << " skiplist <level> <probability>"
       << " <iterations> <keys_file> <debug_level>" << endl;
}

int main(int argc, char *argv[]) {
  if (argc<2) {
    cerr << "Invalid arguments." << endl;
    usage(argv[0]);
    return -1;
  }

  string test(argv[1]);
  //transform(test.begin(), test.end(), test.begin(), tolower);

  if (argc==6 && test=="btree") {
    stringstream ss1(argv[2]), ss2(argv[3]), ss3(argv[5]);
    int degree, iterations, debug;
    if (!(ss1 >> degree) || !(ss2 >> iterations) || !(ss3 >> debug)) {
      cerr << "Invalid arguments." << endl;
      usage(argv[0]);
      return -1;
    }

    vector<int> keys;
    readKeys(argv[4], keys);
    testBTree(degree, iterations, keys, &intCompare, debug);
  }
  else if (argc==7 && test=="skiplist") {
    stringstream ss1(argv[2]), ss2(argv[3]), ss3(argv[4]), ss4(argv[6]);
    int level, iterations, debug;
    double probability;
    if (!(ss1 >> level) || !(ss2 >> probability) || !(ss3 >> iterations)
        || !(ss4 >> debug)) {
      cerr << "Invalid arguments." << endl;
      usage(argv[0]);
      return -1;
    }

    vector<int> keys;
    readKeys(argv[5], keys);
    testSkipList(level, probability, 0x7fffffff, iterations, keys,
                 &intCompare, debug);
  }
  else {
    cerr << "Invalid arguments." << endl;
    usage(argv[0]);
    return -1;
  }

  return 0;
}
