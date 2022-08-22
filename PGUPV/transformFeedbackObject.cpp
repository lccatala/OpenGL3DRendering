
#include "transformFeedbackObject.h"


#include <GL/glew.h>

using PGUPV::TransformFeedbackObject;

TransformFeedbackObject::TransformFeedbackObject() {
	glGenTransformFeedbacks(1, &tao);
}

TransformFeedbackObject::~TransformFeedbackObject() {
	glDeleteTransformFeedbacks(1, &tao);
}

void TransformFeedbackObject::bind() {
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tao);
}

void TransformFeedbackObject::unbind() {
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}
