import numpy as np

class Block(object):
    """The Block container"""
    def __init__(self, _label = "EMPTY", _value = 0):
        super(Block, self).__init__()
        self.__stack = [];
        self.__value = _value;
        self.__label  = str(_label).upper();

    def push(self, _label, _value = None):
        self.__stack.append((self.__label, self.__value))
        l, v = _label, _value;
        if(isinstance(_label, Block)): l = _label.label; v = _label.value;
        self.__value = v;
        self.__label  = str(l).upper();

    def pop(self):
        out = (self.__label, self.__value)
        if(len(self.__stack) != 0):
            _in = self.__stack.pop();
            self.__label  = _in[0];
            self.__value = _in[1];
        return out

    @property
    def value(self):
        return self.__value

    @property
    def label(self):
        return self.__label

    def __str__(self):
        return str(("LABEL", self.label, "VAL", self.value, "STACK#", len(self.__stack)))

class Agent(Block):
    """Agent"""
    def __init__(self):
        super(Agent, self).__init__('AGENT', np.inf)

class Goal(Block):
    """Goal"""
    def __init__(self):
        super(Goal, self).__init__('GOAL', 10)

class Wall(Block):
    """Wall"""
    def __init__(self):
        super(Wall, self).__init__('WALL', -1)

    def push(self, _label, _value = None):
        raise BlockedMove("Cannot push anything to wall!!");