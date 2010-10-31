#include <unistd.h>
#include <string.h>
#include "event.h"
#include "dispatcher.h"

namespace eventrpc {

void Event::Close() {
  if (fd_ != -1) {
    dispatcher_->DelEvent(this);
    close(fd_);
    fd_ = -1;
    event_flags_ = -1;
  }
}

bool Event::UpdateEvent(short new_event_flags) {
  if (event_flags_ == new_event_flags) {
    return true;
  }
  if (event_flags_ != -1 && !dispatcher_->DelEvent(this)) {
    return false;
  }
  event_flags_ = new_event_flags;
  if (!dispatcher_->AddEvent(this)) {
    return false;
  }
  return true;
}

};
