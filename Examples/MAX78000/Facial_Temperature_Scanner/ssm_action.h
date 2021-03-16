// This file just defines a function type that will be used
// by external files and functions in order to interact with the
// scanner state machine. This enables external events to cause
// transitions without providing direct access to the state machine data.

// Ideally I would use an event queue and process events in the state machine
// but this is simpler and works fine for this project


#ifndef SSM_ACTION_H
#define SSM_ACTION_H

// the way peripherals trigger actions in the state machine
typedef void (*ssm_action_fn) (void);

#endif