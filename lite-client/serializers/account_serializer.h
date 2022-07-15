#pragma once
#include <memory>
#include "fill-utils.h"
#include "serializer.h"
#include <ton/ton-types.h>
#include "crypto/block/block.h"


namespace json_serializer {

typedef block::AccountState::Info AccountInfo;
typedef block::gen::AccountStorage::Record Storage;

class AccountSerializer : public Serializer<AccountInfo> {
public:
    typedef Serializer<AccountInfo> Parent;

    AccountSerializer() : Serializer<AccountInfo>() {}
    virtual ~AccountSerializer() {}

    AccountSerializer& addAccountStorage(const Storage& storage);
    AccountSerializer& addStdSmcAddress(const ton::StdSmcAddress& address);
    AccountSerializer& addWorkchain(const ton::WorkchainId workchain_id);

    virtual void onFillStart() override;
    virtual void onFillEnd() override;

private:
    std::shared_ptr<ton::StdSmcAddress> address_ptr;
    std::shared_ptr<Storage> storage_ptr;
    ton::WorkchainId workchain_id;
};

}
