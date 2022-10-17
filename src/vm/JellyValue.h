/**
 * Jelly value.
 */
#ifndef JellyValue_h
#define JellyValue_h

/**
 * Jelly value type.
 */
enum class JellyValueType { NUMBER };

/**
 * Jelly value (tagged union)
 */
struct JellyValue {
  JellyValueType type;
  union {
    double number;
  };
};

// ------------------------------------------------------------
// Constructors

#define NUMBER(value) ((JellyValue){JellyValueType::NUMBER, .number = value})

// ------------------------------------------------------------
// Accessors
#define AS_NUMBER(jellyValue) ((double)(jellyValue).number)

#endif