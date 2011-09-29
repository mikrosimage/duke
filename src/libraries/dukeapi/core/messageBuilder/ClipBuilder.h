#ifndef CLIPBUILDER_H_
#define CLIPBUILDER_H_

#include <player.pb.h>
#include <iostream>

namespace {

namespace dk = ::duke::protocol;

void buildClip(dk::Clip & _c //
               , std::string _name = "defaultClipName" //
               , size_t _recin = 0 //
               , size_t _recout = 1 //
               , size_t _srcin = 0 //
               , std::string _path = ""//
               , std::string _filename = "") {
    _c.set_name(_name);
    _c.set_recin(_recin);
    _c.set_recout(_recout);
    _c.set_srcin(_srcin);
    if (_path != "")
        _c.set_path(_path);
    if (_filename != "")
        _c.set_filename(_filename);
}

dk::Clip * const addClipToPlaylist(dk::Playlist & _p //
                                   , std::string _name = "defaultClipName" //
                                   , size_t _recin = 0 //
                                   , size_t _recout = 1 //
                                   , size_t _srcin = 0 //
                                   , std::string _path = ""//
                                   , std::string _filename = "") {

    dk::Clip * const pClip = _p.add_clip();
    buildClip(*pClip, _name, _recin, _recout, _srcin, _path, _filename);
    return pClip;
}

} // empty namespace

#endif /* CLIPBUILDER_H_ */
