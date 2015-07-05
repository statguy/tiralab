/*

Tietorakenteiden harjoitustyˆ, syksy 2004, Jussi Jousimo
Ohjaaja: Janne Rinta-M‰nty

Toteuttaa B-puun l‰hteess‰ [1] mainituilla algoritmeilla.

L‰hteet:
[1] Introduction to Algorithms Thomas H. Cormen, Charles E. Leiserson, and
    Ronald L. Rivest. MIT-Press, 2001; Chapter 18, B-Trees.

*/

#include <iostream>
#include <csignal>
#include <vector>
#include "btree.h"
#include "rng.h"

using namespace std;

/* Luo B-puun solmun.
   degree = puun aste
   leaf = true=solmu on lehti
   debug = 1=lausekattavuustulostus */
template<typename T> BTreeNode<T>::BTreeNode<T>(int degree, bool leaf,
                                                int debug=0) :
  degree(degree), keys(0), leaf(leaf), maxKeys(2*degree-1),
  maxChildren(2*degree), debug(debug) {
  if (degree<2) {
    cerr << "BTreeNode(): degree must be >= 2." << endl;
    raise(SIGABRT);
    return;
  }
  key=new T[maxKeys];
  child=new BTreeNode<T> *[maxChildren];
  for (int i=0; i<maxChildren; i++) child[i]=NULL;
}

template<typename T> BTreeNode<T>::~BTreeNode<T>() {
  //if (maxKeys>0) {
  //  cerr << "~BTreeNode(): node is not empty." << endl;
  //  raise(SIGABRT);
  //  return;
  //}
  delete[] child;
  delete[] key;
}

/* Palauttaa avainten lukum‰‰r‰n. */
template<typename T> int BTreeNode<T>::numKeys() const {
  return keys;
}

/* Palauttaa lasten lukum‰‰r‰n. */
template<typename T> int BTreeNode<T>::numChildren() const {
  return keys==0 ? 0 : keys+1;
}

/* Palautaan arvon true, jos solmu on lehti. */
template<typename T> bool BTreeNode<T>::isLeaf() const {
  return leaf;
}

/* Palauttaa avaimen kohdasta index. */
template<typename T> T BTreeNode<T>::getKey(int index) const {
  if (index<0 || index>=numKeys()) {
    cerr << "getKey(): Invalid key index." << endl;
    raise(SIGABRT);
    return key[0];
  }
  return key[index];
}

/* Palauttaa lapsiosoittimen kohdasta index tai NULL, jos solmulla ei ole
   lapsia. */
template<typename T> BTreeNode<T> *BTreeNode<T>::getChild(int index) const {
  if (index<0 || index>=numChildren()) {
    cerr << "getChild(): Invalid child index." << endl;
    raise(SIGABRT);
    return NULL;
  }
  if (isLeaf()) return NULL;
  if (!child[index]) {
    cerr << "getChild(): Invalid child." << endl;
    raise(SIGABRT);
    return NULL;
  }
  return child[index];
}

/* Asettaa avaimelle uuden arvon. */
template<typename T> void BTreeNode<T>::setKey(T newKey, int index) {
  if (index<0 || index>=maxKeys) {
    cerr << "setKey(): Invalid key index." << endl;
    raise(SIGABRT);
    return;
  }
  key[index]=newKey;
}

/* Asettaa uuden lapsiosoittimen. */
template<typename T> void BTreeNode<T>::setChild(BTreeNode<T> *newChild,
                                                 int index) {
  if (index<0 || index>=maxChildren) {
    cerr << "setChild(): Invalid child index." << endl;
    raise(SIGABRT);
    return;
  }
  child[index]=newChild;
}

/* Asettaa avainten lukum‰‰r‰n. */
template<typename T> void BTreeNode<T>::setNumKeys(int newNumKeys) {
  if (newNumKeys<0 || newNumKeys>maxKeys) {
    cerr << "setNumKeys(): Invalid number of keys." << endl;
    raise(SIGABRT);
    return;
  }
  keys=newNumKeys;
}

/* Palauttaa solmun ensimm‰isen (pienimm‰n) avaimen. */
template<typename T> T BTreeNode<T>::getFirstKey() const {
  return getKey(0);
}

/* Palauttaa solmun viimeisen (suurimman) avaimen. */
template<typename T> T BTreeNode<T>::getLastKey() const {
  return getKey(numKeys()-1);
}

/* Palauttaa solmun ensimm‰isen lapsen. */
template<typename T> BTreeNode<T> *BTreeNode<T>::getFirstChild() const {
  return getChild(0);
}

/* Palauttaa solmun viimeisen lapsen. */
template<typename T> BTreeNode<T> *BTreeNode<T>::getLastChild() const {
  return getChild(numChildren()-1);
}

/* Siirt‰‰ avaimia ja lapsiosoittimia eteenp‰in ja p‰ivitt‰‰ avainten
   lukum‰‰r‰n.
   fromIndex = indeksi, josta alkaen avaimet siirret‰‰n
   count = siirron pituus */
template<typename T> void BTreeNode<T>::shift(int fromIndex, int count) {
  for (int i=numKeys()-1; i>=fromIndex; i--) {
    setKey(getKey(i), i+count);
  }
  if (!isLeaf()) {
    for (int i=numKeys(); i>=fromIndex; i--) {
      if (debug==1) cout << "shift(): 1" << endl;
      setChild(getChild(i), i+count);
    }
  }
  setNumKeys(numKeys()+count);
}

/* Lis‰‰ avaimen sek‰ sen vasemman- ja oikeanpuoleiset lapsiosoittimet
   solmuun. Tarvittaessa siirt‰‰ avaimia ja lapsiosoittimia yhdell‰
   eteenp‰in sek‰ p‰ivitt‰‰ avainten lukum‰‰r‰n.
   newKey = uusi avain
   leftChild = vasemmanpuoleinen lapsiosoitin; jos NULL ei muuta nykyist‰
   osoitinta
   rightChild = oikeanpuoleinen lapsiosoitin: jos NULL ei muuta nykyist‰
   osoitinta
   index = paikka, johon avain ja lapsiosoittimet lis‰t‰‰n */
template<typename T> void BTreeNode<T>::insert(T newKey,
                                               BTreeNode<T> *leftChild,
                                               BTreeNode<T> *rightChild,
                                               int index) {
  if (index<numKeys()) {
    if (debug==1) cout << "insert(): 1" << endl;
    shift(index, 1);
  }
  else {
    if (debug==1) cout << "insert(): 2" << endl;
    setNumKeys(numKeys()+1);
  }

  setKey(newKey, index);
  if (leftChild) {
    if (debug==1) cout << "insert(): 3" << endl;
    setChild(leftChild, index);
  }
  if (rightChild) {
    if (debug==1) cout << "insert(): 4" << endl;
    setChild(rightChild, index+1);
  }
}

/* Poistaa avaimen ja mahdolliset lapsiosoittimen ja p‰ivitt‰‰ avainten
   lukum‰‰r‰n. Kumpaakin avainta ei voi poistaa yht‰aikaa.
   keyIndex = avaimen indeksi
   leftChild = jos true, poistaa vasemmanpuolisen lapsiosoittimen
   rightChild = jos true, poistaa oikeanpuoleisen lapsiosoittimen */
template<typename T> T BTreeNode<T>::remove(int index, bool leftChild,
                                            bool rightChild) {
  if (leftChild==true && rightChild==true) {
    cerr << "Can't remove both children." << endl;
    raise(SIGABRT);
    return key[0];
  }

  T key=getKey(index);
  for (int i=index; i<numKeys()-1; i++) {
    if (debug==1) cout << "remove(): 1" << endl;
    setKey(getKey(i+1), i);
  }
  if (!isLeaf()) {
    if (leftChild==true) {
      for (int i=index; i<numChildren()-1; i++) {
        if (debug==1) cout << "remove(): 2" << endl;
        setChild(getChild(i+1), i);
      }
    }
    else if (rightChild==true) {
      for (int i=index+1; i<numChildren()-1; i++) {
        if (debug==1) cout << "remove(): 3" << endl;
        setChild(getChild(i+1), i);
      }
    }
  }
  setNumKeys(numKeys()-1);
  return key;
}

/* Kopioi avaimet ja lapsiosoittimet solmusta toiseen. P‰ivitt‰‰ kohdesolmun
   avainten lukum‰‰r‰n, jos viimeinen kopioitava indeksi ylitt‰‰ solmun
   viimeisen k‰ytetyn indeksin.
   fromIndex = l‰hdeindeksi, josta kopioidaan
   count = kopioitavien indeksien m‰‰r‰
   toNode = kohdesolmu
   toIndex = kohdesolmun indeksi, johon kopioidaan */
template<typename T> void BTreeNode<T>::copy(int fromIndex, int count,
                                             BTreeNode<T> *toNode,
                                             int toIndex) {
  for (int i=0; i<count; i++) {
    if (debug==1) cout << "copy(): 1" << endl;
    toNode->setKey(getKey(fromIndex+i), toIndex+i);
  }
  if (!isLeaf()) {
    for (int i=0; i<count+1; i++) {
      if (debug==1) cout << "copy(): 2" << endl;
      toNode->setChild(getChild(fromIndex+i), toIndex+i);
    }
  }

  if (toIndex+count>toNode->numKeys()) {
    if (debug==1) cout << "copy(): 3" << endl;
    toNode->setNumKeys(toIndex+count);
  }
}

/* Tulostaa B-puun solmun sis‰llˆn.
   os = outstream, johon halutaan tulostaa
   node = solmu, joka halutaan tulostaa */
template<typename T> ostream &operator<<(ostream &os,
                                         const BTreeNode<T> *node) {
  if (node) {
    os << "address=" << reinterpret_cast<const void *>(node);
    os << ", leaf=" << node->leaf << ", keys=";
    if (node) {
      for (int i=0; i<node->keys-1; i++) os << node->key[i] << " ";
      if (node->keys>0) os << node->key[node->keys-1];
      os << ", children=";

      if (!node->leaf) {
        for (int i=0; i<node->keys; i++)
          if (node->child[i])
            os << reinterpret_cast<const void *>(node->child[i]) << " ";
          else os << "NULL ";
        if (node->keys>0)
          if (node->child[node->keys])
            os << reinterpret_cast<const void *>(node->child[node->keys]);
          else os << "NULL";
      }
    }
  }
  else os << "address=NULL";
  return os;
}

/* Tuhoaa alipuun.
   branch = tuhottava alipuu */
template<typename T> void BTree<T>::destroyBranch(BTreeNode<T> *branch) {
  if (branch) {
    if (!branch->isLeaf())
      for (int i=0; i<branch->numChildren(); i++)
        destroyBranch(branch->getChild(i));
    delete branch;
  }
}

/* Tulostaa avaimet esij‰rjestyksess‰.
   node = alipuu, jonka avaimet tulostetaan
   depth = rekursiivisesti laskettava alipuun korkeus */
template<typename T> void BTree<T>::printPreorder(BTreeNode<T> *node,
                                                  int depth) {
  if (node) {
    cout << "depth=" << depth << ", " << node << endl;
    if (!node->isLeaf())
      for (int i=0; i<node->numChildren(); i++)
        printPreorder(node->getChild(i), depth+1);
  }
}

/* Tulostaa avaimet sis‰j‰rjestyksess‰.
   node = alipuu, jonka avaimet tulostetaan
   depth = rekursiivisesti laskettava alipuun korkeus */
template<typename T> void BTree<T>::printInorder(BTreeNode<T> *node,
                                                 int depth) {
  if (node) {
    if (!node->isLeaf()) printInorder(node->getFirstChild(), depth+1);
    for (int i=1; i<node->numChildren(); i++) {
      cout << "depth=" << depth << ", key=" << node->getKey(i-1)
           << ", " << node << endl;
      if (!node->isLeaf())
        printInorder(node->getChild(i), depth+1);
    }
  }
}

/* Etsii avaimen alipuusta.
   key = etsitt‰v‰ avain
   result = solmu, jossa lˆydetty avain on tai NULL, jos avainta ei
   lˆytynyt
   index = lˆydetyn avaimen indeksi
   node = alipuu, josta avainta etsit‰‰n */
template<typename T> void BTree<T>::searchBranch(const T &key,
                                                 BTreeNode<T> **result,
                                                 int *index,
                                                 BTreeNode<T> *node) {
  if (node) {
    int i=0;
    while (i<node->numKeys() && compare(key, node->getKey(i))>0) i++;
    if (i<node->numKeys() && compare(key, node->getKey(i))==0) {
      *result=node;
      *index=i;
      return;
    }
    if (!node->isLeaf())
      searchBranch(key, result, index, node->getChild(i));
  }
}

/* Tarkistaa, ett‰ puu t‰ytt‰‰ B-puun m‰‰ritelm‰n.
   node = tarkistettava alipuu
   depth = rekursiivisesti laskettava puun korkeus */
template<typename T> void BTree<T>::validateBranch(BTreeNode<T> *node,
                                                   int depth,
                                                   const vector<T> &keys,
                                                   vector<bool> &checked) {
  if (node) {
    // Merkit‰‰n avain, jos se on puussa.
    for (int i=0; i<node->numKeys(); i++)
      for (unsigned int j=0; j<keys.size(); j++)
        if (compare(node->getKey(i), keys[j])==0) {
          checked[j]=true;
          break;
        }

    if (depth>numDepth) numDepth=depth;
    numNodes++;

    // Tarkistetetaan, ett‰ solmussa on tarpeeksi avaimia.
    if (depth>0) {
      if (node->numKeys()<degree-1) {
        cerr << "VALIDATE: Not enough keys." << endl;
        raise(SIGABRT);
        return;
      }
      if (node->numKeys()>2*degree-1) {
        cerr << "VALIDATE: Too many keys." << endl;
        raise(SIGABRT);
        return;
      }
    }

    numKeys+=node->numKeys();

    // Tarkistetaan, ett‰ avaimet ovat suuruusj‰rjestyksess‰ pienimm‰st‰
    // suurimpaan.
    for (int i=0; i<node->numKeys()-1; i++)
      if (compare(node->getKey(i+1), node->getKey(i))<0) {
        cerr << "VALIDATE: Keys not in order." << endl;
        raise(SIGABRT);
        return;
      }

    if (!node->isLeaf()) {
      // Tarkistetaan, ett‰ solmu ei sis‰ll‰ useita samoja lapsia.
      for (int i=0; i<node->numChildren(); i++)
        for (int j=0; j<node->numChildren(); j++)
          if (i!=j && node->getChild(i)==node->getChild(j)) {
            cerr << "VALIDATE: Multiple same children." << endl;
            raise(SIGABRT);
            return;
          }

      // Tarkistetaan, ett‰ lasten avaimet ovat suuruusj‰rjestyksess‰ solmun
      // avaimiin verrattuna.
      for (int i=0; i<node->numKeys(); i++) {
        if (compare(node->getKey(i),
                    node->getChild(i)->getLastKey())<=0) {
          cerr << "VALIDATE: Left child key not in order." << endl;
          raise(SIGABRT);
          return;
        }
        if (compare(node->getKey(i),
                    node->getChild(i+1)->getFirstKey())>=0) {
          cerr << "VALIDATE: Right child key not in order." << endl;
          raise(SIGABRT);
          return;
        }
      }
    }

    if (!node->isLeaf())
      for (int i=0; i<node->numChildren(); i++)
        if (node->getChild(i))
          validateBranch(node->getChild(i), depth+1, keys, checked);
        else {
          cerr << "VALIDATE: No child." << endl;
          raise(SIGABRT);
          return;
        }
  }
  else {
    cerr << "VALIDATE: Invalid branch." << endl;
    raise(SIGABRT);
    return;
  }
}

/* Jakaa solmun kahteen solmuun, jotta uusi avain voidaan lis‰t‰. [1]
   parent = is‰solmu, jonka lapsisolmu jaetaan
   medianKey = keskimm‰isen avaimen paikka is‰solmussa
   left = solmu, joka jaetaan ja josta tulee vasemmanpuoleinen sisar */
template<typename T> void BTree<T>::splitChild(BTreeNode<T> *parent,
                                               int medianKey,
                                               BTreeNode<T> *left) {
  if (parent==NULL || left==NULL) {
    cerr << "splitChild(): Invalid argument." << endl;
    raise(SIGABRT);
    return;
  }

  // Luodaan uusi solmu, joka tulee vasemmanpuoleisen solmun sisareksi.
  BTreeNode<T> *right=new BTreeNode<T>(degree, left->isLeaf(), debug);

  // Jaetaan vasemmanpuoleinen solmu kahteen yht‰ suureen osaan kopioimalla
  // oikea puoli sisarsolmuun.
  left->copy(degree, degree-1, right, 0);
  left->setNumKeys(degree);

  // Siirret‰‰n keskimm‰inen alkio is‰solmuun ja asetetaan oikeanpuoleinen
  // solmu is‰solmun lapseksi.
  parent->insert(left->getKey(degree-1), NULL, right, medianKey);
  left->remove(degree-1, false, false);
}

/* Lis‰‰ avaimen vaillinaiseen solmuun. [1]
   node = alipuu, johon avain tulee
   key = avain */
template<typename T> void BTree<T>::insertNonfull(BTreeNode<T> *node, T key) {
  if (node==NULL) {
    cerr << "insertNonfull(): Invalid argument." << endl;
    raise(SIGABRT);
    return;
  }

  int i=node->numKeys()-1;
  if (node->isLeaf()) {
    if (debug==1) cout << "insertNonfull(): 1" << endl;
    // Etsint‰ on p‰‰ttynyt lehteen; lis‰t‰‰n avain oikeaan kohtaan.
    while (i>=0 && compare(key, node->getKey(i))<0) {
      if (debug==1) cout << "insertNonfull(): 2" << endl;
      i--;
    }
    node->insert(key, NULL, NULL, i+1);
  }
  else {
    if (debug==1) cout << "insertNonfull(): 3" << endl;

    // Etsit‰‰n rekursiivisesti lehti, johon avain lis‰t‰‰n.
    while (i>=0 && compare(key, node->getKey(i))<0) {
      if (debug==1) cout << "insertNonfull(): 4" << endl;
      i--;
    }
    i++;

    if (node->getChild(i)->numKeys()==2*degree-1) {
      if (debug==1) cout << "insertNonfull(): 5" << endl;
      // Matkan varrella oleva solmu on t‰ynn‰; puolitetaan se.
      splitChild(node, i, node->getChild(i));
      // Tarkistetaan oikea suunta.
      if (compare(key, node->getKey(i))>0) {
        if (debug==1) cout << "insertNonfull(): 6" << endl;
        i++;
      }
    }

    // Jatketaan etsint‰‰.
    insertNonfull(node->getChild(i), key);
  }
}

/* Poistaa ja palauttaa edellisen avaimen. Argumenttina on annettava se
   lapsisolmu, joka edelt‰‰ avainta, jonka edelt‰j‰avain halutaan poistaa.
   branch = alipuu, josta avain poistetaan */
template<typename T> T BTree<T>::removePredecessorKey(BTreeNode<T> *branch) {
  if (branch==NULL) {
    cerr << "removePredecessorKey(): Invalid argument." << endl;
    raise(SIGABRT);
    return *new T;
  }

  T key;
  if (!branch->isLeaf()) {
    if (debug==1) cout << "removePredecessorKey(): 1" << endl;
    key=removePredecessorKey(branch->getLastChild());
  }
  else {
    if (debug==1) cout << "removePredecessorKey(): 2" << endl;
    key=branch->getLastKey();
    removeBranch(key, root);
  }
  return key;
}

/* Poistaa ja palauttaa seuraavan avaimen. Argumenttina on annettava se
   lapsisolmu, joka seuraa avainta, jonka seuraaja-avain halutaan poistaa.
   branch = alipuu, josta avain poistetaan */
template<typename T> T BTree<T>::removeSuccessorKey(BTreeNode<T> *branch) {
  if (branch==NULL) {
    cerr << "removeSuccessorKey(): Invalid argument." << endl;
    raise(SIGABRT);
    return *new T;
  }

  T key;
  if (!branch->isLeaf()) {
    if (debug==1) cout << "removeSuccessorKey(): 1" << endl;
    key=removeSuccessorKey(branch->getFirstChild());
  }
  else {
    if (debug==1) cout << "removeSuccessorKey(): 2" << endl;
    key=branch->getFirstKey();
    removeBranch(key, root);
  }
  return key;
}

/* Lainaa oikeanpuoleiselta sisarsolmulta avaimen siirt‰en sen is‰solmuun
   ja pudottaa is‰solmusta avaimen lapsisolmuun.
   parent = is‰solmu
   index = lapsisolmun indeksi */
template<typename T> void BTree<T>::rotateRight(BTreeNode<T> *parent,
                                                int index) {
  BTreeNode<T> *child=parent->getChild(index),
    *sibling=parent->getChild(index+1);
  child->insert(parent->getKey(index), NULL, sibling->getFirstChild(),
                child->numKeys());
  parent->setKey(sibling->remove(0, true, false), index);
}

/* Lainaa vasemmanpuoleiselta sisarsolmulta avaimen siirt‰en sen is‰solmuun
   ja pudottaa is‰solmusta avaimen lapsisolmuun.
   parent = is‰solmu
   index = lapsisolmun indeksi */
template<typename T> void BTree<T>::rotateLeft(BTreeNode<T> *parent,
                                               int index) {
  BTreeNode<T> *child=parent->getChild(index),
    *sibling=parent->getChild(index-1);
  child->insert(parent->getKey(index-1), sibling->getLastChild(), NULL, 0);
  parent->setKey(sibling->remove(sibling->numKeys()-1, false, true),
                 index-1);
}

/* Yhdist‰‰ kaksi solmua, jotta olisi mahdollista tuhota avain yhdistyn
   solmun alapuolelta. Palauttaa is‰solmun tai yhdistetyn solmun, jos
   is‰solmu tuhotaan.
   parent = is‰solmu, jonka kaksi lapsisolmua yhdistet‰‰n
   mergeIndex = lapsisolmun, johon yhdistet‰‰n sisarsolmu, indeksi */
template<typename T> BTreeNode<T> *BTree<T>::mergeChildren(
  BTreeNode<T> *parent,
  int mergeIndex) {
  if (parent==NULL) {
    cerr << "mergeChildren(): Invalid argument." << endl;
    raise(SIGABRT);
    return NULL;
  }

  // merged = solmu, johon sisarsolmu yhdistet‰‰n
  // removed = solmu, joka kopioidaan merged-solmuun ja tuhotaan
  // medianIndex = merge-solmuun tulevan uuden mediaaniavaimen paikka
  BTreeNode<T> *merged=parent->getChild(mergeIndex);
  BTreeNode<T> *removed;
  int medianIndex=merged->numKeys();

  // Tarkistetaan voidaanko yhdist‰‰ joko oikeanpuoleinen tai
  // vasemmanpuoleinen sisarsolmu. Jomman kumman n‰ist‰ on oltava olemassa.
  if (mergeIndex+1<parent->numKeys()+1) {
    if (debug==1) cout << "mergeChildren(): 1" << endl;
    removed=parent->getChild(mergeIndex+1);

    // Lainataan is‰solmusta mediaaniavain yhdistettyyn solmuun.
    merged->insert(parent->getKey(mergeIndex), NULL, NULL, medianIndex);
    parent->remove(mergeIndex, false, true);

    // Kopiodaan sisarsolmun avaimet ja lapsiosoittimet yhdistett‰v‰‰n
    // solmuun.
    removed->copy(0, removed->numKeys(), merged, merged->numKeys());

    delete removed;
    removed=NULL;
  }
  else if (mergeIndex-1>=0) {
    if (debug==1) cout << "mergeChildren(): 2" << endl;
    removed=parent->getChild(mergeIndex-1);

    // Tehd‰‰n tilaa yhdistett‰v‰‰n solmuun sisarsolmun avaimille ja
    // lapsiosoittimille.
    merged->shift(0, removed->numKeys());
    // Lainataan is‰solmusta mediaaniavain yhdistettyyn solmuun.
    merged->insert(parent->getKey(mergeIndex-1), NULL, NULL, medianIndex);
    parent->remove(mergeIndex-1, true, false);

    // Kopiodaan sisarsolmun avaimet ja lapsiosoittimet yhdistett‰v‰‰n
    // solmuun.
    removed->copy(0, removed->numKeys(), merged, 0);

    delete removed;
    removed=NULL;
  }

  // Is‰solmusta lainattu avain tyhjensi juurisolmun. Tuhotaan solmu ja
  // tehd‰‰n yhdistetyst‰ solmusta uusi juurisolmu. Puun korkeus pienenee
  // yhdell‰.
  if (parent->numKeys()==0) {
    if (debug==1) cout << "mergeChildren(): 3" << endl;
    root=merged;
    delete parent;
    parent=NULL;
    return merged;
  }

  return parent;
}

/* Poistaa avaimen alipuusta.
   key = poistettava avain
   branch = alipuu, josta avain poistetaan */
template<typename T> void BTree<T>::removeBranch(const T &key, BTreeNode<T>
                                                 *branch) {
  if (branch==NULL) {
    cerr << "removeBranch(): Invalid argument." << endl;
    raise(SIGABRT);
    return;
  }

  // i = indeksi, joka johtaa avaimen luo tai itse tuhottavan avaimen
  // indeksi
  int i=0;
  // Etsit‰‰n alipuu, jossa avain on.
  while (i<branch->numKeys() && compare(key, branch->getKey(i))>0) {
    if (debug==1) cout << "removeBranch(): 1" << endl;
    i++;
  }

  if (i<branch->numKeys() && compare(key, branch->getKey(i))==0) {
    // Avain lˆydettiin.
    if (branch->isLeaf()) {
      // 1. [1]
      if (debug==1) cout << "removeBranch(): 2" << endl;

      // Tuhotaan lehdess‰ oleva avain. Avain voidaan tuhota huoletta,
      // sill‰ rekursion yhteydess‰ on varmistettu, ett‰ lehteen j‰‰
      // tarpeeksi avaimia (lukuunottamatta yhden solmun puita).
      branch->remove(i, false, false);
    }
    else {
      // Jos avaimen lapsisolmuissa on tarpeeksi avaimia, voidaan lainata
      // joko oikeasta- tai vasemmasta alipuusta vastaavasti seuraaja- tai
      // edelt‰j‰avain tuhottavan avaimen solmuun paikkaajaksi.
      if (branch->getChild(i)->numKeys()>=degree) {
        // 2a. oikea puoli [1]
        if (debug==1) cout << "removeBranch(): 3" << endl;
        branch->setKey(removePredecessorKey(branch->getChild(i)), i);
      }
      else if (branch->getChild(i+1)->numKeys()>=degree) {
        // 2a. vasen puoli [1]
        if (debug==1) cout << "removeBranch(): 4" << endl;
        branch->setKey(removeSuccessorKey(branch->getChild(i+1)), i);
      }
      else {
        // 2c. [1]
        if (debug==1) cout << "removeBranch(): 5" << endl;

        // Kummassakaan alipuussa ei ollut tarpeeksi avaimia; yhdistet‰‰n
        // solmuja ja tuhotaan avain.
        branch=mergeChildren(branch, i);
        removeBranch(key, branch);
      }
    }
    return;
  }
  if (!branch->isLeaf()) {
    BTreeNode<T> *child=branch->getChild(i);

    // Jos tuhottavan avaimen luokse johtavassa lapsisolmussa on tarpeeksi
    // avaimia, jatketaan rekursiota.
    if (child->numKeys()>=degree) {
      // 3. [1]
      if (debug==1) cout << "removeBranch(): 6" << endl;
      removeBranch(key, child);
    }
    else {
      // Tarkistetaan voidaanko lainata avainta oikean- tai
      // vasemmanpuoleiselta sisarsolmulta.
      if (i+1<branch->numKeys()+1 &&
          branch->getChild(i+1)->numKeys()>=degree) {
        // 3a. oikea puoli [1]
        if (debug==1) cout << "removeBranch(): 7" << endl;
        rotateRight(branch, i);
      }
      else if (i-1>=0 && branch->getChild(i-1)->numKeys()>=degree) {
        // 3a. vasen puoli [1]
        if (debug==1) cout << "removeBranch(): 8" << endl;
        rotateLeft(branch, i);
      }
      else {
        // 3b. [1]
        if (debug==1) cout << "removeBranch(): 9" << endl;
        // Ei voida lainata avainta; solmuja pit‰‰ yhdist‰‰.
        branch=mergeChildren(branch, i);
      }

      // Jatketaan rekursiota tuhottavan avaimen lˆyt‰miseksi.
      removeBranch(key, branch);
    }
  }
}

/* Luo puun.
   degree = puun aste; oltava >= 2; m‰‰r‰‰ avainten m‰‰r‰n solmuissa;
   minimiss‰‰n t-1 ja maksimissaan 2*t-1
   compare = metodi avainten vertailemiseksi
   debug = 1=lausekattavuustulostus */
template<typename T> BTree<T>::BTree<T>(int degree,
  int (*const compare)(const T &, const T &),
                                        int debug=0) :
  degree(degree), compare(compare), debug(debug) {
  if (degree<2) {
    cerr << "Degree must be >= 2." << endl;
    raise(SIGABRT);
    return;
  }
  root=new BTreeNode<T>(degree, true, debug);
}

template<typename T> BTree<T>::~BTree<T>() {
  destroyBranch(root);
}

/* Tulostaa puun avaimet nousevassa j‰rjestykses‰. */
template<typename T> void BTree<T>::print() {
  printInorder(root, 0);
}

/* Tulostaa puun avaimet esij‰rjestyksess‰. */
template<typename T> void BTree<T>::printDebug() {
  printPreorder(root, 0);
}

/* Etsii avaimen puusta.
   key = etsitt‰v‰ avain
   result = osoitin solmuun, jossa lˆydetty avain on tai NULL jos avainta
   ei lˆytynyt
   index = avaimen indeksi */
template<typename T> void BTree<T>::search(const T &key,
                                           BTreeNode<T> **result,
                                           int *index) {
  *result=NULL;
  searchBranch(key, result, index, root);
}

/* Tarkistaa, ett‰ puu t‰ytt‰‰ B-puun m‰‰ritelm‰n. */
template<typename T> void BTree<T>::validate(const vector<T> &keys) {
  vector<bool> checked(keys.size(), false);

  numDepth=numNodes=numKeys=0;
  validateBranch(root, 0, keys, checked);

  if (root->numKeys()<1 && numDepth>0) {
    cerr << "VALIDATE: Not enough keys in the root." << endl;
    raise(SIGABRT);
    return;
  }
  if (root->numKeys()>2*degree-1) {
    cerr << "VALIDATE: Too many keys in the root." << endl;
    raise(SIGABRT);
    return;
  }

  // Tarkistaa, ett‰ kaikki avaimet on merkitty ja n‰in ollen puussa.
  for (unsigned int i=0; i<checked.size(); i++)
    if (checked[i]==false) {
      cerr << "VALIDATE: Missing key." << endl;
      raise(SIGABRT);
      return;
    }
}

/* Tarkistaa, ett‰ puu t‰ytt‰‰ B-puun m‰‰ritelm‰n ja tulostaa
   lis‰tietoja. */
template<typename T> void BTree<T>::printValidate(const vector<T> &keys) {
  validate(keys);
  cout << "VALIDATE: numDepth=" << numDepth << ", numNodes=" << numNodes
       << ", numKeys=" << numKeys << endl;;
}

/* Lis‰‰ avaimen puuhun.
   key = lis‰tt‰va avain */
template<typename T> void BTree<T>::insert(T key) {
  BTreeNode<T> *result=NULL;
  int index;
  search(key, &result, &index);
  if (result!=NULL) {
    cerr << "Insertion of multiple same keys unsupported." << endl;
    raise(SIGABRT);
    return;
  }

  if (root->numKeys()==2*degree-1) {
    if (debug==1) cout << "insert(): 1" << endl;
    // Juuri on t‰ynn‰; luodaan uusi juuri.
    BTreeNode<T> *left=root;
    root=new BTreeNode<T>(degree, false, debug);
    // Asetetaan vanha juuri uuden juuren lapseksi.
    root->setChild(left, 0);
    // Tasapainotetaan puu ja lis‰t‰‰n avain oikeaan kohtaan.
    splitChild(root, 0, left);
    insertNonfull(root, key);
  }
  else {
    if (debug==1) cout << "insert(): 2" << endl;
    insertNonfull(root, key);
  }
}

/* Poistaa avaimen puusta.
   key = poistettava avain */
template<typename T> void BTree<T>::remove(const T &key) {
  removeBranch(key, root);
}
