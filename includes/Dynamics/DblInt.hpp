#ifndef __DBLINT_HPP__
#define __DBLINT_HPP__

#include <Dynamics/Dynamics.hpp>

/**
* This class is intended to be the basic template for double integrator models of any size.
* It is not intended to stand on it's own and therefore should not be directly instatiated.
*/
template <size_t _xDim, size_t _uDim>
class DblInt : public Dynamics<_xDim, _uDim> {
protected:
	DblInt(std::string name, const floatX control_penalty, const floatX poly_degree)
		: Dynamics(name, control_penalty, poly_degree)
	{}

};

#endif // __DBLINT_HPP__