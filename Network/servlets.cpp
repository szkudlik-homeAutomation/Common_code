#include "servlets.h"


#if CONFIG_HTTP_SERVLET_OUTCONTROL_JS
#ifdef OutputControl_js_raw_len
bool tOutputControlJavaScript::ProcessAndResponse()
{
	pOwner->SendFlashString(OutputControl_js_raw,OutputControl_js_raw_len);
	return false;
}
#endif
#endif //CONFIG_HTTP_SERVLET_OUTCONTROL_JS
