//‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹
//›                                                                         ﬁ
//› Module: Internals Example Source File                                   ﬁ
//›                                                                         ﬁ
//› Description: Declarations for the Internals Example Plugin              ﬁ
//›                                                                         ﬁ
//›                                                                         ﬁ
//› This source code module, and all information, data, and algorithms      ﬁ
//› associated with it, are part of CUBE technology (tm).                   ﬁ
//›                 PROPRIETARY AND CONFIDENTIAL                            ﬁ
//› Copyright (c) 1996-2007 Image Space Incorporated.  All rights reserved. ﬁ
//›                                                                         ﬁ
//›                                                                         ﬁ
//› Change history:                                                         ﬁ
//›   tag.2005.11.30: created                                               ﬁ
//›                                                                         ﬁ
//ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ

#include "Example.hpp"          // corresponding header file
#include <math.h>               // for atan2, sqrt
#include <stdio.h>              // for sample output


// plugin information
unsigned g_uPluginID          = 0;
char     g_szPluginName[]     = "bdr27Telemetry - 2013.03.04";
unsigned g_uPluginVersion     = 001;
unsigned g_uPluginObjectCount = 1;
TcpSocket* telemetrySocket;

InternalsPluginInfo g_PluginInfo;

// interface to plugin information
extern "C" __declspec(dllexport)
const char* __cdecl GetPluginName() { return g_szPluginName; }

extern "C" __declspec(dllexport)
unsigned __cdecl GetPluginVersion() { return g_uPluginVersion; }

extern "C" __declspec(dllexport)
unsigned __cdecl GetPluginObjectCount() { return g_uPluginObjectCount; }

// get the plugin-info object used to create the plugin.
extern "C" __declspec(dllexport)
PluginObjectInfo* __cdecl GetPluginObjectInfo( const unsigned uIndex )
{
  switch(uIndex)
  {
    case 0:
      return  &g_PluginInfo;
    default:
      return 0;
  }
}


// InternalsPluginInfo class

InternalsPluginInfo::InternalsPluginInfo()
{
  // put together a name for this plugin
  sprintf( m_szFullName, "%s - %s", g_szPluginName, InternalsPluginInfo::GetName() );
}

const char*    InternalsPluginInfo::GetName()     const { return ExampleInternalsPlugin::GetName(); }
const char*    InternalsPluginInfo::GetFullName() const { return m_szFullName; }
const char*    InternalsPluginInfo::GetDesc()     const { return "bdr27 Telemetry Plugin"; }
const unsigned InternalsPluginInfo::GetType()     const { return ExampleInternalsPlugin::GetType(); }
const char*    InternalsPluginInfo::GetSubType()  const { return ExampleInternalsPlugin::GetSubType(); }
const unsigned InternalsPluginInfo::GetVersion()  const { return ExampleInternalsPlugin::GetVersion(); }
void*          InternalsPluginInfo::Create()      const { return new ExampleInternalsPlugin(); }


// InternalsPlugin class

const char ExampleInternalsPlugin::m_szName[] = "bdr27Telemetry";
const char ExampleInternalsPlugin::m_szSubType[] = "bdr27";
const unsigned ExampleInternalsPlugin::m_uID = 1;
const unsigned ExampleInternalsPlugin::m_uVersion = 3;  // set to 3 for InternalsPluginV3 functionality and added graphical and vehicle info


PluginObjectInfo *ExampleInternalsPlugin::GetInfo()
{
  return &g_PluginInfo;
}


void ExampleInternalsPlugin::Startup()
{
	
  // Open ports, read configs, whatever you need to do.  For now, I'll just clear out the
  // example output data files.
	telemetrySocket = new TcpSocket("127.0.0.1", 27015);
	telemetrySocket->Open();

  // default HW control enabled to true
  mEnabled = false;
}


void ExampleInternalsPlugin::Shutdown()
{
	telemetrySocket->Close();
}


void ExampleInternalsPlugin::StartSession()
{
}


void ExampleInternalsPlugin::EndSession()
{
}


void ExampleInternalsPlugin::EnterRealtime()
{
  // start up timer every time we enter realtime
  mET = 0.0f;
}


void ExampleInternalsPlugin::ExitRealtime()
{
}

float telemUpdateInterval = 0.1f;
float timeSinceLastUpdate = 0.0f;

void ExampleInternalsPlugin::UpdateTelemetry( const TelemInfoV2 &info )
{
	if(timeSinceLastUpdate < telemUpdateInterval){
		timeSinceLastUpdate += info.mDeltaTime ;
		return;
	}
	timeSinceLastUpdate = 0.0f;
	const float metersPerSec = sqrtf( ( info.mLocalVel.x * info.mLocalVel.x ) +
                                      ( info.mLocalVel.y * info.mLocalVel.y ) +
                                      ( info.mLocalVel.z * info.mLocalVel.z ) );
	char sendBuf[64];
	sprintf_s(sendBuf, "Speed=%.0f\n",metersPerSec * 3.6f);
	telemetrySocket->Send(sendBuf);
	sprintf_s(sendBuf, "RPM=%.0f\n",info.mEngineRPM);
	telemetrySocket->Send(sendBuf);
	sprintf_s(sendBuf, "MaxRPM=%.0f\n",info.mEngineMaxRPM);
	telemetrySocket->Send(sendBuf);
	sprintf_s(sendBuf, "Water=%.2f\nOil=%.2f\n", info.mEngineWaterTemp, info.mEngineOilTemp );
	telemetrySocket->Send(sendBuf);
	sprintf_s(sendBuf, "Fuel=%.2f\n", info.mFuel);
	telemetrySocket->Send(sendBuf);
}


void ExampleInternalsPlugin::UpdateGraphics( const GraphicsInfoV2 &info )
{
}


bool ExampleInternalsPlugin::CheckHWControl( const char * const controlName, float &fRetVal )
{
  
  return( false );
}


bool ExampleInternalsPlugin::ForceFeedback( float &forceValue )
{
  return( false );
}


void ExampleInternalsPlugin::UpdateScoring( const ScoringInfoV2 &info )
{
}


bool ExampleInternalsPlugin::RequestCommentary( CommentaryRequestInfo &info )
{
  // COMMENT OUT TO ENABLE EXAMPLE
  return( false );
}

