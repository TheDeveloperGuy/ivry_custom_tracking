/*************************************************************************
*
* Copyright (C) 2016-2021 Mediator Software and/or its subsidiary(-ies).
* All rights reserved.
* Contact: Mediator Software (info@mediator-software.com)
*
* NOTICE:  All information contained herein is, and remains the property of
* Mediator Software and its suppliers, if any.
* The intellectual and technical concepts contained herein are proprietary
* to Mediator Software and its suppliers and may be covered by U.S. and
* Foreign Patents, patents in process, and are protected by trade secret or
* copyright law. Dissemination of this information or reproduction of this
* material is strictly forbidden unless prior written permission is obtained
* from Mediator Software.
*
* If you have questions regarding the use of this file, please contact
* Mediator Software (info@mediator-software.com).
*
***************************************************************************/

#ifndef _IVRY_TRACKING_APP_H
#define _IVRY_TRACKING_APP_H

#include <windows.h>
#include <openvr_driver.h>

/** contains information about one axis on the controller */
typedef struct
{
	float x; // Ranges from -1.0 to 1.0 for joysticks and track pads. Ranges from 0.0 to 1.0 for triggers were 0 is fully released.
	float y; // Ranges from -1.0 to 1.0 for joysticks and track pads. Is always 0.0 for triggers.
} IvryTrackingAxis;

typedef struct
{
	float value;
} IvryTrackingScalarState;

typedef struct
{
	bool isTouched;
	bool isPressed;
	IvryTrackingScalarState scalar;
} IvryTrackingButtonState;

typedef struct
{
	IvryTrackingScalarState scalar;
} IvryTrackingFingerState;

typedef struct
{
	bool isTouched;
	bool isPressed;
	IvryTrackingAxis axis;
} IvryTrackingJoystickState;

typedef struct
{
	vr::DriverPose_t pose;
	IvryTrackingJoystickState touchpad;
	IvryTrackingJoystickState thumbstick;
	IvryTrackingButtonState buttonA, buttonB, buttonX, buttonY;
	IvryTrackingButtonState trigger, grip, enter, back;
	IvryTrackingFingerState thumb, index;
} IvryTrackingControllerState;

typedef enum
{
	IVRY_TRACKING_POWER_UNKNOWN,
	IVRY_TRACKING_POWER_UNPLUGGED,
	IVRY_TRACKING_POWER_CHARGING,
	IVRY_TRACKING_POWER_FULL
} IvryTrackingPowerState;

typedef enum
{
	IVRY_TRACKING_HAND_LEFT,
	IVRY_TRACKING_HAND_RIGHT
} IvryTrackingControllerHand;

/** Private **/
class IvryTrackingApp_Private;

class IvryTrackingApp
{
public:
	/** Constructor/destructor */
	IvryTrackingApp();
	IvryTrackingApp(const char *name);
	~IvryTrackingApp();

	/** Run tracker **/
	virtual DWORD Run() = 0;

	/** Open/close connection to driver **/
	bool Open();
	void Close();

	/** Driver is requesting tracking process quit **/
	virtual void OnQuit() {}

	/** Nofify driver that ext tracking has been enabled/disabled **/
	void TrackingEnabled(bool enable);

	/** Get min/max tracking rates (in Hz) **/
	virtual float GetMinTrackingRate() { return 0; }
	virtual float GetMaxTrackingRate() { return 0; }

	/** Get/Set tracking rate (in Hz) **/
	virtual float GetTrackingRate() { return 0; }
	virtual void SetTrackingRate(float rate) {}

	/** Request that driver recenters headset **/
	void RecenterDeviceOrientation();

	/** Enable/disable device tracking **/
	void EnableDeviceTracking(bool enable);

	/** Enable/disable device orientation tracking **/
	void EnableDeviceOrientation(bool enable);

	/** Enable/disable tracking LEDs (if any) **/
	void EnableDeviceLeds(bool enable);

	/** Set tracking LED brightness **/
	void SetDeviceLedLevel(uint8_t level);

protected:
	/** Device orientation has been enabled/disabled by user **/
	virtual void OnDeviceOrientationEnabled(bool enable) {}

	/** Pose has been recevied from driver **/
	virtual void OnDevicePoseUpdated(const vr::DriverPose_t &pose) {}

	/** IMU sample has been recevied from driver **/
	virtual void OnDeviceImuUpdated(const double(&accelerometer)[3], const double(&gyroscope)[3], uint64_t timestamp) {}

	/** Send pose to driver **/
	virtual void PoseUpdated(const vr::DriverPose_t &pose);

	/** Get device orientation **/
	vr::HmdQuaternion_t GetDeviceOrientation();

	/** Driver has recentered headset **/
	virtual void OnDeviceRecenter() {}

	/** Driver has changed headset yaw offset from SteamVR center (Room Setup) **/
	virtual void OnDeviceYawOffsetChanged(double offset) {}

	/** Notify driver that base station has been added or removed **/
	virtual void BaseStationConnected(uint32_t id, const char *type);
	virtual void BaseStationRemoved(uint32_t id);

	/** Send base station pose to driver **/
	virtual void BaseStationPoseUpdated(uint32_t id, const vr::DriverPose_t &pose);

	/** Send base station power state to driver **/
	virtual void BaseStationPowerUpdated(uint32_t id, IvryTrackingPowerState state, float level);

	/** Notify driver that controller has been added or removed **/
	virtual void ControllerConnected(uint32_t id, IvryTrackingControllerHand hand, const char *type);
	virtual void ControllerRemoved(uint32_t id);

	/** Send controller state to driver **/
	virtual void ControllerUpdated(uint32_t id, const IvryTrackingControllerState *state);

	/** Send controller power state to driver **/
	virtual void ControllerPowerUpdated(uint32_t id, IvryTrackingPowerState state, float level);

	/** Controller haptics request has been received from driver **/
	virtual void OnControllerHaptics(uint32_t id, uint32_t component, float fDurationSeconds, float fFrequency, float fAmplitude) {}

	/** Controller tracking LED level request has been received from driver - use id 0xff for all **/
	virtual void OnControllerLedLevel(uint32_t id, float level) {}

	/** Debug command sent from driver **/
	virtual void OnDebugCommand(const char *cmd) {}

	/** Get last error **/
	DWORD GetLastError();

	/** Send tracker status to driver **/
	void SetStatus(const char* status);

	/** Send log message to driver **/
	void LogMessage(const char* message);

private:
	/** Private **/
	friend IvryTrackingApp_Private;
	IvryTrackingApp_Private *m_pPrivate;
};

#endif // _IVRY_TRACKING_APP_H