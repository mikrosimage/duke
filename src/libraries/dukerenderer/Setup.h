/*
 * Setup.h
 *
 *  Created on: 28 avr. 2010
 *      Author: Guillaume Chatelet
 */

#ifndef SETUP_H_
#define SETUP_H_

#include <dukeio/ImageDescription.h>
#include <communication.pb.h>
#include <vector>

typedef struct Setup {
    size_t m_iFrame;
    std::vector<protocol::duke::Clip> m_Clips;
    std::vector<ImageDescription> m_Images;
} Setup;

#endif /* SETUP_H_ */
