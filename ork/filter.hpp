/*
This file is part of the ORK library.
Full copyright and license terms can be found in the LICENSE.txt file.
*/
#pragma once
#include<array>
#include"boost/circular_buffer.hpp"

#include"ork/ork.hpp"


namespace ork {

/*
Low-pass filter functions

order determines how fast the response decays

alpha determines at what frequency the response begins to decay

Usage typically abuses constant sample rate
Coefficients calculated using WinFilter
*/
namespace detail {

template<unsigned C>
struct ring {
protected:
	static const unsigned BUFF_SIZE = C;
protected:
	boost::circular_buffer<double>_x;
	boost::circular_buffer<double>_y;
public:
	ring() : _x(BUFF_SIZE), _y(BUFF_SIZE) {
		LOOPI(C) {
			_x.push_back(0u);
			_y.push_back(0u);
		}
	}
public:
	double value()const { return _y[0]; }
};

}//namespace detail

template<unsigned order, unsigned inverse_alpha>struct butterworth;

template<>
struct butterworth<2, 4> : public detail::ring<3> {
	double tick(double x) {
		static const std::array<double, BUFF_SIZE> A = {
			0.29289321881333563000,
			0.58578643762667126000,
			0.29289321881333563000
		};
		static const std::array<double, BUFF_SIZE> B = {
			1.00000000000000000000,
			-0.00000000000000019511,
			0.17157287525380990000
		};
		_x.push_front(x);
		_y.push_front(A[0] * x);
		for(size_t i = 1; i != BUFF_SIZE; ++i) {
			_y[0] += A[i] * _x[i] - B[i] * _y[i];
		}
		return _y[0];
	}
};

template<>struct butterworth<2, 8> :public detail::ring<3> {
	double tick(double x) {
		static const std::array<double, BUFF_SIZE> A = {
			0.09763107689932175300,
			0.19526215379864351000,
			0.09763107689932175300
		};
		static const std::array<double, BUFF_SIZE> B = {
			1.00000000000000000000,
			-0.94280904158206325000,
			0.33333333333333326000
		};
		_x.push_front(x);
		_y.push_front(A[0] * x);
		for(size_t i = 1; i != BUFF_SIZE; ++i) {
			_y[0] += A[i] * _x[i] - B[i] * _y[i];
		}
		return _y[0];
	}
};

template<>struct butterworth<2, 16> :public detail::ring<3> {
	double tick(double x) {
		static const std::array<double, BUFF_SIZE> A = {
			0.02995247960866337400,
			0.05990495921732674800,
			0.02995247960866337400
		};
		static const std::array<double, BUFF_SIZE> B = {
			1.00000000000000000000,
			-1.45424358625158500000,
			0.57406191508395477000
		};
		_x.push_front(x);
		_y.push_front(A[0] * x);
		for(size_t i = 1; i != BUFF_SIZE; ++i) {
			_y[0] += A[i] * _x[i] - B[i] * _y[i];
		}
		return _y[0];
	}
};

template<>struct butterworth<2, 32> :public detail::ring<3> {
	double tick(double x) {
		static const std::array<double, BUFF_SIZE> A = {
			0.00831055513615891820,
			0.01662111027231783600,
			0.00831055513615891820
		};
		static const std::array<double, BUFF_SIZE> B = {
			1.00000000000000000000,
			-1.72377617276250890000,
			0.75754694447882898000
		};
		_x.push_front(x);
		_y.push_front(A[0] * x);
		for(size_t i = 1; i != BUFF_SIZE; ++i) {
			_y[0] += A[i] * _x[i] - B[i] * _y[i];
		}
		return _y[0];
	}
};

template<>struct butterworth<4, 4> :public detail::ring<5> {
	double tick(double x) {
		static const std::array<double, BUFF_SIZE> A = {
			0.09398064470252343000,
			0.37592257881009372000,
			0.56388386821514058000,
			0.37592257881009372000,
			0.09398064470252343000
		};
		static const std::array<double, BUFF_SIZE> B = {
			1.00000000000000000000,
			-0.00000000000000058430,
			0.48602882206826942000,
			-0.00000000000000008869,
			0.01766480087244188700
		};
		_x.push_front(x);
		_y.push_front(A[0] * x);
		for(size_t i = 1; i != BUFF_SIZE; ++i) {
			_y[0] += A[i] * _x[i] - B[i] * _y[i];
		}
		return _y[0];
	}
};

template<>struct butterworth<4, 8> :public detail::ring<5> {
	double tick(double x) {
		static const std::array<double, BUFF_SIZE> A = {
			0.01020853419306819500,
			0.04083413677227278000,
			0.06125120515840917100,
			0.04083413677227278000,
			0.01020853419306819500
		};
		static const std::array<double, BUFF_SIZE> B = {
			1.00000000000000000000,
			-1.96842778693851760000,
			1.73586070920888560000,
			-0.72447082950736208000,
			0.12038959989624438000
		};
		_x.push_front(x);
		_y.push_front(A[0] * x);
		for(size_t i = 1; i != BUFF_SIZE; ++i) {
			_y[0] += A[i] * _x[i] - B[i] * _y[i];
		}
		return _y[0];
	}
};

template<>struct butterworth<4, 16> :public detail::ring<5> {
	double tick(double x) {
		static const std::array<double, BUFF_SIZE> A = {
			0.00092507968628719654,
			0.00370031874514878620,
			0.00555047811772317950,
			0.00370031874514878620,
			0.00092507968628719654
		};
		static const std::array<double, BUFF_SIZE> B = {
			1.00000000000000000000,
			-2.97684433369673140000,
			3.42230952937763750000,
			-1.78610660021803840000,
			0.35557738234440961000
		};
		_x.push_front(x);
		_y.push_front(A[0] * x);
		for(size_t i = 1; i != BUFF_SIZE; ++i) {
			_y[0] += A[i] * _x[i] - B[i] * _y[i];
		}
		return _y[0];
	}
};

template<>struct butterworth<4, 32> :public detail::ring<5> {
	double tick(double x) {
		static const std::array<double, BUFF_SIZE> A = {
			0.00006661889502014782,
			0.00026647558008059127,
			0.00039971337012088690,
			0.00026647558008059127,
			0.00006661889502014782
		};
		static const std::array<double, BUFF_SIZE> B = {
			1.00000000000000000000,
			-3.48730774154990010000,
			4.58929123207840740000,
			-2.69888439134075500000,
			0.59806526160088713000
		};
		_x.push_front(x);
		_y.push_front(A[0] * x);
		for(size_t i = 1; i != BUFF_SIZE; ++i) {
			_y[0] += A[i] * _x[i] - B[i] * _y[i];
		}
		return _y[0];
	}
};

template<>struct butterworth<8, 4> :public detail::ring<9> {
	double tick(double x) {
		static const std::array<double, BUFF_SIZE> A = {
			0.00927650977743330230,
			0.07421207821946641800,
			0.25974227376813247000,
			0.51948454753626494000,
			0.64935568442033109000,
			0.51948454753626494000,
			0.25974227376813247000,
			0.07421207821946641800,
			0.00927650977743330230
		};
		static const std::array<double, BUFF_SIZE> B = {
			1.00000000000000000000,
			-0.00000000000000125111,
			1.06093559916223450000,
			-0.00000000000000085602,
			0.29088815727439377000,
			-0.00000000000000014198,
			0.02042958792590485500,
			-0.00000000000000000412,
			0.00017176516419411948
		};
		_x.push_front(x);
		_y.push_front(A[0] * x);
		for(size_t i = 1; i != BUFF_SIZE; ++i) {
			_y[0] += A[i] * _x[i] - B[i] * _y[i];
		}
		return _y[0];
	}
};

template<>struct butterworth<8, 8> :public detail::ring<9> {
	double tick(double x) {
		static const std::array<double, BUFF_SIZE> A = {
			0.00010881080315039874,
			0.00087048642520318989,
			0.00304670248821116470,
			0.00609340497642232950,
			0.00761675622052791160,
			0.00609340497642232950,
			0.00304670248821116470,
			0.00087048642520318989,
			0.00010881080315039874
		};
		static const std::array<double, BUFF_SIZE> B = {
			1.00000000000000000000,
			-3.98378427317419390000,
			7.53623411012089850000,
			-8.59981506480140110000,
			6.40015406034764030000,
			-3.15602526073056790000,
			1.00169657955128470000,
			-0.18634247767748546000,
			0.01550761525498689900
		};
		_x.push_front(x);
		_y.push_front(A[0] * x);
		for(size_t i = 1; i != BUFF_SIZE; ++i) {
			_y[0] += A[i] * _x[i] - B[i] * _y[i];
		}
		return _y[0];
	}
};

template<>struct butterworth<8, 16> :public detail::ring<9> {
	double tick(double x) {
		static const std::array<double, BUFF_SIZE> A = {
			0.00000095022413522387,
			0.00000760179308179098,
			0.00002660627578626845,
			0.00005321255157253689,
			0.00006651568946567112,
			0.00005321255157253689,
			0.00002660627578626845,
			0.00000760179308179098,
			0.00000095022413522387
		};
		static const std::array<double, BUFF_SIZE> B = {
			1.00000000000000000000,
			-5.98842478360558420000,
			15.88837986989357300000,
			-24.35723742416695000000,
			23.57037936537988200000,
			-14.72938334236747300000,
			5.80019014359176670000,
			-1.31502712050829000000,
			0.13135067080953630000
		};
		_x.push_front(x);
		_y.push_front(A[0] * x);
		for(size_t i = 1; i != BUFF_SIZE; ++i) {
			_y[0] += A[i] * _x[i] - B[i] * _y[i];
		}
		return _y[0];
	}
};

template<>struct butterworth<8, 32> :public detail::ring<9> {
	double tick(double x) {
		static const std::array<double, BUFF_SIZE> A = {
			0.00000000727990092326,
			0.00000005823920738611,
			0.00000020383722585138,
			0.00000040767445170275,
			0.00000050959306462844,
			0.00000040767445170275,
			0.00000020383722585138,
			0.00000005823920738611,
			0.00000000727990092326
		};
		static const std::array<double, BUFF_SIZE> B = {
			1.00000000000000000000,
			-6.99371675796337830000,
			21.45552952221124000000,
			-37.70654173701864700000,
			41.51489337314014000000,
			-29.31915987172283900000,
			12.96924361594188800000,
			-3.28499115032662870000,
			0.36474437479314531000
		};
		_x.push_front(x);
		_y.push_front(A[0] * x);
		for(size_t i = 1; i != BUFF_SIZE; ++i) {
			_y[0] += A[i] * _x[i] - B[i] * _y[i];
		}
		return _y[0];
	}
};

}//namespace ork
