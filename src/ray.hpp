
class Ray {

public:
	Eigen::Vector3f origin;
	Eigen::Vector3f direction;

	Ray(Eigen::Vector3f origin, Eigen::Vector3f direction) {
		this.origin = origin;
		this.direction = direction;
	}

	Eigen::Vector3f getPoint(float t);

}