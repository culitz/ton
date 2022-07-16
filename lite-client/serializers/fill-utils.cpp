#include "fill-utils.h"

namespace utils {

void fill_json(Writer& writer, const ton::BlockId& blockId, bool scope) {
    char buffer[64];
    std::string shard_str = std::string{
        buffer, (unsigned)snprintf(buffer, 63, "%016llx",
        static_cast<unsigned long long>(blockId.shard))
    };
    std::string seqno = std::to_string(blockId.seqno);
    if(scope)
        writer.StartObject();
    writer.Key("workchain");
    writer.Int(blockId.workchain);
    writer.Key("shard");
    writer.String(shard_str.c_str());
    writer.Key("seqno");
    writer.String(seqno.c_str());
    writer.Key("strValue");
    writer.String(blockId.to_str().c_str());
    if(scope)
        writer.EndObject();
}

void fill_json(Writer& writer, const ton::BlockIdExt& blockIdExt, bool scope) {
    if(scope)
        writer.StartObject();
    writer.Key("id");
    fill_json(writer, blockIdExt.id);
    writer.Key("rootHash");
    writer.String(blockIdExt.root_hash.to_hex().c_str());
    writer.Key("fileHash");
    std::string file_hash = blockIdExt.file_hash.to_hex();
    writer.String(file_hash.c_str());
    if(scope)
        writer.EndObject();
}


void fill_json(Writer& writer, const std::vector<ton::BlockIdExt>& block_ids, bool scope) {
    if(scope) {
        writer.Key("blocks");
        writer.StartArray();
    }
    for(const ton::BlockIdExt& block_id : block_ids) {
        fill_json(writer, block_id);
    }
    if(scope) {
        writer.EndArray();
    }
}

void fill_json(Writer& writer, const ton::StdSmcAddress& addr, bool scope) {
    if(scope)
        writer.StartObject();
    std::stringstream stream;
    stream << addr.to_hex();
    writer.Key("address");
    writer.String(stream.str().c_str());
    if(scope)
        writer.EndObject();
}

void fill_json(Writer& writer, const block::gen::AccountStorage::Record& store, bool scope) {
    if(scope)
        writer.StartObject();

    int tag = block::gen::t_AccountState.get_tag(*store.state);
    block::CurrencyCollection collect;
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

    writer.Key("status");
    writer.String(status_name.c_str());
    writer.Key("lastTransactLC");
    writer.String(std::to_string(store.last_trans_lt).c_str());
    writer.Key("balance");
    writer.String(stream.str().c_str());

    if(scope)
        writer.EndObject();
}

void fill_json(Writer& writer, block::AccountState::Info info, bool scope) {
    if(scope)
        writer.StartObject();
    std::stringstream stream;
    stream << info.last_trans_lt;
    writer.Key("lastTransactTimeLC");
    writer.String(stream.str().c_str());
    writer.Key("lastTransactHash");
    writer.String(info.last_trans_hash.to_hex().c_str());
    stream.str(std::string());

    {
        using namespace vm;
        CellSlice root_slice = load_cell_slice(info.root);
        std::vector<std::string> fields = {"code", "data"};
        for(unsigned int i = 0; i < fields.size(); i++) {
            writer.Key(fields[i].c_str());
            Ref<Cell> cell = root_slice.prefetch_ref(i);
            if(cell.is_null()) {
                writer.Null();
            } else {
                CellSlice slice = load_cell_slice(cell);
                slice.dump_hex(stream);
                writer.String(stream.str().c_str());
                stream.str(std::string());
            }
        }
    }

    if(scope)
        writer.EndObject();
}

void fill_json(Writer& writer, const block::Transaction::Info info, bool scope) {
    // writer.StartObject();
    writer.Key("blockFrom");
    fill_json(writer, info.blkid);
    // writer.EndObject();
}

void fill_json(Writer& writer, const block::gen::CommonMsgInfo::Record_int_msg_info msg, bool scope) {
    writer.Key("type");
    writer.String("inbound");
    writer.Key("bounce");
    writer.Int(msg.bounce);
    writer.Key("bounced");
    writer.Int(msg.bounced);
    writer.Key("ihr_disabled");
    writer.Int(msg.ihr_disabled);
}


void fill_json(Writer& writer, const InMsgInfo info, bool scope) {
    if(scope)
        writer.StartObject();

    writer.Key("type");
    writer.String(info.inbound.c_str());

    writer.Key("bounce");
    writer.Int(info.bounce);

    writer.Key("bounced");
    writer.Int(info.bounced);

    writer.Key("value");
    writer.String(info.value.c_str());

    writer.Key("src");
    writer.StartObject();
    writer.Key("workchain");
    writer.Int(info.src.workchain);
    writer.Key("addr");
    writer.String(info.src.addr.c_str());
    writer.EndObject();

    writer.Key("dest");
    writer.StartObject();
    writer.Key("workchain");
    writer.Int(info.dest.workchain);
    writer.Key("addr");
    writer.String(info.dest.addr.c_str());
    writer.EndObject();

    if(scope)
        writer.EndObject();
}

void fill_json(Writer& writer, const std::vector<InMsgInfo> infoVect, bool scope) {
    if(scope)
        writer.StartArray();

    for(const InMsgInfo& info : infoVect)
        fill_json(writer, info);

    if(scope)
        writer.EndArray();
}

void fill_json(Writer& writer, const std::vector<ton::BlockId>& ids, bool scope) {
    for(auto& id: ids)
        fill_json(writer, id);
}


bool unpack_addr(vm::Ref<vm::CellSlice> csr, ton::WorkchainId& out_wc, ton::StdSmcAddress& out_addr) {
  if (!block::tlb::t_MsgAddressInt.extract_std_address(std::move(csr), out_wc, out_addr)) {
    return false;
  }
  return true;
}

}
