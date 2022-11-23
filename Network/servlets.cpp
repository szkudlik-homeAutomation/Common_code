#include "servlets.h"

#ifdef OutputControl_js_raw_len
bool tOutputControlJavaScript::ProcessAndResponse()
{
	pOwner->SendFlashString(OutputControl_js_raw,OutputControl_js_raw_len);
	return false;
}
#endif
