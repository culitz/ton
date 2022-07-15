#include "transactions_serializer.h"

namespace json_serializer { 

void TransactionSerializeer::add(TransactionInfo info) {
    inMsg->push_back(utils::InMsgInfoVect{});
    Parent::add(info);
}

void TransactionSerializeer::beforeEach(size_t /*index*/) {
    writer_ptr->StartObject();
}

void TransactionSerializeer::afterEach(size_t index) {
    writer_ptr->Key("messages");
    utils::fill_json(*writer_ptr, (*inMsg)[index]);
    writer_ptr->EndObject();
}

void TransactionSerializeer::fromBlock(TransactionInfo& info) {
    
}

bool TransactionSerializeer::addMessageIn(CellRef msg) {
    utils::InMsgInfo msg_info;

    vm::CellSlice cs{vm::NoVmOrd(), msg};
    switch (block::gen::t_CommonMsgInfo.get_tag(cs)) {
        case block::gen::CommonMsgInfo::ext_in_msg_info: {
          block::gen::CommonMsgInfo::Record_ext_in_msg_info info;
          if (!tlb::unpack(cs, info))
            return false;

          ton::WorkchainId wc;
          ton::StdSmcAddress addr;
          if (!utils::unpack_addr(std::move(info.dest), wc, addr))
            return false;
        }

        case block::gen::CommonMsgInfo::int_msg_info: {
          block::gen::CommonMsgInfo::Record_int_msg_info info;
          if (!tlb::unpack(cs, info))
            return false;

          ton::WorkchainId wc1;
          ton::StdSmcAddress addr1;
          if (!utils::unpack_addr(std::move(info.src), wc1, addr1))
            return false;


          msg_info.src.workchain = wc1;
          msg_info.src.addr = addr1.to_hex();

          ton::WorkchainId wc0;
          ton::StdSmcAddress addr0;
          if (!utils::unpack_addr(std::move(info.dest), wc0, addr0))
              return false;

          msg_info.dest.workchain = wc0;
          msg_info.dest.addr = addr0.to_hex();

          msg_info.bounce = info.bounce;
          msg_info.bounced = info.bounced;
          msg_info.ihr_disabled = info.ihr_disabled;


          std::stringstream ss;
          td::RefInt256 value;
          CellRef extra;
          block::unpack_CurrencyCollection(info.fwd_fee, value, extra);
          ss << value;
          msg_info.value = ss.str();
//          msg_info.ihr_fee = (int) info.ihr_fee;
//          msg_info.fwd_fee = (int) info.fwd_fee;          
          inMsg->back().push_back(msg_info);
        }
        default:
          return false;
    }
}

}
