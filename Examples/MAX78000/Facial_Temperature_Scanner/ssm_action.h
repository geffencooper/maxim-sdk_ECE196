#ifndef SSM_ACTION_H
#define SSM_ACTION_H

// This file just defines a function type that will be used
// by external files and functions in order to interact with the
// scanner state machine. This enables external events to cause
// transitions without providing direct access to the state machine data

// the way peripherals trigger actions in the state machine
typedef void (*ssm_action_fn) (void);

#endif