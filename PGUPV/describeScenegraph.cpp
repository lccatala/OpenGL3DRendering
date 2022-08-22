#include "describeScenegraph.h"
#include <iomanip>

using PGUPV::DescribeScenegraph;
using PGUPV::Group;
using PGUPV::Geode;
using PGUPV::Group;
using PGUPV::Transform;



void DescribeScenegraph::apply(Group &group) {
	os << std::string(currentLevel * spacesPerIndent, ' ');
	os << "Group (" << group.getName() << ") BB: " << group.getBB();
	os << " " << group.getNumChildren() << " hijos\n";
	++currentLevel;
	traverse(group);
	--currentLevel;
}

void DescribeScenegraph::apply(Transform &transform) {
	auto indent = std::string(currentLevel * spacesPerIndent, ' ');
	os << indent;
	os << "Transform (" << transform.getName() << ") BB: " << transform.getBB();
	os << " " << transform.getNumChildren() << " hijos \n";
	auto m = transform.getTransform();
	os << indent << "Mat: [\n";
	os << indent 
#ifndef _DEBUG
		<< std::fixed << std::setprecision(3)
#endif
		<< m[0][0] << " " << m[1][0] << " " << m[2][0] << " " << m[3][0] << "\n"
		<< indent << m[0][1] << " " << m[1][1] << " " << m[2][1] << " " << m[3][1] << "\n"
		<< indent << m[0][2] << " " << m[1][2] << " " << m[2][2] << " " << m[3][2] << "\n"
		<< indent << m[0][3] << " " << m[1][3] << " " << m[2][3] << " " << m[3][3] << "]\n";

	++currentLevel;
	traverse(transform);
	--currentLevel;
}

void DescribeScenegraph::apply(Geode &geode) {
	std::string indent(currentLevel * spacesPerIndent, ' ');
	os << indent;
	os << "Geode (" << geode.getName() << ") BB: " << geode.getBB();
	os << " " << geode.getModel().to_string() << "\n";
}
