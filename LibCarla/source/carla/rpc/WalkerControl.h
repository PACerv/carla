// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/MsgPack.h"

#ifdef LIBCARLA_INCLUDED_FROM_UE4
#  include "Carla/Walker/WalkerControl.h"
#endif // LIBCARLA_INCLUDED_FROM_UE4

namespace carla {
namespace rpc {

  class WalkerControl {
  public:

    WalkerControl() = default;

    WalkerControl(
        geom::Vector3D in_direction,
        float in_speed,
        uint32_t in_animation)
      : direction(in_direction),
        speed(in_speed),
        animation_id(in_animation) {}

    geom::Vector3D direction = {1.0f, 0.0f, 0.0f};

    float speed = 0.0f;

    uint32_t animation_id = 0;

#ifdef LIBCARLA_INCLUDED_FROM_UE4

    WalkerControl(const FWalkerControl &Control)
      : direction(Control.Direction.X, Control.Direction.Y, Control.Direction.Z),
        speed(1e-2f * Control.Speed),
        animation_id{Control.Animation_ID} {}

    operator FWalkerControl() const {
      FWalkerControl Control;
      Control.Direction = {direction.x, direction.y, direction.z};
      Control.Speed = 1e2f * speed;
      Control.Animation_ID = animation_id;
      return Control;
    }

#endif // LIBCARLA_INCLUDED_FROM_UE4

    bool operator!=(const WalkerControl &rhs) const {
      return direction != rhs.direction || speed != rhs.speed || animation_id != rhs.animation_id;
    }

    bool operator==(const WalkerControl &rhs) const {
      return !(*this != rhs);
    }

    MSGPACK_DEFINE_ARRAY(direction, speed, animation_id);
  };

} // namespace rpc
} // namespace carla
