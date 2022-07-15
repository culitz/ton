#pragma once

#include <memory>
#include <vector>
#include "serializer.h"
#include "block/check-proof.h"
#include "block/block-auto.h"
#include "vm/cells.h"
#include "tl-utils/lite-utils.hpp"
#include "fill-utils.h"

namespace json_serializer { 

typedef vm::Cell Cell;
typedef vm::Ref<vm::Cell> CellRef;
typedef block::gen::CommonMsgInfo::Record_int_msg_info InMsgInfo;
typedef block::Transaction::Info TransactionInfo;



class TransactionSerializeer : public Serializer<TransactionInfo> {

private:
    typedef Serializer<TransactionInfo> Parent;
    std::shared_ptr<std::vector<utils::InMsgInfoVect>> inMsg;
    void unpackAddr(CellRef csr);

protected:
    virtual void beforeEach(size_t index) override;
    virtual void afterEach(size_t index) override;

public:
    TransactionSerializeer() : Serializer<TransactionInfo>() {
        inMsg = std::shared_ptr<std::vector<utils::InMsgInfoVect>>(new std::vector<utils::InMsgInfoVect>);
    }
    virtual ~TransactionSerializeer() {}

    void fromBlock(TransactionInfo& info);
    bool addMessageIn(vm::Ref<vm::Cell> msg);
    virtual void add(TransactionInfo info) override;
};

}
