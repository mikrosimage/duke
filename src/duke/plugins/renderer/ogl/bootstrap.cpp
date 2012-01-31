#include <dukeplugin/IPlugin.h>

static bool isEqual( const char* expected, const char* actual )
{
	const char* pExpected = expected;
	const char* pActual   = actual;

	while( true )
	{
		if( *pExpected == '\0' && *pActual == '\0' )
			return true;
		if( *pExpected != *pActual )
			return false;
		++pExpected;
		++pActual;
	}
}

#include "src/OGLRenderer.h"
#include <dukerenderer/ofxRenderer.h>
#include <plugin/RendererSuite.h>
#include <plugin/IRenderer.h>
#include <plugin/SfmlWindow.h>

#include <iostream>
#include <cassert>
#include <stdexcept>
#include <memory>

IRenderer* createRenderer( const duke::protocol::Renderer& Renderer, sf::Window& window, const RendererSuite& suite )
{
	return new OGLRenderer( Renderer, window, suite );
}

class Plugin : public IPlugin
{
std::auto_ptr<RendererSuite> m_pRendererSuite;
std::auto_ptr<SfmlWindow> m_pWindow;

public: Plugin() {}

	~Plugin() {}

	virtual OfxStatus pluginMain( const char* action, const void* handle, OfxPropertySetHandle inArgs, OfxPropertySetHandle outArgs )
	{
		if( isEqual( action, kOfxActionRendererInit ) )
		{
			const duke::protocol::Renderer* pDescriptor = (const duke::protocol::Renderer*) handle;
			assert( pDescriptor );
			m_pWindow.reset( new SfmlWindow( &::createRenderer, *pDescriptor, *m_pRendererSuite ) );
			return kOfxStatOK;
		}
		else if( isEqual( action, kOfxActionWaitForMainLoopEnd ) )
		{
			if( m_pWindow.get() == NULL )
				throw std::logic_error( "No renderer set, cannot execute kOfxActionWaitForMainLoopEnd action" );
			m_pWindow->waitForMainLoopEnd();
			return kOfxStatOK;
		}
		else if( isEqual( action, kOfxActionLoad ) )
		{
			void* pSuite = getOfxHost()->fetchSuite( getOfxHost()->host, kOfxRendererSuite, 1 );
			if( pSuite == NULL )
				return kOfxStatErrMissingHostFeature;
			m_pRendererSuite.reset( new RendererSuite( getOfxHost(), (OfxRendererSuiteV1*) pSuite ) );
			return kOfxStatOK;
		}
		else if( isEqual( action, kOfxActionUnload ) )
		{
			m_pWindow.reset();
			return kOfxStatOK;
		}
		return kOfxStatFailed;
	}

};

// plugin declaration

const int PLUGIN_COUNT = 1;

#include <dukeplugin/PluginBootstrap.h>

OfxPluginInstance<0> plugin( kOfxRendererPluginApi, 1, "fr.mikrosimage.player.renderer.OpenGL", 1, 0, new Plugin() );
