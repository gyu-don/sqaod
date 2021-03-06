#include "Solver.h"
#include "defines.h"


namespace sqaod {

/* name string for float/double */
template<class real> const char *typeString();

template<> const char *typeString<float>() { return "float"; }
template<> const char *typeString<double>() { return "double"; }

}

using namespace sqaod;

template<class real>
void Solver<real>::setPreferences(const Preferences &prefs) {
    for (Preferences::const_iterator it = prefs.begin();
         it != prefs.end(); ++it) {
        setPreference(*it);
    }
}


/* solver state */
template<class real>
void Solver<real>::setState(SolverState state) {
    solverState_ |= state;
}

template<class real>
void Solver<real>::clearState(SolverState state) {
    int stateToClear = 0;
    switch (state) {
    case solProblemSet :
        stateToClear |= solProblemSet;
        /* no break */
    case solPrepared:
        stateToClear |= solPrepared;
        /* no break */
    case solQSet:
        stateToClear |= solQSet;
        /* no break */
    case solEAvailable:
    case solSolutionAvailable:
        stateToClear |= solEAvailable;
        stateToClear |= solSolutionAvailable;
        /* no break */
    default:
        break;
    }
    solverState_ &= ~stateToClear;
}

template<class real>
bool Solver<real>::isRandSeedGiven() const {
    return (solverState_ & solRandSeedGiven) != 0;
}

template<class real>
bool Solver<real>::isProblemSet() const {
    return (solverState_ & solProblemSet) != 0;
}

template<class real>
bool Solver<real>::isPrepared() const {
    return (solverState_ & solPrepared) != 0;
}

template<class real>
bool Solver<real>::isQSet() const {
    return (solverState_ & solQSet) != 0;
}

template<class real>
bool Solver<real>::isEAvailable() const {
    return (solverState_ & solEAvailable) != 0;
}

template<class real>
bool Solver<real>::isSolutionAvailable() const {
    return (solverState_ & solSolutionAvailable) != 0;
}

template<class real>
void Solver<real>::throwErrorIfProblemNotSet() const {
    throwErrorIf(!isProblemSet(), "Problem is not set.");
}

template<class real>
void Solver<real>::throwErrorIfNotPrepared() const {
    throwErrorIfProblemNotSet();
    throwErrorIf(!isPrepared(),
                 "not prepared, call prepare() in advance.");
}

template<class real>
void Solver<real>::throwErrorIfQNotSet() const {
    throwErrorIf(!isQSet(),
                 "Bits(x or q) not initialized.  Plase set or randomize in advance.");
}


template<class real>
Algorithm BFSearcher<real>::selectAlgorithm(Algorithm algo) {
    return algoBruteForceSearch;
}
    
template<class real>
Algorithm BFSearcher<real>::getAlgorithm() const {
    return algoBruteForceSearch;
}

template<class real>
Preferences Annealer<real>::getPreferences() const {
    Preferences prefs;
    prefs.pushBack(Preference(pnAlgorithm, this->getAlgorithm()));
    prefs.pushBack(Preference(pnNumTrotters, m_));
    prefs.pushBack(Preference(pnPrecision, typeString<real>()));
    return prefs;
}

template<class real>
void Annealer<real>::setPreference(const Preference &pref) {
    if (pref.name == pnNumTrotters) {
        throwErrorIf(pref.nTrotters <= 0, "# trotters must be a positive integer.");
        if (m_ != pref.nTrotters)
            Solver<real>::clearState(Solver<real>::solPrepared);
        m_ = pref.nTrotters;
    }
    else if (pref.name == pnAlgorithm) {
        this->selectAlgorithm(pref.algo);
    }
}


template<class real>
void DenseGraphSolver<real>::getProblemSize(SizeType *N) const {
    *N = N_;
}

template<class real>
void BipartiteGraphSolver<real>::getProblemSize(SizeType *N0, SizeType *N1) const {
    *N0 = N0_;
    *N1 = N1_;
}


template<class real>
Preferences DenseGraphBFSearcher<real>::getPreferences() const {
    Preferences prefs;
    prefs.pushBack(Preference(pnAlgorithm, this->getAlgorithm()));
    prefs.pushBack(Preference(pnTileSize, tileSize_));
    prefs.pushBack(Preference(pnPrecision, typeString<real>()));
    return prefs;
}

template<class real>
void DenseGraphBFSearcher<real>::setPreference(const Preference &pref) {
    if (pref.name == pnTileSize) {
        throwErrorIf(pref.tileSize <= 0, "tileSize must be a positive integer.");
        tileSize_ = pref.tileSize;
    }
}


template<class real>
void DenseGraphBFSearcher<real>::search() {
    this->prepare();
    while (!searchRange(NULL));
    this->makeSolution();
}


template<class real>
Preferences BipartiteGraphBFSearcher<real>::getPreferences() const {
    Preferences prefs;
    prefs.pushBack(Preference(pnAlgorithm, algoBruteForceSearch));
    prefs.pushBack(Preference(pnTileSize0, tileSize0_));
    prefs.pushBack(Preference(pnTileSize1, tileSize1_));
    return prefs;
}

template<class real>
void BipartiteGraphBFSearcher<real>::setPreference(const Preference &pref) {
    if (pref.name == pnTileSize0) {
        throwErrorIf(pref.tileSize <= 0, "tileSize0 must be a positive integer.");
        tileSize0_ = pref.tileSize;
    }
    if (pref.name == pnTileSize1) {
        throwErrorIf(pref.tileSize <= 0, "tileSize1 must be a positive integer.");
        tileSize1_ = pref.tileSize;
    }
}

template<class real>
void BipartiteGraphBFSearcher<real>::search() {
    this->prepare();
    while (!searchRange(NULL, NULL));
    this->makeSolution();
}


/* explicit instantiation */
template struct sqaod::Solver<double>;
template struct sqaod::Solver<float>;
template struct sqaod::BFSearcher<double>;
template struct sqaod::BFSearcher<float>;
template struct sqaod::Annealer<double>;
template struct sqaod::Annealer<float>;
template struct sqaod::DenseGraphSolver<double>;
template struct sqaod::DenseGraphSolver<float>;
template struct sqaod::BipartiteGraphSolver<double>;
template struct sqaod::BipartiteGraphSolver<float>;

template struct sqaod::DenseGraphBFSearcher<double>;
template struct sqaod::DenseGraphBFSearcher<float>;
template struct sqaod::DenseGraphAnnealer<double>;
template struct sqaod::DenseGraphAnnealer<float>;
template struct sqaod::BipartiteGraphBFSearcher<double>;
template struct sqaod::BipartiteGraphBFSearcher<float>;
template struct sqaod::BipartiteGraphAnnealer<double>;
template struct sqaod::BipartiteGraphAnnealer<float>;
