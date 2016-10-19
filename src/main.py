from Q import *
from world import *
from exceps import *
from blocks import *

def action_handler(w, current_state, action_to_carry):
	actions = {
		1: 'UP',
		2: 'RIGHT',
		3: 'DOWN',
		4: 'LEFT'
	}
	if(action_to_carry not in actions.keys()):
		raise Exception("Invalid action call!")
	_dir = actions[action_to_carry];
	_fact = (np.sign(w.world[current_state]) * .1);

	if(_dir == 'UP'):
		state = (current_state[0] - 1, current_state[1])
		if(w.validate_state(state, raise_exception = False)):
			try:
				w.world[state].push(Agent())
				w.world[tuple(current_state)].pop();
				w.agent_state = state;
			except BlockedMove: pass
			return w.world[state].value;



w = Maze([6, 6], range(1, 5), action_handler, is_continues = False);

w.define_walls(
	[
		[1, 0], [1, 1], [1, 2],
		[2, 4], [2, 5],
		[4, 2], [4, 3], [4, 4]
	]);

w.define_goals(
	[
		[1, 5],
		[3, 5],
		[5, 0]
	])

w.define_agent([4, 2])

w.print_world()
print
print action_handler(w, w.agent_state, 1)

w.print_world()