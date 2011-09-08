/*
 * main.cpp
 *
 *  Created on: 24 juil. 2009
 *      Author: Michael ETIENNE
 */

#include <dukeapi/protocol/player/communication.pb.h>
#include "OutputStreamHelper.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace ::std;

string readStream( std::ifstream& stream )
{
	std::stringstream buffer;

	buffer << stream.rdbuf();

	std::string contents( buffer.str() );
	return contents;

}

int main( int argc, char** argv )
{
	if( argc < 2 )
		return 1;

	using namespace ::serialize;

	ofstream file( argv[1], ofstream::out | ofstream::binary );
	{
		// OutputStreamHelper need to be destroyed for
		// the stream to be flushed to disk
		OutputStreamHelper helper( &file );

		//
		// First Message : DEBUGSCRIPT message
		Message msg;
		msg.set_type( Message_MessageType_DEBUGSCRIPT );

		::serialize::DebugScript embeddedMsg;
		for( int i = 0; i < 10; ++i )
		{
			ostringstream text;
			text << "command #" << i;
			embeddedMsg.add_output( text.str() );
		}

		string embeddedString;
		embeddedMsg.SerializeToString( &embeddedString );
		msg.set_data( embeddedString );
		helper.appendProtocolBufferMessage( msg );

		//
		// Message : Setup renderer
		Message msgInitRender;
		msgInitRender.set_type( Message_MessageType_Renderer );

		int winWidth  = 1920;
		int winHeight = 1080;

		//	int winWidth = 728;
		//	int winHeight = 513;

		::serialize::Renderer init;
		init.set_bufferwidth( winWidth );
		init.set_bufferheight( winHeight );
		init.set_bufferpixelformatnbbits( 32 );
		bool isWindowMode = false;
		init.set_windowmode( isWindowMode );
		init.SerializeToString( &embeddedString );
		msgInitRender.set_data( embeddedString );
		helper.appendProtocolBufferMessage( msgInitRender );

		//
		// Message Playlist
		Message msgPlaylist;
		msgPlaylist.set_type( Message_MessageType_PLAYLIST );

		::serialize::Playlist playlist;
		playlist.set_id( 222222 );
		playlist.add_paths( "T:\\Images\\Insurge" );
		playlist.set_recin( 0 );
		playlist.set_recout( 10000 );
		//playlist.add_sequences("camine.#######.dpx[107951-107957]");
		playlist.add_sequences( "Linsurge_BOB3.#######.dpx[275300-275600]" );
		//playlist.add_sequences("Ramp##.dpx[0-7]");
		playlist.add_starts( 0 );
		playlist.add_ends( 300 );
		playlist.SerializeToString( &embeddedString );
		msgPlaylist.set_data( embeddedString );
		helper.appendProtocolBufferMessage( msgPlaylist );

		//
		// Message RenderPass
		Message msgRenderPass;
		msgRenderPass.set_type( Message_MessageType_RENDERPASS );

		::serialize::RenderPass pass;
		pass.set_id( 11223344 );

		//
		// Effect declaration
		::serialize::Effect* effect( pass.mutable_effect() );
		effect->set_id( 2000 );
		effect->set_name( "EffectTest" );

		std::ifstream fxstream( argv[2], std::ifstream::in );
		if( !fxstream.is_open() )
		{
			cerr << "Unable to open fx file " << string( argv[2] ) << endl;
			return 1;
		}

		cout << "Fx content: " << endl;
		string fxcode(  readStream( fxstream ) );

		effect->set_code( fxcode );
		cout << "Code fx : " << effect->code() << endl;

		::serialize::Parameter* param = effect->add_dynamic();

		param->set_id( 1024 );
		param->set_name( "WorldViewProj" );
		param->set_type( Parameter_ParameterType_MATRIX );
		::serialize::Matrix* mat = param->mutable_matrixvalue();
		//
		// Identity
		mat->add_value( 1.0 );
		mat->add_value( 0.0 );
		mat->add_value( 0.0 );
		mat->add_value( 0.0 );
		mat->add_value( 0.0 );
		mat->add_value( 1.0 );
		mat->add_value( 0.0 );
		mat->add_value( 0.0 );
		mat->add_value( 0.0 );
		mat->add_value( 0.0 );
		mat->add_value( 1.0 );
		mat->add_value( 0.0 );
		mat->add_value( 0.0 );
		mat->add_value( 0.0 );
		mat->add_value( 0.0 );
		mat->add_value( 1.0 );

		//
		// Texture declaration
		int windowBorderH = 25;
		int windowBorderW = 6;

		if( !isWindowMode )
		{
			windowBorderH = windowBorderW = 0;
		}

		/*
		        int texwidth = winWidth - windowBorderW;
		        int texheight = winHeight - windowBorderH;
		        ::serialize::TextureMapping* texmap = effect->add_mapping();
		        texmap->set_id(111);
		        ::serialize::Texture* tex = texmap->mutable_texture();
		        //
		        // Same id to avoid creation of new texture
		        tex->set_id(112);
		        tex->set_dim(Texture_TextureDim_DIM_2D);
		        //tex->set_format(Texture_TextureFormat_A8R8G8B8);
		        //tex->set_format(Texture_TextureFormat_A2R10G10B10);
		        //tex->set_filename("Test.dpx");
		    //	tex->set_width(texwidth);
		    //	tex->set_height(texheight);
		   //		tex->set_type(Texture_TextureType_PROCEDURAL);
		    //	tex->set_filename("Shaker");
		        tex->set_type(Texture_TextureType_FROMPLAYLIST);
		    //	tex->set_filename("TestNuke.tif");

		        //
		        // Set image number as filename
		   //		ostringstream text;
		   //		text << img;
		        // Set image number as data
		   //		tex->set_data(text.str());
		        // Set sequence pattern as filename ( to reuse same texture for the sequence )
		   //		tex->set_filename("camine.######.jpg[107951-107957]");
		        tex->set_filename("texSampler0");
		 */

		pass.set_clean( true );
		pass.set_cleancolor( 0xffff7f7f );

		::serialize::Mesh* mesh = pass.add_meshes();
		mesh->set_id( 12 );
		//mesh->set_type(Mesh_MeshType_LINESTRIP);
		mesh->set_type( Mesh_MeshType_TRIANGLESTRIP );

		::serialize::VertexBuffer* vBuffer = mesh->mutable_vertexbuffer();

		//for( int i=0; i < 3; i++) {
		vBuffer->add_type( VertexBuffer_VertexType_POS_NORMAL_COLOR_UV1 );
		::serialize::Vertex* v = vBuffer->add_vertices();

		// Position
		//v->add_data(-0.5f);
		//v->add_data(0.5f);
		v->add_data( -1.0f );
		v->add_data( 1.0f );
		v->add_data( 1.0f );

		// Normal
		v->add_data( 1.0f );
		v->add_data( 0.0f );
		v->add_data( 0.0f );

		// Color
		v->add_data( 0xFFFF00FF );

		// UV
		v->add_data( 0.5f / (float)winWidth );
		v->add_data( 0.5f / (float)winHeight );

		vBuffer->add_type( VertexBuffer_VertexType_POS_NORMAL_COLOR_UV1 );
		v = vBuffer->add_vertices();

		// Position
		//v->add_data(0.5f);
		//v->add_data(0.5f);
		v->add_data( 1.0f );
		v->add_data( 1.0f );
		v->add_data( 1.0f );

		// Normal
		v->add_data( 1.0f );
		v->add_data( 0.0f );
		v->add_data( 0.0f );

		// Color
		v->add_data( 0xFFFF00FF );

		// UV
		v->add_data( 1.0f + 0.5f / (float)winWidth );
		v->add_data( 0.5f / (float)winHeight );

		vBuffer->add_type( VertexBuffer_VertexType_POS_NORMAL_COLOR_UV1 );
		v = vBuffer->add_vertices();

		// Position
		//v->add_data(-0.5f);
		//v->add_data(-0.5f);
		v->add_data( -1.0f );
		v->add_data( -1.0f );
		v->add_data( 1.0f );

		// Normal
		v->add_data( 1.0f );
		v->add_data( 0.0f );
		v->add_data( 0.0f );

		// Color
		v->add_data( 0xFFFFFF00 );

		// UV
		v->add_data( 0.5f / (float)winWidth );
		v->add_data( 1.0f + 0.5f / (float)winHeight );

		vBuffer->add_type( VertexBuffer_VertexType_POS_NORMAL_COLOR_UV1 );
		v = vBuffer->add_vertices();

		// Position
		//v->add_data(0.5f);
		//v->add_data(-0.5f);
		v->add_data( 1.0f );
		v->add_data( -1.0f );
		v->add_data( 1.0f );

		// Normal
		v->add_data( 1.0f );
		v->add_data( 0.0f );
		v->add_data( 0.0f );

		// Color
		v->add_data( 0xFFFFFF00 );

		// UV
		v->add_data( 1.0f + 0.5f / (float)winWidth );
		v->add_data( 1.0f + 0.5f / (float)winHeight );
		//}

		::serialize::IndexBuffer* iBuffer = mesh->mutable_indexbuffer();
		iBuffer->add_indices( 0 );
		iBuffer->add_indices( 1 );
		iBuffer->add_indices( 2 );
		iBuffer->add_indices( 3 );

		// Define Render Source & Target
		pass.set_target( RenderPass_RenderTarget_TEXTURETARG );
		pass.set_targetid( 321 );

		pass.SerializeToString( &embeddedString );

		msgRenderPass.set_data( embeddedString );
		std::cout << "RenderPass Message encoding" << std::endl;
		helper.appendProtocolBufferMessage( msgRenderPass );

		//
		// Messages RenderFrame with RenderPass1
		for( int img = 0; img < 1; img++ )
		{
			Message msgRenderFrame;
			msgRenderFrame.set_type( Message_MessageType_RENDERFRAME );
			::serialize::RenderFrame rFrame;

			rFrame.set_renderpassid( 11223344 );
			//			rFrame.set_sequence("Linsurge_BOB3.#######.dpx[275300-275600]");
			//std::cout << "Sequence : " << rFrame.sequence() << std::endl;
			rFrame.set_frame( img );
			//rFrame.set_frame(0);

			rFrame.SerializeToString( &embeddedString );
			msgRenderFrame.set_data( embeddedString );
			helper.appendProtocolBufferMessage( msgRenderFrame );
		}

		// ReDefine other renderpass with different render source & target
		pass.set_id( 11223345 );

		// Define Render Source & Target
		pass.set_source( RenderPass_RenderSource_TEXTURESRC );
		pass.set_sourceid( 321 );

		pass.set_target( RenderPass_RenderTarget_TEXTURETARG );
		pass.set_targetid( 322 );

		pass.SerializeToString( &embeddedString );

		msgRenderPass.set_data( embeddedString );
		std::cout << "RenderPass Message encoding" << std::endl;
		helper.appendProtocolBufferMessage( msgRenderPass );

		//
		// Message GrabTexture
		Message msgGrabTexture;
		msgGrabTexture.set_type( Message_MessageType_GRABTEXTURE );
		::serialize::GrabTexture grab;

		::serialize::RenderFrame* rFrame = grab.mutable_renderframe();

		rFrame->set_renderpassid( 11223345 );
		//		rFrame->set_sequence("Linsurge_BOB3.#######.dpx[275300-275600]");
		rFrame->set_frame( 0 );

		grab.set_filename( "T:\\Sources\\Workspace\\NeoEngineCore\\dist\\JPG\\OutputShaker.jpg" );

		grab.SerializeToString( &embeddedString );
		msgGrabTexture.set_data( embeddedString );
		helper.appendProtocolBufferMessage( msgGrabTexture );

		//
		// Messages RenderFrame with RenderPass1
		for( int img = 0; img < 1; img++ )
		{
			Message msgRenderFrame;
			msgRenderFrame.set_type( Message_MessageType_RENDERFRAME );
			::serialize::RenderFrame rFrame;

			rFrame.set_renderpassid( 11223345 );
			//			rFrame.set_sequence("Linsurge_BOB3.#######.dpx[275300-275600]");
			//std::cout << "Sequence : " << rFrame.sequence() << std::endl;
			rFrame.set_frame( img );
			//rFrame.set_frame(0);

			rFrame.SerializeToString( &embeddedString );
			msgRenderFrame.set_data( embeddedString );
			helper.appendProtocolBufferMessage( msgRenderFrame );
		}

		// ReDefine other renderpass with different render source & target
		pass.set_id( 11223346 );

		// Define Render Source & Target
		pass.set_source( RenderPass_RenderSource_TEXTURESRC );
		pass.set_sourceid( 322 );

		pass.set_target( RenderPass_RenderTarget_SCREEN );
		//pass.set_targetid(322);

		pass.SerializeToString( &embeddedString );

		msgRenderPass.set_data( embeddedString );
		std::cout << "RenderPass Message encoding" << std::endl;
		helper.appendProtocolBufferMessage( msgRenderPass );

		//
		// Messages RenderFrame with RenderPass1
		for( int img = 0; img < 1; img++ )
		{
			Message msgRenderFrame;
			msgRenderFrame.set_type( Message_MessageType_RENDERFRAME );
			::serialize::RenderFrame rFrame;

			rFrame.set_renderpassid( 11223346 );
			//			rFrame.set_sequence("Linsurge_BOB3.#######.dpx[275300-275600]");
			//std::cout << "Sequence : " << rFrame.sequence() << std::endl;
			rFrame.set_frame( img );
			//rFrame.set_frame(0);

			rFrame.SerializeToString( &embeddedString );
			msgRenderFrame.set_data( embeddedString );
			helper.appendProtocolBufferMessage( msgRenderFrame );
		}

		/*		//
		        // Message RenderProcedural
		        Message msgRenderProc;
		        msgRenderProc.set_type(Message_MessageType_RENDERPROCEDURAL);
		        ::serialize::RenderProcedural rProc;

		        rProc.set_renderpassid(11223344);
		        ::serialize::Texture* texProc = rProc.mutable_texture();

		        texProc->set_id(1189);
		        texProc->set_dim(Texture_TextureDim_DIM_2D);
		        //texProc->set_format(Texture_TextureFormat_A8R8G8B8);
		        tex->set_format(Texture_TextureFormat_A2R10G10B10);
		        texProc->set_width(texwidth);
		        texProc->set_height(texheight);
		        texProc->set_type(Texture_TextureType_PROCEDURAL);
		        texProc->set_proceduraltype(Texture_TextureProceduralType_SHAKER);

		        rProc.SerializeToString(&embeddedString);
		        msgRenderProc.set_data(embeddedString);
		        helper.appendProtocolBufferMessage(msgRenderProc);
		 */

		/*		//
		        // Message GrabTexture with a procedural texture: shaker
		        Message msgGrabTexture;
		        msgGrabTexture.set_type(Message_MessageType_GRABTEXTURE);
		        ::serialize::GrabTexture grab;
		        //
		        // RenderProcedural message
		        ::serialize::RenderProcedural* rProc = grab.mutable_renderproc();

		        rProc->set_renderpassid(11223344);
		        ::serialize::Texture* texProc = rProc->mutable_texture();

		        texProc->set_id(1189);
		        texProc->set_dim(Texture_TextureDim_DIM_2D);
		        texProc->set_format(Texture_TextureFormat_A8R8G8B8);
		        //tex->set_format(Texture_TextureFormat_A2R10G10B10);
		        texProc->set_width(texwidth);
		        texProc->set_height(texheight);
		        texProc->set_type(Texture_TextureType_PROCEDURAL);
		        texProc->set_proceduraltype(Texture_TextureProceduralType_SHAKER);
		        //

		   //		::serialize::RenderFrame* rFrame = grab.mutable_renderframe();

		   //		rFrame->set_renderpassid(11223344);
		   //		rFrame->set_sequence("camine.#######.dpx[107951-107957]");
		   //		rFrame->set_frame(0);


		        grab.set_filename("T:\\Sources\\Workspace\\NeoEngineCore\\dist\\JPG\\OutputShaker.jpg");

		        grab.SerializeToString(&embeddedString);
		        msgGrabTexture.set_data(embeddedString);
		        helper.appendProtocolBufferMessage(msgGrabTexture);
		 */

		/*
		        for( int img = 0; img<7; img++ ) {
		            Message msgRenderFrame;
		            msgRenderFrame.set_type(Message_MessageType_RENDERFRAME);
		            ::serialize::RenderFrame rFrame;

		            rFrame.set_renderpassid(11223344);
		            rFrame.set_sequence("camine.######.jpg[107951-107957]");
		            //std::cout << "Sequence : " << rFrame.sequence() << std::endl;
		            rFrame.set_frame(img);

		            rFrame.SerializeToString(&embeddedString);
		            msgRenderFrame.set_data(embeddedString);
		            helper.appendProtocolBufferMessage(msgRenderFrame);
		        }
		        for( int img = 0; img<7; img++ ) {
		            Message msgRenderFrame;
		            msgRenderFrame.set_type(Message_MessageType_RENDERFRAME);
		            ::serialize::RenderFrame rFrame;

		            rFrame.set_renderpassid(11223344);
		            rFrame.set_sequence("camine.######.jpg[107951-107957]");
		            //std::cout << "Sequence : " << rFrame.sequence() << std::endl;
		            rFrame.set_frame(img);

		            rFrame.SerializeToString(&embeddedString);
		            msgRenderFrame.set_data(embeddedString);
		            helper.appendProtocolBufferMessage(msgRenderFrame);
		        }
		 */

		//
		// Second Message : QUIT message
		Message msgQuit;
		msgQuit.set_type( Message_MessageType_QUIT );

		::serialize::Quit quit;
		quit.set_output( std::string( "Quit message" ) );
		quit.set_returncode( 0 );
		quit.SerializeToString( &embeddedString );
		msgQuit.set_data( embeddedString );
		helper.appendProtocolBufferMessage( msgQuit );

	}
	file.close();
	return 0;
}

