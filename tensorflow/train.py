#!/usr/bin/python3

import numpy
import tensorflow as tf
import data_reader

tf.logging.set_verbosity(tf.logging.INFO)

kLabelIfFirstPlayerWin = 1
kLabelIfFirstPlayerLoss = -1

def cnn_model_fn(features, labels, mode):
  pass

def main():
  pass


if __name__ == "__main__":
  data_reader = data_reader.DataReader()
  data, label = data_reader.parse()

  data = numpy.array(data)
  label = numpy.array(label)

  feature_columns = tf.feature_column.numeric_column("x", shape=[100])

  classifier = tf.estimator.DNNClassifier(
      feature_columns=feature_columns,
      hidden_units=[10,20,10],
      n_classes=2,
      model_dir="model_output")

  train_input_fn = tf.estimator.inputs.numpy_input_fn(
      x={"x": numpy.array(data)},
      y=numpy.array(label),
      num_epochs=None,
      shuffle=True)

  classifier.train(input_fn=train_input_fn, steps=2000)

  tf.app.run()
