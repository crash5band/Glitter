/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2014 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

//=========================================================================
//	Most of this code has been copied and modified from the Ogre3D Project.
//=========================================================================

#include "MathGens.h"
#include <math.h>
#include <Windows.h>

namespace Glitter
{
	float rsq(float r)
	{
		return (1.0f / sqrt(r));
	}

    bool Matrix3::toEulerAnglesZYX(float& rfYAngle, float& rfPAngle, float& rfRAngle) {
        rfPAngle = asin(-m[2][0]);
        if (rfPAngle < HALF_PI) {
            if (rfPAngle > -HALF_PI) {
                rfYAngle = atan2(m[1][0],m[0][0]);
                rfRAngle = atan2(m[2][1],m[2][2]);
                return true;
            }
            else {
                float fRmY = atan2(-m[0][1],m[0][2]);
                rfRAngle = 0.0f;
                rfYAngle = rfRAngle - fRmY;
                return false;
            }
        }
        else {
            float fRpY = atan2(-m[0][1],m[0][2]);
            rfRAngle = 0.0f;
            rfYAngle = fRpY - rfRAngle;
            return false;
        }
    }

	bool Matrix3::toEulerAnglesYXZ(float& rfYAngle, float& rfPAngle, float& rfRAngle) {
        rfPAngle = asin(-m[1][2]);
        if (rfPAngle < HALF_PI) {
            if (rfPAngle > -HALF_PI) {
                rfYAngle = atan2(m[0][2],m[2][2]);
                rfRAngle = atan2(m[1][0],m[1][1]);
                return true;
            }
            else {
                float fRmY = atan2(-m[0][1],m[0][0]);
                rfRAngle = 0.0f;
                rfYAngle = rfRAngle - fRmY;
                return false;
            }
        }
        else {
            float fRpY = atan2(-m[0][1],m[0][0]);
            rfRAngle = 0.0f;
            rfYAngle = fRpY - rfRAngle;
            return false;
        }
    }

	bool Matrix3::toEulerAnglesYZX(float& rfYAngle, float& rfPAngle, float& rfRAngle) {
        rfPAngle = asin(m[1][0]);
        if (rfPAngle < HALF_PI) {
            if (rfPAngle > -HALF_PI) {
                rfYAngle = atan2(-m[2][0],m[0][0]);
                rfRAngle = atan2(-m[1][2],m[1][1]);
                return true;
            }
            else {
                float fRmY = atan2(m[2][1],m[2][2]);
                rfRAngle = 0.0f;
                rfYAngle = rfRAngle - fRmY;
                return false;
            }
        }
        else {
            float fRpY = atan2(m[2][1],m[2][2]);
            rfRAngle = 0.0f;
            rfYAngle = fRpY - rfRAngle;
            return false;
        }
    }

    void Matrix3::fromEulerAnglesZYX(const float& fYAngle, const float& fPAngle, const float& fRAngle) {
        float fCos, fSin;

        fCos = cos(fYAngle);
        fSin = sin(fYAngle);
        Matrix3 kZMat(fCos,-fSin,0.0,fSin,fCos,0.0,0.0,0.0,1.0);

        fCos = cos(fPAngle);
        fSin = sin(fPAngle);
        Matrix3 kYMat(fCos,0.0,fSin,0.0,1.0,0.0,-fSin,0.0,fCos);

        fCos = cos(fRAngle);
        fSin = sin(fRAngle);
        Matrix3 kXMat(1.0,0.0,0.0,0.0,fCos,-fSin,0.0,fSin,fCos);

        *this = kZMat*(kYMat*kXMat);
    }

	
    void Matrix3::fromEulerAnglesYXZ(const float& fYAngle, const float& fPAngle, const float& fRAngle) {
        float fCos, fSin;

        fCos = cos(fYAngle);
        fSin = sin(fYAngle);
        Matrix3 kZMat(fCos,-fSin,0.0,fSin,fCos,0.0,0.0,0.0,1.0);

        fCos = cos(fPAngle);
        fSin = sin(fPAngle);
        Matrix3 kYMat(fCos,0.0,fSin,0.0,1.0,0.0,-fSin,0.0,fCos);

        fCos = cos(fRAngle);
        fSin = sin(fRAngle);
        Matrix3 kXMat(1.0,0.0,0.0,0.0,fCos,-fSin,0.0,fSin,fCos);

        *this = kYMat*(kXMat*kZMat);
    }

	
    void Matrix3::fromEulerAnglesYZX(const float& fYAngle, const float& fPAngle, const float& fRAngle) {
        float fCos, fSin;

        fCos = cos(fYAngle);
        fSin = sin(fYAngle);
        Matrix3 kZMat(fCos,-fSin,0.0,fSin,fCos,0.0,0.0,0.0,1.0);

        fCos = cos(fPAngle);
        fSin = sin(fPAngle);
        Matrix3 kYMat(fCos,0.0,fSin,0.0,1.0,0.0,-fSin,0.0,fCos);

        fCos = cos(fRAngle);
        fSin = sin(fRAngle);
        Matrix3 kXMat(1.0,0.0,0.0,0.0,fCos,-fSin,0.0,fSin,fCos);

        *this = kYMat*(kZMat*kXMat);
    }


	Matrix3 Matrix3::operator* (const Matrix3& rkMatrix) {
        Matrix3 kProd;
        for (size_t iRow = 0; iRow < 3; iRow++)
        {
            for (size_t iCol = 0; iCol < 3; iCol++)
            {
                kProd.m[iRow][iCol] =
                    m[iRow][0]*rkMatrix.m[0][iCol] +
                    m[iRow][1]*rkMatrix.m[1][iCol] +
                    m[iRow][2]*rkMatrix.m[2][iCol];
            }
        }
        return kProd;
    }

	void Matrix3::QDUDecomposition(Matrix3& kQ, Vector3& kD, Vector3& kU) {
        float fInvLength = m[0][0]*m[0][0] + m[1][0]*m[1][0] + m[2][0]*m[2][0];
		if (fInvLength != 0) fInvLength = rsq(fInvLength);

        kQ[0][0] = m[0][0]*fInvLength;
        kQ[1][0] = m[1][0]*fInvLength;
        kQ[2][0] = m[2][0]*fInvLength;

        float fDot = kQ[0][0]*m[0][1] + kQ[1][0]*m[1][1] + kQ[2][0]*m[2][1];
        kQ[0][1] = m[0][1]-fDot*kQ[0][0];
        kQ[1][1] = m[1][1]-fDot*kQ[1][0];
        kQ[2][1] = m[2][1]-fDot*kQ[2][0];
		fInvLength = kQ[0][1]*kQ[0][1] + kQ[1][1]*kQ[1][1] + kQ[2][1]*kQ[2][1];
		if (fInvLength != 0) fInvLength = rsq(fInvLength);
        
        kQ[0][1] *= fInvLength;
        kQ[1][1] *= fInvLength;
        kQ[2][1] *= fInvLength;

        fDot = kQ[0][0]*m[0][2] + kQ[1][0]*m[1][2] + kQ[2][0]*m[2][2];
        kQ[0][2] = m[0][2]-fDot*kQ[0][0];
        kQ[1][2] = m[1][2]-fDot*kQ[1][0];
        kQ[2][2] = m[2][2]-fDot*kQ[2][0];

        fDot = kQ[0][1]*m[0][2] + kQ[1][1]*m[1][2] + kQ[2][1]*m[2][2];
        kQ[0][2] -= fDot*kQ[0][1];
        kQ[1][2] -= fDot*kQ[1][1];
        kQ[2][2] -= fDot*kQ[2][1];
        fInvLength = kQ[0][2]*kQ[0][2] + kQ[1][2]*kQ[1][2] + kQ[2][2]*kQ[2][2];

		if (fInvLength != 0) fInvLength = rsq(fInvLength);

		kQ[0][2] *= fInvLength;
        kQ[1][2] *= fInvLength;
        kQ[2][2] *= fInvLength;

        float fDet = kQ[0][0]*kQ[1][1]*kQ[2][2] + kQ[0][1]*kQ[1][2]*kQ[2][0] + kQ[0][2]*kQ[1][0]*kQ[2][1] - kQ[0][2]*kQ[1][1]*kQ[2][0] - kQ[0][1]*kQ[1][0]*kQ[2][2] - kQ[0][0]*kQ[1][2]*kQ[2][1];

        if ( fDet < 0.0 ) {
            for (size_t iRow = 0; iRow < 3; iRow++)
                for (size_t iCol = 0; iCol < 3; iCol++)
                    kQ[iRow][iCol] = -kQ[iRow][iCol];
        }


        Matrix3 kR;
        kR[0][0] = kQ[0][0]*m[0][0] + kQ[1][0]*m[1][0] + kQ[2][0]*m[2][0];
        kR[0][1] = kQ[0][0]*m[0][1] + kQ[1][0]*m[1][1] + kQ[2][0]*m[2][1];
        kR[1][1] = kQ[0][1]*m[0][1] + kQ[1][1]*m[1][1] + kQ[2][1]*m[2][1];
        kR[0][2] = kQ[0][0]*m[0][2] + kQ[1][0]*m[1][2] + kQ[2][0]*m[2][2];
        kR[1][2] = kQ[0][1]*m[0][2] + kQ[1][1]*m[1][2] + kQ[2][1]*m[2][2];
        kR[2][2] = kQ[0][2]*m[0][2] + kQ[1][2]*m[1][2] + kQ[2][2]*m[2][2];

        kD.x = kR[0][0];
        kD.y = kR[1][1];
        kD.z = kR[2][2];

        float fInvD0 = 1.0f/kD.x;
        kU.x = kR[0][1]*fInvD0;
        kU.y = kR[0][2]*fInvD0;
        kU.z = kR[1][2]/kD.y;
    }

	void Quaternion::fromXYZInts(int rx, int ry, int rz) {
		float rot_x=rx * INT_TO_RAD;
		float rot_y=ry * INT_TO_RAD;
		float rot_z=rz * INT_TO_RAD;

		Matrix3 mr;
		mr.fromEulerAnglesZYX(rot_z, rot_y, rot_x);
		fromRotationMatrix(mr);
	}

	void Quaternion::fromZXYInts(int rx, int ry, int rz) {
		float rot_x=rx * INT_TO_RAD;
		float rot_y=ry * INT_TO_RAD;
		float rot_z=rz * INT_TO_RAD;

		Matrix3 mr;
		mr.fromEulerAnglesYXZ(rot_z, rot_y, rot_x);
		fromRotationMatrix(mr);
	}

	void Quaternion::fromXZYInts(int rx, int ry, int rz) {
		float rot_x=rx * INT_TO_RAD;
		float rot_y=ry * INT_TO_RAD;
		float rot_z=rz * INT_TO_RAD;

		Matrix3 mr;
		mr.fromEulerAnglesYZX(rot_z, rot_y, rot_x);
		fromRotationMatrix(mr);
	}

	void Quaternion::fromRotationMatrix(const Matrix3& kRot) {
        float fTrace = kRot[0][0]+kRot[1][1]+kRot[2][2];
        float fRoot;

        if (fTrace > 0.0) {
            fRoot = sqrt(fTrace + 1.0f);
            w = 0.5f*fRoot;
            fRoot = 0.5f/fRoot;
            x = (kRot[2][1]-kRot[1][2])*fRoot;
            y = (kRot[0][2]-kRot[2][0])*fRoot;
            z = (kRot[1][0]-kRot[0][1])*fRoot;
        }
        else {
            static size_t s_iNext[3] = { 1, 2, 0 };
            size_t i = 0;
            if ( kRot[1][1] > kRot[0][0] )
                i = 1;
            if ( kRot[2][2] > kRot[i][i] )
                i = 2;
            size_t j = s_iNext[i];
            size_t k = s_iNext[j];

            fRoot = sqrt(kRot[i][i]-kRot[j][j]-kRot[k][k] + 1.0f);
            float* apkQuat[3] = { &x, &y, &z };
            *apkQuat[i] = 0.5f*fRoot;
            fRoot = 0.5f/fRoot;
            w = (kRot[k][j]-kRot[j][k])*fRoot;
            *apkQuat[j] = (kRot[j][i]+kRot[i][j])*fRoot;
            *apkQuat[k] = (kRot[k][i]+kRot[i][k])*fRoot;
        }
    }

    void Quaternion::toRotationMatrix(Matrix3& kRot) {
        float fTx = x+x;
        float fTy = y+y;
        float fTz = z+z;
        float fTwx = fTx*w;
        float fTwy = fTy*w;
        float fTwz = fTz*w;
        float fTxx = fTx*x;
        float fTxy = fTy*x;
        float fTxz = fTz*x;
        float fTyy = fTy*y;
        float fTyz = fTz*y;
        float fTzz = fTz*z;

        kRot[0][0] = 1.0f-(fTyy+fTzz);
        kRot[0][1] = fTxy-fTwz;
        kRot[0][2] = fTxz+fTwy;
        kRot[1][0] = fTxy+fTwz;
        kRot[1][1] = 1.0f-(fTxx+fTzz);
        kRot[1][2] = fTyz-fTwx;
        kRot[2][0] = fTxz-fTwy;
        kRot[2][1] = fTyz+fTwx;
        kRot[2][2] = 1.0f-(fTxx+fTyy);
    }

	void Matrix4::decomposition(Vector3& position, Vector3& scale, Quaternion& orientation) {
		Matrix3 m3x3;
		extract3x3Matrix(m3x3);

		Matrix3 matQ;
		Vector3 vecU;
		m3x3.QDUDecomposition(matQ, scale, vecU);

		orientation = Quaternion(matQ);
		position = Vector3(m[0][3], m[1][3], m[2][3]);
	}

	void Matrix4::makeTransform( Vector3& position, Vector3& scale, Quaternion& orientation) {
        Matrix3 rot3x3;
        orientation.toRotationMatrix(rot3x3);

        m[0][0] = scale.x * rot3x3[0][0]; m[0][1] = scale.y * rot3x3[0][1]; m[0][2] = scale.z * rot3x3[0][2]; m[0][3] = position.x;
        m[1][0] = scale.x * rot3x3[1][0]; m[1][1] = scale.y * rot3x3[1][1]; m[1][2] = scale.z * rot3x3[1][2]; m[1][3] = position.y;
        m[2][0] = scale.x * rot3x3[2][0]; m[2][1] = scale.y * rot3x3[2][1]; m[2][2] = scale.z * rot3x3[2][2]; m[2][3] = position.z;
        m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
    }

	void Matrix4::makeTransform(Vector3& position, Vector3& scale, Matrix3& rot3x3) {

		m[0][0] = scale.x * rot3x3[0][0]; m[0][1] = scale.y * rot3x3[0][1]; m[0][2] = scale.z * rot3x3[0][2]; m[0][3] = position.x;
		m[1][0] = scale.x * rot3x3[1][0]; m[1][1] = scale.y * rot3x3[1][1]; m[1][2] = scale.z * rot3x3[1][2]; m[1][3] = position.y;
		m[2][0] = scale.x * rot3x3[2][0]; m[2][1] = scale.y * rot3x3[2][1]; m[2][2] = scale.z * rot3x3[2][2]; m[2][3] = position.z;
		m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
	}

	void Quaternion::fromEuler(Vector3 euler) {
		Quaternion quatX, quatY, quatZ;
		quatX.fromAngleAxis(euler.x, Vector3(1,0,0));
		quatY.fromAngleAxis(euler.y, Vector3(0,1,0));
		quatZ.fromAngleAxis(euler.z, Vector3(0,0,1));
		*this = quatY * quatX * quatZ;
	}

	void Quaternion::fromEulerDegrees(Vector3 euler) {
		fromEuler(Vector3(euler.x / RAD_TO_DEGREE, euler.y / RAD_TO_DEGREE, euler.z / RAD_TO_DEGREE));
	}

	void Quaternion::fromAngleAxis(float rfAngle, Vector3 rkAxis) {
        float fHalfAngle ( 0.5*rfAngle );
        float fSin = sin(fHalfAngle);
        w = cos(fHalfAngle);
        x = fSin*rkAxis.x;
        y = fSin*rkAxis.y;
        z = fSin*rkAxis.z;
    }

	 Quaternion Quaternion::slerp(float fT, const Quaternion& rkP, const Quaternion& rkQ, bool shortestPath) {
        float fCos = rkP.dot(rkQ);
        Quaternion rkT;

        if (fCos < 0.0f && shortestPath) {
            fCos = -fCos;
            rkT = -rkQ;
        }
        else {
            rkT = rkQ;
        }

        if (abs(fCos) < 1 - EPSION_QUATERNION) {
            float fSin = sqrt(1 - pow(fCos, 2));
            float fAngle = atan2(fSin, fCos);
            float fInvSin = 1.0f / fSin;
            float fCoeff0 = sin((1.0f - fT) * fAngle) * fInvSin;
            float fCoeff1 = sin(fT * fAngle) * fInvSin;
            return (rkP * fCoeff0) + (rkT * fCoeff1);
        }
        else {
            Quaternion t = (rkP * (1.0f - fT))  + (rkT * fT);
            t.normalise();
            return t;
        }
    }

	float Quaternion::getYaw() {
		float fTx  = 2.0f*x;
		float fTy  = 2.0f*y;
		float fTz  = 2.0f*z;
		float fTwy = fTy*w;
		float fTxx = fTx*x;
		float fTxz = fTz*x;
		float fTyy = fTy*y;

		return atan2(fTxz+fTwy, 1.0f-(fTxx+fTyy));
	}

	float Quaternion::getYawDegrees() {
		return getYaw() * RAD_TO_DEGREE;
	}

	Matrix4 convertBoneMatrixToRegular(Matrix4 m) {
		Matrix3 m3;
		m.extract3x3Matrix(m3);
		Vector3 position=Vector3(m[0][3], m[1][3], m[2][3]);
		position = position * m3;
		position = position * (-1);
		Vector3 pos, scal;
		Quaternion rot;
		m.decomposition(pos, scal, rot);
		m.makeTransform(position, scal, rot);
		return m;
	}

	float AABB::centerX() {
		return (start.x+end.x)/2.0f;
	}

	float AABB::centerY() {
		return (start.y+end.y)/2.0f;
	}

	float AABB::centerZ() {
		return (start.z+end.z)/2.0f;
	}

	float AABB::sizeX() {
		return end.x-start.x;
	}

	float AABB::sizeY() {
		return end.y-start.y;
	}

	float AABB::sizeZ() {
		return end.z-start.z;
	}

	float AABB::sizeMax() {
		float max_size=sizeX();
		float temp_size=sizeY();
		if (temp_size > max_size) max_size = temp_size;

		temp_size=sizeZ();
		if (temp_size > max_size) max_size = temp_size;

		return max_size;
	}

	float AABB::size() {
		return sizeX() * sizeY() * sizeZ();
	}

	Vector3 AABB::center() {
		return (start+end)/2.0f;
	}

	Vector3 AABB::corner(int type) {
		switch (type) {
			case 0 :
				return Vector3(start.x, start.y, start.z);
			case 1 :
				return Vector3(start.x, start.y, end.z);
			case 2 :
				return Vector3(start.x, end.y, start.z);
			case 3 :
				return Vector3(start.x, end.y, end.z);
			case 4 :
				return Vector3(end.x, start.y, start.z);
			case 5 :
				return Vector3(end.x, start.y, end.z);
			case 6 :
				return Vector3(end.x, end.y, start.z);
			case 7 :
				return Vector3(end.x, end.y, end.z);
		}

		return Vector3(0, 0, 0);
	}

	AABB AABB::half(int axis, int side) {
		AABB result=*this;

		switch (axis) {
			case MATH_AXIS_X :
				if (side == MATH_SIDE_LEFT) result.end.x = result.centerX();
				else if (side == MATH_SIDE_RIGHT) result.start.x = result.centerX();
				break;
			case MATH_AXIS_Y :
				if (side == MATH_SIDE_LEFT) result.end.y = result.centerY();
				else if (side == MATH_SIDE_RIGHT) result.start.y = result.centerY();
				break;
			case MATH_AXIS_Z :
				if (side == MATH_SIDE_LEFT) result.end.z = result.centerZ();
				else if (side == MATH_SIDE_RIGHT) result.start.z = result.centerZ();
				break;
			default :
				result.start = Vector3();
				result.end = Vector3();
				break;
		}

		return result;
	}

	bool AABB::intersects(const AABB& aabb) {
		if (end.x < aabb.start.x) return false;
		if (end.y < aabb.start.y) return false;
		if (end.z < aabb.start.z) return false;
		if (start.x > aabb.end.x) return false;
		if (start.y > aabb.end.y) return false;
		if (start.z > aabb.end.z) return false;

		return true;
	}

	bool AABB::intersects(const Vector3& v) {
		return ((v.x >= start.x)  &&  (v.x <= end.x)  && (v.y >= start.y)  &&  (v.y <= end.y)  && (v.z >= start.z)  &&  (v.z <= end.z));
	}

	AABB AABB::intersection(const AABB& aabb) {
		AABB inter;
		inter.start.x = max(start.x, aabb.start.x);
		inter.start.y = max(start.y, aabb.start.y);
		inter.start.z = max(start.z, aabb.start.z);

		inter.end.x = min(end.x, aabb.end.x);
		inter.end.y = min(end.y, aabb.end.y);
		inter.end.z = min(end.z, aabb.end.z);

		// No valid intersection was found
		if ((inter.end.x < inter.start.x) || (inter.end.y < inter.start.y) || (inter.end.z < inter.start.z)) {
			inter.start = inter.end = Vector3(0.0, 0.0, 0.0);
		}

		return inter;
	}

	void AABB::merge(const AABB& aabb) {
		if (aabb.start.x < start.x) start.x = aabb.start.x;
		if (aabb.start.y < start.y) start.y = aabb.start.y;
		if (aabb.start.z < start.z) start.z = aabb.start.z;

		if (aabb.end.x > end.x) end.x = aabb.end.x;
		if (aabb.end.y > end.y) end.y = aabb.end.y;
		if (aabb.end.z > end.z) end.z = aabb.end.z;
	}

	int Vector3::relativeCorner(Vector3 &v) {
		if (v.x <= x) {
			if (v.y <= y) {
				if (v.z <= z) return 0;
				else return 1;
			}
			else {
				if (v.z <= z) return 2;
				else return 3;
			}
		}
		else {
			if (v.y <= y) {
				if (v.z <= z) return 4;
				else return 5;
			}
			else {
				if (v.z <= z) return 6;
				else return 7;
			}
		}

		return 0;
	}

	Vector2 Vector3::toBarycentric(Vector3 &a, Vector3 &b, Vector3 &c) {
		Vector2 uv;
	    Vector3 v0 = c - a;
	    Vector3 v1 = b - a;
	    Vector3 v2 = *this - a;

	    float dot00 = v0.dotProduct(v0);
	    float dot01 = v0.dotProduct(v1);
	    float dot02 = v0.dotProduct(v2);
	    float dot11 = v1.dotProduct(v1);
		float dot12 = v1.dotProduct(v2);

		float invDenom = 1.f / ((dot00 * dot11) - (dot01 * dot01));
		uv.x = (dot11 * dot02 - dot01 * dot12) * invDenom;
		uv.y = (dot00 * dot12 - dot01 * dot02) * invDenom;

		return uv;
	}

	bool Vector3::sameSide(Vector3 &p2, Vector3 &a, Vector3 &b) {
		Vector3 cp1 = (b-a).crossProduct(*this-a);
		Vector3 cp2 = (b-a).crossProduct(p2-a);
		if (cp1.dotProduct(cp2) >= 0) return true;
		else return false;
	}

	bool Triangle::intersects(Vector3 &p) {
		if (p.sameSide(point[0], point[1], point[2]) && p.sameSide(point[1], point[0], point[2]) && p.sameSide(point[2], point[0], point[1])) return true;
		else return false;
	}

	void Triangle::generatePoints(std::vector<Vector3> *list, float unit_size) {
		if (degenerate()) return;

		float area=getArea();
		Vector3 cent=center();

		list->push_back(cent);
		
		if ((getArea() < unit_size) || (lengths[0] < unit_size) || (lengths[1] < unit_size) || (lengths[2] < unit_size)) {
			return;
		}
		else {
			size_t step=1;

			while (true) {
				float radius=step*unit_size;
				float perimeter=2*radius*PI;
				float angle=0;
				int circle_count = (int)(perimeter/(unit_size));
				float angle_step=(PI/(float)circle_count)*2;
				size_t circles=0;

				for (int i=0; i<circle_count; i++) {
					Vector3 point=cent + tangent*cos(angle)*radius + bitangent*sin(angle)*radius;
					if (intersects(point)) {
						list->push_back(point);
						circles++;
					}
					angle+=angle_step;
				}

				if (!circles) {
					break;
				}

				step++;
			}
		}
    }
};