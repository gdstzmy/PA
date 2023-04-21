#include <common.h>
#include <proc.h>


static Context* do_event(Event e, Context* c) {
  Log("begin do event");
  switch (e.event) {
    case EVENT_YIELD:
    Log("do_event:EVENT_YIELD\n");
    break;

    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
