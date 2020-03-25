#include "carla/sensor/s11n/ActorIdSerializer.h"

#include "carla/sensor/data/VisibleActorEvent.h"

namespace carla {
namespace sensor {
namespace s11n {

  SharedPtr<SensorData> ActorIdSerializer::Deserialize(RawData &&data) {
    return SharedPtr<SensorData>(new data::VisibleActorEvent(std::move(data)));
  }

} // namespace s11n
} // namespace sensor
} // namespace carla