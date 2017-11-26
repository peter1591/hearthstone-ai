#!/usr/bin/python3

import numpy
import random
import tensorflow as tf

import data_reader

kTrainingPercent = 0.5
kThreads = 20
kEnableCardIdEmbed = False
kEnableHandCardIdEmbed = False

class NextInputGetter:
  def __init__(self, data):
    self._idx = 0
    self._data = data

  def get_next_slice(self, size):
    ret = tf.slice(self._data, [0, self._idx], [-1, size])
    self._idx = self._idx + size
    return ret

  def get_rest(self):
    return tf.slice(self._data, [0, self._idx], [-1, -1])

def model_hero(inputs):
  hero1 = tf.layers.dense(
      name='hero1',
      inputs=inputs,
      units=2,
      reuse=tf.AUTO_REUSE,
      activation=tf.nn.relu)

  return hero1

def get_embedded_onboard_card_id(card_id):
  if not kEnableCardIdEmbed:
    return None

  kCardIdDimension = 3
  card_id = tf.to_int32(card_id, name='card_id_to_int32')
  with tf.variable_scope("onboard", reuse=tf.AUTO_REUSE):
    card_id_matrix = tf.get_variable(
        'card_embed_matrix',
        [data_reader.kMaxCardId, kCardIdDimension],
        initializer=tf.zeros_initializer())
  card_id_embed = tf.nn.embedding_lookup(
    card_id_matrix, card_id, name='card_id_embed')
  card_id_embed = tf.reshape(card_id_embed, [-1, kCardIdDimension])
  return tf.to_double(card_id_embed)

def get_embedded_hand_card_id(card_id):
  if not kEnableHandCardIdEmbed:
    return None

  kCardIdDimension = 3
  card_id = tf.to_int32(card_id, name='card_id_to_int32')
  with tf.variable_scope("current_hand", reuse=tf.AUTO_REUSE):
    card_id_matrix = tf.get_variable(
        'card_embed_matrix',
        [data_reader.kMaxCardId, kCardIdDimension],
        initializer=tf.zeros_initializer())
  card_id_embed = tf.nn.embedding_lookup(
    card_id_matrix, card_id, name='card_id_embed')
  card_id_embed = tf.reshape(card_id_embed, [-1, kCardIdDimension])
  return tf.to_double(card_id_embed)

def model_minion(input_getter):
  inputs = []

  card_id = input_getter.get_next_slice(1)
  card_id_embed = get_embedded_onboard_card_id(card_id)
  if card_id_embed is not None:
    inputs.append(card_id_embed)

  features = input_getter.get_next_slice(data_reader.kMinionFeatures)
  inputs.append(features)

  minion1 = tf.layers.dense(
      name='minion1',
      inputs=tf.concat(inputs, 1),
      units=10,
      reuse=tf.AUTO_REUSE,
      activation=tf.nn.relu)

  minion2 = tf.layers.dense(
      name='minion2',
      inputs=minion1,
      units=10,
      reuse=tf.AUTO_REUSE,
      activation=tf.nn.relu)

  return minion2

def model_minions(input_getter):
  features = []
  for _ in range(data_reader.kMinions):
    features.append(model_minion(input_getter))

  minions1 = tf.layers.dense(
      name='minions1',
      inputs=tf.concat(features, 1),
      units=30,
      reuse=tf.AUTO_REUSE,
      activation=tf.nn.relu)

  return minions1

def model_current_hand_card(input_getter):
  inputs = []

  card_id = input_getter.get_next_slice(1)
  card_id_embed = get_embedded_hand_card_id(card_id)
  if card_id_embed is not None:
    inputs.append(card_id_embed)

  inputs.append(
      input_getter.get_next_slice(data_reader.kCurrentHandCardFeatures))

  card1 = tf.layers.dense(
      name='hand_card_1',
      inputs=tf.concat(inputs, 1),
      units=10,
      reuse=tf.AUTO_REUSE,
      activation=tf.nn.relu)

  card2 = tf.layers.dense(
      name='hand_card_2',
      inputs=card1,
      units=10,
      reuse=tf.AUTO_REUSE,
      activation=tf.nn.relu)

  return card2

def model_current_hand(input_getter):
  inputs = []
  inputs.append(input_getter.get_next_slice(data_reader.kCurrentHandFeatures))

  for _ in range(data_reader.kCurrentHandCards):
    inputs.append(model_current_hand_card(input_getter))

  hand1 = tf.layers.dense(
    name='hand_1',
    inputs=tf.concat(inputs, 1),
    units=10,
    reuse=tf.AUTO_REUSE,
    activation=tf.nn.relu)

  hand2 = tf.layers.dense(
    name='hand_2',
    inputs=hand1,
    units=10,
    reuse=tf.AUTO_REUSE,
    activation=tf.nn.relu)

  return hand2

def model_rest_features(inputs):
  rest1 = tf.layers.dense(
      name='rest1',
      inputs=inputs,
      units=20,
      activation=tf.nn.relu)

  rest2 = tf.layers.dense(
      name='rest2',
      inputs=rest1,
      units=20,
      activation=tf.nn.relu)

  return rest2

def model_fn(features, labels, mode):
  input_getter = NextInputGetter(features["x"])

  current_hero = model_hero(
      input_getter.get_next_slice(data_reader.kHeroFeatures))
  opponent_hero = model_hero(
      input_getter.get_next_slice(data_reader.kHeroFeatures))
  current_minions = model_minions(input_getter)
  opponent_minions = model_minions(input_getter)
  current_hand = model_current_hand(input_getter)
  rest = model_rest_features(input_getter.get_rest())

  dense1_input_features = [
      current_hero,
      opponent_hero,
      current_minions,
      opponent_minions,
      current_hand,
      rest]

  dense1_input = tf.concat(dense1_input_features, 1)

  dense1 = tf.layers.dense(
      name='dense1',
      inputs=dense1_input,
      units=30,
      activation=tf.nn.relu)

  dense2 = tf.layers.dense(
      name='dense2',
      inputs=dense1,
      units=30,
      activation=tf.nn.relu)

  final = tf.layers.dense(
      name='final',
      inputs=dense2,
      units=1,
      activation=None)

  predictions = {
      "classes": tf.argmax(input=final, axis=1),
      "probabilities": tf.nn.softmax(final, name="softmax_tensor")}

  labels = tf.reshape(labels, [-1, 1])
  loss = tf.losses.mean_squared_error(
      labels=labels,
      predictions=final)

  if mode == tf.estimator.ModeKeys.TRAIN:
    train_step = tf.train.AdamOptimizer(1e-4).minimize(
        loss,
        global_step=tf.train.get_global_step())

    return tf.estimator.EstimatorSpec(
        mode=mode,
        loss=loss,
        train_op=train_step)

  else:  # EVAL mode
    binary_labels = tf.equal(labels, data_reader.kLabelIfFirstPlayerWin)
    predictions = tf.greater(final,
        data_reader.kLabelFirstPlayerWinIfGreaterThan)
    eval_metric_ops = {
        "accuracy": tf.metrics.accuracy(
          labels=binary_labels,
          predictions=predictions)}

    return tf.estimator.EstimatorSpec(
        mode=mode, loss=loss,
        eval_metric_ops=eval_metric_ops)

def main(_):
  dr = data_reader.DataReader()
  data, label = dr.parse()

  data_label = list(zip(data, label))
  random.shuffle(data_label)
  data, label = zip(*data_label)

  data = numpy.array(data)
  label = numpy.array(label)

  rows = len(data)
  rows_training = int(rows * kTrainingPercent)
  data_training = data[:rows_training]
  label_training = label[:rows_training]
  data_validation = data[rows_training:]
  label_validation = label[rows_training:]

  sess_config = tf.ConfigProto(
      intra_op_parallelism_threads=kThreads,
      inter_op_parallelism_threads=kThreads)
  estimator_config = tf.estimator.RunConfig(session_config=sess_config)

  estimator = tf.estimator.Estimator(
      model_fn=model_fn,
      model_dir="model_output",
      config=estimator_config)

  train_input_fn = tf.estimator.inputs.numpy_input_fn(
      x={"x": data_training},
      y=label_training,
      shuffle=True)
  train_spec = tf.estimator.TrainSpec(
      input_fn=train_input_fn,
      max_steps=5000000)

  evaluate_input_fn = tf.estimator.inputs.numpy_input_fn(
      x={"x": data_validation},
      y=label_validation,
      shuffle=False)
  eval_spec = tf.estimator.EvalSpec(
      input_fn=evaluate_input_fn)

  tf.estimator.train_and_evaluate(estimator, train_spec, eval_spec)

if __name__ == "__main__":
  tf.logging.set_verbosity(tf.logging.INFO)

  tf.app.run()
