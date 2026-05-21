#include "NetworkManager.h"
#include <cstdio>
#include <cstring>

NetworkManager::NetworkManager() : host(nullptr), peer(nullptr), connected(false), isHost(false) {}
NetworkManager::~NetworkManager() { Shutdown(); }

bool NetworkManager::Initialize() {
    if (enet_initialize() != 0) {
        printf("ENet init failed\n");
        return false;
    }
    printf("ENet initialized\n");
    return true;
}

void NetworkManager::Shutdown() {
    if (host) {
        enet_host_destroy(host);
        host = nullptr;
    }
    enet_deinitialize();
    connected = false;
}

bool NetworkManager::StartHost(uint16_t port) {
    ENetAddress addr;
    addr.host = ENET_HOST_ANY;
    addr.port = port;
    host = enet_host_create(&addr, 1, 2, 0, 0);
    if (!host) return false;
    isHost = true;
    printf("Host started on port %d\n", port);
    return true;
}

bool NetworkManager::ConnectToHost(const std::string& ip, uint16_t port) {
    host = enet_host_create(nullptr, 1, 2, 0, 0);
    if (!host) return false;
    ENetAddress addr;
    enet_address_set_host(&addr, ip.c_str());
    addr.port = port;
    peer = enet_host_connect(host, &addr, 2, 0);
    if (!peer) return false;
    isHost = false;
    printf("Connecting to %s:%d\n", ip.c_str(), port);
    return true;
}

void NetworkManager::PollEvents() {
    if (!host) return;
    ENetEvent event;
    while (enet_host_service(host, &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                peer = event.peer;
                connected = true;
                if (onConnect) onConnect();
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                HandlePacket(event.packet);
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                connected = false;
                if (onDisconnect) onDisconnect();
                break;
            default: break;
        }
    }
}

void NetworkManager::HandlePacket(ENetPacket* packet) {
    if (packet->dataLength < 1) return;
    NetMsgType type = static_cast<NetMsgType>(packet->data[0]);
    if (type == NetMsgType::GAME_STATE && packet->dataLength >= 1+sizeof(GameStatePacket)) {
        GameStatePacket state;
        memcpy(&state, packet->data+1, sizeof(GameStatePacket));
        if (onGameState) onGameState(state);
    } else if (type == NetMsgType::PADDLE_POS && packet->dataLength >= 1+sizeof(float)) {
        float x;
        memcpy(&x, packet->data+1, sizeof(float));
        if (onPaddlePos) onPaddlePos(x);
    }
}

void NetworkManager::SendPacket(ENetPeer* peer, const void* data, size_t size, bool reliable) {
    if (!peer) return;
    ENetPacket* p = enet_packet_create(data, size, reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
    enet_peer_send(peer, 0, p);
}

void NetworkManager::SendGameState(const GameStatePacket& packet) {
    if (!connected) return;
    size_t total = 1 + sizeof(GameStatePacket);
    std::vector<uint8_t> buf(total);
    buf[0] = (uint8_t)NetMsgType::GAME_STATE;
    memcpy(buf.data()+1, &packet, sizeof(GameStatePacket));
    SendPacket(peer, buf.data(), total, true);
}

void NetworkManager::SendPaddlePos(float x) {
    if (!connected) return;
    size_t total = 1 + sizeof(float);
    std::vector<uint8_t> buf(total);
    buf[0] = (uint8_t)NetMsgType::PADDLE_POS;
    memcpy(buf.data()+1, &x, sizeof(float));
    SendPacket(peer, buf.data(), total, false);
}

bool NetworkManager::IsConnected() const { return connected; }
bool NetworkManager::IsHost() const { return isHost; }

void NetworkManager::SetOnGameStateReceived(std::function<void(const GameStatePacket&)> cb) { onGameState = cb; }
void NetworkManager::SetOnPaddlePosReceived(std::function<void(float)> cb) { onPaddlePos = cb; }
void NetworkManager::SetOnConnect(std::function<void()> cb) { onConnect = cb; }
void NetworkManager::SetOnDisconnect(std::function<void()> cb) { onDisconnect = cb; }
