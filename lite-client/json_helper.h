#pragma once
#include <vector>
#include <memory>
#include <ton/ton-types.h>
#include <td/utils/JsonBuilder.h>
#include "block/block-auto.h"
#include "crypto/block/block.h"
#include"crypto/block/check-proof.h"

namespace liteclient {

using namespace td;
using namespace block;

class JsonHelper {
public:
    JsonHelper();
    virtual ~JsonHelper() {}

    template<typename S, typename T>
    void append(S& s, T& t) {
        append(s, t);
    }

    template<typename T>
    td::Slice one_attr_obj(std::string key, T value) {
        builder_ptr = std::shared_ptr<JsonBuilder>(new JsonBuilder);
        auto jo = builder_ptr->enter_object();
        jo(key, value);
        jo.leave();
        return builder_ptr->string_builder().as_cslice();
    }

    td::Slice account_state(
            ton::StdSmcAddress&,
            gen::AccountStorage::Record&,
            AccountState::Info&,
            ton::WorkchainId&
    );

    td::Slice error_message(std::string msg);

    td::Slice message(std::string msg);

private:
    std::shared_ptr<JsonBuilder> builder_ptr;
    
    void append(JsonObjectScope& scope, ton::BlockId& blockId);

    void append(JsonObjectScope& scope, ton::BlockIdExt& blockIdExt);

    void append(JsonArrayScope& scope, std::vector<ton::BlockIdExt>& blockIds);

    void append(JsonArrayScope& scope, std::vector<ton::BlockId>& blockIds);

    void append(JsonObjectScope& scope, block::gen::AccountStorage& account);

    void append(JsonObjectScope& scope, block::CurrencyCollection& balance);

    void append(JsonObjectScope& scope, block::gen::AccountStorage::Record& store);

    void append(JsonObjectScope& scope, ton::StdSmcAddress& addr);

    void append(JsonObjectScope& scope, block::AccountState::Info& info);


    td::Slice got_server_mc_block_id_json(
        ton::BlockIdExt& blockId, 
        vector<ton::BlockIdExt>& newBlocks
    );
};

}
