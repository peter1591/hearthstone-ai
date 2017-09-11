using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HearthstoneAI.State
{
    enum GameStage
    {
        STAGE_UNKNOWN,
        STAGE_GAME_FLOW,
        STAGE_PLAYER_MULLIGAN,
        STAGE_OPPONENT_MULLIGAN,
        STAGE_PLAYER_CHOICE,
        STAGE_OPPONENT_CHOICE
    }

    class GameStageHelper
    {
        static public GameStage GetGameStage(Game game)
        {
            Game.Entity game_entity;
            if (!game.TryGetGameEntity(out game_entity)) return GameStage.STAGE_UNKNOWN;

            Game.Entity player_entity;
            if (!game.TryGetPlayerEntity(out player_entity)) return GameStage.STAGE_UNKNOWN;

            Game.Entity opponent_entity;
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
    }
}
