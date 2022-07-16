#pragma once
#include <vector>
#include <ton/ton-types.h>
#include "block/block-auto.h"
#include "crypto/block/block.h"
#include "crypto/block/check-proof.h"
#include "crypto/block/block-parse.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"


namespace utils {

struct Src {
    int workchain;
    std::string addr;
};

struct InMsgInfo {
    std::string inbound;
    std::string ihr_disabled;
    int bounce;
    int bounced;
    std::string value;
    int ihr_fee;
    int fwd_fee;
    std::string body;
    Src src;
    Src dest;
};

typedef rapidjson::Writer<rapidjson::StringBuffer> Writer;
typedef std::vector<InMsgInfo> InMsgInfoVect;

void fill_json(Writer&, const ton::BlockId& blockId, bool scope = true);
void fill_json(Writer&, const ton::BlockIdExt& blockIdExt, bool scope = true);
void fill_json(Writer&, const std::vector<ton::BlockIdExt>& block_ids, bool scope = true);
void fill_json(Writer&, const ton::StdSmcAddress& addr, bool scope = true);
void fill_json(Writer&, const block::gen::AccountStorage::Record& store, bool scope = true);
void fill_json(Writer&, const block::AccountState::Info info, bool scope = true);
void fill_json(Writer&, const block::Transaction::Info info, bool scope = true);
void fill_json(Writer&, const block::gen::CommonMsgInfo::Record_int_msg_info msg, bool scope = true);
void fill_json(Writer&, const InMsgInfo, bool scope = true);
void fill_json(Writer&, const std::vector<InMsgInfo>, bool scope = true);
void fill_json(Writer&, const std::vector<ton::BlockId>&, bool);

bool unpack_addr(vm::Ref<vm::CellSlice> csr, ton::WorkchainId& out_wc, ton::StdSmcAddress& out_addr);

}
