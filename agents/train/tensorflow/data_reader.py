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

class DynamicMapper:
  def __init__(self, start_index = 0):
    self._dict_v_to_index = {}
    self._dict_index_to_v = {}
    self._next_index = start_index

  def _add_map(self, v):
    idx = self._next_index
    self._next_index = idx + 1

    self._dict_v_to_index[v] = idx
    self._dict_index_to_v[idx] = v

  def get_index(self, v):
    if v not in self._dict_v_to_index:
      self._add_map(v)
    return self._dict_v_to_index[v]

  def get_index_to_v_dict(self):
    return self._dict_index_to_v

class DataReader:
  def __init__(self, dirname):
    self._dirname = dirname
    self._data = []
    self._hand_card_mapper = DynamicMapper(1)

  @classmethod
  def from_bool(cls, v):
    if v == True:
      return 1.0
    else:
      return -1.0

  def _read_hero_data(self, data):
    hp = data['hero']['hp']
    armor = data['hero']['armor']

    self._data.extend([
      hp + armor])

  def _add_minion_data_placeholder(self):
    self._data.extend([
      0,
      0,
      0,
      0,
      -1.0,
      -1.0,
      -1.0,
      -1.0])

  def _add_minion_data(self, minion):
    attackable = False
    try:
      attackable = minion['attackable']
    except Exception as _:
      attackable = False

    self._data.extend([
      minion['card_id'],
      minion['hp'],
      minion['max_hp'],
      minion['attack'],
      self.from_bool(attackable),
      self.from_bool(minion['taunt']),
      self.from_bool(minion['shield']),
      self.from_bool(minion['stealth'])])

  def _read_minions_data(self, minions):
    if minions is None:
      minions = []

    count = len(minions)

    for idx in range(0, 7):
      if idx < count:
        self._add_minion_data(minions[idx])
      else:
        self._add_minion_data_placeholder()

  def _add_resource(self, resource):
    self._data.extend([
        resource['current'],
        resource['total'],
        resource['overload_next']])

  def _add_current_hand(self, hand):
    if hand is None:
      hand = {}

    playable = 0
    for card in hand:
      if card['playable']:
        playable = playable + 1
    self._data.append(playable)

    def add_data(card_id, cost):
      mapped_card_id = self._hand_card_mapper.get_index(card_id)
      self._data.append(mapped_card_id)
      self._data.append(cost)

    for card in hand:
      card_id = card['card_id']
      cost = card['cost']
      add_data(card_id, cost)
    for _ in range(10 - len(hand)):
      card_id = 0
      cost = -1
      add_data(card_id, cost)

  def _add_opponent_hand(self, hand):
    if hand is None:
      hand = {}

    self._data.extend([
        len(hand)])

  def _add_heropower(self, hero_power):
    self._data.extend([
        self.from_bool(hero_power['playable'])])

  def _read_board(self, board):
    self._data.clear()

    self._read_hero_data(board['current_player'])
    self._read_hero_data(board['opponent_player'])
    self._read_minions_data(board['current_player']['minions'])
    self._read_minions_data(board['opponent_player']['minions'])
    self._add_current_hand(board['current_player']['hand'])
    self._add_opponent_hand(board['opponent_player']['hand'])

    self._add_resource(board['current_player']['resource'])
    self._add_heropower(board['current_player']['hero_power'])

    return list(self._data)

  def _get_label(self, current_player, first_player_win):
    kFirstPlayerString = 'kFirstPlayer'
    kSecondPlayerString = 'kSecondPlayer'

    if kFirstPlayerString == current_player:
      current_player_is_first = True
    elif current_player == kSecondPlayerString:
      current_player_is_first = False
    else:
      assert False

    assert first_player_win is not None
    if current_player_is_first == first_player_win:
      return kLabelIfFirstPlayerWin
    else:
      return kLabelIfFirstPlayerLoss

  def _win_or_loss(self, result):
    kWinString = 'kResultFirstPlayerWin'  # kResultWin for newer datasets
    kLossString = 'kResultSecondPlayerWin'  # kResultLoss for newer datasets

    if result == kWinString:
      return True

    assert result == kLossString
    return False

  def _read_one_json(self, all_data, all_label, json_data):
    first_player_win = None
    for block_data in json_data:
      action_type = block_data['type']
      if action_type == 'kEnd':
        first_player_win = self._win_or_loss(block_data['result'])
    assert first_player_win is not None

    for block_data in json_data:
      action_type = block_data['type']
      if action_type == 'kMainAction':
        board = block_data['board']
        data = self._read_board(board)
        label = self._get_label(board['current_player_id'], first_player_win)

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

    return data, label, self._hand_card_mapper.get_index_to_v_dict()
