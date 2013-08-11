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

#include "../Include/Example.hpp"          // corresponding header file
#include <math.h>               // for atan2, sqrt
#include <stdio.h>              // for sample output


// plugin information
unsigned g_uPluginID          = 0;
char     g_szPluginName[]     = "bdr27Telemetry - 2013.03.04";
unsigned g_uPluginVersion     = 001;
unsigned g_uPluginObjectCount = 1;
bool rpmSet = false;
bool updateTel = false;
bool sendOneTime = true;
long lapNumber = 0;

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
  sprintf_s( m_szFullName, "%s - %s", g_szPluginName, InternalsPluginInfo::GetName() );
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
	telemetrySocket = new TcpSocket("127.0.0.1", 50000);
	telemetrySocket->open();

  // default HW control enabled to true
  mEnabled = false;
}


void ExampleInternalsPlugin::Shutdown()
{
	telemetrySocket->close();
}


void ExampleInternalsPlugin::StartSession()
{
}


void ExampleInternalsPlugin::EndSession()
{
}


void ExampleInternalsPlugin::EnterRealtime()
{
	//Need to change the socket setup to each time you enter and exit real time
	updateTel = true;
	telemetrySocket->tcpSend("UpdateScreen=true\n");
	// start up timer every time we enter realtime
	mET = 0.0f;
}


void ExampleInternalsPlugin::ExitRealtime()
{
	telemetrySocket->tcpSend("UpdateScreen=false\n");
	rpmSet = false;
	updateTel = false;
	sendOneTime = true;
}

float telemUpdateInterval = 0.1f;
float timeSinceLastUpdate = 0.0f;

void ExampleInternalsPlugin::UpdateTelemetry( const TelemInfoV2 &info )
{
	if(updateTel)
	{
		if(timeSinceLastUpdate < telemUpdateInterval){
			timeSinceLastUpdate += info.mDeltaTime ;
			return;
		}

		//Values that only need to be send once
		if(sendOneTime)
		{
			//Vehicle Status
			telemetrySocket->tcpSend("4,6", info.mEngineMaxRPM);
			telemetrySocket->tcpSend("4,7", info.mScheduledStops);

			//Misc Info
			telemetrySocket->tcpSend("6,1", info.mVehicleName);
			telemetrySocket->tcpSend("6,2", info.mTrackName);
			sendOneTime = false;
		}

		//Send Once Per Lap
		if(lapNumber != info.mLapNumber)
		{
			telemetrySocket->tcpSend("1,1", info.mLapNumber);
			telemetrySocket->tcpSend("1,2", info.mLapStartET);
			lapNumber = info.mLapNumber;
		}

		//Send time info
		telemetrySocket->tcpSend("1,0", info.mDeltaTime);

		//Position info
		telemetrySocket->tcpSend("2,0", info.mPos.x, info.mPos.y, info.mPos.z);
		telemetrySocket->tcpSend("2,1", info.mLocalVel.x, info.mLocalVel.y, info.mLocalVel.z);
		telemetrySocket->tcpSend("2,2", info.mLocalAccel.x, info.mLocalAccel.y, info.mLocalAccel.z);

		//Orientation info
		telemetrySocket->tcpSend("3,0", info.mOriX.x, info.mOriX.y, info.mOriX.z);
		telemetrySocket->tcpSend("3,1", info.mOriY.x, info.mOriY.y, info.mOriY.z);
		telemetrySocket->tcpSend("3,2", info.mOriZ.x, info.mOriZ.y, info.mOriZ.z);
		telemetrySocket->tcpSend("3,3", info.mLocalRot.x, info.mLocalRot.y, info.mLocalRot.z);
		telemetrySocket->tcpSend("3,4", info.mLocalRotAccel.x, info.mLocalRotAccel.y, info.mLocalRotAccel.z);

		//Send Vehicle Status
		telemetrySocket->tcpSend("4,0", info.mGear);
		telemetrySocket->tcpSend("4,1", info.mEngineRPM);
		telemetrySocket->tcpSend("4,2", info.mEngineWaterTemp);
		telemetrySocket->tcpSend("4,3", info.mEngineOilTemp);
		telemetrySocket->tcpSend("4,4", info.mClutchRPM);
		telemetrySocket->tcpSend("4,5", info.mFuel);
		const float metersPerSec = sqrtf( ( info.mLocalVel.x * info.mLocalVel.x ) +
                                      ( info.mLocalVel.y * info.mLocalVel.y ) +
                                      ( info.mLocalVel.z * info.mLocalVel.z ) );
		telemetrySocket->tcpSend("4,8", metersPerSec);

		//Driver Input
		telemetrySocket->tcpSend("5,0", info.mUnfilteredThrottle);
		telemetrySocket->tcpSend("5,1", info.mUnfilteredBrake);
		telemetrySocket->tcpSend("5,2", info.mUnfilteredSteering);
		telemetrySocket->tcpSend("5,3", info.mUnfilteredClutch);

		//Misc
		telemetrySocket->tcpSend("6,0", info.mSteeringArmForce);

		//State / Damage
		telemetrySocket->tcpSend("7,0", (long) info.mOverheating);
		telemetrySocket->tcpSend("7,1", (long) info.mDetached);
		for(int i = 0; i < 8; i++)
		{
			telemetrySocket->tcpSend(("7,2," + to_string(i)), info.mDentSeverity[i]);
		}
		telemetrySocket->tcpSend("7,3", info.mLastImpactET);
		telemetrySocket->tcpSend("7,4", info.mLastImpactMagnitude);
		telemetrySocket->tcpSend("7,5", info.mLastImpactPos.x, info.mLastImpactPos.y, info.mLastImpactPos.z);

		//Tire details
		for(int i = 0; i < 4; i++)
		{
			TelemWheelV2 wheel = info.mWheel[i];
			telemetrySocket->tcpSend(("8,0," + to_string(i)), wheel.mRotation);
			telemetrySocket->tcpSend(("8,1," + to_string(i)), wheel.mSuspensionDeflection);
			telemetrySocket->tcpSend(("8,2," + to_string(i)), wheel.mRideHeight);
			telemetrySocket->tcpSend(("8,3," + to_string(i)), wheel.mTireLoad);
			telemetrySocket->tcpSend(("8,4," + to_string(i)), wheel.mLateralForce);
			telemetrySocket->tcpSend(("8,5," + to_string(i)), wheel.mGripFract);
			telemetrySocket->tcpSend(("8,6," + to_string(i)), wheel.mBrakeTemp);
			telemetrySocket->tcpSend(("8,7," + to_string(i)), wheel.mPressure);
			for(int j = 0; j < 3; j++)
			{
				telemetrySocket->tcpSend(("8,8," + to_string(i) + "," + to_string(j)), wheel.mTemperature[j]);
			}			
			telemetrySocket->tcpSend(("8,9," + to_string(i)), wheel.mWear);
			telemetrySocket->tcpSend(("8,10," + to_string(i)), wheel.mTerrainName);
			telemetrySocket->tcpSend(("8,11," + to_string(i)), wheel.mSurfaceType);
			telemetrySocket->tcpSend(("8,12," + to_string(i)), (long) wheel.mFlat);
			telemetrySocket->tcpSend(("8,13," + to_string(i)), (long) wheel.mDetached);

		}
	
		timeSinceLastUpdate = 0.0f;
	
		/*const float metersPerSec = sqrtf( ( info.mLocalVel.x * info.mLocalVel.x ) +
                                      ( info.mLocalVel.y * info.mLocalVel.y ) +
                                      ( info.mLocalVel.z * info.mLocalVel.z ) );	
		*/


	}
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

