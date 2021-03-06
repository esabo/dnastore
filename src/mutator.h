#ifndef MUTATOR_INCLUDED
#define MUTATOR_INCLUDED

#include <iostream>
#include "kmer.h"
#include "trans.h"
#include "logsumexp.h"

struct MutatorParams {
  double pDelOpen, pDelExtend, pTanDup, pTransition, pTransversion;
  vguard<double> pLen;
  bool local;

  MutatorParams& initMaxDupLen (size_t maxDupLen);

  void writeJSON (ostream& out) const;
  void readJSON (istream& in);
  string asJSON() const;
  static MutatorParams fromJSON (istream& in);
  static MutatorParams fromFile (const char* filename);

  inline double pMatch() const { return 1. - pTransition - pTransversion; }
  inline double pSub (Base x, Base y) const {
    return x == y ? pMatch() : (isTransition(x,y) ? pTransition : (pTransversion/2.));
  }
  inline double pFwdDup() const { return 0.; }
  inline double pRevDup() const { return 0.; }
  inline double pNoGap() const { return 1. - pDelOpen - pTanDup; }
  inline double pDelEnd() const { return 1. - pDelExtend; }
  inline size_t maxDupLen() const { return pLen.size(); }
};

struct MutatorScores {
  LogProb delOpen, tanDup, noGap;
  LogProb delExtend, delEnd;
  vguard<vguard<LogProb> > sub;  // sub[base][observed]
  vguard<LogProb> len;
  MutatorScores (const MutatorParams& params);
  void writeJSON (ostream& out) const;
  string toJSON() const;
};

struct MutatorCounts {
  double nDelOpen, nTanDup, nNoGap;
  double nDelExtend, nDelEnd;
  vguard<vguard<double> > nSub;
  vguard<double> nLen;

  MutatorCounts (const MutatorParams& params);
  MutatorCounts& initLaplace (double n = 1);

  void writeJSON (ostream& out) const;
  string asJSON() const;

  MutatorCounts& operator+= (const MutatorCounts& c);
  MutatorCounts operator+ (const MutatorCounts& c) const;

  double nMatch() const;
  double nTransition() const;
  double nTransversion() const;

  MutatorParams mlParams() const;  // will set local to true
  MutatorParams mlParams (const MutatorCounts& prior) const;  // will set local to true

  LogProb logPrior (const MutatorParams& params) const;
  LogProb logLikelihood (const MutatorParams& params) const;
};

#endif /* MUTATOR_INCLUDED */
