

#include <Shaders/MaterialData.h>

struct DrawData {
  uint transformId;
  uint materialId;
};

layout(std430, buffer_reference) readonly buffer TransformBuffer {
  mat4 model[];
};

layout(std430, buffer_reference) readonly buffer DrawDataBuffer {
  DrawData dd[];
};

layout(std430, buffer_reference) readonly buffer MaterialBuffer {
  MaterialData material[];
};

layout(push_constant) uniform PerFrameData {
  mat4 viewProj;
  TransformBuffer transforms;
  DrawDataBuffer drawData;
  MaterialBuffer materials;
} pc;