#include "servlets.h"
#include "../../tTimestamp.h"
#include "../../TLE8457_serial/TLE8457_serial_lib.h"

#if CONFIG_HTTP_SERVER

#if CONFIG_HTTP_SERVLET_OUTCONTROL_JS

bool tOutputControlJavaScript::ProcessAndResponse()
{
	pOwner->SendFlashString(OutputControl_js_raw,OutputControl_js_raw_len);
	return false;
}

#endif //CONFIG_HTTP_SERVLET_OUTCONTROL_JS

#if CONFIG_TIMESTAMP_SERVLET

bool tTimestampServlet::ProcessAndResponse()
{
    uint16_t NewTimestamp;

    if (GetParameter("Timestamp",&NewTimestamp))
    {
        SendResponse400();
    }
    else
    {
#if CONFIG_TIMESTAMP
        tTimestamp::set(NewTimestamp);  // timestamp may be set locally...
#endif
#if CONFIG_TLE8457_COMM_LIB
        // and/or broadcasted
        tMessageTypeTimestamp Msg;
        Msg.timestamp = NewTimestamp;
        CommSenderProcess::Instance->Enqueue(DEVICE_ID_BROADCAST,MESSAGE_TYPE_TIMESTAMP,sizeof(Msg),&Msg);
#endif // CONFIG_TLE8457_COMM_LIB
        SendResponse200();

    }
    return false;
}

#endif // CONFIG_TIMESTAMP_SERVLET
#endif //CONFIG_HTTP_SERVER
