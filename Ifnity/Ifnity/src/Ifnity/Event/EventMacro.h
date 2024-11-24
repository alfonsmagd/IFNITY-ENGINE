

#pragma once

class IFNITY_API Event
{public:
	virtual ~Event() = default;
    bool Handled = false;
};


#define EVENT_DESTRUCTOR(EventType) \
public: \
   virtual ~EventType() = default;

#define EVENT_BEGIN(EventType) \
class IFNITY_API EventType : public Event  \
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


#define EVENT_TOSTRING(EventType, Description) \
public: \
   std::string ToString() const  \
   { \
      std::stringstream ss; \
      ss << Description; \
      return ss.str(); \
   }

#define EVENT_TOSTRING2(EventType, Parameter1Name, Parameter2Name) \
public: \
   std::string ToString() const  \
   { \
      std::stringstream ss; \
      ss << #EventType << ": " << get ## Parameter1Name() << ", " << get ## Parameter2Name(); \
      return ss.str(); \
   }

// Event with 3 parameters to String    
#define EVENT_TOSTRING3(EventType, Parameter1Name, Parameter2Name, Parameter3Name) \
public: \
   std::string ToString() const  \
   { \
	  std::stringstream ss; \
	  ss << #EventType << ": " << get ## Parameter1Name() << ", " << get ## Parameter2Name() << ", " << get ## Parameter3Name(); \
	  return ss.str(); \
   }


#define EVENT_TOSTRING1(EventType, Parameter1Name) \
public: \
   std::string ToString() const  \
   { \
      std::stringstream ss; \
      ss << #EventType << ": " << get ## Parameter1Name(); \
      return ss.str(); \
   }
/// Macro for defining a class representing an event with no parameters
#define EVENT(EventType,Description) \
EVENT_BEGIN(EventType) \
EVENT_TOSTRING(EventType, Description) \
EVENT_DESTRUCTOR(EventType) \
EVENT_END()

/// Macro for defining a class representing an event with one parameter
#define EVENT1(EventType, ParameterType, ParameterName) \
EVENT_BEGIN(EventType) \
public: \
   EventType(ParameterType value) : m_ ## ParameterName(value) \
   { \
   } \
\
EVENT_TOSTRING1(EventType, ParameterName)\
EVENT_PARAMETER(ParameterType, ParameterName) \
EVENT_DESTRUCTOR(EventType) \
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
EVENT_TOSTRING2(EventType, Parameter1Name, Parameter2Name) \
EVENT_DESTRUCTOR(EventType) \
EVENT_END()

/// Macro for defining a class representing an event with two parameters
#define EVENT3(EventType, Parameter1Type, Parameter1Name, Parameter2Type, Parameter2Name, Parameter3Type, Parameter3Name) \
EVENT_BEGIN(EventType) \
public: \
   EventType(Parameter1Type value1, Parameter2Type value2, Parameter3Type value3) \
	  : m_ ## Parameter1Name(value1) \
	  , m_ ## Parameter2Name(value2) \
	  , m_ ## Parameter3Name(value3) \
   { \
   } \
\
EVENT_PARAMETER(Parameter1Type, Parameter1Name) \
EVENT_PARAMETER(Parameter2Type, Parameter2Name) \
EVENT_PARAMETER(Parameter3Type, Parameter3Name) \
EVENT_TOSTRING3(EventType, Parameter1Name, Parameter2Name, Parameter3Name) \
EVENT_DESTRUCTOR(EventType) \
EVENT_END()



//Helper And Macros. 
#define CONNECT_EVENT_LAYER(eventType, eventBus) \
    IFNITY::events::connect<IFNITY::eventType>(*eventBus, *this)

//Helper And 
#define CONNECT_SINGLE_EVENT(eventType, eventBus) \
    IFNITY::events::connect<IFNITY::eventType>(*eventBus, *this)

//This implementation assumes that eventCamera its a pointer to a class that inherits from EventListener
#define CONNECT_EVENT_CAMERA(eventType, eventBus,eventCamera) \
	IFNITY::events::connect<IFNITY::eventType>(*eventBus, *eventCamera)

#define CONNECT_EVENT(x) ConnectEvent<x>()