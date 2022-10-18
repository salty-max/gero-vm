/**
 * Jelly value.
 */
#ifndef JellyValue_h
#define JellyValue_h

#include <string>

/**
 * Jelly value type.
 */
enum class JellyValueType { NUMBER, BOOLEAN, OBJECT };

/**
 * Object type.
 */
enum class ObjectType { STRING, CODE };

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
    bool boolean;
    Object *object;
  };
};

/**
 * Code object.
 */
struct CodeObject : public Object {
  CodeObject(const std::string &name) : Object(ObjectType::CODE), name(name) {}
  /**
   * Name of the unit (usually function name).
   */
  std::string name;

  /**
   * Constant pool.
   */
  std::vector<JellyValue> constants;

  /**
   * Bytecode.
   */
  std::vector<uint8_t> code;
};

// ------------------------------------------------------------
// Constructors
#define NUMBER(value) ((JellyValue){JellyValueType::NUMBER, .number = value})
#define BOOLEAN(value) ((JellyValue){JellyValueType::BOOLEAN, .boolean = value})

#define ALLOC_STRING(value)                                                    \
  ((JellyValue){JellyValueType::OBJECT,                                        \
                .object = (Object *)new StringObject(value)})

#define ALLOC_CODE(name)                                                       \
  ((JellyValue){JellyValueType::OBJECT,                                        \
                .object = (Object *)new CodeObject(name)})

// ------------------------------------------------------------
// Accessors
#define AS_NUMBER(jellyValue) ((double)(jellyValue).number)
#define AS_BOOLEAN(jellyValue) ((bool)(jellyValue).boolean)
#define AS_OBJECT(jellyValue) ((Object *)(jellyValue).object)

#define AS_STRING(jellyValue) ((StringObject *)(jellyValue).object)
#define AS_CPPSTRING(jellyValue) (AS_STRING(jellyValue)->string)

#define AS_CODE(jellyValue) ((CodeObject *)(jellyValue).object)

// ------------------------------------------------------------
// Testers
#define IS_NUMBER(jellyValue) ((jellyValue).type == JellyValueType::NUMBER)
#define IS_BOOLEAN(jellyValue) ((jellyValue).type == JellyValueType::BOOLEAN)
#define IS_OBJECT(jellyValue) ((jellyValue).type == JellyValueType::OBJECT)

#define IS_OBJECT_TYPE(jellyValue, objectType)                                 \
  (IS_OBJECT(jellyValue) && AS_OBJECT(jellyValue)->type == objectType)

#define IS_STRING(jellyValue) IS_OBJECT_TYPE(jellyValue, ObjectType::STRING)
#define IS_CODE(jellyValue) IS_OBJECT_TYPE(jellyValue, ObjectType::CODE)

/**
 * String representation used in constants for debug.
 */
std::string jellyValueToTypeString(const JellyValue &jellyValue) {
  if (IS_NUMBER(jellyValue)) {
    return "NUMBER";
  } else if (IS_BOOLEAN(jellyValue)) {
    return "BOOLEAN";
  } else if (IS_STRING(jellyValue)) {
    return "STRING";
  } else if (IS_CODE(jellyValue)) {
    return "CODE";
  } else {
    DIE << "jellyValueToTypeString: unknown type " << (int)jellyValue.type;
  }

  return ""; // Unreachable
}

/**
 * String representation used in constants for debug.
 */
std::string jellyValueToConstantString(const JellyValue &jellyValue) {
  std::stringstream ss;

  if (IS_NUMBER(jellyValue)) {
    ss << jellyValue.number;
  } else if (IS_BOOLEAN(jellyValue)) {
    ss << (jellyValue.boolean == true ? "true" : "false");
  } else if (IS_STRING(jellyValue)) {
    ss << '"' << AS_CPPSTRING(jellyValue) << '"';
  } else if (IS_CODE(jellyValue)) {
    auto code = AS_CODE(jellyValue);
    ss << "code " << code << ": " << code->name;
  } else {
    DIE << "jellyValueToConstantString: unknown type " << (int)jellyValue.type;
  }

  return ss.str();
}

/**
 * Output stream.
 */
std::ostream &operator<<(std::ostream &os, const JellyValue &jellyValue) {
  return os << "JellyValue (" << jellyValueToTypeString(jellyValue)
            << "): " << jellyValueToConstantString(jellyValue);
}

#endif