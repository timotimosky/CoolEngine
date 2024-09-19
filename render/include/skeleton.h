#pragma once
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macro.h"
#include "private.h"
#include "maths.h"

typedef struct {
	int joint_index;
	int parent_index;
	mat4_t inverse_bind;
	/* translations */
	int num_translations;
	float* translation_times;
	vec3_t* translation_values;
	/* rotations */
	int num_rotations;
	float* rotation_times;
	quat_t* rotation_values;
	/* scales */
	int num_scales;
	float* scale_times;
	vec3_t* scale_values;
	/* interpolated */
	mat4_t transform;
} joint_t;

struct skeleton {
	float min_time;
	float max_time;
	int num_joints;
	joint_t* joints;
	/* cached result */
	mat4_t* joint_matrices;
	mat3_t* normal_matrices;
	float last_time;
};

typedef struct skeleton skeleton_t;

/* skeleton loading/releasing */
skeleton_t *skeleton_load(const char *filename);
//void skeleton_release(skeleton_t *skeleton);
//
///* joint updating/retrieving */
//void skeleton_update_joints(skeleton_t *skeleton, float frame_time);
//mat4_t *skeleton_get_joint_matrices(skeleton_t *skeleton);
//mat3_t *skeleton_get_normal_matrices(skeleton_t *skeleton);

