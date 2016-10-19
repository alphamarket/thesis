import numpy as np
from exceps import *
from blocks import *

class World(object):
    """The world container"""
    def __init__(self, state_config, action_lists, action_handler, is_continues):
        super(World, self).__init__()

        self.agent_state = [];
        self.state_config = state_config;
        self.action_lists = action_lists;
        self.is_continues = is_continues;
        self.action_handler = action_handler

        self.world = np.empty(shape = state_config, dtype = Block);
        for state, value in np.ndenumerate(self.world): self.world[state] = Block();

    def validate_state(self, state, raise_exception = True):
        if(any(np.asarray(state) >= self.world.shape) or any(np.asarray(state) < 0)):
            if(raise_exception): raise Exception("State overflow!");
            else: return False;
        return True;

    def define_value(self, state, value):
        state, value = np.asarray(state), np.asarray(value)

        if(len(value.shape) == 0):
            value = np.asarray([value]);
            assert len(state) == 2, "The value# and state# does not match!!"
            state = np.asarray([state])

        assert state.shape[0] == value.shape[0], "The value# and state# does not match!!"

        for s, v in zip(state, value):
            self.validate_state(s)
            self.world[tuple(s)] = v;
        return self;

    def define_agent(self, state):
        self.define_value(state, Agent())
        self.agent_state = state;

    def print_world(self, values_only = True):
        for x in xrange(self.world.shape[0]):
            for y in xrange(self.world.shape[1]):
                if(values_only):
                    print self.world[(x,y)].value,
                else: print self.world[(x,y)],
            print


class Maze(World):
    """The maze world"""
    def __init__(self, state_config, action_lists, action_handler, is_continues):
        super(Maze, self).__init__(state_config, action_lists, action_handler, is_continues)

    def define_walls(self, states):
        self.define_value(states, [Wall() for i in xrange(np.asarray(states).shape[0])]);

    def define_goals(self, states):
        self.define_value(states, [Goal() for i in xrange(np.asarray(states).shape[0])]);
