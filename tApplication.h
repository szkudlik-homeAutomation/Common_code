/*
 * tApplication.h
 *
 *  Created on: 11 lip 2024
 *      Author: szkud
 */

#ifndef SRC_COMMON_CODE_TAPPLICATION_H_
#define SRC_COMMON_CODE_TAPPLICATION_H_

#include "../../global.h"

class tApplication {
public:
	static tApplication *Instance;
	tApplication() {Instance = this;}

	void Setup();

protected:
	/* called just after the logger is initialized
	 * before any other initialization
	 */
	virtual void AppSetupBefore() {}

	/* called just after the logger is initialized
	 * after all other initialization
	 * before SYSTEM INITIALIZED message
	 */
	virtual void AppSetupAfter() {}
};

#endif /* SRC_COMMON_CODE_TAPPLICATION_H_ */
