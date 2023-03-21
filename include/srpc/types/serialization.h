#ifndef SRPC_TYPES_SERIALIZATION_H_
#define SRPC_TYPES_SERIALIZATION_H_

namespace srpc {

template <typename T, typename = void>
struct Marshal;

template <typename T, typename = void>
struct Unmarshal;

}  // namespace srpc

#endif  // SRPC_TYPES_SERIALIZATION_H_
