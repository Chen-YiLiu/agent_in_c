'''
An app that runs the openai gym environment and a TCP server.
Any reinforcwment learning agent can connect to this server via TCP
to read the observation of the environment and to send actions.

Author: Chen-Yi Liu
May, 2017
'''

import gym
import struct
import numpy as np
import time
from Server_Socket import Server_Socket

# define some constants
MAX_EPISODES = 500
MAX_STEPS_EPISODE = 500
STATE_DIM = 3
ACTION_DIM = 1
ENVIRONMENT_NAME = 'Pendulum-v0'
NOISE_MEAN = 0.0
NOISE_SD = 1.0

# start a server on port 1000
myserver = Server_Socket(port = 1000)
myserver.accept()

# a tool to dissect a contiguous float values to individual float values
converter = struct.Struct('=f')

# a bytearray to pack the state values to be sent over tcp socket
# the length is the number of values times the number of bytes in a float (i.e. 4)
# the number of values is determined by the dimension of the observation space plus a reward value
state_binary = bytearray((STATE_DIM + 1) * 4)

env = gym.make(ENVIRONMENT_NAME)

for episode in range(MAX_EPISODES):
  
  # start a new episode
  observation = env.reset()
  reward = 0.0

  # generate some noise in the action
  action_noise = np.random.normal(NOISE_MEAN, NOISE_SD, MAX_STEPS_EPISODE)

  print('Episode', episode)

  for step in range(MAX_STEPS_EPISODE):

    # draw the environment on the screen
    env.render()

    # pack a list of float values to a bytearray
    for i in range(STATE_DIM):
      converter.pack_into(state_binary, i*converter.size, observation[i])
    converter.pack_into(state_binary, STATE_DIM*converter.size, reward)

    # send the observation of the environment to the agent on the client side
    myserver.send(state_binary, len(state_binary))

    # get actions from the client
    action_binary = myserver.receive(ACTION_DIM * 4) # 4 is the number of bytes in a float

    # extract action values from a bytearray
    action = []
    for i in range(ACTION_DIM):
      action.append(converter.unpack_from(action_binary, offset= i * converter.size)[0])

    # execute the agent's action in the environment
    observation, reward, done, info = env.step(action + action_noise[step])

    # print out the observation and reward for debugging
    #print(observation, reward)

    # wait awhile before going further so that I can read the values on the screen
    #time.sleep(1.0)

    if done:
      print('Ended after', step+1, 'steps.')
      break


# close the server socket and stop any existing communication
myserver.close()



















