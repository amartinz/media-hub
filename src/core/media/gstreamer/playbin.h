/*
 * Copyright © 2013 Canonical Ltd.
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

#ifndef GSTREAMER_PLAYBIN_H_
#define GSTREAMER_PLAYBIN_H_

#include <core/media/player.h>

#include "bus.h"
#include "../mpris/player.h"

#include <gio/gio.h>
#include <gst/gst.h>

#include <chrono>
#include <string>

// Uncomment to generate a dot file at the time that the pipeline
// goes to the PLAYING state. Make sure to export GST_DEBUG_DUMP_DOT_DIR
// before starting media-hub-server. To convert the dot file to something
// other image format, use: dot pipeline.dot -Tpng -o pipeline.png
//#define DEBUG_GST_PIPELINE

namespace gstreamer
{
struct Playbin
{
    enum PlayFlags
    {
        GST_PLAY_FLAG_VIDEO = (1 << 0),
        GST_PLAY_FLAG_AUDIO = (1 << 1),
        GST_PLAY_FLAG_TEXT = (1 << 2)
    };

    enum MediaFileType
    {
        MEDIA_FILE_TYPE_NONE,
        MEDIA_FILE_TYPE_AUDIO,
        MEDIA_FILE_TYPE_VIDEO
    };

    static std::string get_audio_role_str(core::ubuntu::media::Player::AudioStreamRole audio_role);

    static const std::string& pipeline_name();

    static void about_to_finish(GstElement*, gpointer user_data);

    static void source_setup(GstElement*,
                             GstElement *source,
                             gpointer user_data);

    Playbin();
    ~Playbin();

    void reset();
    void reset_pipeline();

    void on_new_message(const Bus::Message& message);
    void on_new_message_async(const Bus::Message& message);
    void process_message_element(GstMessage *message);

    gstreamer::Bus& message_bus();

    void setup_pipeline_for_audio_video();

    void create_video_sink(uint32_t texture_id);

    void set_volume(double new_volume);

    void set_lifetime(core::ubuntu::media::Player::Lifetime);
    core::ubuntu::media::Player::Orientation orientation_lut(const gchar *orientation);

    /** Sets the new audio stream role on the pulsesink in playbin */
    void set_audio_stream_role(core::ubuntu::media::Player::AudioStreamRole new_audio_role);

    uint64_t position() const;
    uint64_t duration() const;

    void set_uri(const std::string& uri, const core::ubuntu::media::Player::HeadersType& headers, bool do_pipeline_reset = true);
    std::string uri() const;

    void setup_source(GstElement *source);

    // Sets the pipeline's state (stopped, playing, paused, etc). Optional parameter makes this call
    // in the main_loop context.
    bool set_state_and_wait(GstState new_state);
    bool seek(const std::chrono::microseconds& ms);

    core::ubuntu::media::video::Dimensions get_video_dimensions() const;
    void emit_video_dimensions_changed_if_changed(const core::ubuntu::media::video::Dimensions &new_dimensions);

    std::string file_info_from_uri(const std::string& uri) const;
    std::string encode_uri(const std::string& uri) const;
    std::string decode_uri(const std::string& uri) const;
    std::string get_file_content_type(const std::string& uri) const;

    bool is_audio_file(const std::string& uri) const;
    bool is_video_file(const std::string& uri) const;

    MediaFileType media_file_type() const;

    bool can_play_streams() const;

    GstElement* pipeline;
    gstreamer::Bus bus;
    MediaFileType file_type;
    GstElement* video_sink;
    GstElement* audio_sink;
    core::Connection on_new_message_connection_async;
    bool is_seeking;
    mutable uint64_t previous_position;
    core::ubuntu::media::video::Dimensions cached_video_dimensions;
    core::ubuntu::media::Player::HeadersType request_headers;
    core::ubuntu::media::Player::Lifetime player_lifetime;
    gulong about_to_finish_handler_id;
    gulong source_setup_handler_id;
    struct
    {
        core::Signal<void> about_to_finish;
        core::Signal<Bus::Message::Detail::ErrorWarningInfo> on_error;
        core::Signal<Bus::Message::Detail::ErrorWarningInfo> on_warning;
        core::Signal<Bus::Message::Detail::ErrorWarningInfo> on_info;
        core::Signal<Bus::Message::Detail::Tag> on_tag_available;
        core::Signal<std::pair<Bus::Message::Detail::StateChanged,std::string>> on_state_changed;
        core::Signal<uint64_t> on_seeked_to;
        core::Signal<void> on_end_of_stream;
        core::Signal<core::ubuntu::media::Player::PlaybackStatus> on_playback_status_changed;
        core::Signal<core::ubuntu::media::Player::Orientation> on_orientation_changed;
        core::Signal<core::ubuntu::media::video::Dimensions> on_video_dimensions_changed;
        core::Signal<void> client_disconnected;
        core::Signal<int64_t> on_buffering_changed;
    } signals;
    bool is_missing_audio_codec;
    bool is_missing_video_codec;
    gint audio_stream_id;
    gint video_stream_id;
};
}

#endif // GSTREAMER_PLAYBIN_H_
