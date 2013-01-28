/*
 * Benchmark.h
 *
 *  Created on: Jan 28, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef BENCHMARK_H_
#define BENCHMARK_H_

#include <duke/NonCopyable.h>
#include <duke/gl/GlFwApp.h>

namespace duke {

struct Benchmark : public noncopyable {
    Benchmark();
private:
    DukeGLFWApplication m_Application;
};

} /* namespace duke */
#endif /* BENCHMARK_H_ */
