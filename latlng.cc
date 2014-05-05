#include <node.h>
#include <nan.h>
#include "node_object_wrap.h"           // for ObjectWrap
#include "v8.h"                         // for Handle, String, Integer, etc

#include "s2.h"
#include "s2latlng.h"
#include "latlng.h"
#include "point.h"

using namespace v8;

Persistent<FunctionTemplate> LatLng::constructor;

void LatLng::Init(Handle<Object> target) {
    NanScope();

    constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(LatLng::New));
    Local<String> name = String::NewSymbol("LatLng");

    constructor->InstanceTemplate()->SetInternalFieldCount(1);
    constructor->SetClassName(name);

    // Add all prototype methods, getters and setters here.
    NODE_SET_PROTOTYPE_METHOD(constructor, "lat", Lat);
    NODE_SET_PROTOTYPE_METHOD(constructor, "lng", Lng);
    NODE_SET_PROTOTYPE_METHOD(constructor, "normalized", Normalized);
    NODE_SET_PROTOTYPE_METHOD(constructor, "isValid", IsValid);
    NODE_SET_PROTOTYPE_METHOD(constructor, "toPoint", ToPoint);

    // This has to be last, otherwise the properties won't show up on the
    // object in JavaScript.
    target->Set(name, constructor->GetFunction());
}

LatLng::LatLng()
    : ObjectWrap(),
      this_() {}

Handle<Value> LatLng::New(const Arguments& args) {
    NanScope();

    if (!args.IsConstructCall()) {
        return NanThrowError("Use the new operator to create instances of this object.");
    }

    if (args[0]->IsExternal()) {
        Local<External> ext = Local<External>::Cast(args[0]);
        void* ptr = ext->Value();
        LatLng* ll = static_cast<LatLng*>(ptr);
        ll->Wrap(args.This());
        return args.This();
    }

    if (args.Length() != 2) {
        return NanThrowError("(number, number) required");
    }

    LatLng* obj = new LatLng();

    obj->Wrap(args.This());

    obj->this_ = S2LatLng::FromDegrees(
        args[0]->ToNumber()->Value(),
        args[1]->ToNumber()->Value());

    return args.This();
}

Handle<Value> LatLng::New(S2LatLng s2latlng) {
    NanScope();
    LatLng* obj = new LatLng();
    obj->this_ = s2latlng;
    Handle<Value> ext = External::New(obj);
    Handle<Object> handleObject = constructor->GetFunction()->NewInstance(1, &ext);
    return scope.Close(handleObject);
}

NAN_METHOD(LatLng::Lat) {
    NanScope();
    LatLng* obj = ObjectWrap::Unwrap<LatLng>(args.This());
    NanReturnValue(NanNew<Number>(obj->this_.lat().degrees()));
}

NAN_METHOD(LatLng::Lng) {
    NanScope();
    LatLng* obj = ObjectWrap::Unwrap<LatLng>(args.This());
    NanReturnValue(NanNew<Number>(obj->this_.lng().degrees()));
}

NAN_METHOD(LatLng::IsValid) {
    NanScope();
    LatLng* obj = ObjectWrap::Unwrap<LatLng>(args.This());
    NanReturnValue(NanNew<Boolean>(obj->this_.is_valid()));
}

NAN_METHOD(LatLng::Normalized) {
    NanScope();
    LatLng* obj = ObjectWrap::Unwrap<LatLng>(args.This());
    return scope.Close(LatLng::New(obj->this_.Normalized()));
}

NAN_METHOD(LatLng::ToPoint) {
    NanScope();
    LatLng* obj = ObjectWrap::Unwrap<LatLng>(args.This());
    return scope.Close(Point::New(obj->this_.ToPoint()));
}
