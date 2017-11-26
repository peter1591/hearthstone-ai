#!/usr/bin/python3

import numpy
import random
import tensorflow as tf

import data_reader

kTrainingPercent = 0.5

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
      units=3,
      reuse=tf.AUTO_REUSE,
      activation=tf.nn.relu)

  hero2 = tf.layers.dense(
      name='hero2',
      inputs=hero1,
      units=3,
      reuse=tf.AUTO_REUSE,
      activation=tf.nn.relu)

  return hero2

def model_minion(inputs):
  minion1 = tf.layers.dense(
      name='minion1',
      inputs=inputs,
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
    inputs = input_getter.get_next_slice(data_reader.kMinionFeatures)
    features.append(model_minion(inputs))

  minions1 = tf.layers.dense(
      name='minions1',
      inputs=tf.concat(features, 1),
      units=10,
      reuse=tf.AUTO_REUSE,
      activation=tf.nn.relu)

  minions2 = tf.layers.dense(
      name='minions2',
      inputs=minions1,
      units=10,
      reuse=tf.AUTO_REUSE,
      activation=tf.nn.relu)

  return minions2

def model_fn(features, labels, mode):
  input_getter = NextInputGetter(features["x"])

  current_hero = input_getter.get_next_slice(data_reader.kHeroFeatures)
  opponent_hero = input_getter.get_next_slice(data_reader.kHeroFeatures)
  current_minions = model_minions(input_getter)
  opponent_minions = model_minions(input_getter)
  rest = input_getter.get_rest()

  current_hero_features = model_hero(current_hero)
  opponent_hero_features = model_hero(opponent_hero)

  dense1_input_features = [
      current_hero_features,
      opponent_hero_features,
      current_minions,
      opponent_minions,
      rest]

  dense1_input = tf.concat(dense1_input_features, 1)

  dense1 = tf.layers.dense(
      name='dense1',
      inputs=dense1_input,
      units=20,
      activation=tf.nn.relu)

  dense2 = tf.layers.dense(
      name='dense2',
      inputs=dense1,
      units=20,
      activation=tf.nn.relu)

  dense3 = tf.layers.dense(
      name='dense3',
      inputs=dense2,
      units=20,
      activation=tf.nn.relu)

  final = tf.layers.dense(
      name='final',
      inputs=dense3,
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

  estimator = tf.estimator.Estimator(
      model_fn=model_fn,
      model_dir="model_output")

  train_input_fn = tf.estimator.inputs.numpy_input_fn(
      x={"x": data_training},
      y=label_training,
      shuffle=True)
  train_spec = tf.estimator.TrainSpec(
      input_fn=train_input_fn,
      max_steps=50000)

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
