/*
 * MessageHolder.h
 *
 *  Created on: 1 juil. 2010
 *      Author: Guillaume Chatelet
 */

#ifndef MESSAGEHOLDER_H_
#define MESSAGEHOLDER_H_

#include <dukeapi/protocol/player/communication.pb.h>

#include <memory>
#include <stdexcept>
#include <iostream>
#include <boost/unordered_map.hpp>

namespace MessageHelper {

class MessageTypeHelper {
private:
    boost::unordered_map<protocol::duke::MessageType_Type, const google::protobuf::Message*> m_TypeToDefaultInstance;
    boost::unordered_map<const google::protobuf::Descriptor*, protocol::duke::MessageType_Type> m_DescriptorToType;
    void addPair(const ::protocol::duke::MessageType_Type type, const google::protobuf::Message& defaultMsg) {
        m_TypeToDefaultInstance.insert(std::make_pair(type, &defaultMsg));
        m_DescriptorToType.insert(std::make_pair(defaultMsg.GetDescriptor(), type));
    }
public:
    MessageTypeHelper() {
        using namespace google::protobuf;
        using namespace protocol::duke;
        addPair(MessageType_Type_INIT_PLAYLIST, Playlist::default_instance());
        addPair(MessageType_Type_INIT_RENDERER, protocol::duke::Renderer::default_instance());
        addPair(MessageType_Type_DEBUG_MSG, Debug::default_instance());
        addPair(MessageType_Type_EVENT, Event::default_instance());
        addPair(MessageType_Type_QUIT, Quit::default_instance());
        addPair(MessageType_Type_TRANSPORT, Transport::default_instance());
        addPair(MessageType_Type_SHADER, Shader::default_instance());
        addPair(MessageType_Type_MESH, Mesh::default_instance());
        addPair(MessageType_Type_TEXTURE, Texture::default_instance());
        addPair(MessageType_Type_STATIC_PARAMETER, StaticParameter::default_instance());
        addPair(MessageType_Type_AUTOMATIC_PARAMETER, AutomaticParameter::default_instance());
        addPair(MessageType_Type_GRADING, Grading::default_instance());
        addPair(MessageType_Type_ENGINE, Engine::default_instance());

        using namespace protocol::shader_assembler;
        addPair(MessageType_Type_SHADING_FUNCTION, FunctionPrototype::default_instance());
        addPair(MessageType_Type_SHADING_PROGRAM, Program::default_instance());
    }

    std::unique_ptr<google::protobuf::Message> newMessage(const protocol::duke::MessageType_Type type) const {
        const auto itr = m_TypeToDefaultInstance.find(type);
        if (itr == m_TypeToDefaultInstance.end())
            throw std::runtime_error("cannot create newMessage : unknown type");
        return std::unique_ptr<google::protobuf::Message>(itr->second->New());
    }

    std::unique_ptr<google::protobuf::Message> copy(const ::google::protobuf::Message& message) const {
        if (!message.IsInitialized())
            throw std::runtime_error("Cannot make a copy of an uninitialized message");
        using namespace google::protobuf;
        std::unique_ptr<Message> pCopy(newMessage(typeOf(message)));
        pCopy->CopyFrom(message);
        return pCopy;
    }

    protocol::duke::MessageType_Type typeOf(const google::protobuf::Message& msg) const {
        const auto itr = m_DescriptorToType.find(msg.GetDescriptor());
        if (itr == m_DescriptorToType.end())
            throw std::runtime_error("unknown message type");
        return itr->second;
    }
};

extern const MessageTypeHelper instance;

} // namespace MessageHelper

struct MessageCopy {
private:
    const std::unique_ptr<google::protobuf::Message> m_pMessage;
    const ::protocol::duke::MessageType_Action m_Operation;
public:
    MessageCopy(std::unique_ptr<google::protobuf::Message>& pMessage, ::protocol::duke::MessageType_Action action) :
        m_pMessage(std::move(pMessage)), m_Operation(action) {
        if (!m_pMessage)
            throw std::runtime_error("Cannot use a NULL message");
    }

    MessageCopy(const ::google::protobuf::Message& message, ::protocol::duke::MessageType_Action action) :
        m_pMessage(MessageHelper::instance.copy(message)), m_Operation(action) {
    }

    ::protocol::duke::MessageType_Type type() const {
        return MessageHelper::instance.typeOf(message());
    }

    inline ::protocol::duke::MessageType_Action operation() const {
        return m_Operation;
    }

    inline ::protocol::duke::MessageType messageType() const {
        ::protocol::duke::MessageType typeMessage;
        typeMessage.set_action(operation());
        typeMessage.set_type(type());
        return typeMessage;
    }

    template<class MESSAGE_TYPE = ::google::protobuf::Message>
    inline const MESSAGE_TYPE& message() const {
        return dynamic_cast<MESSAGE_TYPE&> (*m_pMessage.get());
    }
};

typedef std::shared_ptr<MessageCopy> SharedMessage;

#endif /* MESSAGEHOLDER_H_ */
