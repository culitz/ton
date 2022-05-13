#include "lite-client/json_helper.h"

namespace liteclient {

JsonHelper::JsonHelper() {
    builder_ptr = std::shared_ptr<td::JsonBuilder>(new td::JsonBuilder());
}

void JsonHelper::append(JsonObjectScope& scope, ton::BlockId& blockId) {
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

void JsonHelper::append(JsonObjectScope& scope, ton::BlockIdExt& blockIdExt) {
    auto id_builder_ptr = std::shared_ptr<td::JsonBuilder>(new td::JsonBuilder());
    auto sub_scope = id_builder_ptr->enter_object();
    append<JsonObjectScope, ton::BlockId>(sub_scope, blockIdExt.id);
    scope("id", td::JsonRaw(id_builder_ptr->string_builder().as_cslice()));
    scope("rootHash", blockIdExt.root_hash.to_hex());
    scope("fileHash", blockIdExt.file_hash.to_hex());
}

void JsonHelper::append(JsonArrayScope& scope, std::vector<ton::BlockIdExt>& blockIdsExt) {
    for(ton::BlockIdExt& id : blockIdsExt) {
        td::JsonBuilder builder;
        auto obj = builder.enter_object();
        append<JsonObjectScope, ton::BlockIdExt>(obj, id);
        obj.leave();
        scope << td::JsonRaw(builder.string_builder().as_cslice());
    }
}

void JsonHelper::append(JsonArrayScope& scope, std::vector<ton::BlockId>& blockIds) {
    for(ton::BlockId& id : blockIds) {
        td::JsonBuilder builder;
        auto obj = builder.enter_object();
        append<JsonObjectScope, ton::BlockId>(obj, id);
        obj.leave();
        scope << td::JsonRaw(std::move(builder.string_builder().as_cslice()));
    }
    scope.leave();
}

void JsonHelper::append(JsonObjectScope &scope, block::CurrencyCollection &balance) {
    std::stringstream ss;
    ss << balance.grams;
    scope("balance", ss.str());
}

void JsonHelper::append(JsonObjectScope &scope, block::gen::AccountStorage::Record& store) {
    int tag = block::gen::t_AccountState.get_tag(*store.state);
    CurrencyCollection collect;
    collect.unpack(store.balance);

    std::string status_name = "null";
    switch(tag) {
    case block::gen::AccountState::account_active:
        status_name = "active";
        break;

    case block::gen::AccountState::account_uninit:
        status_name = "uninit";
        break;

    case block::gen::AccountState::account_frozen:
        status_name = "frozen";
        return;

    default:
        break;
    }
    scope("status", status_name);
    scope("lastTransactLC", std::to_string(store.last_trans_lt));
    std::stringstream stream;
    stream << collect.grams;
    scope("balance", stream.str());
}

void JsonHelper::append(JsonObjectScope &scope, ton::StdSmcAddress& addr) {
    std::stringstream stream;
    stream << addr.to_hex();
    scope("address", stream.str());
}

void JsonHelper::append(JsonObjectScope &scope, block::AccountState::Info info) {
    std::stringstream stream;
    stream << info.last_trans_lt;
    scope("lastTransactTimeLC", stream.str());
    scope("lastTransactHash", info.last_trans_hash.to_hex());
    stream.str(string());

    {
        using namespace vm;
        CellSlice root_slice = load_cell_slice(info.root);
        std::vector<string> fields = {"code", "data"};
        for(unsigned int i = 0; i < fields.size(); i++) {
            Ref<Cell> cell = root_slice.prefetch_ref(i);
            CellSlice slice = load_cell_slice(cell);
            slice.dump_hex(stream);
            scope(fields[i], stream.str());
            stream.str(string());
        }
    }
}

td::Slice JsonHelper::got_server_mc_block_id_json(
    ton::BlockIdExt& blockId, 
    std::vector<ton::BlockIdExt>& newBlocks
) {
    td::JsonBuilder out_json;
    auto jo = out_json.enter_object();
    liteclient::JsonHelper helper;
    helper.append<JsonObjectScope, ton::BlockIdExt>(jo, blockId);
    auto blk_ids_ptr = std::shared_ptr<std::vector<ton::BlockIdExt>>(new std::vector<ton::BlockIdExt>{});
    td::JsonBuilder blk_list_builder;
    auto blk_list = blk_list_builder.enter_array();
    helper.append<JsonArrayScope, std::vector<ton::BlockIdExt>>(blk_list, newBlocks);
    blk_list.leave();

    jo("blocks", td::JsonRaw(blk_list_builder.string_builder().as_cslice()));
    jo.leave();
    return out_json.string_builder().as_cslice();
}

td::Slice JsonHelper::account_state(
        ton::StdSmcAddress& addr,
        block::gen::AccountStorage::Record& store,
        block::AccountState::Info& info,
        ton::WorkchainId& workchain
) {
    builder_ptr = std::shared_ptr<JsonBuilder>(new JsonBuilder);
    auto jo = builder_ptr->enter_object();
    append<JsonObjectScope, ton::StdSmcAddress>(jo, addr);
    append<JsonObjectScope, block::gen::AccountStorage::Record>(jo, store);
    append<JsonObjectScope, block::AccountState::Info>(jo, info);
    jo("workchain", workchain);
    jo.leave();
    return td::Slice(builder_ptr->string_builder().as_cslice());
}

Slice JsonHelper::error_message(std::string msg) {
    return one_attr_obj<std::string>("error", msg);
}

Slice JsonHelper::message(std::string msg) {
    return one_attr_obj<std::string>("message", msg);
}

}
