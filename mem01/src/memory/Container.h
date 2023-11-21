// ================================================================================================
//
// If not explicitly stated: Copyright (C) 2016, all rights reserved,
//      Rüdiger Göbl
//		Email r.goebl@tum.de
//      Chair for Computer Aided Medical Procedures
//      Technische Universität München
//      Boltzmannstr. 3, 85748 Garching b. München, Germany
//
// ================================================================================================

#ifndef __CONTAINER_H__
#define __CONTAINER_H__

//#define HAVE_CUDA

#include "ContainerFactory.h"
#include "esiglobal.h"
#ifdef HAVE_CUDA
#include "utilities/cudaUtility.h"
#endif
#include "utilities/DataType.h"

#include <cassert>
#include <exception>
#include <memory>
#include <vector>

BEGIN_NAMESPACE_ESI

class ContainerBase {
  public:
    virtual ~ContainerBase(){};
    virtual DataType getType() const { return TypeUnknown; };
};

template <typename T> class Container : public ContainerBase {
  public:
    typedef ContainerFactory::ContainerStreamType ContainerStreamType;

    Container(ContainerLocation location, ContainerStreamType associatedStream, size_t numel, const char *name = nullptr) {
        assert(numel > 0);
#ifndef HAVE_CUDA
        location = LocationHost;
#endif
#ifdef HAVE_CUDA
        // m_creationEvent = nullptr;
#endif
        m_numel = numel;
        m_location = location;
        m_associatedStream = associatedStream;
        if (name)
            strcpy(m_name, name);

        m_buffer =
            reinterpret_cast<T *>(ContainerFactoryContainerInterface::acquireMemory(m_numel * sizeof(T), m_location));
    };

    Container(ContainerLocation location, ContainerStreamType associatedStream, const std::vector<T> &data,
              bool waitFinished = true, const char *name = nullptr)
        : Container(location, associatedStream, data.size(), name) {
#ifdef HAVE_CUDA
        if (location == LocationGpu) {
            cudaSafeCallWithName(cudaMemcpyAsync(this->get(), data.data(), this->size() * sizeof(T), cudaMemcpyDefault,
                                                 associatedStream), m_name);
            createAndRecordEvent();
        } else if (location == LocationBoth) {
            cudaSafeCallWithName(cudaMemcpyAsync(this->get(), data.data(), this->size() * sizeof(T), cudaMemcpyDefault,
                                                 associatedStream), m_name);
            createAndRecordEvent();
        } else {
            std::copy(data.begin(), data.end(), this->get());
        }
        if (waitFinished) {
            waitCreationFinished();
        }
#else
        std::copy(data.begin(), data.end(), this->get());
#endif
    };

    Container(ContainerLocation location, ContainerStreamType associatedStream, const T *dataBegin, const T *dataEnd,
              bool waitFinished = true, const char *name = nullptr)
        : Container(location, associatedStream, dataEnd - dataBegin, name) {
#ifdef HAVE_CUDA
        cudaSafeCallWithName(
            cudaMemcpyAsync(this->get(), dataBegin, this->size() * sizeof(T), cudaMemcpyDefault, associatedStream), m_name);
        createAndRecordEvent();
        if (waitFinished) {
            waitCreationFinished();
        }
#else
        std::copy(dataBegin, dataEnd, this->get());
#endif
    };

    Container(ContainerLocation location, const Container<T> &source, bool waitFinished = true, const char *name = nullptr)
        : Container(location, source.getStream(), source.size(), name) {
        if (source.m_location == LocationHost && location == LocationHost) {
            std::copy(source.get(), source.get() + source.size(), this->get());
            return;
        }
#ifdef HAVE_CUDA
        else if (source.m_location == LocationHost && location == LocationGpu) {
            cudaSafeCallWithName(cudaMemcpyAsync(this->get(), source.get(), source.size() * sizeof(T), cudaMemcpyDefault,
                                                 source.getStream()), m_name);
            createAndRecordEvent();
        } else if (source.m_location == LocationGpu && location == LocationHost) {
            cudaSafeCallWithName(cudaMemcpyAsync(this->get(), source.get(), source.size() * sizeof(T), cudaMemcpyDefault,
                                                 source.getStream()), m_name);
            createAndRecordEvent();
        } else if (source.m_location == LocationGpu && location == LocationGpu) {
            cudaSafeCallWithName(cudaMemcpyAsync(this->get(), source.get(), source.size() * sizeof(T), cudaMemcpyDefault,
                                                 source.getStream()), m_name);
            createAndRecordEvent();
        } else {
            cudaSafeCallWithName(cudaMemcpyAsync(this->get(), source.get(), source.size() * sizeof(T), cudaMemcpyDefault,
                                                 source.getStream()), m_name);
            createAndRecordEvent();
        }
        if (waitFinished) {
            waitCreationFinished();
        }
#else
        std::copy(source.get(), source.get() + source.size(), this->get());
#endif
    };

    ~Container() {
#ifdef HAVE_CUDA
        auto ret = cudaStreamQuery(m_associatedStream);
        if (ret != cudaSuccess && ret != cudaErrorNotReady && ret != cudaErrorCudartUnloading) {
            cudaSafeCallWithName(ret, m_name);
        }
        // If the driver is currently unloading, we cannot free the memory in any way. Exit will clean up.
        else if (ret != cudaErrorCudartUnloading) {
            if (ret == cudaSuccess) {
                ContainerFactoryContainerInterface::returnMemory(reinterpret_cast<uint8_t *>(m_buffer),
                                                                 m_numel * sizeof(T), m_location);
            } else {
                auto buffer = m_buffer;
                auto numel = m_numel;
                auto location = m_location;
                addCallbackStream(
                    [buffer, numel, location]([[maybe_unused]] cudaStream_t s, [[maybe_unused]] cudaError_t e) -> void {
                        ContainerFactoryContainerInterface::returnMemory(reinterpret_cast<uint8_t *>(buffer),
                                                                         numel * sizeof(T), location);
                    });
            }
        }
#else
        ContainerFactoryContainerInterface::returnMemory(reinterpret_cast<uint8_t *>(m_buffer), m_numel * sizeof(T),
                                                         m_location);
#endif
    };

    const T *get() const { return m_buffer; };
    T *get() { return m_buffer; };

    T *getCopyHostRaw() const {
#ifdef HAVE_CUDA
        auto ret = new T[this->size()];

        if (m_location == LocationHost) {
            std::copy(this->get(), this->get() + this->size(), ret);
        } else if (m_location == LocationGpu) {
            cudaSafeCallWithName(
                cudaMemcpyAsync(ret, this->get(), this->size() * sizeof(T), cudaMemcpyDeviceToHost, getStream()), m_name);
            cudaSafeCallWithName(cudaStreamSynchronize(getStream()), m_name);
        } else {
            cudaSafeCallWithName(cudaMemcpy(ret, this->get(), this->size() * sizeof(T), cudaMemcpyDefault), m_name);
        }
        return ret;
#else
        return nullptr;
#endif
    }

    void copyTo(T *dst, size_t maxSize) const {
#ifdef HAVE_CUDA
        assert(maxSize >= this->size());
        cudaSafeCallWithName(cudaMemcpy(dst, this->get(), this->size() * sizeof(T), cudaMemcpyDefault), m_name);
#endif
    }

    void waitCreationFinished() {
#ifdef HAVE_CUDA
        // if (m_creationEvent) {
        cudaSafeCallWithName(cudaEventSynchronize(m_creationEvent), m_name);
        cudaSafeCallWithName(cudaEventDestroy(m_creationEvent), m_name);
        // m_creationEvent = nullptr;
        //}
#endif
    }

    // returns the number of elements that can be stored in this container
    size_t size() const { return m_numel; };

    bool isHost() const { return m_location == ContainerLocation::LocationHost; };
    bool isGPU() const { return m_location == ContainerLocation::LocationGpu; };
    bool isBoth() const { return m_location == ContainerLocation::LocationBoth; };
    ContainerLocation getLocation() const { return m_location; };
    ContainerStreamType getStream() const { return m_associatedStream; }
    DataType getType() const { return DataTypeGet<T>(); }

  private:
    void createAndRecordEvent() {
#ifdef HAVE_CUDA
        // if (!m_creationEvent) {
        // cudaSafeCall(cudaEventCreateWithFlags(&m_creationEvent, cudaEventBlockingSync | cudaEventDisableTiming));
        cudaSafeCallWithName(cudaEventCreateWithFlags(&m_creationEvent, cudaEventDisableTiming), m_name);
        //}
        cudaSafeCallWithName(cudaEventRecord(m_creationEvent, m_associatedStream), m_name);
#endif
    }

#ifdef HAVE_CUDA
    void addCallbackStream(std::function<void(cudaStream_t, cudaError_t)> func) {
        auto funcPointer = new std::function<void(cudaStream_t, cudaError_t)>(func);
        cudaSafeCallWithName(cudaStreamAddCallback(m_associatedStream, &(Container<T>::cudaDeleteCallback), funcPointer, 0), m_name);
    }
#endif

#ifdef HAVE_CUDA
    static void CUDART_CB cudaDeleteCallback(cudaStream_t stream, cudaError_t status, void *userData) {
        std::unique_ptr<std::function<void(cudaStream_t, cudaError_t)>> func =
            std::unique_ptr<std::function<void(cudaStream_t, cudaError_t)>>(
                reinterpret_cast<std::function<void(cudaStream_t, cudaError_t)> *>(userData));
        (*func)(stream, status);
    }
#endif
    // The number of elements this container can store
    size_t m_numel;
    ContainerLocation m_location;    

    ContainerStreamType m_associatedStream;
    T *m_buffer;
    char m_name[50];

#ifdef HAVE_CUDA
    cudaEvent_t m_creationEvent;
#endif
};

END_NAMESPACE_ESI

#endif //!__CONTAINER_H__
