/**
 * \file
 *
 * \brief This module defines a collection of matrix classes.
 *
 * Copyright (c) 2011-2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _MATRIX_MATH_H_
#define _MATRIX_MATH_H_

#include "vector.h"

namespace math {

class matrix2d {
private:

	/** \brief column vector storage */
	vector2d  C[2];

public:

	/** \name class construction and destruction */
	/** @{ */
	matrix2d () {}

	matrix2d
		(const vector2d & c0,
		 const vector2d & c1)
		{ C[0] = c0; C[1] = c1;}

	matrix2d
		(double m11, double m12,
		 double m21, double m22)
		{
			C[0] = vector2d (m11, m21);
			C[1] = vector2d (m12, m22);
		}

	matrix2d
		(double s)
		{
			C[0] = vector2d (s, 0);
			C[1] = vector2d (0, s);
		}

	/* this ctor is not intended to be an implied type converter */
	explicit matrix2d
		(const double * m)
		{
			C[0] = vector2d (m[0], m[2]);
			C[1] = vector2d (m[1], m[3]);
		}
	/** @} */

	const matrix2d adjoint () const;
	const matrix2d inverse () const{
			matrix2d T( C[1][1],-C[1][0],
					   -C[0][1], C[0][0] );
			T *= (1/(*this).determinant());
			return T;
		}

	const double determinant () const
		{ return C[0].x*C[1].y-C[0].y*C[1].x; }

	void  identity ()
	{
		C[0] = vector2d (1, 0);
		C[1] = vector2d (0, 1);
	}

	matrix2d  transpose ()
	{
		matrix2d const m (*this);

		return matrix2d(m.C[0][0], m.C[1][0], m.C[0][1], m.C[1][1]);
	}

	void  zero () { C[1] = C[0] = vector2d (0, 0); }

	/** \name class member operators */
	/** @{ */
	double & operator() (int i, int j)
	{
		return C[j][i];
	}

	const double & operator() (int i, int j) const
	{
		return C[j][i];
	}

	const vector2d operator * (const vector2d & v) const
	{ return ((C[0] * v.x) + (C[1] * v.y)); }

	const matrix2d & operator += (const matrix2d & b)
	{
		C[0] += b.C[0]; C[1] += b.C[1];
		return *this;
	}

	const matrix2d & operator -= (const matrix2d & b)
	{
		C[0] -= b.C[0]; C[1] -= b.C[1];
		return *this;
	}

	const matrix2d & operator *= (const matrix2d & b)
	{
		matrix2d const m (*this);

		C[0] = m * b.C[0]; C[1] = m * b.C[1];
		return *this;
	}

	const matrix2d & operator *= (const double & s)
	{
		C[0] *= s; C[1] *= s;
		return *this;
	}

	const matrix2d operator + (const matrix2d & m) const
		{ return matrix2d (C[0] + m.C[0], C[1] + m.C[1]); }

	const matrix2d operator - (const matrix2d & m) const
		{ return matrix2d (C[0] - m.C[0], C[1] - m.C[1]); }

	const matrix2d operator * (const matrix2d & m) const
		{
		return matrix2d ((*this) * m.C[0], (*this) * m.C[1]);
		}
	/** @} */

	/** \name class friend operators */
	/** @{ */
	friend const matrix2d operator * (const double & s, const matrix2d & A)
		{ return (matrix2d (A) *= s); }

	friend const matrix2d operator - (const matrix2d & A)
		{ return (matrix2d (A) *= -1); }
	/** @} */
};

/** \brief 3-Dimensional Matrix Class */
class matrix3d {
private:

	/** \brief column vector storage */
	vector3d  C[3];

public:

	/** \name class construction and destruction */
	/** @{ */
	matrix3d () {}

	matrix3d
		(const vector3d & c0,
		 const vector3d & c1,
		 const vector3d & c2)
		{ C[0] = c0; C[1] = c1; C[2] = c2; }

	matrix3d
		(double m11, double m12, double m13,
		 double m21, double m22, double m23,
		 double m31, double m32, double m33)
		{
		C[0] = vector3d (m11, m21, m31);
		C[1] = vector3d (m12, m22, m32);
		C[2] = vector3d (m13, m23, m33);
		}

	/* this ctor is not intended to be an implied type converter */
	explicit matrix3d
		(const double * m)
		{
		C[0] = vector3d (m[0], m[3], m[6]);
		C[1] = vector3d (m[1], m[4], m[7]);
		C[2] = vector3d (m[2], m[5], m[8]);
		}
	/** @} */

	const matrix3d adjoint () const;
	const matrix3d inverse () const;

	const double determinant () const
		{ return C[0].dot (C[1].cross (C[2])); }

	void  identity ()
	{
		C[0] = vector3d (1, 0, 0);
		C[1] = vector3d (0, 1, 0);
		C[2] = vector3d (0, 0, 1);
	}

	void  transpose ()
	{
		matrix3d const m (*this);

		C[0] = vector3d (m.C[0][0], m.C[1][0], m.C[2][0]);
		C[1] = vector3d (m.C[0][1], m.C[1][1], m.C[2][1]);
		C[2] = vector3d (m.C[0][2], m.C[1][2], m.C[2][2]);
	}

	void  zero () { C[2] = C[1] = C[0] = vector3d (0, 0, 0); }

	/** \name class member operators */
	/** @{ */
	double & operator() (int i, int j)
	{
		return C[j][i];
	}

	const double & operator() (int i, int j) const
	{
		return C[j][i];
	}

	const vector3d operator * (const vector3d & v) const
		{ return ((C[0] * v.x) + (C[1] * v.y) + (C[2] * v.z)); }

	const matrix3d & operator += (const matrix3d & b)
	{
		C[0] += b.C[0]; C[1] += b.C[1]; C[2] += b.C[2];
		return *this;
	}

	const matrix3d & operator -= (const matrix3d & b)
	{
		C[0] -= b.C[0]; C[1] -= b.C[1]; C[2] -= b.C[2];
		return *this;
	}

	const matrix3d & operator *= (const matrix3d & b)
	{
		matrix3d const m (*this);

		C[0] = m * b.C[0]; C[1] = m * b.C[1]; C[2] = m * b.C[2];
		return *this;
	}

	const matrix3d & operator *= (const double & s)
	{
		C[0] *= s; C[1] *= s; C[2] *= s;
		return *this;
	}

	const matrix3d operator + (const matrix3d & m) const
		{ return matrix3d (C[0] + m.C[0], C[1] + m.C[1], C[2] + m.C[2]); }

	const matrix3d operator - (const matrix3d & m) const
		{ return matrix3d (C[0] - m.C[0], C[1] - m.C[1], C[2] - m.C[2]); }

	const matrix3d operator * (const matrix3d & m) const
		{
		return matrix3d ((*this) * m.C[0], (*this) * m.C[1], (*this) * m.C[2]);
		}
	/** @} */

	/** \name class friend operators */
	/** @{ */
	friend const matrix3d operator * (const double & s, const matrix3d & A)
		{ return (matrix3d (A) *= s); }

	friend const matrix3d operator - (const matrix3d & A)
		{ return (matrix3d (A) *= -1); }
	/** @} */
};


/** \brief 4-Dimensional Matrix Class */
class matrix4d {

private:

	/** \brief column vector storage */
	vector4d C[4];

public:

	/** \name class construction and destruction */
	/** @{ */
	matrix4d () {}

	matrix4d
		(const vector4d & c0,
		 const vector4d & c1,
		 const vector4d & c2,
		 const vector4d & c3)
		{ C[0] = c0; C[1] = c1; C[2] = c2; C[3] = c3; }

	matrix4d
		(double m11, double m12, double m13, double m14,
		 double m21, double m22, double m23, double m24,
		 double m31, double m32, double m33, double m34,
		 double m41, double m42, double m43, double m44)
		{
		C[0] = vector4d (m11, m21, m31, m41);
		C[1] = vector4d (m12, m22, m32, m42);
		C[2] = vector4d (m13, m23, m33, m43);
		C[3] = vector4d (m14, m24, m34, m44);
		}

	/* this ctor is not intended to be an implied type converter */
	explicit matrix4d (const double * m)
	{
		C[0] = vector4d (m[0], m[4], m[8],  m [12]);
		C[1] = vector4d (m[1], m[5], m[9],  m [13]);
		C[2] = vector4d (m[2], m[6], m[10], m [14]);
		C[3] = vector4d (m[3], m[7], m[11], m [15]);
	}
	/** @} */

	const matrix4d adjoint () const;
	const matrix4d inverse () const;
	const double determinant () const;

	void identity ()
	{
		C[0] = vector4d (1, 0, 0, 0);
		C[1] = vector4d (0, 1, 0, 0);
		C[2] = vector4d (0, 0, 1, 0);
		C[3] = vector4d (0, 0, 0, 1);
	}

	void transpose ()
	{
		matrix4d const m (*this);

		C[0] = vector4d (m.C[0][0], m.C[1][0], m.C[2][0], m.C[3][0]);
		C[1] = vector4d (m.C[0][1], m.C[1][1], m.C[2][1], m.C[3][1]);
		C[2] = vector4d (m.C[0][2], m.C[1][2], m.C[2][2], m.C[3][2]);
		C[3] = vector4d (m.C[0][3], m.C[1][3], m.C[2][3], m.C[3][3]);
	}

	void zero () { C[3] = C[2] = C[1] = C[0] = vector4d (0, 0, 0, 0); }

	/** \name class member operators */
	/** @{ */
	double & operator() (int i, int j)
	{
		return C[j][i];
	}
	const double & operator() (int i, int j) const
	{
		return C[j][i];
	}

	const vector4d operator * (const vector4d & v) const
	{
		return ((C[0] * v.x) + (C[1] * v.y) + (C[2] * v.z) + (C[3] * v.w));
	}

	const matrix4d & operator += (const matrix4d & b)
	{
		C[0] += b.C[0]; C[1] += b.C[1];
		C[2] += b.C[2]; C[3] += b.C[3];

		return *this;
	}

	const matrix4d & operator -= (const matrix4d & b)
	{
		C[0] -= b.C[0]; C[1] -= b.C[1];
		C[2] -= b.C[2]; C[3] -= b.C[3];

		return *this;
	}

	const matrix4d & operator *= (const matrix4d & b)
	{
		matrix4d const m (*this);

		C[0] = m * b.C[0]; C[1] = m * b.C[1];
		C[2] = m * b.C[2]; C[3] = m * b.C[3];

		return *this;
	}

	const matrix4d & operator *= (const double & s)
	{
		C[0] *= s; C[1] *= s;
		C[2] *= s; C[3] *= s;

		return *this;
	}

	const matrix4d operator + (const matrix4d & m) const
	{
		return matrix4d
			(C[0] + m.C[0], C[1] + m.C[1],
			 C[2] + m.C[2], C[3] + m.C[3]);
	}

	const matrix4d operator - (const matrix4d & m) const
	{
		return matrix4d
			(C[0] - m.C[0], C[1] - m.C[1],
			 C[2] - m.C[2], C[3] - m.C[3]);
	}

	const matrix4d operator * (const matrix4d & m) const
	{
		return matrix4d
			((*this) * m.C[0], (*this) * m.C[1],
			 (*this) * m.C[2], (*this) * m.C[3]);
	}
	/** @} */

	friend const matrix4d operator * (const double & s, const matrix4d & A)
		{ return (matrix4d (A) *= s); }

	friend const matrix4d operator - (const matrix4d & A)
		{ return (matrix4d (A) *= -1); }
};

}

#endif