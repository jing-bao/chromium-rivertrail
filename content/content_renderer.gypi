# Copyright (c) 2011 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'dependencies': [
    '../jingle/jingle.gyp:jingle_glue',
    '../net/net.gyp:net',
    '../ppapi/ppapi_internal.gyp:ppapi_proxy',
    '../ppapi/ppapi_internal.gyp:ppapi_shared',
    '../skia/skia.gyp:skia',
    '../third_party/ffmpeg/ffmpeg.gyp:ffmpeg',
    '../third_party/icu/icu.gyp:icuuc',
    '../third_party/icu/icu.gyp:icui18n',
    '../third_party/libjingle/libjingle.gyp:libjingle',
    '../third_party/libjingle/libjingle.gyp:libjingle_p2p',
    '../third_party/npapi/npapi.gyp:npapi',
    '../third_party/WebKit/Source/WebKit/chromium/WebKit.gyp:webkit',
    '../ui/gfx/surface/surface.gyp:surface',
    '../v8/tools/gyp/v8.gyp:v8',
    '../webkit/support/webkit_support.gyp:webkit_media',
    '../webkit/support/webkit_support.gyp:webkit_gpu',
  ],
  'include_dirs': [
    '..',
  ],
  'sources': [
    'public/renderer/content_renderer_client.h',
    'public/renderer/document_state.cc',
    'public/renderer/document_state.h',
    'public/renderer/navigation_state.cc',
    'public/renderer/navigation_state.h',
    'public/renderer/render_process_observer.cc',
    'public/renderer/render_process_observer.h',
    'public/renderer/render_thread.cc',
    'public/renderer/render_thread.h',
    'public/renderer/render_view.h',
    'public/renderer/render_view_observer.cc',
    'public/renderer/render_view_observer.h',
    'public/renderer/render_view_observer_tracker.h',
    'public/renderer/render_view_visitor.h',
    'public/renderer/v8_value_converter.h',
    'renderer/active_notification_tracker.cc',
    'renderer/active_notification_tracker.h',
    'renderer/device_orientation_dispatcher.cc',
    'renderer/device_orientation_dispatcher.h',
    'renderer/devtools_agent.cc',
    'renderer/devtools_agent.h',
    'renderer/devtools_agent_filter.cc',
    'renderer/devtools_agent_filter.h',
    'renderer/devtools_client.cc',
    'renderer/devtools_client.h',
    'renderer/external_popup_menu.cc',
    'renderer/external_popup_menu.h',
    'renderer/gamepad_shared_memory_reader.cc',
    'renderer/gamepad_shared_memory_reader.h',
    'renderer/geolocation_dispatcher.cc',
    'renderer/geolocation_dispatcher.h',
    'renderer/gpu/compositor_thread.cc',
    'renderer/gpu/compositor_thread.h',
    'renderer/gpu/input_event_filter.cc',
    'renderer/gpu/input_event_filter.h',
    'renderer/gpu/gpu_channel_host.cc',
    'renderer/gpu/gpu_channel_host.h',
    'renderer/gpu/gpu_video_decode_accelerator_host.cc',
    'renderer/gpu/gpu_video_decode_accelerator_host.h',
    'renderer/gpu/renderer_gl_context.cc',
    'renderer/gpu/renderer_gl_context.h',
    'renderer/gpu/transport_texture_host.cc',
    'renderer/gpu/transport_texture_host.h',
    'renderer/gpu/transport_texture_service.cc',
    'renderer/gpu/transport_texture_service.h',
    'renderer/gpu/webgraphicscontext3d_command_buffer_impl.cc',
    'renderer/gpu/webgraphicscontext3d_command_buffer_impl.h',
    'renderer/idle_user_detector.cc',
    'renderer/idle_user_detector.h',
    'renderer/indexed_db_dispatcher.cc',
    'renderer/indexed_db_dispatcher.h',
    'renderer/intents_dispatcher.cc',
    'renderer/intents_dispatcher.h',
    'renderer/java/java_bridge_channel.cc',
    'renderer/java/java_bridge_channel.h',
    'renderer/java/java_bridge_dispatcher.cc',
    'renderer/java/java_bridge_dispatcher.h',
    'renderer/load_progress_tracker.cc',
    'renderer/load_progress_tracker.h',
    'renderer/media/audio_device.cc',
    'renderer/media/audio_device.h',
    'renderer/media/audio_hardware.cc',
    'renderer/media/audio_hardware.h',
    'renderer/media/audio_input_device.cc',
    'renderer/media/audio_input_device.h',
    'renderer/media/audio_input_message_filter.cc',
    'renderer/media/audio_input_message_filter.h',
    'renderer/media/audio_message_filter.cc',
    'renderer/media/audio_message_filter.h',
    'renderer/media/audio_renderer_impl.cc',
    'renderer/media/audio_renderer_impl.h',
    'renderer/media/capture_video_decoder.cc',
    'renderer/media/capture_video_decoder.h',
    'renderer/media/media_stream_dispatcher.cc',
    'renderer/media/media_stream_dispatcher.h',
    'renderer/media/media_stream_dispatcher_eventhandler.h',
    'renderer/media/media_stream_impl.cc',
    'renderer/media/media_stream_impl.h',
    'renderer/media/render_media_log.cc',
    'renderer/media/render_media_log.h',
    'renderer/media/rtc_video_decoder.cc',
    'renderer/media/rtc_video_decoder.h',
    'renderer/media/video_capture_impl.cc',
    'renderer/media/video_capture_impl.h',
    'renderer/media/video_capture_impl_manager.cc',
    'renderer/media/video_capture_impl_manager.h',
    'renderer/media/video_capture_message_filter.cc',
    'renderer/media/video_capture_message_filter.h',
    'renderer/mhtml_generator.cc',
    'renderer/mhtml_generator.h',
    'renderer/notification_provider.cc',
    'renderer/notification_provider.h',
    'renderer/paint_aggregator.cc',
    'renderer/paint_aggregator.h',
    'renderer/pepper_parent_context_provider.cc',
    'renderer/pepper_parent_context_provider.h',
    'renderer/pepper_platform_context_3d_impl.cc',
    'renderer/pepper_platform_context_3d_impl.h',
    'renderer/pepper_platform_video_decoder_impl.cc',
    'renderer/pepper_platform_video_decoder_impl.h',
    'renderer/pepper_plugin_delegate_impl.cc',
    'renderer/pepper_plugin_delegate_impl.h',
    'renderer/plugin_channel_host.cc',
    'renderer/plugin_channel_host.h',
    'renderer/render_process.h',
    'renderer/render_process_impl.cc',
    'renderer/render_process_impl.h',
    'renderer/render_thread_impl.cc',
    'renderer/render_thread_impl.h',
    'renderer/render_view_impl.cc',
    'renderer/render_view_impl.h',
    'renderer/render_view_linux.cc',
    'renderer/render_view_selection.cc',
    'renderer/render_view_selection.h',
    'renderer/render_widget.cc',
    'renderer/render_widget.h',
    'renderer/render_widget_fullscreen.cc',
    'renderer/render_widget_fullscreen.h',
    'renderer/render_widget_fullscreen_pepper.cc',
    'renderer/render_widget_fullscreen_pepper.h',
    'renderer/renderer_accessibility.cc',
    'renderer/renderer_accessibility.h',
    'renderer/renderer_clipboard_client.cc',
    'renderer/renderer_clipboard_client.h',
    'renderer/renderer_gpu_video_decoder_factories.cc',
    'renderer/renderer_gpu_video_decoder_factories.h',
    'renderer/renderer_main.cc',
    'renderer/renderer_main_platform_delegate.h',
    'renderer/renderer_main_platform_delegate_linux.cc',
    'renderer/renderer_main_platform_delegate_mac.mm',
    'renderer/renderer_main_platform_delegate_win.cc',
    'renderer/renderer_webapplicationcachehost_impl.cc',
    'renderer/renderer_webapplicationcachehost_impl.h',
    'renderer/renderer_webaudiodevice_impl.cc',
    'renderer/renderer_webaudiodevice_impl.h',
    'renderer/renderer_webcookiejar_impl.cc',
    'renderer/renderer_webcookiejar_impl.h',
    'renderer/renderer_webidbcursor_impl.cc',
    'renderer/renderer_webidbcursor_impl.h',
    'renderer/renderer_webidbdatabase_impl.cc',
    'renderer/renderer_webidbdatabase_impl.h',
    'renderer/renderer_webidbfactory_impl.cc',
    'renderer/renderer_webidbfactory_impl.h',
    'renderer/renderer_webidbindex_impl.cc',
    'renderer/renderer_webidbindex_impl.h',
    'renderer/renderer_webidbobjectstore_impl.cc',
    'renderer/renderer_webidbobjectstore_impl.h',
    'renderer/renderer_webidbtransaction_impl.cc',
    'renderer/renderer_webidbtransaction_impl.h',
    'renderer/renderer_webkitplatformsupport_impl.cc',
    'renderer/renderer_webkitplatformsupport_impl.h',
    'renderer/renderer_webstoragearea_impl.cc',
    'renderer/renderer_webstoragearea_impl.h',
    'renderer/renderer_webstoragenamespace_impl.cc',
    'renderer/renderer_webstoragenamespace_impl.h',
    'renderer/speech_input_dispatcher.cc',
    'renderer/speech_input_dispatcher.h',
    'renderer/text_input_client_observer.cc',
    'renderer/text_input_client_observer.h',
    'renderer/v8_value_converter_impl.cc',
    'renderer/v8_value_converter_impl.h',
    'renderer/webplugin_delegate_proxy.cc',
    'renderer/webplugin_delegate_proxy.h',
    'renderer/websharedworker_proxy.cc',
    'renderer/websharedworker_proxy.h',
    'renderer/websharedworkerrepository_impl.cc',
    'renderer/websharedworkerrepository_impl.h',
    'renderer/web_ui_bindings.cc',
    'renderer/web_ui_bindings.h',
  ],
  'conditions': [
    ['p2p_apis==1', {
      'sources': [
        'renderer/p2p/host_address_request.cc',
        'renderer/p2p/host_address_request.h',
        'renderer/p2p/ipc_network_manager.cc',
        'renderer/p2p/ipc_network_manager.h',
        'renderer/p2p/ipc_socket_factory.cc',
        'renderer/p2p/ipc_socket_factory.h',
        'renderer/p2p/p2p_transport_impl.cc',
        'renderer/p2p/p2p_transport_impl.h',
        'renderer/p2p/port_allocator.cc',
        'renderer/p2p/port_allocator.h',
        'renderer/p2p/socket_client.cc',
        'renderer/p2p/socket_client.h',
        'renderer/p2p/socket_dispatcher.cc',
        'renderer/p2p/socket_dispatcher.h',
      ],
    }],
    ['enable_gpu==1', {
      'dependencies': [
        '../gpu/gpu.gyp:gles2_c_lib',
      ],
      'sources': [
        'renderer/gpu/command_buffer_proxy.cc',
        'renderer/gpu/command_buffer_proxy.h',
      ],
    }],
    ['toolkit_uses_gtk == 1', {
      'conditions': [
        [ 'linux_use_tcmalloc==1', {
          'dependencies': [
              '../base/allocator/allocator.gyp:allocator',
          ],
        }],
        ['input_speech==0', {
          'sources!': [
            'renderer/speech_input_dispatcher.cc',
            'renderer/speech_input_dispatcher.h',
          ]
        }],
        ['notifications==0', {
          'sources!': [
            'renderer/notification_provider.cc',
            'renderer/active_notification_tracker.cc',
          ],
        }],
      ],
      'dependencies': [
        '../build/linux/system.gyp:gtk',
      ],
    }],
    ['OS=="mac"', {
      'sources!': [
        'common/process_watcher_posix.cc',
      ],
    }],
    ['OS=="win" and win_use_allocator_shim==1', {
      'dependencies': [
          '../base/allocator/allocator.gyp:allocator',
      ],
    }],
    ['enable_webrtc==1', {
      'dependencies': [
        '../third_party/webrtc/modules/modules.gyp:video_capture_module',
        '../third_party/webrtc/system_wrappers/source/system_wrappers.gyp:system_wrappers',
        '../third_party/webrtc/video_engine/video_engine.gyp:video_engine_core',
        '../third_party/webrtc/voice_engine/voice_engine.gyp:voice_engine_core',
      ],
      'sources': [
        'renderer/media/video_capture_module_impl.cc',
        'renderer/media/video_capture_module_impl.h',
        'renderer/media/webrtc_audio_device_impl.cc',
        'renderer/media/webrtc_audio_device_impl.h',
      ],
    }],
    ['java_bridge==1', {
      'defines': [
        'ENABLE_JAVA_BRIDGE',
      ],
    }, {
      'sources!': [
        'renderer/java/java_bridge_channel.cc',
        'renderer/java/java_bridge_channel.h',
        'renderer/java/java_bridge_dispatcher.cc',
        'renderer/java/java_bridge_dispatcher.h',
      ],
    }],
  ],
}
