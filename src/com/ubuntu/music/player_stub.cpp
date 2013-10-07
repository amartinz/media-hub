/*
 * Copyright © 2013 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY {} without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Thomas Voß <thomas.voss@canonical.com>
 */

#include <com/ubuntu/music/property.h>
#include <com/ubuntu/music/service.h>
#include <com/ubuntu/music/track_list.h>

#include "player_stub.h"
#include "player_traits.h"
#include "property_stub.h"
#include "the_session_bus.h"
#include "track_list_stub.h"

#include "mpris/player.h"

#include <org/freedesktop/dbus/types/object_path.h>

#include <limits>

namespace dbus = org::freedesktop::dbus;
namespace music = com::ubuntu::music;

struct music::PlayerStub::Private
{
    Private(const std::shared_ptr<Service>& parent,
            const std::shared_ptr<dbus::Service>& remote,
            const dbus::types::ObjectPath& path
            ) : parent(parent),
                path(path),
                object(remote->object_for_path(path)),                
                properties
                {
                    object->get_property<mpris::Player::Properties::CanPlay>(),
                    object->get_property<mpris::Player::Properties::CanPause>(),
                    object->get_property<mpris::Player::Properties::CanSeek>(),
                    object->get_property<mpris::Player::Properties::CanControl>(),
                    object->get_property<mpris::Player::Properties::CanGoNext>(),
                    object->get_property<mpris::Player::Properties::CanGoPrevious>(),
                    object->get_property<mpris::Player::Properties::PlaybackStatus>(),
                    object->get_property<mpris::Player::Properties::LoopStatus>(),
                    object->get_property<mpris::Player::Properties::PlaybackRate>(),
                    object->get_property<mpris::Player::Properties::Shuffle>(),
                    object->get_property<mpris::Player::Properties::MetaData>(),
                    object->get_property<mpris::Player::Properties::Volume>(),
                    object->get_property<mpris::Player::Properties::MinimumRate>(),
                    object->get_property<mpris::Player::Properties::MaximumRate>()
                }
    {
    }

    std::shared_ptr<Service> parent;
    std::shared_ptr<TrackList> track_list;

    dbus::Bus::Ptr bus;
    dbus::types::ObjectPath path;
    dbus::Object::Ptr object;
    
    struct
    {
        PropertyStub<bool, mpris::Player::Properties::CanPlay> can_play;
        PropertyStub<bool, mpris::Player::Properties::CanPause> can_pause;
        PropertyStub<bool, mpris::Player::Properties::CanSeek> can_seek;
        PropertyStub<bool, mpris::Player::Properties::CanControl> can_control;
        PropertyStub<bool, mpris::Player::Properties::CanGoNext> can_go_next;
        PropertyStub<bool, mpris::Player::Properties::CanGoPrevious> can_go_previous;
        
        PropertyStub<Player::PlaybackStatus, mpris::Player::Properties::PlaybackStatus> playback_status;
        PropertyStub<Player::LoopStatus, mpris::Player::Properties::LoopStatus> loop_status;
        PropertyStub<Player::PlaybackRate, mpris::Player::Properties::PlaybackRate> playback_rate;
        PropertyStub<bool, mpris::Player::Properties::Shuffle> is_shuffle;
        PropertyStub<Track::MetaData, mpris::Player::Properties::MetaData> meta_data_for_current_track;
        PropertyStub<Player::Volume, mpris::Player::Properties::Volume> volume;
        PropertyStub<Player::PlaybackRate, mpris::Player::Properties::MinimumRate> minimum_playback_rate;
        PropertyStub<Player::PlaybackRate, mpris::Player::Properties::MaximumRate> maximum_playback_rate;
    } properties;
};

music::PlayerStub::PlayerStub(
    const std::shared_ptr<Service>& parent,
    const dbus::types::ObjectPath& object_path)
        : dbus::Stub<Player>(the_session_bus()),
          d(new Private{parent, access_service(), object_path})
{
}

music::PlayerStub::~PlayerStub()
{
}

std::shared_ptr<music::TrackList> music::PlayerStub::track_list()
{
    if (!d->track_list)
    {
        d->track_list = std::make_shared<music::TrackListStub>(
                    shared_from_this(),
                    dbus::types::ObjectPath(d->path.as_string() + "/TrackList"));
    }
    return d->track_list;
}

bool music::PlayerStub::open_uri(const music::Track::UriType& uri)
{
    auto op = d->object->invoke_method_synchronously<mpris::Player::OpenUri, music::Track::UriType>(uri);

    return op.is_error();
}

void music::PlayerStub::next()
{
    auto op = d->object->invoke_method_synchronously<mpris::Player::Next, void>();

    if (op.is_error())
        throw std::runtime_error("Problem switching to next track on remote object");
}

void music::PlayerStub::previous()
{
    auto op = d->object->invoke_method_synchronously<mpris::Player::Previous, void>();

    if (op.is_error())
        throw std::runtime_error("Problem switching to previous track on remote object");
}

void music::PlayerStub::play()
{
    auto op = d->object->invoke_method_synchronously<mpris::Player::Play, void>();

    if (op.is_error())
        throw std::runtime_error("Problem starting playback on remote object");
}

void music::PlayerStub::pause()
{
    auto op = d->object->invoke_method_synchronously<mpris::Player::Pause, void>();

    if (op.is_error())
        throw std::runtime_error("Problem pausing playback on remote object");
}

void music::PlayerStub::seek_to(const std::chrono::microseconds& offset)
{
    auto op = d->object->invoke_method_synchronously<mpris::Player::Seek, void, uint64_t>(offset.count());

    if (op.is_error())
        throw std::runtime_error("Problem seeking on remote object");
}

void music::PlayerStub::stop()
{
    auto op = d->object->invoke_method_synchronously<mpris::Player::Stop, void>();

    if (op.is_error())
        throw std::runtime_error("Problem stopping playback on remote object");
}

const music::Property<bool>& music::PlayerStub::can_play() const 
{
    return d->properties.can_play;
}

const music::Property<bool>& music::PlayerStub::can_pause() const
{
    return d->properties.can_pause;
}

const music::Property<bool>& music::PlayerStub::can_seek() const
{
    return d->properties.can_seek;
}

const music::Property<bool>& music::PlayerStub::can_go_previous() const
{
    return d->properties.can_go_previous;
}

const music::Property<bool>& music::PlayerStub::can_go_next() const
{
    return d->properties.can_go_next;
}

const music::Property<music::Player::PlaybackStatus>& music::PlayerStub::playback_status() const
{
    return d->properties.playback_status;
}

const music::Property<music::Player::LoopStatus>& music::PlayerStub::loop_status() const
{
    return d->properties.loop_status;
}

const music::Property<music::Player::PlaybackRate>& music::PlayerStub::playback_rate() const
{
    return d->properties.playback_rate;
}

const music::Property<bool>& music::PlayerStub::is_shuffle() const
{
    return d->properties.is_shuffle;
}

const music::Property<music::Track::MetaData>& music::PlayerStub::meta_data_for_current_track() const
{
    return d->properties.meta_data_for_current_track;
}

const music::Property<music::Player::Volume>& music::PlayerStub::volume() const
{
    return d->properties.volume;
}

const music::Property<music::Player::PlaybackRate>& music::PlayerStub::minimum_playback_rate() const
{
    return d->properties.minimum_playback_rate;
}

const music::Property<music::Player::PlaybackRate>& music::PlayerStub::maximum_playback_rate() const
{
    return d->properties.maximum_playback_rate;
}

music::Property<music::Player::LoopStatus>& music::PlayerStub::loop_status()
{
    return d->properties.loop_status;
}

music::Property<music::Player::PlaybackRate>& music::PlayerStub::playback_rate()
{
    return d->properties.playback_rate;
}

music::Property<bool>& music::PlayerStub::is_shuffle()
{
    return d->properties.is_shuffle;
}

music::Property<music::Player::Volume>& music::PlayerStub::volume()
{
    return d->properties.volume;
}

const music::Signal<uint64_t>& music::PlayerStub::seeked_to() const
{
    static music::Signal<uint64_t> signal;
    return signal;
}
