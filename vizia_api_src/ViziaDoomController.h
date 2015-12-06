#ifndef __VIZIA_DOOM_CONTROLLER_H__
#define __VIZIA_DOOM_CONTROLLER_H__

#include "ViziaDefines.h"

#include <string>

#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include "boost/process.hpp"

namespace Vizia{

    namespace b = boost;
    namespace bip = boost::interprocess;
    namespace bpr = boost::process;
    namespace bpri = boost::process::initializers;

#define SM_NAME_BASE "ViziaSM"

#define MQ_NAME_CTR_BASE "ViziaMQCtr"
#define MQ_NAME_DOOM_BASE "ViziaMQDoom"
#define MQ_MAX_MSG_NUM 32
#define MQ_MAX_MSG_SIZE sizeof(DoomController::MessageCommandStruct)
#define MQ_MAX_CMD_LEN 32

#define MSG_CODE_DOOM_READY 10
#define MSG_CODE_DOOM_TIC 11
#define MSG_CODE_DOOM_CLOSE 12
#define MSG_CODE_DOOM_ERROR 13

#define MSG_CODE_READY 20
#define MSG_CODE_TIC 21
#define MSG_CODE_CLOSE 22
#define MSG_CODE_ERROR 23
#define MSG_CODE_COMMAND 24

    class DoomController {

    public:

        struct InputStruct {
            int MS_X;
            int MS_Y;
            int MS_MAX_X;
            int MS_MAX_Y;
            bool BT[ButtonsNumber];
            bool BT_AVAILABLE[ButtonsNumber];
        };

        struct GameVarsStruct {
            unsigned int GAME_TIC;
            unsigned int GAME_SEED;
            unsigned int GAME_STATIC_SEED;

            unsigned int SCREEN_WIDTH;
            unsigned int SCREEN_HEIGHT;
            size_t SCREEN_PITCH;
            size_t SCREEN_SIZE;
            int SCREEN_FORMAT;

            int MAP_REWARD;
            int SHAPING_REWARD;

            int MAP_USER_VARS[30];

            unsigned int MAP_START_TIC;
            unsigned int MAP_TIC;

            int MAP_KILLCOUNT;
            int MAP_ITEMCOUNT;
            int MAP_SECRETCOUNT;
            bool MAP_END;

            bool PLAYER_DEAD;

            int PLAYER_KILLCOUNT;
            int PLAYER_ITEMCOUNT;
            int PLAYER_SECRETCOUNT;
            int PLAYER_FRAGCOUNT; //for multiplayer

            bool PLAYER_ONGROUND;

            int PLAYER_HEALTH;
            int PLAYER_ARMOR;

            int PLAYER_SELECTED_WEAPON;
            int PLAYER_SELECTED_WEAPON_AMMO;

            int PLAYER_AMMO[10];
            bool PLAYER_WEAPON[10];
            bool PLAYER_KEY[10];
        };

        static Button getButtonId(std::string name);
        static GameVar getGameVarId(std::string name);

        DoomController();
        ~DoomController();

        //FLOW CONTROL

        bool init();
        void close();

        bool tic();
        bool update();
        void restartMap();
        void resetMap();
        void restartGame();
        bool isDoomRunning();
        void sendCommand(std::string command);
        void resetConfig();

        //SETTINGS

        //GAME & MAP SETTINGS

        void setInstanceId(std::string id);
        void setGamePath(std::string path);
        void setIwadPath(std::string path);
        void setFilePath(std::string path);
        void setMap(std::string map);
        void setSkill(int skill);
        void setConfigPath(std::string path);

        void setAutoMapRestart(bool set);
        void setAutoMapRestartOnTimeout(bool set);
        void setAutoMapRestartOnPlayerDeath(bool set);
        void setAutoMapRestartOnMapEnd(bool set);

        unsigned int getMapTimeout();
        void setMapTimeout(unsigned int tics);

        bool isMapLastTic();
        bool isMapFirstTic();
        bool isMapEnded();

        //GRAPHIC SETTINGS

        void setScreenResolution(int width, int height);
        void setScreenWidth(int width);
        void setScreenHeight(int height);
        void setScreenFormat(ScreenFormat format);

        void setRenderHud(bool hud);
        void setRenderWeapon(bool weapon);
        void setRenderCrosshair(bool crosshair);
        void setRenderDecals(bool decals);
        void setRenderParticles(bool particles);

        int getScreenWidth();
        int getScreenHeight();
        size_t getScreenPitch();
        size_t getScreenSize();

        ScreenFormat getScreenFormat();

        //PUBLIC SETTERS & GETTERS

        uint8_t *const getScreen();

        InputStruct *const getInput();

        GameVarsStruct *const getGameVars();

        void setMouse(int x, int y);
        void setMouseX(int x);
        void setMouseY(int y);

        void setButtonState(Button button, bool state);
        void toggleButtonState(Button button);
        void setAllowButton(Button button, bool allow);
        void allowAllButtons();

        void resetInput();

        int getGameVar(GameVar var);

        int getGameTic();

        int getMapReward();
        int getMapShapingReward();

        int getUserVar(int number);

        int getMapTic();

        int getMapKillCount();
        int getMapItemCount();
        int getMapSecretCount();

        bool isPlayerDead();

        int getPlayerKillCount();
        int getPlayerItemCount();
        int getPlayerSecretCount();
        int getPlayerFragCount();

        int getPlayerHealth();
        int getPlayerArmor();
        int getPlayerSelectedWeaponAmmo();
        int getPlayerSelectedWeapon();

        int getPlayerAmmo1();
        int getPlayerAmmo2();
        int getPlayerAmmo3();
        int getPlayerAmmo4();

        bool getPlayerWeapon1();
        bool getPlayerWeapon2();
        bool getPlayerWeapon3();
        bool getPlayerWeapon4();
        bool getPlayerWeapon5();
        bool getPlayerWeapon6();
        bool getPlayerWeapon7();

        bool getPlayerKey1();
        bool getPlayerKey2();
        bool getPlayerKey3();

    private:

        void generateInstanceId();

        std::string instanceId;

        b::thread *doomThread;
        //bpr::child doomProcess;
        bool doomRunning;
        bool doomTic;

        //MESSAGE QUEUES

        struct MessageSignalStruct {
            uint8_t code;
        };

        struct MessageCommandStruct {
            uint8_t code;
            char command[MQ_MAX_CMD_LEN];
        };

        bool MQInit();

        void MQSend(uint8_t code);

        void MQSelfSend(uint8_t code);

        bool MQTrySend(uint8_t code);

        void MQSend(uint8_t code, const char *command);

        bool MQTrySend(uint8_t code, const char *command);

        void MQRecv(void *msg, unsigned long &size, unsigned int &priority);

        bool MQTryRecv(void *msg, unsigned long &size, unsigned int &priority);

        void MQClose();

        bip::message_queue *MQController;
        bip::message_queue *MQDoom;
        std::string MQControllerName;
        std::string MQDoomName;

        //SHARED MEMORY

        bool SMInit();

        void SMClose();

        bip::shared_memory_object SM;
        std::string SMName;

        bip::mapped_region *InputSMRegion;
        InputStruct *Input;

        bip::mapped_region *GameVarsSMRegion;
        GameVarsStruct *GameVars;

        bip::mapped_region *ScreenSMRegion;
        uint8_t *Screen;

        //HELPERS

        void waitForDoomStart();

        void waitForDoomTic();

        void lunchDoom();

        // OPTIONS

        unsigned int screenWidth, screenHeight;
        size_t screenPitch, screenSize;
        ScreenFormat screenFormat;

        bool hud, weapon, crosshair, decals, particles;

        std::string gamePath;
        std::string iwadPath;
        std::string filePath;
        std::string map;
        std::string configPath;
        int skill;

        // AUTO RESTART

        bool autoRestart;
        bool autoRestartOnTimeout;
        bool autoRestartOnPlayersDeath;
        bool autoRestartOnMapEnd;

        unsigned int mapStartTime;
        unsigned int mapTimeout;
        unsigned int mapRestartCount;
        bool mapRestarting;
        bool mapEnded;
        unsigned int mapLastTic;

    };

}

#endif