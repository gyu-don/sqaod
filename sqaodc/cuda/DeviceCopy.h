#pragma once

#include <sqaodc/cuda/cudafuncs.h>
#include <sqaodc/cuda/DeviceMatrix.h>
#include <sqaodc/cuda/DeviceArray.h>
#include <sqaodc/cuda/DeviceStream.h>
#include <sqaodc/cuda/DeviceKernels.h>
#include <sqaodc/cuda/DeviceObjectAllocator.h>
#include <sqaodc/cuda/HostObjectAllocator.h>
#include <sqaodc/cuda/Assertion.h>


namespace sqaod_cuda {

namespace sq = sqaod;

class Device;

struct DeviceCopy {
 
    template<class V>
    void copy(V *d_buf, const V *v, sq::SizeType nElms) const;

    template<class V>
    void broadcast(V *d_buf, const V &v, sq::SizeType nElms) const;

    template<class V>
    void broadcastStrided(V *d_buf, const V &v, sq::SizeType size,
                          sq::SizeType stride, sq::IdxType offset) const;
    
    template<class V>
    void copyRowwise(DeviceMatrixType<V> *dst, const DeviceVectorType<V> &src) const;

    template<class Vdst, class Vsrc>
    void cast(DeviceMatrixType<Vdst> *dst, const DeviceMatrixType<Vsrc> &src);
    template<class Vdst, class Vsrc>
    void cast(DeviceVectorType<Vdst> *dst, const DeviceVectorType<Vsrc> &src);

    /* sq::MatrixType<V> <-> DeviceMatrixType<V> */
    template<class V>
    void operator()(V *dst, const V *src, sq::SizeType nElms) const;
    
    template<class V>
    void operator()(DeviceMatrixType<V> *dst, const sq::MatrixType<V> &src);
    
    template<class V>
    void operator()(sq::MatrixType<V> *dst, const DeviceMatrixType<V> &src) const;
    
    template<class V>
    void operator()(DeviceMatrixType<V> *dst, const DeviceMatrixType<V> &src);
    
    template<class V>
    void operator()(DeviceMatrixType<V> *dst, const V &src) const;
    
    template<class V>
    void operator()(DeviceMatrixType<V> *dst, const V &src, sq::SizeType size,
                    sq::SizeType stride, sq::IdxType offset) const;
    
    /* sq::VectorType<V> <-> DeviceVectorType<V> */
    
    template<class V>
    void operator()(DeviceVectorType<V> *dst, const sq::VectorType<V> &src);
    
    template<class V>
    void operator()(sq::VectorType<V> *dst, const DeviceVectorType<V> &src) const;
    
    template<class V>
    void operator()(DeviceVectorType<V> *dst, const DeviceVectorType<V> &src);
    
    template<class V>
    void operator()(DeviceVectorType<V> *dst, const V &src) const;

    /* Host scalar variables <-> DeviceScalar */
    
    template<class V>
    void operator()(DeviceScalarType<V> *dst, const V &src);
    
    template<class V>
    void operator()(V *dst, const DeviceScalarType<V> &src) const;

    /* Host array <-> Device Array */
    template<class V>
    void operator()(DeviceArrayType<V> *dst, const DeviceArrayType<V> &src);
    
    void synchronize() const;

    DeviceCopy();

    DeviceCopy(Device &device, DeviceStream *stream = NULL);
    
    void assignDevice(Device &device, DeviceStream *stream = NULL);
    
private:
    DeviceObjectAllocator *devAlloc_;
    DeviceCopyKernels kernels_;
    cudaStream_t stream_;
};


template<class V> inline void DeviceCopy::
copy(V *d_buf, const V *v, sq::SizeType nElms) const {
    throwOnError(cudaMemcpyAsync(d_buf, v, sizeof(V) * nElms, cudaMemcpyDefault, stream_));
    DEBUG_SYNC;
}

template<class V> inline
void DeviceCopy::broadcast(V *d_buf, const V &v, sq::SizeType size) const {
    kernels_.copyBroadcast(d_buf, v, size);
}

template<class V> void DeviceCopy::
broadcastStrided(V *d_buf, const V &v, sq::SizeType size,
                 sq::SizeType stride, sq::IdxType offset) const {
    kernels_.copyBroadcastStrided(d_buf, v, size, stride, offset);
}


template<class V> inline void DeviceCopy::
copyRowwise(DeviceMatrixType<V> *dst, const DeviceVectorType<V> &src) const {
    throwErrorIf(dst->cols != src.size, "matrix rows and vector size does not match.");
    kernels_.copyBroadcastVector(dst->d_data, src.d_data, src.size, dst->rows);
}

template<class Vdst, class Vsrc> inline void DeviceCopy::
cast(DeviceMatrixType<Vdst> *dst, const DeviceMatrixType<Vsrc> &src) {
    devAlloc_->allocateIfNull(dst, src.dim());
    assertSameShape(*dst, src, __func__);
    kernels_.cast(dst->d_data, src.d_data, src.rows * src.cols);
}

template<class Vdst, class Vsrc> inline void DeviceCopy::
cast(DeviceVectorType<Vdst> *dst, const DeviceVectorType<Vsrc> &src) {
    devAlloc_->allocateIfNull(dst, src.size);
    assertSameShape(*dst, src, __func__);
    kernels_.cast(dst->d_data, src.d_data, src.size);
}

template<class V> inline void DeviceCopy::
operator()(V *dst, const V *v, sq::SizeType nElms) const {
    copy(dst, v, nElms);
}

template<class V> void DeviceCopy::
operator()(DeviceMatrixType<V> *dst, const sq::MatrixType<V> &src) {
    devAlloc_->allocateIfNull(dst, src.dim());
    assertSameShape(*dst, src, __func__);
    copy(dst->d_data, src.data, src.rows * src.cols);
}

template<class V> void DeviceCopy::
operator()(sq::MatrixType<V> *dst, const DeviceMatrixType<V> &src) const {
    if (dst->data == NULL)
        dst->resize(src.dim());
    assertSameShape(*dst, src, __func__);
    copy(dst->data, src.d_data, src.rows * src.cols);
}

template<class V> void DeviceCopy::
operator()(DeviceMatrixType<V> *dst, const DeviceMatrixType<V> &src) {
    devAlloc_->allocateIfNull(dst, src.dim());
    assertSameShape(*dst, src, __func__);
    copy(dst->d_data, src.d_data, src.rows * src.cols);
}

template<class V> void DeviceCopy::
operator()(DeviceMatrixType<V> *dst, const V &src) const {
    assertValidMatrix(*dst, __func__);
    broadcast(dst->d_data, src, dst->rows * dst->cols);
}

template<class V> void DeviceCopy::
operator()(DeviceMatrixType<V> *dst, const V &src, sq::SizeType size,
           sq::SizeType stride, sq::IdxType offset) const {
    assertValidMatrix(*dst, __func__);
    broadcastStrided(dst->d_data, src, size, stride, offset);
}
    
template<class V> void DeviceCopy::
operator()(DeviceVectorType<V> *dst, const sq::VectorType<V> &src) {
    devAlloc_->allocateIfNull(dst, src.size);
    assertSameShape(*dst, src, __func__);
    copy(dst->d_data, src.data, src.size);
}
    
template<class V> void DeviceCopy::
operator()(sq::VectorType<V> *dst, const DeviceVectorType<V> &src) const {
    if (dst->data == NULL)
        dst->allocate(src.size);
    assertSameShape(*dst, src, __func__);
    copy(dst->data, src.d_data, src.size);
}

template<class V> void DeviceCopy::
operator()(DeviceVectorType<V> *dst, const DeviceVectorType<V> &src) {
    devAlloc_->allocateIfNull(dst, src.size);
    assertSameShape(*dst, src, __func__);
    copy(dst->d_data, src.d_data, src.size);
}

template<class V> void DeviceCopy::
operator()(DeviceVectorType<V> *dst, const V &src) const {
    assertValidVector(*dst, __func__);
    copyBroadcast(dst->d_data, src, 1);
}

/* Host scalar variables <-> DeviceScalar */
    
template<class V> void DeviceCopy::
operator()(DeviceScalarType<V> *dst, const V &src) {
    devAlloc_->allocateIfNull(dst);
    copy(dst->d_data, &src, 1);
}

template<class V> void DeviceCopy::
operator()(V *dst, const DeviceScalarType<V> &src) const {
    copy(dst, src.d_data, 1);
}

/* Packed bits */
template<class V> void DeviceCopy::
operator()(DeviceArrayType<V> *dst, const DeviceArrayType<V> &src) {
    devAlloc_->allocateIfNull(dst, src.capacity);
    throwErrorIf(dst->capacity < src.size, "Array capacity is too small.");
    copy(dst->d_data, src.d_data, src.size);
    dst->size = src.size;
}

}
