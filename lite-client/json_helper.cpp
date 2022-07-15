#include "lite-client/json_helper.h"

namespace liteclient {

JsonHelper::JsonHelper() {
    buffer_ptr = std::shared_ptr<JsonBuffer>(new JsonBuffer);
    writer_ptr = std::shared_ptr<JsonWriter>(new JsonWriter(*buffer_ptr));
}

std::string JsonHelper::got_server_mc_block_id_json(
    ton::BlockIdExt& blockId, 
    std::vector<ton::BlockIdExt>& newBlocks
) {
    writer_ptr->StartObject();
    fill_json(blockId, false);
    fill_json(newBlocks);
    writer_ptr->EndObject();
    return buffer_ptr->GetString();
}

std::string JsonHelper::account_state(
        ton::StdSmcAddress& addr,
        block::gen::AccountStorage::Record& store,
        block::AccountState::Info& info,
        ton::WorkchainId& workchain
) {     
     writer_ptr->StartObject();
     fill_json(addr, false);
     fill_json(store, false);
     fill_json(info, false);
     writer_ptr->Key("workchain");
     writer_ptr->Int(workchain);
     writer_ptr->EndObject();
     return buffer_ptr->GetString();
}

std::string JsonHelper::got_all_shards(const std::vector<ton::BlockId>& ids) {
    writer_ptr->StartArray();
    for(auto& id: ids)
        fill_json(id);
    writer_ptr->EndArray();
    return buffer_ptr->GetString();
}

std::string JsonHelper::known(const std::vector<ton::BlockIdExt>& known) {
    writer_ptr->StartArray();
    fill_json(known, false);
    writer_ptr->EndArray();
    return buffer_ptr->GetString();
}

std::string JsonHelper::got_last_transactions(const std::vector<Transaction::Info>& transactions) {
    writer_ptr->StartArray();
    for(const Transaction::Info& transaction : transactions) {
        writer_ptr->EndObject();
        writer_ptr->Key("fromBlock");
        fill_json(transaction.blkid, false);
        writer_ptr->Key("messages");
        writer_ptr->StartArray();
        writer_ptr->EndArray();
    }
    writer_ptr->EndArray();
    return buffer_ptr->GetString();
}

std::string JsonHelper::msg(std::string key, std::string msg) {
    writer_ptr->StartObject();
    writer_ptr->Key(key.c_str());
    writer_ptr->String(msg.c_str());
    writer_ptr->EndObject();
    return buffer_ptr->GetString();
}

std::string JsonHelper::error_message(std::string m) {
    std::string key = "error";
    return msg(key, m);
}

std::string JsonHelper::message(std::string m) {
    std::string key = "message";
    return msg(key, m);
}

void JsonHelper::fill_json(const ton::BlockId& blockId, bool scope) {
    char buffer[64];
    std::string shard_str = std::string{
        buffer, (unsigned)snprintf(buffer, 63, "%016llx",
        static_cast<unsigned long long>(blockId.shard))
    };
    std::string seqno = std::to_string(blockId.seqno);
    if(scope)
        writer_ptr->StartObject();
    writer_ptr->Key("workchain");
    writer_ptr->Int(blockId.workchain);
    writer_ptr->Key("shard");
    writer_ptr->String(shard_str.c_str());
    writer_ptr->Key("seqno");
    writer_ptr->String(seqno.c_str());
    writer_ptr->Key("strValue");
    writer_ptr->String(blockId.to_str().c_str());
    if(scope)
        writer_ptr->EndObject();
}

void JsonHelper::fill_json(const ton::BlockIdExt& blockIdExt, bool scope) {
    if(scope)
        writer_ptr->StartObject();
    writer_ptr->Key("id");
    fill_json(blockIdExt.id);
    writer_ptr->Key("rootHash");
    writer_ptr->String(blockIdExt.root_hash.to_hex().c_str());
    writer_ptr->Key("fileHash");
    std::string file_hash = blockIdExt.file_hash.to_hex();  
    writer_ptr->String(file_hash.c_str());
    if(scope)
        writer_ptr->EndObject();
}


void JsonHelper::fill_json(const vector<ton::BlockIdExt>& block_ids, bool scope) {
    if(scope) {
        writer_ptr->Key("blocks");
        writer_ptr->StartArray();
    }
    for(const ton::BlockIdExt& block_id : block_ids) {
        fill_json(block_id);
    }
    if(scope) {
        writer_ptr->EndArray();
    }
}

void JsonHelper::fill_json(const ton::StdSmcAddress& addr, bool scope) {
    if(scope)
        writer_ptr->StartObject();
    std::stringstream stream;
    stream << addr.to_hex();
    writer_ptr->Key("address");
    writer_ptr->String(stream.str().c_str());
    if(scope)
        writer_ptr->EndObject();
}

void JsonHelper::fill_json(const block::gen::AccountStorage::Record& store, bool scope) {
    if(scope)
        writer_ptr->StartObject();

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

    std::stringstream stream;
    stream << collect.grams;

    writer_ptr->Key("status");
    writer_ptr->String(status_name.c_str());
    writer_ptr->Key("lastTransactLC");
    writer_ptr->String(std::to_string(store.last_trans_lt).c_str());
    writer_ptr->Key("balance");
    writer_ptr->String(stream.str().c_str());

    if(scope)
        writer_ptr->EndObject();
}

void JsonHelper::fill_json(block::AccountState::Info info, bool scope) {
    if(scope)
        writer_ptr->StartObject();
    std::stringstream stream;
    stream << info.last_trans_lt;
    writer_ptr->Key("lastTransactTimeLC");
    writer_ptr->String(stream.str().c_str());
    writer_ptr->Key("lastTransactHash");
    writer_ptr->String(info.last_trans_hash.to_hex().c_str());
    stream.str(string());

    {
        using namespace vm;
        CellSlice root_slice = load_cell_slice(info.root);
        std::vector<string> fields = {"code", "data"};
        for(unsigned int i = 0; i < fields.size(); i++) {
            writer_ptr->Key(fields[i].c_str());
            Ref<Cell> cell = root_slice.prefetch_ref(i);
            if(cell.is_null()) {
                writer_ptr->Null();
            } else {
                CellSlice slice = load_cell_slice(cell);
                slice.dump_hex(stream);
                writer_ptr->String(stream.str().c_str());
                stream.str(string());
            }
        }
    }

    if(scope)
        writer_ptr->EndObject();
}

}
