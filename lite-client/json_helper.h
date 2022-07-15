#pragma once
#include <vector>
#include <memory>
#include <map>
#include <ton/ton-types.h>
#include <td/utils/JsonBuilder.h>
#include "block/block-auto.h"
#include "crypto/block/block.h"
#include "crypto/block/check-proof.h"

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"
#include "serializers/fill-utils.h"


namespace liteclient {

using namespace td;
using namespace block;

typedef rapidjson::StringBuffer JsonBuffer;
typedef rapidjson::Writer<rapidjson::StringBuffer> JsonWriter;
typedef std::shared_ptr<JsonBuffer> JsonBufferPtr;
typedef std::shared_ptr<JsonWriter> JsonWriterPtr;


class JsonHelper {
public:
    JsonHelper();
    virtual ~JsonHelper() {}

    std::string msg(std::string key, std::string msg);
    std::string error_message(std::string msg);
    std::string message(std::string msg);

    template<typename T>
    std::string to_json(T& t) {
        // writer_ptr->SetIndent(' ', 4);
        fill_json(t, *writer_ptr);
        return std::string(buffer_ptr->GetString());
    }

    std::string got_server_mc_block_id_json(
        ton::BlockIdExt& blockId, 
        vector<ton::BlockIdExt>& newBlocks
    );

    std::string account_state(
            ton::StdSmcAddress&,
            gen::AccountStorage::Record&,
            AccountState::Info&,
            ton::WorkchainId&
    );

    std::string got_all_shards(const std::vector<ton::BlockId>& ids);
    std::string known(const std::vector<ton::BlockIdExt>& known);
    std::string got_last_transactions(const std::vector<Transaction::Info>& transactions);

private:
    JsonBufferPtr buffer_ptr;
    JsonWriterPtr writer_ptr;

    void fill_json(const ton::BlockId& blockId, bool scope = true);
    void fill_json(const ton::BlockIdExt& blockIdExt, bool scope = true);
    void fill_json(const vector<ton::BlockIdExt>& block_ids, bool scope = true);
    void fill_json(const ton::StdSmcAddress& addr, bool scope = true);
    void fill_json(const block::gen::AccountStorage::Record& store, bool scope = true);
    void fill_json(const block::AccountState::Info info, bool scope = true);
};

}
