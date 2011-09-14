#ifndef SFMLPROTOBUFUTILS_H_
#define SFMLPROTOBUFUTILS_H_

#include <dukeapi/protocol/player/protocol.pb.h>
#include <SFML/Window/Event.hpp>

inline ::duke::protocol::Event_Type Get( const ::sf::Event::EventType& eventType )
{
	using namespace ::duke::protocol;

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

inline ::duke::protocol::KeyEvent_KeyCode Get( const sf::Keyboard::Key& keyCode )
{
    using namespace ::sf;
    using namespace ::duke::protocol;

	switch( keyCode )
	{
		case Keyboard::A:
			return KeyEvent_KeyCode_A;
		case Keyboard::B:
			return KeyEvent_KeyCode_B;
		case Keyboard::C:
			return KeyEvent_KeyCode_C;
		case Keyboard::D:
			return KeyEvent_KeyCode_D;
		case Keyboard::E:
			return KeyEvent_KeyCode_E;
		case Keyboard::F:
			return KeyEvent_KeyCode_F;
		case Keyboard::G:
			return KeyEvent_KeyCode_G;
		case Keyboard::H:
			return KeyEvent_KeyCode_H;
		case Keyboard::I:
			return KeyEvent_KeyCode_I;
		case Keyboard::J:
			return KeyEvent_KeyCode_J;
		case Keyboard::K:
			return KeyEvent_KeyCode_K;
		case Keyboard::L:
			return KeyEvent_KeyCode_L;
		case Keyboard::M:
			return KeyEvent_KeyCode_M;
		case Keyboard::N:
			return KeyEvent_KeyCode_N;
		case Keyboard::O:
			return KeyEvent_KeyCode_O;
		case Keyboard::P:
			return KeyEvent_KeyCode_P;
		case Keyboard::Q:
			return KeyEvent_KeyCode_Q;
		case Keyboard::R:
			return KeyEvent_KeyCode_R;
		case Keyboard::S:
			return KeyEvent_KeyCode_S;
		case Keyboard::T:
			return KeyEvent_KeyCode_T;
		case Keyboard::U:
			return KeyEvent_KeyCode_U;
		case Keyboard::V:
			return KeyEvent_KeyCode_V;
		case Keyboard::W:
			return KeyEvent_KeyCode_W;
		case Keyboard::X:
			return KeyEvent_KeyCode_X;
		case Keyboard::Y:
			return KeyEvent_KeyCode_Y;
		case Keyboard::Z:
			return KeyEvent_KeyCode_Z;
		case Keyboard::Num0:
			return KeyEvent_KeyCode_Num0;
		case Keyboard::Num1:
			return KeyEvent_KeyCode_Num1;
		case Keyboard::Num2:
			return KeyEvent_KeyCode_Num2;
		case Keyboard::Num3:
			return KeyEvent_KeyCode_Num3;
		case Keyboard::Num4:
			return KeyEvent_KeyCode_Num4;
		case Keyboard::Num5:
			return KeyEvent_KeyCode_Num5;
		case Keyboard::Num6:
			return KeyEvent_KeyCode_Num6;
		case Keyboard::Num7:
			return KeyEvent_KeyCode_Num7;
		case Keyboard::Num8:
			return KeyEvent_KeyCode_Num8;
		case Keyboard::Num9:
			return KeyEvent_KeyCode_Num9;
		case Keyboard::Escape:
			return KeyEvent_KeyCode_Escape;
		case Keyboard::LControl:
			return KeyEvent_KeyCode_LControl;
		case Keyboard::LShift:
			return KeyEvent_KeyCode_LShift;
		case Keyboard::LAlt:
			return KeyEvent_KeyCode_LAlt;
		case Keyboard::LSystem:
			return KeyEvent_KeyCode_LSystem;
		case Keyboard::RControl:
			return KeyEvent_KeyCode_RControl;
		case Keyboard::RShift:
			return KeyEvent_KeyCode_RShift;
		case Keyboard::RAlt:
			return KeyEvent_KeyCode_RAlt;
		case Keyboard::RSystem:
			return KeyEvent_KeyCode_RSystem;
		case Keyboard::Menu:
			return KeyEvent_KeyCode_Menu;
		case Keyboard::LBracket:
			return KeyEvent_KeyCode_LBracket;
		case Keyboard::RBracket:
			return KeyEvent_KeyCode_RBracket;
		case Keyboard::SemiColon:
			return KeyEvent_KeyCode_SemiColon;
		case Keyboard::Comma:
			return KeyEvent_KeyCode_Comma;
		case Keyboard::Period:
			return KeyEvent_KeyCode_Period;
		case Keyboard::Quote:
			return KeyEvent_KeyCode_Quote;
		case Keyboard::Slash:
			return KeyEvent_KeyCode_Slash;
		case Keyboard::BackSlash:
			return KeyEvent_KeyCode_BackSlash;
		case Keyboard::Tilde:
			return KeyEvent_KeyCode_Tilde;
		case Keyboard::Equal:
			return KeyEvent_KeyCode_Equal;
		case Keyboard::Dash:
			return KeyEvent_KeyCode_Dash;
		case Keyboard::Space:
			return KeyEvent_KeyCode_Space;
		case Keyboard::Return:
			return KeyEvent_KeyCode_Return;
		case Keyboard::Back:
			return KeyEvent_KeyCode_Back;
		case Keyboard::Tab:
			return KeyEvent_KeyCode_Tab;
		case Keyboard::PageUp:
			return KeyEvent_KeyCode_PageUp;
		case Keyboard::PageDown:
			return KeyEvent_KeyCode_PageDown;
		case Keyboard::End:
			return KeyEvent_KeyCode_End;
		case Keyboard::Home:
			return KeyEvent_KeyCode_Home;
		case Keyboard::Insert:
			return KeyEvent_KeyCode_Insert;
		case Keyboard::Delete:
			return KeyEvent_KeyCode_Delete;
		case Keyboard::Add:
			return KeyEvent_KeyCode_Add;
		case Keyboard::Subtract:
			return KeyEvent_KeyCode_Subtract;
		case Keyboard::Multiply:
			return KeyEvent_KeyCode_Multiply;
		case Keyboard::Divide:
			return KeyEvent_KeyCode_Divide;
		case Keyboard::Left:
			return KeyEvent_KeyCode_Left;
		case Keyboard::Right:
			return KeyEvent_KeyCode_Right;
		case Keyboard::Up:
			return KeyEvent_KeyCode_Up;
		case Keyboard::Down:
			return KeyEvent_KeyCode_Down;
		case Keyboard::Numpad0:
			return KeyEvent_KeyCode_Numpad0;
		case Keyboard::Numpad1:
			return KeyEvent_KeyCode_Numpad1;
		case Keyboard::Numpad2:
			return KeyEvent_KeyCode_Numpad2;
		case Keyboard::Numpad3:
			return KeyEvent_KeyCode_Numpad3;
		case Keyboard::Numpad4:
			return KeyEvent_KeyCode_Numpad4;
		case Keyboard::Numpad5:
			return KeyEvent_KeyCode_Numpad5;
		case Keyboard::Numpad6:
			return KeyEvent_KeyCode_Numpad6;
		case Keyboard::Numpad7:
			return KeyEvent_KeyCode_Numpad7;
		case Keyboard::Numpad8:
			return KeyEvent_KeyCode_Numpad8;
		case Keyboard::Numpad9:
			return KeyEvent_KeyCode_Numpad9;
		case Keyboard::F1:
			return KeyEvent_KeyCode_F1;
		case Keyboard::F2:
			return KeyEvent_KeyCode_F2;
		case Keyboard::F3:
			return KeyEvent_KeyCode_F3;
		case Keyboard::F4:
			return KeyEvent_KeyCode_F4;
		case Keyboard::F5:
			return KeyEvent_KeyCode_F5;
		case Keyboard::F6:
			return KeyEvent_KeyCode_F6;
		case Keyboard::F7:
			return KeyEvent_KeyCode_F7;
		case Keyboard::F8:
			return KeyEvent_KeyCode_F8;
		case Keyboard::F9:
			return KeyEvent_KeyCode_F9;
		case Keyboard::F10:
			return KeyEvent_KeyCode_F10;
		case Keyboard::F11:
			return KeyEvent_KeyCode_F11;
		case Keyboard::F12:
			return KeyEvent_KeyCode_F12;
		case Keyboard::F13:
			return KeyEvent_KeyCode_F13;
		case Keyboard::F14:
			return KeyEvent_KeyCode_F14;
		case Keyboard::F15:
			return KeyEvent_KeyCode_F15;
		case Keyboard::Pause:
			return KeyEvent_KeyCode_Pause;
		default:
            std::cerr << "Unknown keycode " << keyCode << std::endl;
	}
	return KeyEvent_KeyCode_Unknown;
}

inline ::duke::protocol::MouseEvent_Button Get( const sf::Mouse::Button& mouseButton )
{
    using namespace ::sf;
    using namespace ::duke::protocol;
	switch( mouseButton )
	{
		case Mouse::Left:
			return MouseEvent_Button_LEFT;
		case Mouse::Right:
			return MouseEvent_Button_RIGHT;
		case Mouse::Middle:
			return MouseEvent_Button_MIDDLE;
		case Mouse::XButton1:
			return MouseEvent_Button_XBUTTONA;
		case Mouse::XButton2:
			return MouseEvent_Button_XBUTTONB;
		default:
		    std::cerr << "Unknown mouse button " << mouseButton << std::endl;
	}
	return MouseEvent_Button_UNKNOWN;
}

inline void Update( ::duke::protocol::Event& toUpdate, const ::sf::Event& withEvent )
{
    using namespace ::duke::protocol;
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
