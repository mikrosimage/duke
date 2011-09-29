#include "InteractiveMessageIO.h"
#include <protocol.pb.h>
#include <player.pb.h>
#include <iostream>
#include <cassert>

using namespace ::duke::protocol;
using namespace ::google::protobuf::serialize;

namespace {

Transport MAKE(const Transport_TransportType type //
               , const int value = -1 //
               , const bool cueRelative = false //
               , const bool cueClip = false) {
    Transport transport;
    transport.set_type(type);
    if (type == Transport_TransportType_CUE) {
        if (!cueRelative && value < 0)
            throw std::runtime_error("can't cue to a negative frame");
        Transport_Cue *cue = transport.mutable_cue();
        cue->set_cueclip(cueClip);
        cue->set_cuerelative(cueRelative);
        cue->set_value(value);
    }
    return transport;
}

#define PUSH(X) ::push(m_ToApplicationQueue,X)

} // namespace

InteractiveMessageIO::InteractiveMessageIO(MessageQueue& initialMessages) :
    m_ToApplicationQueue(initialMessages), m_bPlay(false), m_iFitMode(0) {
}

InteractiveMessageIO::~InteractiveMessageIO() {
}

bool InteractiveMessageIO::tryPop(SharedHolder& holder) {
    return m_ToApplicationQueue.tryPop(holder);
}

void InteractiveMessageIO::waitPop(SharedHolder& holder) {
    m_ToApplicationQueue.waitPop(holder);
}

void InteractiveMessageIO::push(const SharedHolder& pHolder) {
    using namespace ::duke::protocol;

    if (!pHolder)
        throw std::runtime_error("Trying to interact with a NULL message");
    const MessageHolder &holder = *pHolder;

    // we are taking into account only Event type messages
    if (!isType<Event> (holder))
        return;

    Event event;
    unpack(holder, event);
    switch (event.type()) {
        case Event_Type_KEYPRESSED: {
            switch (event.keyevent().code()) {
                case KeyEvent_KeyCode_Space:
                    m_bPlay = !m_bPlay;
                    if (m_bPlay)
                        PUSH(MAKE(Transport_TransportType_PLAY));
                    else
                        PUSH(MAKE(Transport_TransportType_STOP));
                    break;
                case KeyEvent_KeyCode_Left:
                    m_bPlay = false;
                    if (event.keyevent().shift())
                        PUSH(MAKE(Transport_TransportType_CUE, -100, true));
                    else
                        PUSH(MAKE(Transport_TransportType_CUE, -1, true));
                    break;
                case KeyEvent_KeyCode_Right:
                    m_bPlay = false;
                    if (event.keyevent().shift())
                        PUSH(MAKE(Transport_TransportType_CUE, 100, true));
                    else
                        PUSH(MAKE(Transport_TransportType_CUE, 1, true));
                    break;
                case KeyEvent_KeyCode_Home:
                    m_bPlay = false;
                    PUSH(MAKE(Transport_TransportType_CUE_FIRST));
                    break;
                case KeyEvent_KeyCode_End:
                    m_bPlay = false;
                    PUSH(MAKE(Transport_TransportType_CUE_LAST));
                    break;
                case KeyEvent_KeyCode_Escape:
                    PUSH(Quit());
                    break;
                case KeyEvent_KeyCode_PageUp:
                    PUSH(MAKE(Transport_TransportType_CUE, 1, true, true));
                    break;
                case KeyEvent_KeyCode_PageDown:
                    PUSH(MAKE(Transport_TransportType_CUE, -1, true, true));
                    break;
                case KeyEvent_KeyCode_F: {
                    m_iFitMode = (m_iFitMode + 1) % 4;
                    StaticParameter displayMode;
                    displayMode.set_name("displayMode");
                    displayMode.set_type(StaticParameter_Type_FLOAT);
                    displayMode.add_floatvalue(m_iFitMode);
                    PUSH(displayMode);
                    break;
                }
                case KeyEvent_KeyCode_I: {
                    Debug d;
                    d.add_line("INFO:");
                    d.add_line(". current frame: %0");
                    d.add_line(". associated file(s): %1");
                    d.add_line(". FPS: %2");
                    d.add_content(Debug_Content_FRAME);
                    d.add_content(Debug_Content_FILENAMES);
                    d.add_content(Debug_Content_FPS);
                    PUSH(d);
                    break;
                }
                case KeyEvent_KeyCode_G: {
                    PUSH(MAKE(Transport_TransportType_CUE, atoi(m_ssSeek.str().c_str()), false, false));
                    m_ssSeek.str("");
                    m_ssSeek.clear();
                    break;
                }
                case KeyEvent_KeyCode_Num0:
                case KeyEvent_KeyCode_Numpad0:
                    m_ssSeek << 0;
                    break;
                case KeyEvent_KeyCode_Num1:
                case KeyEvent_KeyCode_Numpad1:
                    m_ssSeek << 1;
                    break;
                case KeyEvent_KeyCode_Num2:
                case KeyEvent_KeyCode_Numpad2:
                    m_ssSeek << 2;
                    break;
                case KeyEvent_KeyCode_Num3:
                case KeyEvent_KeyCode_Numpad3:
                    m_ssSeek << 3;
                    break;
                case KeyEvent_KeyCode_Num4:
                case KeyEvent_KeyCode_Numpad4:
                    m_ssSeek << 4;
                    break;
                case KeyEvent_KeyCode_Num5:
                case KeyEvent_KeyCode_Numpad5:
                    m_ssSeek << 5;
                    break;
                case KeyEvent_KeyCode_Num6:
                case KeyEvent_KeyCode_Numpad6:
                    m_ssSeek << 6;
                    break;
                case KeyEvent_KeyCode_Num7:
                case KeyEvent_KeyCode_Numpad7:
                    m_ssSeek << 7;
                    break;
                case KeyEvent_KeyCode_Num8:
                case KeyEvent_KeyCode_Numpad8:
                    m_ssSeek << 8;
                    break;
                case KeyEvent_KeyCode_Num9:
                case KeyEvent_KeyCode_Numpad9:
                    m_ssSeek << 9;
                    break;
                default:
                    break;
            }
            break;
        }
        case Event_Type_CLOSED:
            PUSH(Quit());
            break;
        default:
            break;
    }
}
