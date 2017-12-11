#!/usr/bin/python3

import numpy
import random
import tensorflow as tf

import data_reader
import model

kTrainingPercent = 0.5
kThreads = 20


def main(_):
  dr = data_reader.DataReader()
  data, label, _ = dr.parse()

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

  training_model = model.Model()
  def model_fn(features, labels, mode):
    training_model.set_mode(mode)
    return training_model.get_model(features, labels)

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
