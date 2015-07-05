/*

Tietorakenteiden harjoitusty�, syksy 2004, Jussi Jousimo
Ohjaaja: Janne Rinta-M�nty

*/

#ifndef RNG_H
#define RNG_H

class RandomNumberGenerator {
  int seed;
public:
  RandomNumberGenerator();
  RandomNumberGenerator(int seed);
  int operator()(int n) const;
  int getSeed();
};

#endif
