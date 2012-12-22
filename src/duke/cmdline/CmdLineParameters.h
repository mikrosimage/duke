/*
 * CmdLineParameters.h
 *
 *  Created on: Dec 2, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef CMDLINEPARAMETERS_H_
#define CMDLINEPARAMETERS_H_

struct CmdLineParameters {
	CmdLineParameters(int argc, char**argv);
	unsigned swapBufferInterval;
};

#endif /* CMDLINEPARAMETERS_H_ */
