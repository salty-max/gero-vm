/**
 * Jelly value.
 */
#ifndef JellyValue_h
#define JellyValue_h

#include <string>

/**
 * Jelly value type.
 */
enum class JellyValueType { NUMBER, OBJECT };

/**
 * Object type.
 */
enum class ObjectType { STRING };

/**
 * Base Object.
 */
struct Object {
  Object(ObjectType type) : type(type) {}
  ObjectType type;
};

/**
 * String object.
 */
struct StringObject : public Object {
  StringObject(const std::string &str)
      : Object(ObjectType::STRING), string(str) {}
  std::string string;
};

/**
 * Jelly value (tagged union)
 */
struct JellyValue {
  JellyValueType type;
  union {
    double number;
    Object *object;
  };
};

// ------------------------------------------------------------
// Constructors

#define NUMBER(value) ((JellyValue){JellyValueType::NUMBER, .number = value})

#define ALLOC_STRING(value)                                                    \
  ((JellyValue){JellyValueType::OBJECT,                                        \
                .object = (Object *)new StringObject(value)})

// ------------------------------------------------------------
// Accessors
#define AS_NUMBER(jellyValue) ((double)(jellyValue).number)

#define AS_STRING(jellyValue) ((StringObject *)(jellyValue).object)
#define AS_CPPSTRING(jellyValue) (AS_STRING(jellyValue)->string)

#endif