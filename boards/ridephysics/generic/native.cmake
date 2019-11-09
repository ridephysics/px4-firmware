
px4_add_board(
	VENDOR ridephysics
	MODEL generic
	LABEL native
	PLATFORM posix
	TESTING

	DRIVERS
		ridephysics

	DF_DRIVERS # NOTE: DriverFramework is migrating to intree PX4 drivers

	MODULES
		attitude_estimator_q
		commander
		dataman
		ekf2
		events
		land_detector
		landing_target_estimator
		load_mon
		local_position_estimator
		logger
		mavlink
		sensors
		airspeed_selector

	SYSTEMCMDS
		param
		perf
		topic_listener
		tune_control
		ver
		work_queue

	EXAMPLES
	)
