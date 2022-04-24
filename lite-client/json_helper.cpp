#include "lite-client/json_helper.h"

namespace liteclient {

JsonHelper::JsonHelper() {
    builder_ptr = std::shared_ptr<td::JsonBuilder>(new td::JsonBuilder());
}

void JsonHelper::append(td::JsonObjectScope& scope, ton::BlockId& blockId) {
    char buffer[64];
    std::string shard_str = std::string{
        buffer, (unsigned)snprintf(buffer, 63, "%016llx",
        static_cast<unsigned long long>(blockId.shard))
    };

    scope("workchain", blockId.workchain);
    scope("shard", shard_str);
    scope("seqno", std::to_string(blockId.seqno));
    scope("strValue", blockId.to_str());
    scope.leave();
}

void JsonHelper::append(td::JsonObjectScope& scope, ton::BlockIdExt& blockIdExt) {
    auto id_builder_ptr = std::shared_ptr<td::JsonBuilder>(new td::JsonBuilder());
    auto sub_scope = id_builder_ptr->enter_object();
    append<td::JsonObjectScope, ton::BlockId>(sub_scope, blockIdExt.id);
    scope("id", td::JsonRaw(id_builder_ptr->string_builder().as_cslice()));
    scope("rootHash", blockIdExt.root_hash.to_hex());
    scope("fileHash", blockIdExt.file_hash.to_hex());
}

void JsonHelper::append(td::JsonArrayScope& scope, std::vector<ton::BlockIdExt>& blockIdsExt) {
    for(ton::BlockIdExt& id : blockIdsExt) {
        td::JsonBuilder builder;
        auto obj = builder.enter_object();
        append<td::JsonObjectScope, ton::BlockIdExt>(obj, id);
        obj.leave();
        scope << td::JsonRaw(builder.string_builder().as_cslice());
    }
}

void JsonHelper::append(td::JsonArrayScope& scope, std::vector<ton::BlockId>& blockIds) {
    for(ton::BlockId& id : blockIds) {
        td::JsonBuilder builder;
        auto obj = builder.enter_object();
        append<td::JsonObjectScope, ton::BlockId>(obj, id);
        obj.leave();
        scope << td::JsonRaw(std::move(builder.string_builder().as_cslice()));
    }
    scope.leave();
}

td::Slice JsonHelper::got_server_mc_block_id_json(
    ton::BlockIdExt& blockId, 
    std::vector<ton::BlockIdExt>& newBlocks
) {
    td::JsonBuilder out_json;
    auto jo = out_json.enter_object();
    liteclient::JsonHelper helper;
    helper.append<td::JsonObjectScope, ton::BlockIdExt>(jo, blockId);
    auto blk_ids_ptr = std::shared_ptr<std::vector<ton::BlockIdExt>>(new std::vector<ton::BlockIdExt>{});
    td::JsonBuilder blk_list_builder;
    auto blk_list = blk_list_builder.enter_array();
    helper.append<td::JsonArrayScope, std::vector<ton::BlockIdExt>>(blk_list, newBlocks);
    blk_list.leave();

    jo("blocks", td::JsonRaw(blk_list_builder.string_builder().as_cslice()));
    jo.leave();
    return out_json.string_builder().as_cslice();
}

}