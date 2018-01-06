#!/usr/bin/python3

import argparse
import numpy
import random
import tensorflow as tf

import data_reader
import model

kTrainingPercent = 0.5
kThreads = 20


def _get_estimator():
  sess_config = tf.ConfigProto(
      intra_op_parallelism_threads=kThreads,
      inter_op_parallelism_threads=kThreads)
  estimator_config = tf.estimator.RunConfig(session_config=sess_config)
  
  training_model = model.Model()
  def model_fn(features, labels, mode):
    training_model.set_mode(mode)
    return training_model.get_model(features, labels)

  return tf.estimator.Estimator(
      model_fn=model_fn,
      model_dir="model_output",
      config=estimator_config)
	  
def train_model(data_dir):
  dr = data_reader.DataReader(data_dir)
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

  estimator = _get_estimator()

  train_input_fn = tf.estimator.inputs.numpy_input_fn(
      x={"x": data_training},
      y=label_training,
      shuffle=True,
      num_epochs=None)
  train_spec = tf.estimator.TrainSpec(
      input_fn=train_input_fn,
      max_steps=5000000)

  evaluate_input_fn = tf.estimator.inputs.numpy_input_fn(
      x={"x": data_validation},
      y=label_validation,
      shuffle=False)
  eval_spec = tf.estimator.EvalSpec(
      input_fn=evaluate_input_fn,
      throttle_secs=30)

  tf.estimator.train_and_evaluate(estimator, train_spec, eval_spec)
	  
def export_saved_model():
  estimator = _get_estimator()
  
  # TODO: remove magic number of batch=32, size=140
  feature = {"x": tf.placeholder(dtype=tf.float32, shape=[32, 140])}
  
  # TODO: create export folder first
  estimator.export_savedmodel(
	  "./export",
	  tf.estimator.export.build_raw_serving_input_receiver_fn(feature))

def main():
  parser = argparse.ArgumentParser(description="Train Model")
  parser.add_argument('-d', '--data', nargs='?', required=True, help="Data folder name")
  parser.add_argument('--export', action='store_true', help="Export model")
  try:
    args = parser.parse_args()
  except Exception:
    parser.print_help()
    sys.exit(0)

  if args.export:
    export_saved_model()
  else:
    train_model(args.data)

if __name__ == "__main__":
  tf.logging.set_verbosity(tf.logging.INFO)
  main()
