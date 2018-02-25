/* -*- c++ -*- */
#pragma once

#include <sqaodc/common/Common.h>
#include <sqaodc/cuda/Device.h>
#include <sqaodc/cuda/DeviceMatrix.h>
#include <sqaodc/cuda/DeviceArray.h>
#include <sqaodc/cuda/DeviceDenseGraphBatchSearch.h>
#include <sqaodc/cuda/DeviceRandom.h>
#include <sqaodc/cuda/DeviceRandomBuffer.h>


namespace sqaod_cuda {

namespace sq = sqaod;

template<class real>
class CUDABipartiteGraphAnnealer : public sq::BipartiteGraphAnnealer<real> {
    typedef sq::MatrixType<real> HostMatrix;
    typedef sq::VectorType<real> HostVector;
    typedef sq::BitsPairArray BitsPairArray;
    typedef sq::Bits Bits;
    typedef sq::SizeType SizeType;
    typedef DeviceMatrixType<real> DeviceMatrix;
    typedef DeviceVectorType<real> DeviceVector;
    typedef DeviceScalarType<real> DeviceScalar;
    typedef DeviceBipartiteGraphFormulas<real> DeviceFormulas;
    
public:
    CUDABipartiteGraphAnnealer();

    CUDABipartiteGraphAnnealer(Device &device);

    ~CUDABipartiteGraphAnnealer();

    void deallocate();
    
    void assignDevice(Device &device);

    virtual sq::Algorithm selectAlgorithm(sq::Algorithm algo);
    
    virtual sq::Algorithm getAlgorithm() const;
    
    void seed(unsigned int seed);

    /* void getProblemSize(SizeType *N0, SizeType *N1) const; */

    void setProblem(const HostVector &b0, const HostVector &b1, const HostMatrix &W,
                    sq::OptimizeMethod om = sq::optMinimize);

    /* Preferences getPreferences() const; */

    /* void setPreference(const Preference &pref); */

    const HostVector &get_E() const;

    const sq::BitsPairArray &get_x() const;

    void set_x(const Bits &x0, const Bits &x1);

    /* Ising machine / spins */

    void get_hJc(HostVector *h0, HostVector *h1, HostMatrix *J, real *c) const;

    const BitsPairArray &get_q() const;

    void randomize_q();

    void calculate_E();

    void initAnneal();

    void finAnneal();

    void annealOneStep(real G, real kT);
    

    /* public for debug */
    void calculate_Jq(DeviceMatrix *d_Jq, const DeviceMatrix &d_J, MatrixOp op,
                      const DeviceMatrix &d_qFixed);

    /* public for debug */
    void tryFlip(DeviceMatrix *d_qAnneal, const DeviceMatrix &d_Jq, int N, int m, 
                 int nTrottersToFlipe, int offset,
                 const DeviceVector &d_h, const real *d_realRand, real G, real kT);

private:
    void deallocateProblem();
    void deallocateInternalObjects();

    enum {
        /* FIXME: parameterise */
        nRunsPerRandGen = 10
    };

    void syncBits();

    DeviceRandom d_random_;
    DeviceRandomBuffer d_randReal_;
    DeviceVector d_h0_, d_h1_;
    DeviceMatrix d_J_;
    DeviceScalar d_c_;
    DeviceMatrix d_matq0_, d_matq1_;
    DeviceBitMatrix h_q0_, h_q1_;
    
    DeviceMatrix d_Jq0_;
    DeviceMatrix d_Jq1_;
    
    DeviceVector h_E_; /* host mem */
    HostVector E_;
    BitsPairArray bitsPairX_;
    BitsPairArray bitsPairQ_;
    
    DeviceStream *devStream_;
    DeviceFormulas devFormulas_;
    DeviceCopy devCopy_;
    DeviceObjectAllocator *devAlloc_;

    typedef sq::BipartiteGraphAnnealer<real> Base;
    using Base::om_;
    using Base::N0_;
    using Base::N1_;
    using Base::m_;
    /* annealer state */
    using Base::solRandSeedGiven;
    using Base::solInitialized;
    using Base::solProblemSet;
    using Base::solQSet;
    using Base::solSolutionAvailable;
    using Base::setState;
    using Base::clearState;
    using Base::isRandSeedGiven;
    using Base::isProblemSet;
    using Base::isInitialized;
    using Base::throwErrorIfProblemNotSet;
    using Base::throwErrorIfNotInitialized;
    using Base::throwErrorIfQNotSet;
    using Base::throwErrorIfSolutionNotAvailable;
};

}