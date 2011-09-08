/*
 * TextureBuilder.h
 *
 *  Created on: 14 avr. 2011
 *      Author: Nicolas Rondaud
 */

#ifndef TEXTUREBUILDER_H_
#define TEXTUREBUILDER_H_

#include <duke_pb/IMessageIO.h>
#include <communication.pb.h>
#include <iostream>

namespace {

namespace dk = ::protocol::duke;

void buildTexture(dk::Texture & _t //
                  , std::string _name //
                  , dk::Texture_TextureDim _dimension //
                  , dk::Texture_TextureFormat _format //
                  , size_t _width //
                  , size_t _height //
                  , const void * _data //
                  , size_t _dataSize //
                  , bool _mipmap = false //
                  , std::string _clipName = "") {

    if (_clipName != "")
        _name += "_" + _clipName;

    _t.set_name(_name);
    _t.set_dimension(_dimension);
    _t.set_format(_format);
    _t.set_width(_width);
    _t.set_height(_height);
    _t.set_data(_data, _dataSize);
    _t.set_mipmap(_mipmap);
}

void buildRawTexture(dk::Texture & _t //
                     , std::string _name //
                     , const void * _data //
                     , const unsigned int _dataWidth //
                     , const unsigned int _dataHeight, //
                     std::string _clipName = "") {
    buildTexture(_t, _name, dk::Texture_TextureDim_DIM_2D, dk::Texture_TextureFormat_R32G32B32A32F, _dataWidth, _dataHeight, _data, _dataWidth * _dataHeight * 16, false, _clipName);
}

void build8bitTexture(dk::Texture & _t //
                      , std::string _name //
                      , const void * _data //
                      , const unsigned int _dataWidth //
                      , const unsigned int _dataHeight, //
                      std::string _clipName = "") {
    buildTexture(_t, _name, dk::Texture_TextureDim_DIM_2D, dk::Texture_TextureFormat_R8G8B8A8, _dataWidth, _dataHeight, _data, _dataWidth * _dataHeight * 4, false, _clipName);
}

void addRawTexture(IMessageIO & _queue //
                   , std::string _name //
                   , const void * _data //
                   , const unsigned int _dataWidth //
                   , const unsigned int _dataHeight, std::string _clipName = "") {
    dk::Texture t;
    buildTexture(t, _name, dk::Texture_TextureDim_DIM_2D, dk::Texture_TextureFormat_R32G32B32A32F, _dataWidth, _dataHeight, _data, _dataWidth * _dataHeight * 16, false, _clipName);
    push(_queue, t);
}

void add8bitTexture(IMessageIO & _queue //
                    , std::string _name //
                    , const void * _data //
                    , const unsigned int _dataWidth //
                    , const unsigned int _dataHeight //
                    , std::string _clipName = "") {
    dk::Texture t;
    buildTexture(t, _name, dk::Texture_TextureDim_DIM_2D, dk::Texture_TextureFormat_R8G8B8A8, _dataWidth, _dataHeight, _data, _dataWidth * _dataHeight * 4, false, _clipName);
    push(_queue, t);
}

} // empty namespace

#endif /* TEXTUREBUILDER_H_ */
