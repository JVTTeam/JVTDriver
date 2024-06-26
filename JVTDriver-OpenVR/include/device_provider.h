//============ Copyright (c) Valve Corporation, All rights reserved. ============
#pragma once

#include <memory>

#include "controller_device_driver.h"
#include "openvr_driver.h"

// make sure your class is publicly inheriting vr::IServerTrackedDeviceProvider!
class DeviceProvider : public vr::IServerTrackedDeviceProvider
{
public:
	vr::EVRInitError Init( vr::IVRDriverContext *pDriverContext ) override;
	const char *const *GetInterfaceVersions() override;

	void RunFrame() override;

	bool ShouldBlockStandbyMode() override;
	void EnterStandby() override;
	void LeaveStandby() override;

	void Cleanup() override;

private:
	std::unique_ptr<ControllerDeviceDriver> my_left_controller_device_;
	std::unique_ptr<ControllerDeviceDriver> my_right_controller_device_;
};