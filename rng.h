/*

Tietorakenteiden harjoitustyö, syksy 2004, Jussi Jousimo
Ohjaaja: Janne Rinta-Mänty

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
