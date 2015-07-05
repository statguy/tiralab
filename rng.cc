/*

Tietorakenteiden harjoitusty�, syksy 2004, Jussi Jousimo
Ohjaaja: Janne Rinta-M�nty

*/

#include <cstdlib>
#include <sys/timeb.h>
#include "rng.h"

using namespace std;

static int count=0;

RandomNumberGenerator::RandomNumberGenerator() {
  // Jos siemen alustetaan usein, se ei v�lt�mm�tt� ehdi p�ivitty�.
  // T�t� varten k�ytet��n siemenen osana my�s tavallista laskuria
  // millisekuntikellon lis�ksi.
  struct timeb t;
  ftime(&t);
  seed=t.time+t.millitm+::count++;
  srand(seed);
}

RandomNumberGenerator::RandomNumberGenerator(int seed) : seed(seed) {
  srand(seed);
}

int RandomNumberGenerator::operator()(int n) const {
  return rand()%n;
}

int RandomNumberGenerator::getSeed() {
  return seed;
}
