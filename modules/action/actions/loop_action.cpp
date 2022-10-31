#include "loop_action.h"
#include <tbox/base/json.hpp>
#include <tbox/base/assert.h>

namespace tbox {
namespace action {

LoopAction::LoopAction(event::Loop &loop, const std::string &id, Action *child, Mode mode) :
  Action(loop, id),
  child_(child),
  mode_(mode)
{
  assert(child != nullptr);

  child_->setFinishCallback(
    [this] (bool is_succ) {
      if ((mode_ == Mode::kUntilSucc && is_succ) ||
          (mode_ == Mode::kUntilFail && !is_succ)) {
        finish(true);
      } else {
        child_->start();
      }
    }
  );
}

LoopAction::~LoopAction() {
  delete child_;
}

void LoopAction::toJson(Json &js) const {
  Action::toJson(js);
  child_->toJson(js["child"]);
}

bool LoopAction::onStart() {
  return child_->start();
}

bool LoopAction::onPause() {
  return child_->pause();
}

bool LoopAction::onResume() {
  return child_->resume();
}

bool LoopAction::onStop() {
  return child_->stop();
}

}
}