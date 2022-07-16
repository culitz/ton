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

std::string TransactionSerializeer::unpackFeeValue(vm::Ref<vm::CellSlice> csref) {
  block::CurrencyCollection collection;
  if(collection.unpack(csref)) {
      std::stringstream ss;
      ss << collection.grams;
      return ss.str();
  }
  return "0";
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

          msg_info.value = unpackFeeValue(info.value);
          msg_info.fwd_fee = unpackFeeValue(info.fwd_fee);
          msg_info.ihr_fee = unpackFeeValue(info.ihr_fee);
          inMsg->back().push_back(msg_info);
        }
        default:
          return false;
    }
}

}
