#include "driver_controller.h"

ULONG driver_controller::get_client_base(ULONG process_id)
{
    INFO_STRUCT info = { 0 };
    info.code = CLIENT_MODULE_REQUEST;
    info.process_id = process_id;

    hook::call_hook(&info);

    return info.client_base;
}

ULONG driver_controller::get_engine_base(ULONG process_id)
{
    INFO_STRUCT info = { 0 };
    info.code = ENGINE_MODULE_REQUEST;
    info.process_id = process_id;

    hook::call_hook(&info);

    return info.client_base;
}
