#include "account_serializer.h"

namespace json_serializer {

AccountSerializer& AccountSerializer::addAccountStorage(const Storage& storage) {
    storage_ptr = std::make_shared<Storage>(storage);
    return *this;
}

AccountSerializer& AccountSerializer::addWorkchain(const ton::WorkchainId workchain_id_) {
    workchain_id = workchain_id_;
    return *this;
}

AccountSerializer& AccountSerializer::addStdSmcAddress(const ton::StdSmcAddress& address) {
    address_ptr = std::make_shared<ton::StdSmcAddress>(address);
    return *this;
}

void AccountSerializer::onFillStart() {
    if(!address_ptr || !storage_ptr) {
        throw "Not enough data";
    }
    Parent::onFillStart();
}

void AccountSerializer::onFillEnd() {
    utils::fill_json(*writer_ptr, *address_ptr, false);
    utils::fill_json(*writer_ptr, *storage_ptr, false);
    writer_ptr->Key("workchain");
    writer_ptr->Int(workchain_id);
    Parent::onFillEnd();
}

}