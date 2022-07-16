#include "block_id_serializer.h"

namespace json_serializer {

void BlockIdSerializer::beforeEach(size_t index) {
    writer_ptr->StartObject();
}

void BlockIdSerializer::afterEach(size_t index) {
    writer_ptr->EndObject();
}

}