#!/usr/bin/python3

import numpy
import random
import tensorflow as tf

import data_reader

kTrainingPercent = 0.7
kThreads = 20

def model_fn(features, labels, mode):
  input_layer = tf.reshape(features["x"], [-1, 100])

  dense1 = tf.layers.dense(
      inputs=input_layer,
      units=20,
      activation=tf.nn.relu)

  dense2 = tf.layers.dense(
      inputs=dense1,
      units=20,
      activation=tf.nn.relu)

  dense3 = tf.layers.dense(
      inputs=dense2,
      units=20,
      activation=tf.nn.relu)

  final = tf.layers.dense(
      inputs=dense3,
      units=2,
      activation=None)

  predictions = {
      "classes": tf.argmax(input=final, axis=1),
      "probabilities": tf.nn.softmax(final, name="softmax_tensor")}

  onehot_labels = tf.one_hot(
      indices=tf.cast(labels, tf.int32),
      depth=2)
  loss = tf.losses.softmax_cross_entropy(
      onehot_labels=onehot_labels,
      logits=final)

  if mode == tf.estimator.ModeKeys.TRAIN:
    train_step = tf.train.AdamOptimizer(1e-4).minimize(
        loss,
        global_step=tf.train.get_global_step())

    return tf.estimator.EstimatorSpec(
        mode=mode,
        loss=loss,
        train_op=train_step)

  else:  # EVAL mode
    eval_metric_ops = {
        "accuracy": tf.metrics.accuracy(
          labels=labels, predictions=predictions["classes"])}

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

  classifier = tf.estimator.Estimator(
      model_fn=model_fn,
      model_dir="model_output")

  train_input_fn = tf.estimator.inputs.numpy_input_fn(
      x={"x": data_training},
      y=label_training,
      num_epochs=None,
      batch_size=32,
      shuffle=False)

  classifier.train(
      input_fn=train_input_fn,
      steps=20000)

  evaluate_input_gn = tf.estimator.inputs.numpy_input_fn(
      x={"x": data_validation},
      y=label_validation,
      num_epochs=1,
      shuffle=False)
  eval_result = classifier.evaluate(input_fn=evaluate_input_gn)
  print(eval_result)

if __name__ == "__main__":
  tf.logging.set_verbosity(tf.logging.INFO)

  tf.app.run()
