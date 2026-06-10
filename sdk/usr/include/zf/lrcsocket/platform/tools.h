#pragma once



namespace lrcsocket
{
    /// get pc-time [ms]
	unsigned int get_time_ms();

    /// sleep this thread
	void sleep_ms(unsigned int t);
}
