#include "script_continuation.hpp"

gorc::cog::scripts::script_continuation::script_continuation(int InstanceId,
        int SenderId, int SenderRef, flags::message_type SenderType,
        int SourceRef, flags::message_type SourceType,
        cog::vm::value Param0, cog::vm::value Param1, cog::vm::value Param2, cog::vm::value Param3,
        size_t ProgramCounter)
    : instance_id(InstanceId),
      sender_id(SenderId), sender_ref(SenderRef), sender_type(SenderType),
      source_ref(SourceRef), source_type(SourceType),
      params({ Param0, Param1, Param2, Param3 }), program_counter(ProgramCounter) {
    return;
}
