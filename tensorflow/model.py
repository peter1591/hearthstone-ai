#!/usr/bin/python3

import os
import json
import tensorflow as tf

import data_reader


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

class Model:
  def __init__(self):
    self.kEnableCardIdEmbed = False
    self.kEnableHandCardIdEmbed = False
    self.kCardIdDimension = 3

  def _model_hero(self, inputs):
    hero1 = tf.layers.dense(
        name='hero1',
        inputs=inputs,
        units=2,
        reuse=tf.AUTO_REUSE,
        activation=tf.nn.relu)

    return hero1

  def _get_embedded_onboard_card_id(self, card_id):
    if not self.kEnableCardIdEmbed:
      return None

    card_id = tf.to_int32(card_id, name='card_id_to_int32')
    with tf.variable_scope("onboard", reuse=tf.AUTO_REUSE):
      card_id_matrix = tf.get_variable(
          'card_embed_matrix',
          [data_reader.kMaxCardId, self.kCardIdDimension],
          initializer=tf.zeros_initializer())
    card_id_embed = tf.nn.embedding_lookup(
      card_id_matrix, card_id, name='card_id_embed')
    card_id_embed = tf.reshape(card_id_embed, [-1, self.kCardIdDimension])
    return tf.to_double(card_id_embed)

  def _model_minion(self, input_getter):
    inputs = []

    card_id = input_getter.get_next_slice(1)
    card_id_embed = self._get_embedded_onboard_card_id(card_id)
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

  def _model_minions(self, input_getter):
    features = []
    for _ in range(data_reader.kMinions):
      features.append(self._model_minion(input_getter))

    minions1 = tf.layers.dense(
        name='minions1',
        inputs=tf.concat(features, 1),
        units=30,
        reuse=tf.AUTO_REUSE,
        activation=tf.nn.relu)

    return minions1

  def _get_embedded_hand_card_id(self, card_id):
    if not self.kEnableHandCardIdEmbed:
      return None

    card_id = tf.to_int32(card_id, name='card_id_to_int32')
    with tf.variable_scope("current_hand", reuse=tf.AUTO_REUSE):
      card_id_matrix = tf.get_variable(
          'card_embed_matrix',
          [data_reader.kMaxCardId, self.kCardIdDimension],
          initializer=tf.zeros_initializer())
    card_id_embed = tf.nn.embedding_lookup(
      card_id_matrix, card_id, name='card_id_embed')
    card_id_embed = tf.reshape(card_id_embed, [-1, self.kCardIdDimension])
    return tf.to_double(card_id_embed)

  def _model_current_hand_card(self, input_getter):
    inputs = []

    card_id = input_getter.get_next_slice(1)
    card_id_embed = self._get_embedded_hand_card_id(card_id)
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

  def _model_current_hand(self, input_getter):
    inputs = []
    inputs.append(input_getter.get_next_slice(data_reader.kCurrentHandFeatures))

    for _ in range(data_reader.kCurrentHandCards):
      inputs.append(self._model_current_hand_card(input_getter))

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

  def _model_rest_features(self, inputs):
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

  def get_model(self, features, labels, mode):
    input_getter = NextInputGetter(features["x"])
    current_hero = self._model_hero(
        input_getter.get_next_slice(data_reader.kHeroFeatures))
    opponent_hero = self._model_hero(
        input_getter.get_next_slice(data_reader.kHeroFeatures))
    current_minions = self._model_minions(input_getter)
    opponent_minions = self._model_minions(input_getter)
    current_hand = self._model_current_hand(input_getter)
    rest = self._model_rest_features(input_getter.get_rest())

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
