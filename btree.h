/*

Tietorakenteiden harjoitustyˆ, syksy 2004, Jussi Jousimo
Ohjaaja: Janne Rinta-M‰nty

Toteuttaa hyppylistan l‰hteess‰ [1] mainituilla algoritmeilla.

L‰hteet:
[1] Introduction to Algorithms Thomas H. Cormen, Charles E. Leiserson, and
    Ronald L. Rivest. MIT-Press, 2001; Chapter 18, B-Trees.

*/

#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include <vector>

/* B-puun solmun toteuttava luokka, joka sis‰lt‰‰ avaimet ja osoittimet
   lapsisolmuihin sek‰ metodit solmujen k‰sittelyyn. */
template<typename T> class BTreeNode {
  T *key;
  const int degree;
  int keys;
  bool leaf;
  BTreeNode<T> **child;
  const int maxKeys, maxChildren;
  const int debug;

public:
  BTreeNode<T>(int degree, bool leaf, int debug);
  ~BTreeNode<T>();

  /* Palauttaa avainten lukum‰‰r‰n. */
  int numKeys() const;

  /* Palauttaa lasten lukum‰‰r‰n. */
  int numChildren() const;

  /* Palautaan arvon true, jos solmu on lehti. */
  bool isLeaf() const;

  /* Palauttaa avaimen kohdasta index. */
  T getKey(int index) const;

  /* Palauttaa lapsiosoittimen kohdasta index tai NULL, jos solmulla ei ole
     lapsia. */
  BTreeNode<T> *getChild(int index) const;

  /* Asettaa avaimelle uuden arvon. */
  void setKey(T newKey, int index);

  /* Asettaa uuden lapsiosoittimen. */
  void setChild(BTreeNode<T> *newChild, int index);

  /* Asettaa avainten lukum‰‰r‰n. */
  void setNumKeys(int newNumKeys);

  /* Palauttaa solmun ensimm‰isen (pienimm‰n) avaimen. */
  T getFirstKey() const;

  /* Palauttaa solmun viimeisen (suurimman) avaimen. */
  T getLastKey() const;

  /* Palauttaa solmun ensimm‰isen lapsen. */
  BTreeNode<T> *getFirstChild() const;

  /* Palauttaa solmun viimeisen lapsen. */
  BTreeNode<T> *getLastChild() const;

  /* Siirt‰‰ avaimia ja lapsiosoittimia eteenp‰in ja p‰ivitt‰‰ avainten
     lukum‰‰r‰n.
     fromIndex = indeksi, josta alkaen avaimet siirret‰‰n
     count = siirron pituus */
  void shift(int fromIndex, int count);

  /* Lis‰‰ avaimen sek‰ sen vasemman- ja oikeanpuoleiset lapsiosoittimet
     solmuun. Tarvittaessa siirt‰‰ avaimia ja lapsiosoittimia yhdell‰
     eteenp‰in sek‰ p‰ivitt‰‰ avainten lukum‰‰r‰n.
     newKey = uusi avain
     leftChild = vasemmanpuoleinen lapsiosoitin; jos NULL ei muuta nykyist‰
                 osoitinta
     rightChild = oikeanpuoleinen lapsiosoitin: jos NULL ei muuta nykyist‰
                  osoitinta
     index = paikka, johon avain ja lapsiosoittimet lis‰t‰‰n */
  void insert(T newKey, BTreeNode<T> *leftChild, BTreeNode<T> *rightChild,
              int index);

  /* Poistaa avaimen ja mahdolliset lapsiosoittimen ja p‰ivitt‰‰ avainten
     lukum‰‰r‰n. Kumpaakin avainta ei voi poistaa yht‰aikaa.
     keyIndex = avaimen indeksi
     leftChild = jos true, poistaa vasemmanpuolisen lapsiosoittimen
     rightChild = jos true, poistaa oikeanpuoleisen lapsiosoittimen */
  T remove(int index, bool leftChild, bool rightChild);

  /* Kopioi avaimet ja lapsiosoittimet solmusta toiseen. P‰ivitt‰‰ kohdesolmun
     avainten lukum‰‰r‰n, jos viimeinen kopioitava indeksi ylitt‰‰ solmun
     viimeisen k‰ytetyn indeksin.
     fromIndex = l‰hdeindeksi, josta kopioidaan
     count = kopioitavien indeksien m‰‰r‰
     toNode = kohdesolmu
     toIndex = kohdesolmun indeksi, johon kopioidaan */
  void copy(int fromIndex, int count, BTreeNode<T> *toNode, int toIndex);

  /* Tulostaa B-puun solmun sis‰llˆn.
     os = outstream, johon halutaan tulostaa
     node = solmu, joka halutaan tulostaa */
  friend std::ostream &operator<<<T>(std::ostream &os,
                                    const BTreeNode<T> *node);
};

/* B-puun toteuttava luokka, joka sis‰lt‰‰ puun juuren ja puun k‰sittelyyn
   liittyvi‰ metodeja. */
template<typename T> class BTree {
  const int degree;
  BTreeNode<T> *root;
  int (*const compare)(const T &, const T &);
  int numDepth, numNodes, numKeys;
  const int debug;

protected:
  /* Tuhoaa alipuun.
     branch = tuhottava alipuu */
  void destroyBranch(BTreeNode<T> *branch);

  /* Tulostaa avaimet esij‰rjestyksess‰.
     node = alipuu, jonka avaimet tulostetaan
     depth = rekursiivisesti laskettava alipuun korkeus */
  void printPreorder(BTreeNode<T> *node, int depth);

  /* Tulostaa avaimet sis‰j‰rjestyksess‰.
     node = alipuu, jonka avaimet tulostetaan
     depth = rekursiivisesti laskettava alipuun korkeus */
  void printInorder(BTreeNode<T> *node, int depth);

  /* Etsii avaimen alipuusta.
     key = etsitt‰v‰ avain
     result = solmu, jossa lˆydetty avain on tai NULL, jos avainta ei
              lˆytynyt
     index = lˆydetyn avaimen indeksi
     node = alipuu, josta avainta etsit‰‰n */
  void searchBranch(const T &key, BTreeNode<T> **result, int *index,
                    BTreeNode<T> *node);

  /* Tarkistaa, ett‰ puu t‰ytt‰‰ B-puun m‰‰ritelm‰n.
     node = tarkistettava alipuu
     depth = rekursiivisesti laskettava puun korkeus */
  void validateBranch(BTreeNode<T> *node, int depth,
                      const std::vector<T> &keys,
                      std::vector<bool> &checked);

  /* Jakaa solmun kahteen solmuun, jotta uusi avain voidaan lis‰t‰. [1]
     parent = is‰solmu, jonka lapsisolmu jaetaan
     medianKey = keskimm‰isen avaimen paikka is‰solmussa
     left = solmu, joka jaetaan ja josta tulee vasemmanpuoleinen sisar */
  void splitChild(BTreeNode<T> *parent, int medianKey, BTreeNode<T> *left);

  /* Lis‰‰ avaimen vaillinaiseen solmuun. [1]
     node = alipuu, johon avain tulee
     key = avain */
  void insertNonfull(BTreeNode<T> *node, T key);

  /* Poistaa ja palauttaa edellisen avaimen. Argumenttina on annettava se
     lapsisolmu, joka edelt‰‰ avainta, jonka edelt‰j‰avain halutaan poistaa.
     branch = alipuu, josta avain poistetaan */
  T removePredecessorKey(BTreeNode<T> *branch);

  /* Poistaa ja palauttaa seuraavan avaimen. Argumenttina on annettava se
     lapsisolmu, joka seuraa avainta, jonka seuraaja-avain halutaan poistaa.
     branch = alipuu, josta avain poistetaan */
  T removeSuccessorKey(BTreeNode<T> *branch);

  /* Lainaa oikeanpuoleiselta sisarsolmulta avaimen siirt‰en sen is‰solmuun
     ja pudottaa is‰solmusta avaimen lapsisolmuun.
     parent = is‰solmu
     index = lapsisolmun indeksi */
  void rotateRight(BTreeNode<T> *parent, int index);

  /* Lainaa vasemmanpuoleiselta sisarsolmulta avaimen siirt‰en sen is‰solmuun
     ja pudottaa is‰solmusta avaimen lapsisolmuun.
     parent = is‰solmu
     index = lapsisolmun indeksi */
  void rotateLeft(BTreeNode<T> *parent, int index);

  /* Yhdist‰‰ kaksi solmua, jotta olisi mahdollista tuhota avain yhdistyn
     solmun alapuolelta. Palauttaa is‰solmun tai yhdistetyn solmun, jos
     is‰solmu tuhotaan.
     parent = is‰solmu, jonka kaksi lapsisolmua yhdistet‰‰n
     mergeIndex = lapsisolmun, johon yhdistet‰‰n sisarsolmu, indeksi */
  BTreeNode<T> *mergeChildren(BTreeNode<T> *parent, int mergeIndex);

  /* Poistaa avaimen alipuusta.
     key = poistettava avain
     branch = alipuu, josta avain poistetaan */
  void removeBranch(const T &key, BTreeNode<T> *branch);

public:
  /* Luo puun.
     degree = puun aste; oltava >= 2; m‰‰r‰‰ avainten m‰‰r‰n solmuissa;
              minimiss‰‰n t-1 ja maksimissaan 2*t-1
     compare = metodi avainten vertailemiseksi */
  BTree<T>(int degree, int (*const compare)(const T &, const T &), int debug);

  ~BTree<T>();

  /* Tulostaa puun avaimet nousevassa j‰rjestykses‰. */
  void print();

  /* Tulostaa puun avaimet esij‰rjestyksess‰. */
  void printDebug();

  /* Etsii avaimen puusta.
     key = etsitt‰v‰ avain
     result = osoitin solmuun, jossa lˆydetty avain on tai NULL jos avainta
              ei lˆytynyt
     index = avaimen indeksi */
  void search(const T &key, BTreeNode<T> **result, int *index);

  /* Tarkistaa, ett‰ puu t‰ytt‰‰ B-puun m‰‰ritelm‰n. */
  void validate(const std::vector<T> &keys);

  /* Tarkistaa, ett‰ puu t‰ytt‰‰ B-puun m‰‰ritelm‰n ja tulostaa
     lis‰tietoja. */
  void printValidate(const std::vector<T> &keys);

  /* Lis‰‰ avaimen puuhun.
     key = lis‰tt‰va avain */
  void insert(T key);

  /* Poistaa avaimen puusta.
     key = poistettava avain */
  void remove(const T &key);
};

#endif
