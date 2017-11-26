#!/usr/bin/python3

import numpy
import random
import tensorflow as tf

import data_reader

kTrainingPercent = 0.5

def model_fn(features, labels, mode):
  input_layer = features["x"]
  labels = tf.reshape(labels, [-1, 1])

  dense1 = tf.layers.dense(
      name='dense1',
      inputs=input_layer,
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
