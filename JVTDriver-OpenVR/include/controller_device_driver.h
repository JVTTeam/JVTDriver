//============ Copyright (c) Valve Corporation, All rights reserved. ============
#pragma once

#include <array>
#include <atomic>
#include <memory>
#include <string>
#include <thread>

#include "hand_simulation.h"

#include "communication/feedback_comm_manager.h"

#include "openvr_driver.h"

enum Component
{

	Component_indexFinger,
	Component_middleFinger,
	Component_ringFinger,
	Component_pinkyFinger,

	Component_skeleton,

	Component_MAX
};

//-----------------------------------------------------------------------------
// Purpose: Represents a single tracked device in the system.
// What this device actually is (controller, hmd) depends on the
// properties you set within the device (see implementation of Activate)
//-----------------------------------------------------------------------------
class ControllerDeviceDriver : public vr::ITrackedDeviceServerDriver
{
public:
	ControllerDeviceDriver(vr::ETrackedControllerRole role);

	vr::EVRInitError Activate(uint32_t unObjectId) override;

	void EnterStandby() override;

	void *GetComponent(const char *pchComponentNameAndVersion) override;

	void DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize) override;

	vr::DriverPose_t GetPose() override;

	void Deactivate() override;

	// ----- Functions we declare ourselves below -----

	const std::string &MyGetSerialNumber();

private:
	void MyInputThread();

	std::thread my_input_thread_;

	std::unique_ptr<HandSimulation> my_hand_simulation_;

	std::atomic<bool> is_active_ = false;

	std::atomic<int> frame_ = 0;
	std::atomic<float> last_curl_ = 0.f;
	std::atomic<float> last_splay_ = 0.f;

	vr::TrackedDeviceIndex_t my_controller_index_ = vr::k_unTrackedDeviceIndexInvalid;

	vr::ETrackedControllerRole my_controller_role_ = vr::TrackedControllerRole_Invalid;

	std::string my_controller_model_number_;
	std::string my_controller_serial_number_;

	std::array<vr::VRInputComponentHandle_t, Component_MAX> input_handles_;

	std::unique_ptr<FeedbackCommManager> m_feedbackCommManager;
};