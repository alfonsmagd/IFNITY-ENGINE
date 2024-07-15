



#define EVENT_BEGIN(EventType) \
class EventType \
{


#define EVENT_END() };

/// Helper macro for defining the member varibale and the getter of an event
/// parameter.

#define EVENT_PARAMETER(ParameterType, ParameterName) \
private: \
   ParameterType m_ ## ParameterName; \
\
public: \
   ParameterType get ## ParameterName() const \
   { \
      return m_ ## ParameterName; \
   }

/// Macro for defining a class representing an event with no parameters
#define EVENT(EventType) \
EVENT_BEGIN(EventType) \
EVENT_END()

/// Macro for defining a class representing an event with one parameter
#define EVENT1(EventType, ParameterType, ParameterName) \
EVENT_BEGIN(EventType) \
public: \
   EventType(ParameterType value) : m_ ## ParameterName(value) \
   { \
   } \
\
EVENT_PARAMETER(ParameterType, ParameterName) \
EVENT_END()

/// Macro for defining a class representing an event with two parameters
#define EVENT2(EventType, Parameter1Type, Parameter1Name, Parameter2Type, Parameter2Name) \
EVENT_BEGIN(EventType) \
public: \
   EventType(Parameter1Type value1, Parameter2Type value2) \
      : m_ ## Parameter1Name(value1) \
      , m_ ## Parameter2Name(value2) \
   { \
   } \
\
EVENT_PARAMETER(Parameter1Type, Parameter1Name) \
EVENT_PARAMETER(Parameter2Type, Parameter2Name) \
EVENT_END()

