#!/usr/bin/python3

import os
import json


kLabelIfFirstPlayerLoss = -1
kLabelIfFirstPlayerWin = 1
kLabelFirstPlayerWinIfGreaterThan = 0

kHeroFeatures = 1
kMinionFeatures = 7  # exclude card id
kMinions = 7

kCurrentHandCards = 10
kCurrentHandCardFeatures = 1  # exclude card id
kCurrentHandFeatures = 1

kMaxCardId = 1860

class DataReader:
  def __init__(self, dirname='data'):
    self._dirname = dirname

  @classmethod
  def from_bool(cls, v):
    if v == True:
      return 1.0
    else:
      return -1.0

  def _read_hero_data(self, data):
    hp = data['hero']['hp']
    armor = data['hero']['armor']

    return [hp+armor]

  def _add_minion_data_placeholder(self):
    return [
      0,
      0,
      0,
      0,
      -1.0,
      -1.0,
      -1.0,
      -1.0]

  def _add_minion_data(self, minion):
    attackable = False
    try:
      attackable = minion['attackable']
    except Exception as _:
      attackable = False

    return [
      minion['card_id'],
      minion['hp'],
      minion['max_hp'],
      minion['attack'],
      self.from_bool(attackable),
      self.from_bool(minion['taunt']),
      self.from_bool(minion['shield']),
      self.from_bool(minion['stealth'])]

  def _read_minions_data(self, minions):
    if minions is None:
      minions = []

    count = len(minions)
    data = []

    for idx in range(0, 7):
      if idx < count:
        data.extend(self._add_minion_data(minions[idx]))
      else:
        data.extend(self._add_minion_data_placeholder())

    return data

  def _add_resource(self, resource):
    return [
        resource['current'],
        resource['total'],
        resource['overload_next']]

  def _add_current_hand(self, hand):
    data = []

    if hand is None:
      hand = {}

    playable = 0
    for card in hand:
      if card['playable']:
        playable = playable + 1
    data.append(playable)

    for card in hand:
      data.append(card['card_id'])
      data.append(card['cost'])
    for _ in range(10 - len(hand)):
      data.extend([0, -1])

    return data

  def _add_opponent_hand(self, hand):
    if hand is None:
      hand = {}

    return [
        len(hand)]

  def _add_heropower(self, hero_power):
    return [
        self.from_bool(hero_power['playable'])]

  def _read_board(self, board):
    data = []

    data.extend(self._read_hero_data(board['current_player']))
    data.extend(self._read_hero_data(board['opponent_player']))
    data.extend(self._read_minions_data(board['current_player']['minions']))
    data.extend(self._read_minions_data(board['opponent_player']['minions']))
    data.extend(self._add_current_hand(board['current_player']['hand']))
    data.extend(self._add_opponent_hand(board['opponent_player']['hand']))

    data.extend(self._add_resource(board['current_player']['resource']))
    data.extend(self._add_heropower(board['current_player']['hero_power']))

    return data

  def _get_label(self, board, first_player_win):
    current_player_is_first = (board['current_player_id'] == 'kFirstPlayer')
    assert current_player_is_first is not None
    assert first_player_win is not None

    if current_player_is_first == first_player_win:
      return kLabelIfFirstPlayerWin
    else:
      return kLabelIfFirstPlayerLoss

  def _read_one_json(self, all_data, all_label, json_data):
    first_player_win = None
    for block_data in json_data:
      action_type = block_data['type']
      if action_type == 'kEnd':
        first_player_win = (block_data['result'] == 'kResultWin')

    for block_data in json_data:
      action_type = block_data['type']
      if action_type == 'kMainAction':
        board = block_data['board']
        data = self._read_board(board)
        label = self._get_label(board, first_player_win)

        all_data.append(data)
        all_label.append(label)

  def parse(self):
    data = []
    label = []

    for (dirpath, _, filenames) in os.walk(self._dirname):
      for idx, filename in enumerate(filenames):
        fullpath = os.path.join(dirpath, filename)
        print("Reading file (%d / %d): %s " % (idx+1, len(filenames), fullpath))
        with open(fullpath) as data_file:
          try:
            json_data = json.load(data_file)
          except Exception:
            print("Skipped file %s: Failed to read json" % fullpath)
            continue
          self._read_one_json(data, label, json_data)
      break

    return data, label
