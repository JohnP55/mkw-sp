#include "Update.hh"

#include "sp/net/Net.hh"
#include "sp/net/Socket.hh"

#include <common/Bytes.hh>
extern "C" {
#include <common/Paths.h>
}

#include <algorithm>
#include <cstring>

namespace SP::Update {

#define TMP_CONTENTS_PATH "/tmp/contents.arc"

static const u8 serverPK[hydro_kx_PUBLICKEYBYTES] = {
    0x4f, 0x4d, 0xd7, 0x2e, 0x58, 0x3f, 0x46, 0x74, 0x94, 0x8d, 0xe2, 0x99, 0x57, 0x51, 0xd5, 0x64,
    0xe7, 0xa4, 0x1e, 0xed, 0xb9, 0xdb, 0xb4, 0x2b, 0xf1, 0xcf, 0xbb, 0xad, 0x88, 0x74, 0xca, 0x35,
};
static const u8 signPK[hydro_sign_PUBLICKEYBYTES] = {
    0x3f, 0x75, 0x15, 0x49, 0xac, 0x48, 0x78, 0x55, 0xb5, 0xa2, 0xd2, 0xac, 0xe4, 0x31, 0x7a, 0x7f,
    0xb5, 0x75, 0x9c, 0x98, 0xac, 0xc0, 0x45, 0x54, 0x44, 0x68, 0xbd, 0x73, 0xa2, 0xbd, 0xe9, 0x03,
};
static Status status = Status::Idle;
static std::optional<Info> info;

Status GetStatus() {
    return status;
}

std::optional<Info> GetInfo() {
    return info;
}

static bool Sync(bool update) {
    status = Status::Connect;
    SP::Net::Socket socket("localhost", 0x5350, serverPK, "update  ");
    if (!socket.ok()) {
        return false;
    }

    status = Status::SendInfo;
    {
        u8 message[8] = {};
        Bytes::Write<u16>(message, 0, update);
        Bytes::Write<u16>(message, 2, versionInfo.major);
        Bytes::Write<u16>(message, 4, versionInfo.minor);
        Bytes::Write<u16>(message, 6, versionInfo.patch);
        if (!socket.write(message, sizeof(message))) {
            return false;
        }
    }

    status = Status::ReceiveInfo;
    {
        u8 message[78] = {};
        if (!socket.read(message, sizeof(message))) {
            return false;
        }
        Info newInfo{};
        newInfo.version.major = Bytes::Read<u16>(message, 0);
        newInfo.version.minor = Bytes::Read<u16>(message, 2);
        newInfo.version.patch = Bytes::Read<u16>(message, 4);
        newInfo.size = Bytes::Read<u32>(message, 6);
        memcpy(newInfo.signature, message + 14, sizeof(newInfo.signature));
        if (!update) {
            if (newInfo.version > versionInfo) {
                info.emplace(newInfo);
            }
            status = Status::Idle;
            return true;
        } else if (memcmp(&*info, &newInfo, sizeof(Info))) {
            info.reset();
            return false;
        }
    }

    status = Status::Download;
    {
        OSTime startTime = OSGetTime();
        hydro_sign_state state;
        if (hydro_sign_init(&state, "update  ") != 0) {
            return false;
        }
        NANDPrivateDelete(TMP_CONTENTS_PATH);
        u8 perms = NAND_PERM_OWNER_MASK | NAND_PERM_GROUP_MASK | NAND_PERM_OTHER_MASK;
        if (NANDPrivateCreate(TMP_CONTENTS_PATH, perms, 0) != NAND_RESULT_OK) {
            return false;
        }
        NANDFileInfo fileInfo;
        if (NANDPrivateOpen(TMP_CONTENTS_PATH, &fileInfo, NAND_ACCESS_WRITE) != NAND_RESULT_OK) {
            return false;
        }
        for (info->downloadedSize = 0; info->downloadedSize < info->size;) {
            alignas(0x20) u8 message[0x1000] = {};
            u16 chunkSize = std::min(info->size - info->downloadedSize, static_cast<u32>(0x1000));
            if (!socket.read(message, chunkSize)) {
                NANDClose(&fileInfo);
                return false;
            }
            if (hydro_sign_update(&state, message, chunkSize) != 0) {
                NANDClose(&fileInfo);
                return false;
            }
            if (NANDWrite(&fileInfo, message, chunkSize) != chunkSize) {
                NANDClose(&fileInfo);
                return false;
            }
            info->downloadedSize += chunkSize;
            OSTime duration = OSGetTime() - startTime;
            info->throughput = OSSecondsToTicks(static_cast<u64>(info->downloadedSize)) / duration;
        }
        if (NANDClose(&fileInfo) != NAND_RESULT_OK) {
            return false;
        }
        if (hydro_sign_final_verify(&state, info->signature, signPK) != 0) {
            return false;
        }
    }

    status = Status::Move;
    if (NANDPrivateMove(TMP_CONTENTS_PATH, UPDATE_PATH) != NAND_RESULT_OK) {
        return false;
    }

    info->updated = true;
    status = Status::Idle;
    return true;
}

bool Check() {
    if (info) {
        status = Status::Idle;
        return true;
    }
    if (!Sync(false)) {
        SP::Net::Restart();
        return false;
    }
    return true;
}

bool Update() {
    assert(info);
    if (!Sync(true)) {
        info.reset();
        SP::Net::Restart();
        return false;
    }
    return true;
}

} // namespace SP::Update
