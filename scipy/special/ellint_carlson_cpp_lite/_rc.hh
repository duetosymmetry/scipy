#ifndef ELLINT_RC_GENERIC_GUARD
#define ELLINT_RC_GENERIC_GUARD


#include <algorithm>
#include <complex>
#include "ellint_typing.hh"
#include "ellint_argcheck.hh"
#include "ellint_common.hh"
#include "ellint_carlson.hh"


namespace ellint_carlson {

template<typename T, typename TR>
ExitStatus
rc(const T& x, const T& y, const TR& rerr, T& res)
{
    typedef typing::decplx_t<T> RT;

    ExitStatus status = ExitStatus::success;
    if ( argcheck::invalid_rerr(rerr, 2.0e-4) )
    {
	res = typing::nan<T>();
	return ExitStatus::bad_rerr;
    }

    /* Cauchy principal value with real negative y */
    if ( argcheck::too_small(std::imag(y)) &&
        ( std::real(y) < (RT)0.0 ) )
    {
	T tmpres;
	status = ellint_carlson::rc(x - y, -y, rerr, tmpres);
	if ( is_horrible(status) )
	{
	    res = typing::nan<T>();
	}
	res = tmpres * std::sqrt(x / (x - y));
	return status;
    } else if ( argcheck::too_small(y) || !argcheck::ph_good(x) ) {
	res = typing::nan<T>();
	return ExitStatus::bad_args;
    } else if ( argcheck::isinf(x) || argcheck::isinf(y) ) {
	res = T(0.0);
	return ExitStatus::success;
    }

    T Am = (x + (RT)2.0 * y) / (RT)3.0;
    RT fterm = std::abs(Am - x) / arithmetic::ocrt((TR)3.0 * rerr);

    T xm = x;
    T ym = y;
    T sm = y - Am;

    unsigned int m = 0;  /* loop variable */
    while ( std::max(std::abs(xm - ym), fterm) >= std::abs(Am) )
    {
	if ( m > config::max_iter )
	{
	    status = ExitStatus::n_iter;
	    break;
	}

	T lam = (RT)2.0 * std::sqrt(xm) * std::sqrt(ym) + ym;
        Am = (Am + lam) * (RT)0.25;
        xm = (xm + lam) * (RT)0.25;
        ym = (ym + lam) * (RT)0.25;
	sm *= (RT)0.25;
	fterm *= (RT)0.25;

	++m;
    }
    Am = (xm + ym + ym) / (RT)3.0;
    sm /= Am;
    res = arithmetic::comp_horner(sm, constants::RC_C) /
          (std::sqrt(Am) * (RT)(constants::RC_C[0]));
    return status;
}


}  /* namespace ellint_carlson */
#endif /* ELLINT_RC_GENERIC_GUARD */
