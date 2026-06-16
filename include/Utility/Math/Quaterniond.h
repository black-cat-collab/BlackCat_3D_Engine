#pragma once

namespace bc
{
    class Quaterniond
    {
	public:
#ifdef __GNUC__
#pragma pack (32)
		struct
		{
			float w, x, y, z;
		};

		//__m128  _sse;
#pragma pack ()
#else			
		union __declspec(align(32))
		{
			struct
			{
				double w, x, y, z;
			};

			__m256  _sse;
		};
#endif		

    public:
        inline Quaterniond(double fW = 1.0f, double fX = 0.0f, double fY = 0.0f, double fZ = 0.0f)
		{
			w = fW;
			x = fX;
			y = fY;
			z = fZ;
		}
		inline bool ZeroRotation() const { return x == 0.0f && y == 0.0f &&x == 0.0f && w == 1.0f; }

        /// Construct a quaternion from a rotation matrix
        inline Quaterniond(const Matrix3d& rot)
        {
            this->FromRotationMatrix(rot);
        }

		double GetAngles(const  Quaterniond& q);
        /// Construct a quaternion from an angle/axis
        inline Quaterniond(const Radiand& rfAngle, const Vector3d& rkAxis)
        {
            this->FromAngleAxis(rfAngle, rkAxis);
        }
        /// Construct a quaternion from 3 orthonormal local axes
        inline Quaterniond(const Vector3d& xaxis, const Vector3d& yaxis, const Vector3d& zaxis)
        {
            this->FromAxes(xaxis, yaxis, zaxis);
        }
        /// Construct a quaternion from 3 orthonormal local axes
        inline Quaterniond(const Vector3d* akAxis)
        {
            this->FromAxes(akAxis);
        }
		/// Construct a quaternion from 4 manual w/x/y/z values
		inline Quaterniond(double* pData)
		{
			memcpy(&w, pData, sizeof(double) * 4);
		}

		/// Array accessor operator
		inline const double operator [] (const size_t& i) const
		{
			assert( i < 4 );

			return *(&w+i);
		}

		/// Array accessor operator
		inline double& operator [] (const size_t& i)
		{
		
			assert( i < 4 );
			
			return (double&)*(&w+i);
		}

		/// Pointer accessor for direct copying
		inline double* Ptr()
		{
			return (double*)&w;
		}

		/// Pointer accessor for direct copying
		inline const double* Ptr() const
		{
			return (double*)&w;
		}

		void FromRotationMatrix (const Matrix3d& kRot);
        void ToRotationMatrix (Matrix3d& kRot) const;
		void FromAxis (const Axisd& vAxis );
	    Axisd ToAxis () const;
        void FromAngleAxis (const Radiand& rfAngle, const Vector3d& rkAxis);
        void ToAngleAxis (Radiand& rfAngle, Vector3d& rkAxis) const;
        inline void ToAngleAxis (Degreed& dAngle, Vector3d& rkAxis) const 
        {
            Radiand rAngle;
            ToAngleAxis ( rAngle, rkAxis );
            dAngle = rAngle;
        }
        void FromAxes (const Vector3d* akAxis);
        void FromAxes (const Vector3d& xAxis, const Vector3d& yAxis, const Vector3d& zAxis);
        void ToAxes (Vector3d* akAxis) const;
        void ToAxes (Vector3d& xAxis, Vector3d& yAxis, Vector3d& zAxis) const;
        /// Get the local x-axis
        Vector3d xAxis(void) const;
        /// Get the local y-axis
        Vector3d yAxis(void) const;
        /// Get the local z-axis
        Vector3d zAxis(void) const;

        inline Quaterniond& operator= (const Quaterniond& rkQ)
		{
			w = rkQ.w;
			x = rkQ.x;
			y = rkQ.y;
			z = rkQ.z;
			return *this;
		}
        Quaterniond operator+ (const Quaterniond& rkQ) const;
        Quaterniond operator- (const Quaterniond& rkQ) const;
        Quaterniond operator* (const Quaterniond& rkQ) const;
        Quaterniond operator* (double fScalar) const;
        friend Quaterniond operator* (double fScalar, const Quaterniond& rkQ);
        Quaterniond operator- () const;
        inline bool operator== (const Quaterniond& rhs) const
		{
			return (rhs.x == x) && (rhs.y == y) &&
				(rhs.z == z) && (rhs.w == w);
		}
        inline bool operator!= (const Quaterniond& rhs) const
		{
			return !operator==(rhs);
		}
        // functions of a quaternion
        double Dot (const Quaterniond& rkQ) const;  // dot product
        double SquaredLength() const;  // squared-length
        /// Normalises this quaternion, and returns the previous length
        double Normalise(void); 
        Quaterniond Inverse () const;  // apply to non-zero quaternion
        Quaterniond UnitInverse () const;  // apply to unit-length quaternion
        Quaterniond Exp () const;
        Quaterniond Log () const;

        // rotation of a vector by a quaternion
        Vector3d operator* (const Vector3d& rkVector) const;

		Vector3d GetAngles() const;
	

   		/** Calculate the local roll element of this quaternion.
		@param reprojectAxis By default the method returns the 'intuitive' result
			that is, if you projected the local Y of the quaternion onto the X and
			Y axes, the angle between them is returned. If set to false though, the
			result is the actual yaw that will be used to implement the quaternion,
			which is the shortest possible path to get to the same orientation and 
			may involve less axial rotation. 
		*/
		Radiand GetRoll(bool reprojectAxis = true) const;
   		/** Calculate the local pitch element of this quaternion
		@param reprojectAxis By default the method returns the 'intuitive' result
			that is, if you projected the local Z of the quaternion onto the X and
			Y axes, the angle between them is returned. If set to true though, the
			result is the actual yaw that will be used to implement the quaternion,
			which is the shortest possible path to get to the same orientation and 
			may involve less axial rotation. 
		*/
		Radiand GetPitch(bool reprojectAxis = true) const;
   		/** Calculate the local yaw element of this quaternion
		@param reprojectAxis By default the method returns the 'intuitive' result
			that is, if you projected the local Z of the quaternion onto the X and
			Z axes, the angle between them is returned. If set to true though, the
			result is the actual yaw that will be used to implement the quaternion,
			which is the shortest possible path to get to the same orientation and 
			may involve less axial rotation. 
		*/
		Radiand GetYaw(bool reprojectAxis = true) const;		
		/// Equality with tolerance (tolerance is max angle difference)
		bool Equals(const Quaterniond& rhs, const Radiand& tolerance) const;
		
	    // spherical linear interpolation
        static Quaterniond Slerp (double fT, const Quaterniond& rkP,
            const Quaterniond& rkQ, bool shortestPath = false);

        static Quaterniond SlerpExtraSpins (double fT,
            const Quaterniond& rkP, const Quaterniond& rkQ,
            int iExtraSpins);

        // setup for spherical quadratic interpolation
        static void Intermediate (const Quaterniond& rkQ0,
            const Quaterniond& rkQ1, const Quaterniond& rkQ2,
            Quaterniond& rka, Quaterniond& rkB);

        // spherical quadratic interpolation
        static Quaterniond Squad(double fT, const Quaterniond& rkP,
            const Quaterniond& rkA, const Quaterniond& rkB,
            const Quaterniond& rkQ, bool shortestPath = false);

        // normalised linear interpolation - faster but less accurate (non-constant rotation velocity)
        static Quaterniond Nlerp(double fT, const Quaterniond& rkP,
            const Quaterniond& rkQ, bool shortestPath = false);

        // cutoff for sine near zero
        static const double ms_fEpsilon;

        // special values
        static const Quaterniond ZERO;
        static const Quaterniond IDENTITY;
    };
#ifdef _MSC_VER

	static_assert(sizeof(Quaterniond) == (4 * sizeof(double)), "Error size of Quaterniond");

#endif


}

