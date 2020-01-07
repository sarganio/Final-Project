#include <stdlib.h>
#include "Stk.h"


namespace stk {

	StkFloat Stk::srate_ = (StkFloat)SRATE;
	std::string Stk::rawwavepath_ = RAWWAVE_PATH;
	const Stk::StkFormat Stk::STK_SINT8 = 0x1;
	const Stk::StkFormat Stk::STK_SINT16 = 0x2;
	const Stk::StkFormat Stk::STK_SINT24 = 0x4;
	const Stk::StkFormat Stk::STK_SINT32 = 0x8;
	const Stk::StkFormat Stk::STK_FLOAT32 = 0x10;
	const Stk::StkFormat Stk::STK_FLOAT64 = 0x20;
	bool Stk::showWarnings_ = true;
	bool Stk::printErrors_ = true;
	std::vector<Stk*> Stk::alertList_;
	std::ostringstream Stk::oStream_;

	Stk::Stk(void)
		: ignoreSampleRateChange_(false)
	{
	}

	Stk :: ~Stk(void)
	{
	}

	void Stk::setSampleRate(StkFloat rate)
	{
		if (rate > 0.0 && rate != srate_) {
			StkFloat oldRate = srate_;
			srate_ = rate;

			for (unsigned int i = 0; i < alertList_.size(); i++)
				alertList_[i]->sampleRateChanged(srate_, oldRate);
		}
	}

	void Stk::sampleRateChanged(StkFloat /*newRate*/, StkFloat /*oldRate*/)
	{
		// This function should be reimplemented in classes that need to
		// make internal variable adjustments in response to a global sample
		// rate change.
	}

	void Stk::addSampleRateAlert(Stk* ptr)
	{
		for (unsigned int i = 0; i < alertList_.size(); i++)
			if (alertList_[i] == ptr) return;

		alertList_.push_back(ptr);
	}

	void Stk::removeSampleRateAlert(Stk* ptr)
	{
		for (unsigned int i = 0; i < alertList_.size(); i++) {
			if (alertList_[i] == ptr) {
				alertList_.erase(alertList_.begin() + i);
				return;
			}
		}
	}

	void Stk::setRawwavePath(std::string path)
	{
		if (!path.empty())
			rawwavepath_ = path;

		// Make sure the path includes a "/"
		if (rawwavepath_[rawwavepath_.length() - 1] != '/')
			rawwavepath_ += "/";
	}

	void Stk::swap16(unsigned char* ptr)
	{
		unsigned char val;

		// Swap 1st and 2nd bytes
		val = *(ptr);
		*(ptr) = *(ptr + 1);
		*(ptr + 1) = val;
	}

	void Stk::swap32(unsigned char* ptr)
	{
		unsigned char val;

		// Swap 1st and 4th bytes
		val = *(ptr);
		*(ptr) = *(ptr + 3);
		*(ptr + 3) = val;

		//Swap 2nd and 3rd bytes
		ptr += 1;
		val = *(ptr);
		*(ptr) = *(ptr + 1);
		*(ptr + 1) = val;
	}

	void Stk::swap64(unsigned char* ptr)
	{
		unsigned char val;

		// Swap 1st and 8th bytes
		val = *(ptr);
		*(ptr) = *(ptr + 7);
		*(ptr + 7) = val;

		// Swap 2nd and 7th bytes
		ptr += 1;
		val = *(ptr);
		*(ptr) = *(ptr + 5);
		*(ptr + 5) = val;

		// Swap 3rd and 6th bytes
		ptr += 1;
		val = *(ptr);
		*(ptr) = *(ptr + 3);
		*(ptr + 3) = val;

		// Swap 4th and 5th bytes
		ptr += 1;
		val = *(ptr);
		*(ptr) = *(ptr + 1);
		*(ptr + 1) = val;
	}

#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))
#include <unistd.h>
#elif defined(__OS_WINDOWS__)
#include <windows.h>
#endif

	void Stk::sleep(unsigned long milliseconds)
	{
#if defined(__OS_WINDOWS__)
		Sleep((DWORD)milliseconds);
#elif (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))
		usleep((unsigned long)(milliseconds * 1000.0));
#endif
	}

	void Stk::handleError(StkError::Type type) const
	{
		handleError(oStream_.str(), type);
		oStream_.str(std::string()); // reset the ostringstream buffer
	}

	void Stk::handleError(const char* message, StkError::Type type)
	{
		std::string msg(message);
		handleError(msg, type);
	}

	void Stk::handleError(std::string message, StkError::Type type)
	{
		if (type == StkError::WARNING || type == StkError::STATUS) {
			if (!showWarnings_) return;
			std::cerr << '\n' << message << '\n' << std::endl;
		}
		else if (type == StkError::DEBUG_PRINT) {
#if defined(_STK_DEBUG_)
			std::cerr << '\n' << message << '\n' << std::endl;
#endif
		}
		else {
			if (printErrors_) {
				// Print error message before throwing.
				std::cerr << '\n' << message << '\n' << std::endl;
			}
			throw StkError(message, type);
		}
	}

	//
	// StkFrames definitions
	//

	StkFrames::StkFrames(unsigned int nFrames, unsigned int nChannels)
		: _data(0), _nFrames(nFrames), _nChannels(nChannels)
	{
		_size = _nFrames * _nChannels;
		_bufferSize = _size;

		if (_size > 0) {
			_data = (StkFloat*)calloc(_size, sizeof(StkFloat));
#if defined(_STK_DEBUG_)
			if (_data == NULL) {
				std::string error = "StkFrames: memory allocation error in constructor!";
				Stk::handleError(error, StkError::MEMORY_ALLOCATION);
			}
#endif
		}

		_dataRate = Stk::sampleRate();
	}

	StkFrames::StkFrames(const StkFloat& value, unsigned int nFrames, unsigned int nChannels)
		: _data(0), _nFrames(nFrames), _nChannels(nChannels)
	{
		_size = _nFrames * _nChannels;
		_bufferSize = _size;
		if (_size > 0) {
			_data = (StkFloat*)malloc(_size * sizeof(StkFloat));
#if defined(_STK_DEBUG_)
			if (_data == NULL) {
				std::string error = "StkFrames: memory allocation error in constructor!";
				Stk::handleError(error, StkError::MEMORY_ALLOCATION);
			}
#endif
			for (long i = 0; i < (long)_size; i++) _data[i] = value;
		}

		_dataRate = Stk::sampleRate();
	}

	StkFrames :: ~StkFrames()
	{
		if (_data) free(_data);
	}

	StkFrames::StkFrames(const StkFrames& f)
		: _data(0), _size(0), _bufferSize(0)
	{
		resize(f.frames(), f.channels());
		_dataRate = Stk::sampleRate();
		for (unsigned int i = 0; i < _size; i++) _data[i] = f[i];
	}

	StkFrames& StkFrames :: operator= (const StkFrames& f)
	{
		if (_data) free(_data);
		_data = 0;
		_size = 0;
		_bufferSize = 0;
		resize(f.frames(), f.channels());
		_dataRate = Stk::sampleRate();
		for (unsigned int i = 0; i < _size; i++) _data[i] = f[i];
		return *this;
	}

	void StkFrames::resize(size_t nFrames, unsigned int nChannels)
	{
		_nFrames = nFrames;
		_nChannels = nChannels;

		_size = _nFrames * _nChannels;
		if (_size > _bufferSize) {
			if (_data) free(_data);
			_data = (StkFloat*)malloc(_size * sizeof(StkFloat));
#if defined(_STK_DEBUG_)
			if (_data == NULL) {
				std::string error = "StkFrames::resize: memory allocation error!";
				Stk::handleError(error, StkError::MEMORY_ALLOCATION);
			}
#endif
			_bufferSize = _size;
		}
	}

	void StkFrames::resize(size_t nFrames, unsigned int nChannels, StkFloat value)
	{
		this->resize(nFrames, nChannels);

		for (size_t i = 0; i < _size; i++) _data[i] = value;
	}

	StkFrames& StkFrames::getChannel(unsigned int sourceChannel, StkFrames& destinationFrames, unsigned int destinationChannel) const
	{
#if defined(_STK_DEBUG_)
		if (sourceChannel > channels() - 1) {
			std::ostringstream error;
			error << "StkFrames::getChannel invalid sourceChannel (" << sourceChannel << ")";
			Stk::handleError(error.str(), StkError::FUNCTION_ARGUMENT);
		}
		if (destinationChannel > destinationFrames.channels() - 1) {
			std::ostringstream error;
			error << "StkFrames::getChannel invalid destinationChannel (" << destinationChannel << ")";
			Stk::handleError(error.str(), StkError::FUNCTION_ARGUMENT);
		}
		if (destinationFrames.frames() < frames()) {
			std::ostringstream error;
			error << "StkFrames::getChannel destination.frames() < frames()";
			Stk::handleError(error.str(), StkError::MEMORY_ACCESS);
		}
#endif
		int sourceHop = _nChannels;
		int destinationHop = destinationFrames._nChannels;
		for (unsigned int i = sourceChannel, j = destinationChannel;
			i < _nFrames * _nChannels;
			i += sourceHop, j += destinationHop)
		{
			destinationFrames[j] = _data[i];
		}
		return destinationFrames;

	}

	void StkFrames::setChannel(unsigned int destinationChannel, const stk::StkFrames& sourceFrames, unsigned int sourceChannel)
	{
#if defined(_STK_DEBUG_)
		if (sourceChannel > sourceFrames.channels() - 1) {
			std::ostringstream error;
			error << "StkFrames::setChannel invalid sourceChannel (" << sourceChannel << ")";
			Stk::handleError(error.str(), StkError::FUNCTION_ARGUMENT);
		}
		if (destinationChannel > channels() - 1) {
			std::ostringstream error;
			error << "StkFrames::setChannel invalid channel (" << destinationChannel << ")";
			Stk::handleError(error.str(), StkError::FUNCTION_ARGUMENT);
		}
		if (sourceFrames.frames() != frames()) {
			std::ostringstream error;
			error << "StkFrames::setChannel sourceFrames.frames() != frames()";
			Stk::handleError(error.str(), StkError::MEMORY_ACCESS);
		}
#endif
		unsigned int sourceHop = sourceFrames._nChannels;
		unsigned int destinationHop = _nChannels;
		for (unsigned int i = destinationChannel, j = sourceChannel;
			i < _nFrames * _nChannels;
			i += destinationHop, j += sourceHop)
		{
			_data[i] = sourceFrames[j];
		}
	}

	StkFloat StkFrames::interpolate(StkFloat frame, unsigned int channel) const
	{
#if defined(_STK_DEBUG_)
		if (frame < 0.0 || frame >(StkFloat) (_nFrames - 1) || channel >= _nChannels) {
			std::ostringstream error;
			error << "StkFrames::interpolate: invalid frame (" << frame << ") or channel (" << channel << ") value!";
			Stk::handleError(error.str(), StkError::MEMORY_ACCESS);
		}
#endif

		size_t iIndex = (size_t)frame;                    // integer part of index
		StkFloat output, alpha = frame - (StkFloat)iIndex;  // fractional part of index

		iIndex = iIndex * _nChannels + channel;
		output = _data[iIndex];
		if (alpha > 0.0)
			output += (alpha * (_data[iIndex + _nChannels] - output));

		return output;
	}

} // stk namespace