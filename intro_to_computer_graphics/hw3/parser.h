#ifndef __HW1__PARSER__
#define __HW1__PARSER__

#include <string>
#include <vector>
#include <math.h>

namespace parser
{

	

    //Notice that all the structures are as simple as possible
    //so that you are not enforced to adopt any style or design.
    struct Vec3f
    {
        float x, y, z;

		inline Vec3f operator-() const{
			return Vec3f{ -x, -y, -z };
		}

		inline Vec3f operator-(const Vec3f& rhs) const{
			return Vec3f{ x - rhs.x, y - rhs.y, z - rhs.z };
		}

		inline Vec3f operator+(const Vec3f& rhs) const{
			return Vec3f{ x + rhs.x, y + rhs.y, z + rhs.z };
		}
		inline float operator*(const Vec3f& rhs) const{
			return x * rhs.x + y * rhs.y + z * rhs.z;
		}
		inline Vec3f operator*(const float& rhs) const{
			return Vec3f{ rhs*x, rhs*y, rhs*z };
		}

		inline Vec3f crossProduct(const Vec3f&rhs) const{
			return Vec3f{ y*rhs.z - z*rhs.y, -x*rhs.z + z*rhs.x, x*rhs.y - y*rhs.x };
		}


		inline float distance(const Vec3f&rhs) const{
			return (x - rhs.x)*(x - rhs.x) + (y - rhs.y)*(y - rhs.y) + (z - rhs.z)*(z - rhs.z);
		}

		inline Vec3f operator+(const float& rhs){
			return Vec3f{ x + rhs, y + rhs, z + rhs };
		}

		inline Vec3f operator/ (const float& rhs){
			return Vec3f{ x / rhs, y / rhs, z / rhs };
		}

		inline bool operator!=(const Vec3f& rhs){
			return x != rhs.x || y != rhs.y || z != rhs.z;
		}


    };

    struct Vec3i
    {
        int x, y, z;
    };

    struct Vec4f
    {
        float x, y, z, w;
    };

    struct Camera
    {
        Vec3f position;
        Vec3f gaze;
        Vec3f up;
        Vec4f near_plane;
        float near_distance;
        float far_distance;
        int image_width, image_height;
    };

    struct PointLight
    {
        Vec3f position;
        Vec3f intensity;
        bool status;
    };

    struct Material
    {
        Vec3f ambient;
        Vec3f diffuse;
        Vec3f specular;
        float phong_exponent;
    };

    struct Transformation
    {
        std::string transformation_type;
        int id;
    };

    struct Face
    {
        int v0_id;
        int v1_id;
        int v2_id;
    };

    struct Mesh
    {
        int material_id;
        std::vector<Face> faces;
        std::vector<Transformation> transformations;
        std::string mesh_type;
    };

    struct Scene
    {
        //Data
        Vec3i background_color;
        Camera camera;
        Vec3f ambient_light;
        std::vector<PointLight> point_lights;
        std::vector<Material> materials;
        std::vector<Vec3f> vertex_data;
        std::vector<Vec3f> translations;
        std::vector<Vec3f> scalings;
        std::vector<Vec4f> rotations;
        std::vector<Mesh> meshes;

        //Functions
        void loadFromXml(const std::string& filepath);
    };
}

#endif