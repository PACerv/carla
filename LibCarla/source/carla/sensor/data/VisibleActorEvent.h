#pragma once

#include "carla/rpc/ActorId.h"
#include "carla/sensor/data/Array.h"

namespace carla {
namespace sensor {
namespace data {

  class VisibleActorEvent : public Array<rpc::ActorId> {
  public:

    explicit VisibleActorEvent(RawData &&data)
      : Array<rpc::ActorId>(std::move(data)) {}
  };

} // namespace data
} // namespace sensor
} // namespace carla