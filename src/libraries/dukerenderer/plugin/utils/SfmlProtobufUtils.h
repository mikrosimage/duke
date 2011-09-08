/*
 * SfmlProtobufUtils.h
 *
 *  Created on: 6 juil. 2010
 *      Author: Guillaume Chatelet
 */

#ifndef SFMLPROTOBUFUTILS_H_
#define SFMLPROTOBUFUTILS_H_

#include <communication.pb.h>
#include <SFML/Window/Event.hpp>

inline ::protocol::duke::Event_Type Get( const ::sf::Event::EventType& eventType )
{
	using namespace ::protocol::duke;

	switch( eventType )
	{
		case ::sf::Event::Closed:
			return Event_Type_CLOSED;
		case ::sf::Event::Resized:
			return Event_Type_RESIZED;
		case ::sf::Event::LostFocus:
			return Event_Type_LOSTFOCUS;
		case ::sf::Event::GainedFocus:
			return Event_Type_GAINEDFOCUS;
		case ::sf::Event::TextEntered:
			return Event_Type_TEXTENTERED;
		case ::sf::Event::KeyPressed:
			return Event_Type_KEYPRESSED;
		case ::sf::Event::KeyReleased:
			return Event_Type_KEYRELEASED;
		case ::sf::Event::MouseWheelMoved:
			return Event_Type_MOUSEWHEELMOVED;
		case ::sf::Event::MouseButtonPressed:
			return Event_Type_MOUSEBUTTONPRESSED;
		case ::sf::Event::MouseButtonReleased:
			return Event_Type_MOUSEBUTTONRELEASED;
		case ::sf::Event::MouseMoved:
			return Event_Type_MOUSEMOVED;
		case ::sf::Event::MouseEntered:
			return Event_Type_MOUSEENTERED;
		case ::sf::Event::MouseLeft:
			return Event_Type_MOUSELEFT;
		default:
			std::cerr << "Unknown event type " << eventType << std::endl;
	}
	return Event_Type_Type_MIN;
}

inline ::protocol::duke::KeyEvent_KeyCode Get( const sf::Key::Code& keyCode )
{
    using namespace ::sf::Key;
    using namespace ::protocol::duke;

	switch( keyCode )
	{
		case A:
			return KeyEvent_KeyCode_A;
		case B:
			return KeyEvent_KeyCode_B;
		case C:
			return KeyEvent_KeyCode_C;
		case D:
			return KeyEvent_KeyCode_D;
		case E:
			return KeyEvent_KeyCode_E;
		case F:
			return KeyEvent_KeyCode_F;
		case G:
			return KeyEvent_KeyCode_G;
		case H:
			return KeyEvent_KeyCode_H;
		case I:
			return KeyEvent_KeyCode_I;
		case J:
			return KeyEvent_KeyCode_J;
		case K:
			return KeyEvent_KeyCode_K;
		case L:
			return KeyEvent_KeyCode_L;
		case M:
			return KeyEvent_KeyCode_M;
		case N:
			return KeyEvent_KeyCode_N;
		case O:
			return KeyEvent_KeyCode_O;
		case P:
			return KeyEvent_KeyCode_P;
		case Q:
			return KeyEvent_KeyCode_Q;
		case R:
			return KeyEvent_KeyCode_R;
		case S:
			return KeyEvent_KeyCode_S;
		case T:
			return KeyEvent_KeyCode_T;
		case U:
			return KeyEvent_KeyCode_U;
		case V:
			return KeyEvent_KeyCode_V;
		case W:
			return KeyEvent_KeyCode_W;
		case X:
			return KeyEvent_KeyCode_X;
		case Y:
			return KeyEvent_KeyCode_Y;
		case Z:
			return KeyEvent_KeyCode_Z;
		case Num0:
			return KeyEvent_KeyCode_Num0;
		case Num1:
			return KeyEvent_KeyCode_Num1;
		case Num2:
			return KeyEvent_KeyCode_Num2;
		case Num3:
			return KeyEvent_KeyCode_Num3;
		case Num4:
			return KeyEvent_KeyCode_Num4;
		case Num5:
			return KeyEvent_KeyCode_Num5;
		case Num6:
			return KeyEvent_KeyCode_Num6;
		case Num7:
			return KeyEvent_KeyCode_Num7;
		case Num8:
			return KeyEvent_KeyCode_Num8;
		case Num9:
			return KeyEvent_KeyCode_Num9;
		case sf::Key::Escape:
			return KeyEvent_KeyCode_Escape;
		case LControl:
			return KeyEvent_KeyCode_LControl;
		case LShift:
			return KeyEvent_KeyCode_LShift;
		case LAlt:
			return KeyEvent_KeyCode_LAlt;
		case LSystem:
			return KeyEvent_KeyCode_LSystem;
		case RControl:
			return KeyEvent_KeyCode_RControl;
		case RShift:
			return KeyEvent_KeyCode_RShift;
		case RAlt:
			return KeyEvent_KeyCode_RAlt;
		case RSystem:
			return KeyEvent_KeyCode_RSystem;
		case Menu:
			return KeyEvent_KeyCode_Menu;
		case LBracket:
			return KeyEvent_KeyCode_LBracket;
		case RBracket:
			return KeyEvent_KeyCode_RBracket;
		case SemiColon:
			return KeyEvent_KeyCode_SemiColon;
		case Comma:
			return KeyEvent_KeyCode_Comma;
		case Period:
			return KeyEvent_KeyCode_Period;
		case Quote:
			return KeyEvent_KeyCode_Quote;
		case Slash:
			return KeyEvent_KeyCode_Slash;
		case BackSlash:
			return KeyEvent_KeyCode_BackSlash;
		case Tilde:
			return KeyEvent_KeyCode_Tilde;
		case Equal:
			return KeyEvent_KeyCode_Equal;
		case Dash:
			return KeyEvent_KeyCode_Dash;
		case Space:
			return KeyEvent_KeyCode_Space;
		case Return:
			return KeyEvent_KeyCode_Return;
		case Back:
			return KeyEvent_KeyCode_Back;
		case Tab:
			return KeyEvent_KeyCode_Tab;
		case PageUp:
			return KeyEvent_KeyCode_PageUp;
		case PageDown:
			return KeyEvent_KeyCode_PageDown;
		case End:
			return KeyEvent_KeyCode_End;
		case Home:
			return KeyEvent_KeyCode_Home;
		case Insert:
			return KeyEvent_KeyCode_Insert;
		case Delete:
			return KeyEvent_KeyCode_Delete;
		case Add:
			return KeyEvent_KeyCode_Add;
		case Subtract:
			return KeyEvent_KeyCode_Subtract;
		case Multiply:
			return KeyEvent_KeyCode_Multiply;
		case Divide:
			return KeyEvent_KeyCode_Divide;
		case Left:
			return KeyEvent_KeyCode_Left;
		case Right:
			return KeyEvent_KeyCode_Right;
		case Up:
			return KeyEvent_KeyCode_Up;
		case Down:
			return KeyEvent_KeyCode_Down;
		case Numpad0:
			return KeyEvent_KeyCode_Numpad0;
		case Numpad1:
			return KeyEvent_KeyCode_Numpad1;
		case Numpad2:
			return KeyEvent_KeyCode_Numpad2;
		case Numpad3:
			return KeyEvent_KeyCode_Numpad3;
		case Numpad4:
			return KeyEvent_KeyCode_Numpad4;
		case Numpad5:
			return KeyEvent_KeyCode_Numpad5;
		case Numpad6:
			return KeyEvent_KeyCode_Numpad6;
		case Numpad7:
			return KeyEvent_KeyCode_Numpad7;
		case Numpad8:
			return KeyEvent_KeyCode_Numpad8;
		case Numpad9:
			return KeyEvent_KeyCode_Numpad9;
		case F1:
			return KeyEvent_KeyCode_F1;
		case F2:
			return KeyEvent_KeyCode_F2;
		case F3:
			return KeyEvent_KeyCode_F3;
		case F4:
			return KeyEvent_KeyCode_F4;
		case F5:
			return KeyEvent_KeyCode_F5;
		case F6:
			return KeyEvent_KeyCode_F6;
		case F7:
			return KeyEvent_KeyCode_F7;
		case F8:
			return KeyEvent_KeyCode_F8;
		case F9:
			return KeyEvent_KeyCode_F9;
		case F10:
			return KeyEvent_KeyCode_F10;
		case F11:
			return KeyEvent_KeyCode_F11;
		case F12:
			return KeyEvent_KeyCode_F12;
		case F13:
			return KeyEvent_KeyCode_F13;
		case F14:
			return KeyEvent_KeyCode_F14;
		case F15:
			return KeyEvent_KeyCode_F15;
		case Pause:
			return KeyEvent_KeyCode_Pause;
		case Count:
			return KeyEvent_KeyCode_Unknown;
		default:
            std::cerr << "Unknown keycode " << keyCode << std::endl;
	}
	return KeyEvent_KeyCode_Unknown;
}

inline ::protocol::duke::MouseEvent_Button Get( const sf::Mouse::Button& mouseButton )
{
    using namespace ::sf::Mouse;
    using namespace ::protocol::duke;
	switch( mouseButton )
	{
		case Left:
			return MouseEvent_Button_LEFT;
		case Right:
			return MouseEvent_Button_RIGHT;
		case Middle:
			return MouseEvent_Button_MIDDLE;
		case XButton1:
			return MouseEvent_Button_XBUTTONA;
		case XButton2:
			return MouseEvent_Button_XBUTTONB;
		default:
		    std::cerr << "Unknown mouse button " << mouseButton << std::endl;
	}
	return MouseEvent_Button_UNKNOWN;
}

inline void Update( ::protocol::duke::Event& toUpdate, const ::sf::Event& withEvent )
{
    using namespace ::protocol::duke;
	// setting event type
	toUpdate.set_type( Get( withEvent.Type ) );
	switch( withEvent.Type )
	{
		case ::sf::Event::KeyPressed:
		case ::sf::Event::KeyReleased: {
			KeyEvent* const keyEvent = toUpdate.mutable_keyevent();
			keyEvent->set_alt( withEvent.Key.Alt );
			keyEvent->set_control( withEvent.Key.Control );
			keyEvent->set_shift( withEvent.Key.Shift );
			keyEvent->set_code( Get( withEvent.Key.Code ) );
			break;
		}
		case ::sf::Event::Resized: {
			ResizeEvent* const resizeEvent = toUpdate.mutable_resizeevent();
			resizeEvent->set_width( withEvent.Size.Width );
			resizeEvent->set_height( withEvent.Size.Height );
			break;
		}
		case ::sf::Event::MouseButtonPressed:
		case ::sf::Event::MouseButtonReleased: {
			MouseEvent* const mouseEvent = toUpdate.mutable_mouseevent();
			mouseEvent->set_button( Get( withEvent.MouseButton.Button ) );
			mouseEvent->set_x( withEvent.MouseButton.X );
			mouseEvent->set_y( withEvent.MouseButton.Y );
			break;
		}
		case ::sf::Event::MouseMoved: {
			MouseEvent* const mouseEvent = toUpdate.mutable_mouseevent();
			mouseEvent->set_x( withEvent.MouseMove.X );
			mouseEvent->set_y( withEvent.MouseMove.Y );
			break;
		}
		case ::sf::Event::MouseWheelMoved: {
			MouseEvent* const mouseEvent = toUpdate.mutable_mouseevent();
			mouseEvent->set_deltawheel( withEvent.MouseWheel.Delta);
			break;
		}
		default:
			break;
	}
}

#endif /* SFMLPROTOBUFUTILS_H_ */
