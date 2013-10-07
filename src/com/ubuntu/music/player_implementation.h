/*
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Thomas Voß <thomas.voss@canonical.com>
 */

#ifndef COM_UBUNTU_MUSIC_PLAYER_IMPLEMENTATION_H_
#define COM_UBUNTU_MUSIC_PLAYER_IMPLEMENTATION_H_

#include "player_skeleton.h"

#include <memory>

namespace com
{
namespace ubuntu
{
namespace music
{
class Engine;
class Service;

class PlayerImplementation : public PlayerSkeleton
{
public:
    PlayerImplementation(
            const org::freedesktop::dbus::types::ObjectPath& session_path,
            const std::shared_ptr<Service>& service,
            const std::shared_ptr<Engine>& engine);
    ~PlayerImplementation();

    virtual std::shared_ptr<TrackList> track_list();

    virtual bool open_uri(const Track::UriType& uri);
    virtual void next();
    virtual void previous();
    virtual void play();
    virtual void pause();
    virtual void stop();
    virtual void seek_to(const std::chrono::microseconds& offset);

private:
    struct Private;
    std::unique_ptr<Private> d;
};
}
}
}
#endif // COM_UBUNTU_MUSIC_PLAYER_IMPLEMENTATION_H_

