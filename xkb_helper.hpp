#pragma once

#include <cpp_helper.hpp>

#include <xkbcommon/xkbcommon.h>

namespace xkb_helper {

using cpp_helper::configure;

template <typename T>
class add_context : public T {
public:
  using parent = T;
  add_context(const configure auto& conf) : parent{conf} {
      context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
  }
  ~add_context() {
      xkb_context_unref(context);
  }
  auto get_context() { return context; }

private:
  xkb_context* context;
};

template<typename T>
class add_keymap : public T {
public:
    using parent = T;
    add_keymap(const configure auto& conf) : parent{conf} {
    }
    ~add_keymap() {
        xkb_keymap_unref(keymap);
    }
    auto use_keymap_string(const char* keymap_string) {
        auto ctx = parent::get_context();
        xkb_keymap_unref(keymap);
        /* From the wl_keyboard::keymap event. */
        keymap = xkb_keymap_new_from_string(ctx, keymap_string,
                                            XKB_KEYMAP_FORMAT_TEXT_V1,
                                            XKB_KEYMAP_COMPILE_NO_FLAGS);
    }
    auto get_keymap() {
        return keymap;
    }
private:
    xkb_keymap* keymap;
};

template<typename T>
class add_state : public T {
public:
    using parent = T;
    add_state(const configure auto& conf) : parent{conf} {
    }
    ~add_state() {
        xkb_state_unref(state);
    }
    auto use_keymap_string(const char* keymap_string) {
        parent::use_keymap_string(keymap_string);
        auto keymap = parent::get_keymap();
        xkb_state_unref(state);
        state = xkb_state_new(keymap);
    }
    auto get_state() {
        return state;
    }
    auto get_keysym(xkb_keycode_t keycode) {
        xkb_keysym_t keysym;

        keysym = xkb_state_key_get_one_sym(state, keycode);
        return keysym;
    }
private:
    xkb_state* state;
};

template<typename T>
concept key_event_processable = requires (T t) {
    t.process_key_event(0,0);
};
template<typename T>
concept keysym_event_processable = requires (T t) {
    t.process_keysym_event(0,0);
};
template<typename T>
concept keymap_processable = requires (T t) {
    t.process_keymap(0,0);
};
template<typename T>
concept pointer_motion_event_processable = requires (T t) {
    t.process_pointer_motion_event(0,0);
};
template<typename T>
concept pointer_button_event_processable = requires (T t) {
    t.process_pointer_button_event(0,0);
};

template<typename T>
class add_process_keymap : public T {
public:
    using parent = T;
    add_process_keymap(const configure auto& conf) : parent{conf} {
    }

    auto process_keymap(int fd, int size) {
        char* keymap_string = reinterpret_cast<char*>(mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0));
        parent::use_keymap_string(keymap_string);
        munmap(keymap_string, size);
    }
};

template<typename T>
class add_process_key_event : public T {
public:
    using parent = T;
    add_process_key_event(const configure auto& conf) : parent{conf} {
    }
};
template<keysym_event_processable T>
class add_process_key_event<T> : public T {
public:
    using parent = T;
    add_process_key_event(const configure auto& conf) : parent{conf} {
    }
    auto process_key_event(int key, int state) {
        auto keysym = parent::get_keysym(key+8);
        parent::process_keysym_event(keysym, state);
    }
};

}
