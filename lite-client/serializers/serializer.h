#pragma once

#include <vector>
#include <memory>
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <ton/ton-types.h>
#include "block/block-auto.h"
#include "crypto/block/block.h"
#include "crypto/block/check-proof.h"
#include "fill-utils.h"

namespace json_serializer {

typedef rapidjson::StringBuffer JsonBuffer;
typedef rapidjson::Writer<rapidjson::StringBuffer> JsonWriter;
typedef std::shared_ptr<JsonBuffer> JsonBufferPtr;
typedef std::shared_ptr<JsonWriter> JsonWriterPtr;


template<typename T>
class Serializer {
protected:
    JsonBufferPtr buffer_ptr;
    JsonWriterPtr writer_ptr;
    std::shared_ptr<std::vector<T>> obj_ptr;
    size_t objectsCount;
    bool always_array_wrap;

    virtual void fill() {
        bool needSelfScope = isMultiplie();
        for(size_t i = 0; i < obj_ptr->size(); i++) {
            beforeEach(i);
            utils::fill_json(*writer_ptr, (*obj_ptr)[i], needSelfScope);
            afterEach(i);
        }
    }

    virtual void onFillEnd() {
        if(always_array_wrap) {
            writer_ptr->EndArray();
        } else {
            if(!isMultiplie())
                writer_ptr->EndObject();
            else
                writer_ptr->EndArray();
        }
    }

    virtual void onFillStart() {
        if(always_array_wrap) {
            writer_ptr->StartArray();
        } else {
            if(!isMultiplie())
                writer_ptr->StartObject();
            else
                writer_ptr->StartArray();
        }
    }

    virtual void beforeEach(size_t index) {}
    virtual void afterEach(size_t index) {}

public:
    Serializer(bool always_wrap_array = false) : always_array_wrap(always_wrap_array) {
        objectsCount = 0;
        buffer_ptr = std::shared_ptr<JsonBuffer>(new JsonBuffer);
        writer_ptr = std::shared_ptr<JsonWriter>(new JsonWriter(*buffer_ptr));
        obj_ptr = std::shared_ptr<std::vector<T>>(new std::vector<T>());
    }
    virtual ~Serializer() {}

    virtual void add(T t) {
        obj_ptr->push_back(t);
        objectsCount = obj_ptr->size();
    }

    std::string json() {
        onFillStart();
        fill();
        onFillEnd();
        return buffer_ptr->GetString();
    }

    bool isMultiplie() { return objectsCount != 1; }
};

}
