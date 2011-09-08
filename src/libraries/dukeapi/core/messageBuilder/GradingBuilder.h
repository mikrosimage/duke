/*
 * GradingBuilder.h
 *
 *  Created on: 14 avr. 2011
 *      Author: Nicolas Rondaud
 */

#ifndef GRADINGBUILDER_H_
#define GRADINGBUILDER_H_

#include <dukeapi/core/IMessageIO.h>
#include <dukeapi/protocol/player/communication.pb.h>
#include <iostream>

namespace {

namespace dk = ::protocol::duke;

// ------------------------------------------------------
// RenderPass
// ------------------------------------------------------
void buildRenderPass(dk::RenderPass* const _pPass //
                     , std::string _name //
                     , std::string _meshName //
                     , bool _clean) {
    _pPass->set_name(_name);
    _pPass->set_clean(_clean);
    _pPass->set_cleancolor(0xFF00FF00);
    _pPass->add_meshname(_meshName);
}

dk::RenderPass* const addRenderPassToGrading(dk::Grading & _g //
                                             , std::string _name //
                                             , std::string _meshName //
                                             , bool _clean) {
    dk::RenderPass* const pPass = _g.add_pass();
    buildRenderPass(pPass, _name, _meshName, _clean);
    return pPass;
}

// ------------------------------------------------------
// Effect
// ------------------------------------------------------
void buildEffect(dk::Effect* const _pEffect //
                 , std::string _psName //
                 , std::string _vsName) {
    _pEffect->set_pixelshadername(_psName);
    _pEffect->set_vertexshadername(_vsName);
    _pEffect->set_alphablend(false);
}

void addEffectToRenderPass(dk::RenderPass & _r //
                           , std::string _psName //
                           , std::string _vsName) {
    dk::Effect* pEffect = _r.mutable_effect();
    buildEffect(pEffect, _psName, _vsName);
}

void addEffectToGrading(dk::Grading & _g //
                        , std::string _psName //
                        , std::string _vsName //
                        , std::string _meshName //
                        , bool _clean) {
    dk::RenderPass* const pPass = _g.add_pass();
    buildRenderPass(pPass, "pass", _meshName, _clean);

    addEffectToRenderPass(*pPass, _psName, _vsName);
}

// ------------------------------------------------------
// Grading
// ------------------------------------------------------
void buildGrading(dk::Grading & _g //
                  , std::string _name = "grading" //
                  , std::string _clipName = "") {

    if (_clipName != "")
        _name += "_" + _clipName;

    _g.set_name(_name);
}

dk::Grading * const addGradingToClip(dk::Clip & _c //
                                     , std::string _name = "grading") {
    if (!_c.has_name()) {
        throw std::runtime_error("Error building Grading: undefined clip name.");
    }
    dk::Grading * const pGrading = _c.mutable_grade();
    buildGrading(*pGrading, _name, "");

    return pGrading;
}

void addGradingToQueue(IMessageIO & _queue //
                       , std::string _name //
                       , std::string _clipName = "") {
    dk::Grading g;
    buildGrading(g, _name, _clipName);
    push(_queue, g);
}

} // empty namespace

#endif /* GRADINGBUILDER_H_ */
