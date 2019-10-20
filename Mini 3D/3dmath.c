//
//  pd3d.c
//  Playdate Simulator
//
//  Created by Dave Hayden on 8/25/15.
//  Copyright (c) 2015 Panic, Inc. All rights reserved.
//

#include <string.h>
#include "3dmath.h"

#if TARGET_PLAYDATE
  #define USE_CMSIS_DSP 1
  #define __FPU_PRESENT 1
  #define ARM_MATH_CM7
  #include <stdint.h>
  #include "arm_math.h"
#endif

Matrix3D identityMatrix = { .isIdentity = 1, .m = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}, .dx = 0, .dy = 0, .dz = 0 };

Matrix3D Matrix3DMake(float m11, float m12, float m13, float m21, float m22, float m23, float m31, float m32, float m33)
{
	return (Matrix3D){ .isIdentity = 0, .m = {{m11, m12, m13}, {m21, m22, m23}, {m31, m32, m33}}, .dx = 0, .dy = 0, .dz = 0 };
}

Matrix3D Matrix3D_multiply(Matrix3D l, Matrix3D r)
{
	Matrix3D m = { .isIdentity = 0 };
	
	if ( l.isIdentity )
	{
		if ( r.isIdentity )
			m = identityMatrix;
		else
			memcpy(&m.m, &r.m, sizeof(r.m));
		
		m.dx = l.dx + r.dx;
		m.dy = l.dy + r.dy;
		m.dz = l.dz + r.dz;
	}
	else
	{
		if ( !r.isIdentity )
		{
#if USE_CMSIS_DSP
			arm_matrix_instance_f32 m1 = { 3, 3, &l.m[0][0] };
			arm_matrix_instance_f32 m2 = { 3, 3, &r.m[0][0] };
			
			arm_matrix_instance_f32 dst = { 3, 3, &m.m[0][0] };
			
			arm_mat_mult_f32(&m1, &m2, &dst);
#else
			m.m[0][0] = l.m[0][0] * r.m[0][0] + l.m[1][0] * r.m[0][1] + l.m[2][0] * r.m[0][2];
			m.m[1][0] = l.m[0][0] * r.m[1][0] + l.m[1][0] * r.m[1][1] + l.m[2][0] * r.m[1][2];
			m.m[2][0] = l.m[0][0] * r.m[2][0] + l.m[1][0] * r.m[2][1] + l.m[2][0] * r.m[2][2];

			m.m[0][1] = l.m[0][1] * r.m[0][0] + l.m[1][1] * r.m[0][1] + l.m[2][1] * r.m[0][2];
			m.m[1][1] = l.m[0][1] * r.m[1][0] + l.m[1][1] * r.m[1][1] + l.m[2][1] * r.m[1][2];
			m.m[2][1] = l.m[0][1] * r.m[2][0] + l.m[1][1] * r.m[2][1] + l.m[2][1] * r.m[2][2];

			m.m[0][2] = l.m[0][2] * r.m[0][0] + l.m[1][2] * r.m[0][1] + l.m[2][2] * r.m[0][2];
			m.m[1][2] = l.m[0][2] * r.m[1][0] + l.m[1][2] * r.m[1][1] + l.m[2][2] * r.m[1][2];
			m.m[2][2] = l.m[0][2] * r.m[2][0] + l.m[1][2] * r.m[2][1] + l.m[2][2] * r.m[2][2];
#endif
		}
		else
			memcpy(&m.m, &l.m, sizeof(l.m));

#if USE_CMSIS_DSP
		arm_matrix_instance_f32 m1 = { 3, 3, &l.m[0][0] };
		arm_matrix_instance_f32 m2 = { 3, 1, &r.dx };
		arm_matrix_instance_f32 m3 = { 3, 1, &l.dx };

		arm_matrix_instance_f32 dst = { 3, 1, &m.dx };
		
		arm_mat_mult_f32(&m1, &m2, &dst);
		arm_mat_add_f32(&dst, &m3, &dst);
#else
		m.dx = l.m[0][0] * r.dx + l.m[1][0] * r.dy + l.m[2][0] * r.dz + l.dx;
		m.dy = l.m[0][1] * r.dx + l.m[1][1] * r.dy + l.m[2][1] * r.dz + l.dy;
		m.dz = l.m[0][2] * r.dx + l.m[1][2] * r.dy + l.m[2][2] * r.dz + l.dz;
#endif
	}
	
	return m;
}

Point3D Matrix3D_apply(Matrix3D m, Point3D p)
{
	if ( m.isIdentity )
		return Point3DMake(p.x + m.dx, p.y + m.dy, p.z + m.dz);

#if USE_CMSIS_DSP
	float f[3];
	
	arm_matrix_instance_f32 m1 = { 3, 3, &m.m[0][0] };
	arm_matrix_instance_f32 m2 = { 3, 1, &p.x };
	arm_matrix_instance_f32 m3 = { 3, 1, &m.dx };
	
	arm_matrix_instance_f32 dst = { 3, 1, &f[0] };
	
	arm_mat_mult_f32(&m1, &m2, &dst);
	arm_mat_add_f32(&dst, &m3, &dst);

	return Point3DMake(f[0], f[1], f[2]);
#else
	float x = m.m[0][0] * p.x + m.m[1][0] * p.y + m.m[2][0] * p.z + m.dx;
	float y = m.m[0][1] * p.x + m.m[1][1] * p.y + m.m[2][1] * p.z + m.dy;
	float z = m.m[0][2] * p.x + m.m[1][2] * p.y + m.m[2][2] * p.z + m.dz;
	
	return Point3DMake(x, y, z);
#endif
}

Vector3D Vector3D_normalize(Vector3D v)
{
	float d = fisr(v.dx * v.dx + v.dy * v.dy + v.dz * v.dz);
	
	return Vector3DMake(v.dx * d, v.dy * d, v.dz * d);
}

Vector3D Vector3DMakeNormal(Point3D* p1, Point3D* p2, Point3D* p3)
{
	Vector3D v = Vector3DCross(Vector3DMake(p2->x - p1->x, p2->y - p1->y, p2->z - p1->z),
							   Vector3DMake(p3->x - p1->x, p3->y - p1->y, p3->z - p1->z));
	
	return Vector3D_normalize(v);
}
