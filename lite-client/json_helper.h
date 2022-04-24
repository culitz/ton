#pragma once
#include <vector>
#include <memory>
#include <ton/ton-types.h>
#include <td/utils/JsonBuilder.h>

namespace liteclient {

class JsonHelper {
public:
    JsonHelper();
    virtual ~JsonHelper() {}

    template<typename S, typename T>
    void append(S& s, T& t) {
        append(s, t);
    }

private:
    std::shared_ptr<td::JsonBuilder> builder_ptr;
    
    void append(td::JsonObjectScope& scope, ton::BlockId& blockId);

    void append(td::JsonObjectScope& scope, ton::BlockIdExt& blockIdExt);

    void append(td::JsonArrayScope& scope, std::vector<ton::BlockIdExt>& blockIds);

    void append(td::JsonArrayScope& scope, std::vector<ton::BlockId>& blockIds);


    td::Slice got_server_mc_block_id_json(
        ton::BlockIdExt& blockId, 
        std::vector<ton::BlockIdExt>& newBlocks
    );
};

}