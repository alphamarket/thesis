import numpy as np

def center_of_frame(frame):
	c = np.asarray(frame.shape[0:2]);
	return np.floor(c / 2);

def validate_hash(_hash, _keys):
	assert all(k in _hash for k in _keys), 'Expecting to have following keys %s but do not have them!' %str(_keys);