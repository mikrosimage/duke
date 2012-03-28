/*
 * SceneBuilder.h
 *
 *  Created on: 27 mars 2012
 *      Author: Guillaume Chatelet
 */

#ifndef SCENEBUILDER_H_
#define SCENEBUILDER_H_

#include <dukeapi/ProtobufSerialize.h>
#include <playlist.pb.h>
#include <player.pb.h>
#include <vector>

void normalize(duke::playlist::Playlist &playlist);
std::vector<google::protobuf::serialize::SharedHolder> getMessages(const duke::playlist::Playlist &playlist, const duke::protocol::Scene::PlaybackMode mode);

#endif /* SCENEBUILDER_H_ */
