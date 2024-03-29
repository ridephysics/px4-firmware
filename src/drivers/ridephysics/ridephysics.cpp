/****************************************************************************
 *
 * Copyright (c) 2016 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file ridephysics.cpp
 * SensorDriver for receiving ridephysics data.
 *
 * @author Michael Zimmermann <sigmaepsilon92@gmail.com>
 */

#include <px4_platform_common/px4_config.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <px4_platform_common/getopt.h>
#include <errno.h>

#include <systemlib/err.h>

#include <drivers/drv_hrt.h>
#include <drivers/drv_baro.h>

#include <board_config.h>
//#include <mathlib/math/filter/LowPassFilter2p.hpp>

#include <DevObj.hpp>
#include <DevMgr.hpp>

#define DEV_PATH "/dev/null/ridephysics/dev"
#define DEV_CLASS_PATH "/dev/null/ridephysics/class"


extern "C" { __EXPORT int ridephysics_main(int argc, char *argv[]); }

using namespace DriverFramework;


class Ridephysics : public DevObj
{
public:
	Ridephysics();
	~Ridephysics();


	/**
	 * Start automatic measurement.
	 *
	 * @return 0 on success
	 */
	int		start();

	/**
	 * Stop automatic measurement.
	 *
	 * @return 0 on success
	 */
	int		stop();

private:
	virtual void _measure() override;

	orb_advert_t		_baro_topic;

	int			_baro_orb_class_instance;

};

Ridephysics::Ridephysics() :
	DevObj("RideohysicsSensor", DEV_PATH, DEV_CLASS_PATH, DeviceBusType_UNKNOWN, 5000),
	_baro_topic(nullptr),
	_baro_orb_class_instance(-1)
{
}

Ridephysics::~Ridephysics()
{
}

int Ridephysics::start()
{
	PX4_INFO("ridephysics started");

	return 0;
}

int Ridephysics::stop()
{
	PX4_INFO("ridephysics stopped");

	return 0;
}

void Ridephysics::_measure()
{
	
}

#if 0
int Ridephysics::_publish(struct baro_sensor_data &data)
{
	sensor_baro_s baro_report = {};
	baro_report.timestamp   = hrt_absolute_time();
	baro_report.error_count = data.error_counter;

	baro_report.pressure    = data.pressure_pa / 100.0f; // to mbar
	baro_report.temperature = data.temperature_c;

	// TODO: when is this ever blocked?
	if (!(m_pub_blocked)) {

		if (_baro_topic == nullptr) {
			_baro_topic = orb_advertise_multi(ORB_ID(sensor_baro), &baro_report,
							  &_baro_orb_class_instance, ORB_PRIO_DEFAULT);

		} else {
			orb_publish(ORB_ID(sensor_baro), _baro_topic, &baro_report);
		}
	}

	return 0;
};
#endif


namespace ridephysics
{

Ridephysics *g_dev = nullptr;

int start(/* enum Rotation rotation */);
int stop();
int info();
void usage();

int start(/*enum Rotation rotation*/)
{
	g_dev = new Ridephysics(/*rotation*/);

	if (g_dev == nullptr) {
		PX4_ERR("failed instantiating Ridephysics object");
		return -1;
	}

	int ret = g_dev->start();

	if (ret != 0) {
		PX4_ERR("Ridephysics start failed");
		return ret;
	}

	return 0;
}

int stop()
{
	if (g_dev == nullptr) {
		PX4_ERR("driver not running");
		return 1;
	}

	int ret = g_dev->stop();

	if (ret != 0) {
		PX4_ERR("driver could not be stopped");
		return ret;
	}

	delete g_dev;
	g_dev = nullptr;
	return 0;
}

/**
 * Print a little info about the driver.
 */
int
info()
{
	if (g_dev == nullptr) {
		PX4_ERR("driver not running");
		return 1;
	}

	PX4_DEBUG("state @ %p", g_dev);

	return 0;
}

void
usage()
{
	PX4_WARN("Usage: ridephysics 'start', 'info', 'stop'");
}

} // namespace ridephysics


int
ridephysics_main(int argc, char *argv[])
{
	int ret = 0;
	int myoptind = 1;

	if (argc <= 1) {
		ridephysics::usage();
		return 1;
	}

	const char *verb = argv[myoptind];


	if (!strcmp(verb, "start")) {
		ret = ridephysics::start();
	}

	else if (!strcmp(verb, "stop")) {
		ret = ridephysics::stop();
	}

	else if (!strcmp(verb, "info")) {
		ret = ridephysics::info();
	}

	else {
		ridephysics::usage();
		return 1;
	}

	return ret;
}
