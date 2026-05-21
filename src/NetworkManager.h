#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <enet/enet.h>
#include <string>
#include <functional>
#include <vector>
#include <cstdint>

enum class NetMsgType : uint8_t {
    NONE,
    GAME_STATE,
    PADDLE_POS
};

struct GameStatePacket {
    float ballX, ballY;
    float ballSpeedX, ballSpeedY;
    float paddle1X;
    float paddle2X;
    uint32_t score;
    int8_t lives;
    uint16_t brickCount;
};

class NetworkManager {
public:
    NetworkManager();
    ~NetworkManager();

    bool Initialize();
    void Shutdown();

    bool StartHost(uint16_t port);
    bool ConnectToHost(const std::string& ip, uint16_t port);

    void PollEvents();
    bool IsConnected() const;
    bool IsHost() const;

    void SendGameState(const GameStatePacket& packet);
    void SendPaddlePos(float paddleX);

    void SetOnGameStateReceived(std::function<void(const GameStatePacket&)> callback);
    void SetOnPaddlePosReceived(std::function<void(float)> callback);
    void SetOnConnect(std::function<void()> callback);
    void SetOnDisconnect(std::function<void()> callback);

private:
    ENetHost* host;
    ENetPeer* peer;
    bool connected;
    bool isHost;

    std::function<void(const GameStatePacket&)> onGameState;
    std::function<void(float)> onPaddlePos;
    std::function<void()> onConnect;
    std::function<void()> onDisconnect;

    void HandlePacket(ENetPacket* packet);
    void SendPacket(ENetPeer* peer, const void* data, size_t size, bool reliable);
};

#endif
