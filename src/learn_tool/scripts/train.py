import argparse
import math
import sys
import numpy as np
import tensorflow as tf

data_file='/local/scratch/markus/tmp/train_data_2.dat'

MAX_MOVES = 1009
NU_FEATURES = 28

np.set_printoptions(threshold=np.nan)

data = np.fromfile(data_file, dtype=np.float32)
data = data.reshape(-1, MAX_MOVES * (NU_FEATURES + 1))
inputs = data[:, 0 : MAX_MOVES * NU_FEATURES]
inputs = inputs.reshape(-1, MAX_MOVES, NU_FEATURES)
labels = data[:, MAX_MOVES * NU_FEATURES : MAX_MOVES * NU_FEATURES + MAX_MOVES]
labels = labels.reshape(-1, MAX_MOVES, 1)

def create_model():
    m = tf.placeholder(tf.float32, shape=[None, MAX_MOVES, NU_FEATURES], name='input')
    tf.placeholder(tf.float32, shape=[None, MAX_MOVES, 1], name='label')
    weights = tf.Variable(tf.random_normal([1, NU_FEATURES, 1]), name='weights')
    m = tf.nn.conv1d(m, weights, stride=1, padding='VALID', name='conv')
    return m

model = create_model()
cost = tf.reduce_mean(
    tf.nn.softmax_cross_entropy_with_logits(logits=model,
        labels=tf.get_default_graph().get_tensor_by_name('label:0')))
optimizer = tf.train.AdamOptimizer(learning_rate=0.001).minimize(cost)

init = tf.global_variables_initializer()
with tf.Session() as sess:
    sess.run(init)
    print sess.run('weights:0')
    for step in xrange(0, 10001):
        loss, _ = sess.run([cost, optimizer],
                           feed_dict={'input:0': inputs, 'label:0': labels})
        print step, loss
    print sess.run('weights:0')
