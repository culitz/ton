#pragma once
#include <vector>
#include "crypto/block/block.h"
#include "serializer.h"

namespace json_serializer {


class BlockIdSerializer : public Serializer<ton::BlockId> {
public:
    BlockIdSerializer() : Serializer<ton::BlockId>(true) {}
    virtual ~BlockIdSerializer() {}
protected:
    virtual void beforeEach(size_t index) override;
    virtual void afterEach(size_t index) override;
};

}