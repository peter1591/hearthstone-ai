using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.Serialization.Json;

namespace HearthstoneAI
{
    public partial class frmMain : Form
    {
        LogReader log_reader;
        Communicator.AICommunicator ai_communicator;

        public string HearthstoneInstallationPath
        {
            get { return this.txtHearthstoneInstallationPath.Text; }
        }

        public frmMain()
        {
            InitializeComponent();

            this.ai_communicator = new Communicator.AICommunicator(this);
        }

        private void btnChangeHearthstoneInstallationPath_Click(object sender, EventArgs e)
        {
            folderBrowserDialog1.ShowNewFolderButton = false;
            folderBrowserDialog1.SelectedPath = txtHearthstoneInstallationPath.Text;
            folderBrowserDialog1.ShowDialog();
            txtHearthstoneInstallationPath.Text = folderBrowserDialog1.SelectedPath;
        }

        private void btnStart_Click(object sender, EventArgs e)
        {
            this.ai_communicator.Start();

            this.log_reader = new LogReader(this);

            timerMainLoop.Enabled = true;
        }

        private int last_invoke_log_change_id = -1;
        private int last_stable_change_id = -1;
        private DateTime last_stable_time = DateTime.Now;
        private TimeSpan stable_time_to_invoke_AI = new TimeSpan(0, 0, 0, 0, 100);
        private Board.Game last_invoke_board = new Board.Game();
        private void UpdateBoardIfNecessary()
        {
            int current_change_id = this.log_reader.GetChangeId();

            if (current_change_id != last_stable_change_id)
            {
                this.last_stable_change_id = current_change_id;
                this.last_stable_time = DateTime.Now;
                return;
            }

            if ((DateTime.Now - this.last_stable_time) < this.stable_time_to_invoke_AI)
            {
                // not stable enough
                return;
            }

            // now the log is stable enough
            // --> invoke AI if we didn't do it yet
            if (this.last_invoke_log_change_id == current_change_id)
            {
                // we've already invoked the AI for this stable log
                return;
            }

            this.last_invoke_log_change_id = current_change_id;

            Board.Game board = new Board.Game();
            bool parse_success = board.Parse(this.log_reader.GetGameState());

            if (parse_success == false)
            {
                this.txtGameEntity.Text = "parse failed";
                return;
            }
            
            if (board.Equals(this.last_invoke_board))
            {
                //this.AddLog("Log changed, but board are the same as the last-invoked one -> skipping");
                return;
            }

            var game = this.log_reader.GetGameState();

            GameState.Entity game_entity;
            if (!game.TryGetGameEntity(out game_entity))
            {
                this.txtGameEntity.Text = "Parse failed: game entity not exists";
                return;
            }

            var game_stage = this.GetGameStage(game);

            txtGameEntity.Text = "Stage: " + game_stage.ToString() + Environment.NewLine;

            if (game_stage == GameStage.STAGE_PLAYER_CHOICE)
            {
                this.ai_communicator.HandleGameBoard(board);

                this.last_invoke_board = board;
            } else
            {
                this.ai_communicator.Cancel();
            }

            this.UpdateBoard(board);
        }

        private void timerMainLoop_Tick(object sender, EventArgs e)
        {
            this.log_added = false;

            int change_id = this.log_reader.GetChangeId();
            this.log_reader.Process();

            this.UpdateBoardIfNecessary();

            this.ai_communicator.Process();

            if (this.log_added)
            {
                this.listBoxProcessedLogs.SelectedIndex = this.listBoxProcessedLogs.Items.Count - 1;
                this.listBoxProcessedLogs.TopIndex = this.listBoxProcessedLogs.Items.Count - 1;
            }
        }

        enum GameStage
        {
            STAGE_UNKNOWN,
            STAGE_GAME_FLOW,
            STAGE_PLAYER_MULLIGAN,
            STAGE_OPPONENT_MULLIGAN,
            STAGE_PLAYER_CHOICE,
            STAGE_OPPONENT_CHOICE
        }

        private GameStage GetGameStage(GameState game)
        {
            GameState.Entity game_entity;
            if (!game.TryGetGameEntity(out game_entity)) return GameStage.STAGE_UNKNOWN;

            GameState.Entity player_entity;
            if (!game.TryGetPlayerEntity(out player_entity)) return GameStage.STAGE_UNKNOWN;

            GameState.Entity opponent_entity;
            if (!game.TryGetOpponentEntity(out opponent_entity)) return GameStage.STAGE_UNKNOWN;

            if (player_entity.GetTagOrDefault(GameTag.MULLIGAN_STATE, (int)TAG_MULLIGAN.INVALID) == (int)TAG_MULLIGAN.INPUT)
            {
                return GameStage.STAGE_PLAYER_MULLIGAN;
            }

            if (opponent_entity.GetTagOrDefault(GameTag.MULLIGAN_STATE, (int)TAG_MULLIGAN.INVALID) == (int)TAG_MULLIGAN.INPUT)
            {
                return GameStage.STAGE_OPPONENT_MULLIGAN;
            }

            if (!game_entity.HasTag(GameTag.STEP)) return GameStage.STAGE_UNKNOWN;

            TAG_STEP game_entity_step = (TAG_STEP)game_entity.GetTag(GameTag.STEP);
            if (game_entity_step != TAG_STEP.MAIN_ACTION) return GameStage.STAGE_GAME_FLOW;

            bool player_first = false;
            if (player_entity.GetTagOrDefault(GameTag.FIRST_PLAYER, 0) == 1) player_first = true;
            else if (opponent_entity.GetTagOrDefault(GameTag.FIRST_PLAYER, 0) == 1) player_first = false;
            else throw new Exception("parse failed");

            int turn = game_entity.GetTagOrDefault(GameTag.TURN, -1);
            if (turn < 0) return GameStage.STAGE_UNKNOWN;

            if (player_first && (turn % 2 == 1)) return GameStage.STAGE_PLAYER_CHOICE;
            else if (!player_first && (turn % 2 == 0)) return GameStage.STAGE_PLAYER_CHOICE;
            else return GameStage.STAGE_OPPONENT_CHOICE;
        }

        private string GetGameEntityText(GameState game)
        {
            string result = "";

            GameState.Entity game_entity;
            if (!game.TryGetGameEntity(out game_entity)) return "";

            if (game_entity.HasTag(GameTag.STEP))
            {
                TAG_STEP step = (TAG_STEP)game_entity.GetTag(GameTag.STEP);
                result += "[TAG] STEP = " + step.ToString() + Environment.NewLine;
            }
            result += Environment.NewLine;

            result += "All tags: " + Environment.NewLine;
            foreach (var tag in game_entity.Tags)
            {
                result += "[TAG] " + tag.Key.ToString() + " -> " + tag.Value.ToString() + Environment.NewLine;
            }

            return result;
        }

        private string GetHeroClassFromCardId(string card_id)
        {
            switch (card_id)
            {
                case "HERO_01":
                    return "Warrior";
                case "HERO_02":
                    return "Shaman";
                case "HERO_03":
                    return "Rogue";
                case "HERO_04":
                    return "Paladin";
                case "HERO_05":
                case "HERO_05a":
                    return "Hunter";
                case "HERO_06":
                    return "Druid";
                case "HERO_07":
                    return "Warlock";
                case "HERO_08":
                case "HERO_08a":
                    return "Mage";
                case "HERO_09":
                    return "Priest";
            }
            return "(unknown)";
        }

        private string PrintStateText(string name, int value)
        {
            if (value <= 0) return "";
            return "[" + name + ": " + value.ToString() + "] ";
        }

        private string GetEntityExtraStateText(Board.Status status)
        {
            string result = "";
            result += this.PrintStateText("CHARGE", status.charge);
            result += this.PrintStateText("TAUNT", status.taunt);
            result += this.PrintStateText("DIVINE_SHIELD", status.divine_shield);
            result += this.PrintStateText("STEALTH", status.stealth);
            result += this.PrintStateText("FORGETFUL", status.forgetful);
            result += this.PrintStateText("FREEZE", status.freeze);
            result += this.PrintStateText("FROZEN", status.frozen);
            result += this.PrintStateText("POISONOUS", status.poisonous);
            result += this.PrintStateText("WINDFURY", status.windfury);

            if (result != "") result += Environment.NewLine;

            return result;
        }

        private string GetHeroPowerText(Board.HeroPower hero_power)
        {
            string result = "Hero Power: " + Environment.NewLine;

            result += "\tCard = " + hero_power.card_id+ Environment.NewLine;
            if (hero_power.used) result += "\t[USED]" + Environment.NewLine;

            return result;
        }

        private string GetHeroEntityText(Board.Hero hero)
        {
            string result = "";

            result += "Class: " + this.GetHeroClassFromCardId(hero.card_id) + Environment.NewLine;

            int current_hp = hero.max_hp - hero.damage;
            result += "HP: " + current_hp.ToString() + " + " + hero.armor.ToString() + Environment.NewLine;

            if (hero.attack > 0)
            {
                result += "Attack: " + hero.attack.ToString() + Environment.NewLine;
            }

            result += "attacked times: " + hero.attacks_this_turn + Environment.NewLine;

            result += this.GetEntityExtraStateText(hero.status);

            result += this.GetHeroPowerText(hero.hero_power);

            return result;
        }

        private string GetPlayerEntityText(Board.Player player)
        {
            string result = "";
            result += "Crystal: " + (player.crystal.total  - player.crystal.used + player.crystal.this_turn).ToString() + " / " + player.crystal.total.ToString() + Environment.NewLine;

            result += "Overload: current = " + player.crystal.overload.ToString() +
                " next = " + player.crystal.overload_next_turn.ToString() + Environment.NewLine;

            result += "Fatigue: " + player.fatigue.ToString() + Environment.NewLine;
            result += this.GetEnchantmentsText(player.enchantments);

            result += "[HERO ENTITY]" + Environment.NewLine;
            result += this.GetHeroEntityText(player.hero);
            result += this.GetEnchantmentsText(player.hero.enchantments);

            if (player.weapon.equipped)
            {
                result += "[WEAPON] " + player.weapon.card_id + Environment.NewLine;
                result += "Atk: " + player.weapon.attack.ToString();
                result += " durability: " + (player.weapon.durability - player.weapon.damage).ToString() + " / " + player.weapon.durability.ToString() + Environment.NewLine;
                result += this.GetEnchantmentsText(player.weapon.enchantments);
            }

            return result;
        }

        private string GetHandText(Board.Hand hand)
        {
            string result = "";

            foreach (var card in hand.cards)
            {
                result += "[Card] Card ID = " + card + Environment.NewLine;
            }

            return result;
        }

        private string GetDeckText(Board.Deck deck)
        {
            string result = "";

            // get played cards
            result += "[played hand cards]" + Environment.NewLine;
            foreach (var played_card in deck.played_cards)
            {
                result += played_card + Environment.NewLine;
            }

            result += Environment.NewLine;
            result += "Rest cards: " + deck.total_cards.ToString() + Environment.NewLine;

            foreach (var card in deck.known_cards)
            {
                result += "Known Card ID = " + card + Environment.NewLine;
            }

            // get known cards from jousts
            result += Environment.NewLine;
            result += "[Joust flip cards]" + Environment.NewLine;
            foreach (var joust_card in deck.joust_cards)
            {
                result += joust_card + Environment.NewLine;
            }

            return result;
        }

        private string GetEnchantmentsText(Board.Enchantments enchantments)
        {
            string result = "[Enchantments]" + Environment.NewLine;
            foreach (var enchant in enchantments)
            {
                result += "   " + enchant.card_id + " Creator: " + enchant.creator_entity_id.ToString() + Environment.NewLine;
            }
            return result;
        }

        private string GetPlayMinionText(Board.Minion minion)
        {
            string result = "[Minion]" + Environment.NewLine;

            result += "Entity ID = " + minion.entity_id + Environment.NewLine;
            result += "Card ID = " + minion.card_id + Environment.NewLine;

            int current_hp = minion.max_hp - minion.damage;
            result += "HP: " + current_hp.ToString() + " / " + minion.max_hp.ToString() + Environment.NewLine;

            result += "Attack: " + minion.attack.ToString() + Environment.NewLine;

            result += "attacked times: " + minion.attacks_this_turn + Environment.NewLine;

            result += "exhausted: " + minion.exhausted + Environment.NewLine;

            if (minion.silenced) result += "Silenced!" + Environment.NewLine;

            result += this.GetEntityExtraStateText(minion.status);

            result += this.GetEnchantmentsText(minion.enchantments);

            return result;
        }

        private string GetPlayMinionsText(Board.Minions minions)
        {
            string result = "";

            foreach (var minion in minions)
            {
                result += this.GetPlayMinionText(minion) + Environment.NewLine;
            }

            return result;
        }

        private string GetSecretsText(Board.Secrets secrets)
        {
            string result = "";
            foreach (var secret in secrets)
            {
                result += "Class: " + secret.@class + " Card: " + secret.card_id + Environment.NewLine;
            }
            return result;
        }

        private string GetMulliganText(GameState game, GameState.Entity player)
        {
            string result = "";

            if (player.HasTag(GameTag.MULLIGAN_STATE) == false)
            {
                return "State: UNKNOWN" + Environment.NewLine;
            }

            var state = (TAG_MULLIGAN)player.GetTag(GameTag.MULLIGAN_STATE);
            result += "State: " + state.ToString() + Environment.NewLine;
            
            if (state == TAG_MULLIGAN.INPUT)
            {
                var choices = game.EntityChoices.LastOrDefault(e => e.Value.player_entity_id == player.Id);
                if (choices.Value != null)
                {
                    foreach (var choice in choices.Value.choices)
                    {
                        var choice_entity_id = choice.Value;
                        var choice_entity = game.Entities[choice_entity_id];
                        result += "  Card: " + choice_entity.CardId + Environment.NewLine;
                    }
                }
            }

            return result;
        }

        private string GetChoicesText(GameState game)
        {
            GameState.Entity player;
            if (!game.TryGetPlayerEntity(out player)) return "";

            GameState.Entity opponent;
            if (!game.TryGetOpponentEntity(out opponent)) return "";

            string result = "[Player Mulligan]" + Environment.NewLine;
            result += this.GetMulliganText(game, player);
            result += Environment.NewLine;

            result += "[Opponent Mulligan]" + Environment.NewLine;
            result += this.GetMulliganText(game, opponent);
            result += Environment.NewLine;

            int last_choice_id = -1;
            foreach (var choice_id in game.EntityChoices.Keys) last_choice_id = Math.Max(last_choice_id, choice_id);
            var last_choice = game.EntityChoices[last_choice_id];
            if (last_choice.choice_type != "MULLIGAN" && last_choice.player_entity_id == game.PlayerEntityId)
            {
                if (last_choice.choices_has_sent == false)
                {
                    result += "[Player Choices]" + Environment.NewLine;
                    foreach (var choice in last_choice.choices)
                    {
                        string card_id = game.Entities[choice.Value].CardId;

                        result += "   [EntityId:" + choice.Value + "] " + card_id + Environment.NewLine;
                    }
                } 
            }

            return result;
        }

        private void UpdateBoard(Board.Game game)
        {
            var raw_game = this.log_reader.GetGameState();

            this.txtGameEntity.Text += this.GetGameEntityText(raw_game);
            this.txtChoices.Text = this.GetChoicesText(raw_game);

            this.txtPlayerHero.Text = GetPlayerEntityText(game.player);
            this.txtOpponentHero.Text = GetPlayerEntityText(game.opponent);
            this.txtPlayerSecrets.Text = this.GetSecretsText(game.player.secrets);
            this.txtOpponentSecrets.Text = this.GetSecretsText(game.opponent.secrets);
            this.txtPlayerHand.Text = this.GetHandText(game.player.hand);
            this.txtOpponentHand.Text = this.GetHandText(game.opponent.hand);
            this.txtPlayerDeck.Text = this.GetDeckText(game.player.deck);
            this.txtOpponentDeck.Text = this.GetDeckText(game.opponent.deck);
            this.txtPlayerMinions.Text = this.GetPlayMinionsText(game.player.minions);
            this.txtOpponentMinions.Text = this.GetPlayMinionsText(game.opponent.minions);
        }

        private void frmMain_Load(object sender, EventArgs e)
        {

        }

        private bool log_added;
        public void AddLog(string log)
        {
            string prefix = DateTime.Now.ToString("HH:mm:ss.FFFFFF");

            this.listBoxProcessedLogs.Items.Add(prefix + " " + log);
            this.log_added = true;
        }
    }
}
