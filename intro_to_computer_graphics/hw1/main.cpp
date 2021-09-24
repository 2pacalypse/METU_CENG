/*
 *
 * q + su - sv formülünü düzelt
 *
 */

#include <iostream>
#include "parser.h"
#include "ppm.h"
#include <climits>
#include <cmath>
#include <utility>
typedef unsigned char RGB[3];


parser::Scene scene;

void mesh_ray(
		const parser::Mesh& mesh,
		const parser::Vec3f& o,
		const parser::Vec3f& d,
		int threshold_min,
		int threshold_max,
		float& t_min,
		int& m_min,
		parser::Vec3f& normal_min){

	for(const parser::Face& face: mesh.faces){
		parser::Vec3f normal, vec1, vec2;

		//check if the ray intersects the triangle's plane
		vec1 = scene.vertex_data[face.v2_id -1] - scene.vertex_data[face.v1_id - 1];
		vec2 = scene.vertex_data[face.v0_id -1] - scene.vertex_data[face.v1_id - 1];
		normal = vec1.crossProduct(vec2);
		float intersection = normal*(scene.vertex_data[face.v0_id - 1] - o)/(d*normal);
		if(intersection <= threshold_min || intersection > threshold_max)
			continue;


		//check if the intersection point is really inside the triangle
		parser::Vec3f p = o + d*intersection;
		//edge 0
		parser::Vec3f C = (p - scene.vertex_data[face.v2_id - 1] ).crossProduct(scene.vertex_data[face.v1_id - 1] -scene.vertex_data[face.v2_id - 1]);
		if(normal*C < 0 )
			continue;
		//edge 1
		parser::Vec3f C1 = (p - scene.vertex_data[face.v0_id - 1]).crossProduct(scene.vertex_data[face.v2_id - 1] -scene.vertex_data[face.v0_id - 1]);
		if(normal*C1 < 0 )
			continue;
		//edge2
		parser::Vec3f C2 = (p - scene.vertex_data[face.v1_id - 1]).crossProduct(scene.vertex_data[face.v0_id - 1] -scene.vertex_data[face.v1_id - 1]);
		if(normal*C2 < 0 )
			continue;

		if(intersection < t_min ){
			t_min = intersection;
			m_min = mesh.material_id;
			normal_min = normal;
		}
	 }
}


void triangle_ray(
		const parser::Triangle& triangle,
		const parser::Vec3f& o,
		const parser::Vec3f& d,
		int threshold_min,
		int threshold_max,
		float& t_min,
		int& m_min,
		parser::Vec3f& normal_min){

	//check if the ray intersects the triangle's plane
	parser::Vec3f normal, vec1, vec2;
	vec1 = scene.vertex_data[triangle.indices.v2_id -1] - scene.vertex_data[triangle.indices.v1_id - 1];
	vec2 = scene.vertex_data[triangle.indices.v0_id -1] - scene.vertex_data[triangle.indices.v1_id - 1];
	normal = vec1.crossProduct(vec2);
	float intersection = normal*(scene.vertex_data[triangle.indices.v0_id - 1] - o)/(d*normal);
	if(intersection <= threshold_min || intersection > threshold_max)
		return;

	//check if the intersection point is really inside the triangle
	parser::Vec3f p = o + d*intersection;

	//edge 0
	parser::Vec3f C = (p - scene.vertex_data[triangle.indices.v2_id - 1] ).crossProduct(scene.vertex_data[triangle.indices.v1_id - 1] -scene.vertex_data[triangle.indices.v2_id - 1]);
	if(normal*C < 0 )
		return;
	//edge 1
	parser::Vec3f C1 = (p - scene.vertex_data[triangle.indices.v0_id - 1]).crossProduct(scene.vertex_data[triangle.indices.v2_id - 1] -scene.vertex_data[triangle.indices.v0_id - 1]);
	if(normal*C1 < 0 )
		return;
	//edge2
	parser::Vec3f C2 = (p - scene.vertex_data[triangle.indices.v1_id - 1]).crossProduct(scene.vertex_data[triangle.indices.v0_id - 1] -scene.vertex_data[triangle.indices.v1_id - 1]);
	if(normal*C2 < 0 )
		return;

	if(intersection < t_min){
		t_min = intersection;
		m_min = triangle.material_id;
		normal_min = normal;
	}
}


void ray_sphere(
		const parser::Sphere& sphere,
		const parser::Vec3f& o,
		const parser::Vec3f& d,
		int threshold_min,
		float& t_min,
		int& m_min,
		parser::Vec3f& normal_min){

	 parser::Vec3f c = scene.vertex_data[sphere.center_vertex_id - 1]; //center coords of the sphere

	 float r = sphere.radius;

	 float delta = pow(2 * ((d)*(o-c)), 2) - 4*(d*d)*( ((o-c)*(o-c)) - r*r);


	 if(delta > 0){

		 float r1, r2;

		 r1 = (2*(d*(c-o)) + sqrt(delta))/(2*(d*d));
		 r2 = (2*(d*(c-o)) - sqrt(delta))/(2*(d*d));


		 if(r1 < t_min && r1 > threshold_min ){
			 parser::Vec3f p = o + d*r1;
			 normal_min = p - c;
			 t_min = r1;
			 m_min = sphere.material_id;
		 }
		 if(r2 < t_min && r2 > threshold_min ){
			 parser::Vec3f p = o + d*r2;
			 normal_min = p - c;
			 t_min = r2;
			 m_min = sphere.material_id;
		 }
	 }
}


parser::Vec3f get_color(parser::Vec3f o, parser::Vec3f d, int bounce_count){
	 float t_min = INT_MAX;
	 int m_min = INT_MAX;
	 parser::Vec3f normal_min;

 //handle spheres
  for(const parser::Sphere& sphere: scene.spheres){
	  ray_sphere(sphere, o, d, 0, t_min, m_min, normal_min);
 }

 //handle triangles
 for(const parser::Triangle& triangle: scene.triangles){
	 triangle_ray(triangle, o, d, 0, INT_MAX, t_min, m_min, normal_min);
 }

 //handle meshes
 for(const parser::Mesh& mesh: scene.meshes){
	 mesh_ray(mesh, o, d, 0, INT_MAX, t_min, m_min, normal_min);
 }

 float red, green, blue;
 red = green = blue = 0;


// 5 am in the mornin
// still breathin

 //obj not NULL
 if(t_min != INT_MAX){
	 parser::Vec3f p_intersect = o + (d*t_min);


	 //ambient shading
	 red = scene.ambient_light.x * (scene.materials[m_min - 1].ambient.x);
	 green = scene.ambient_light.y * (scene.materials[m_min - 1].ambient.y);
	 blue = scene.ambient_light.z * (scene.materials[m_min - 1].ambient.z);




	 for(const parser::PointLight& pointlight : scene.point_lights){

		 parser::Vec3f light_pos = pointlight.position;

		 parser::Vec3f a = light_pos-p_intersect;     // c + a.t
		 parser::Vec3f b = light_pos;
		 parser::Vec3f c = p_intersect;

		 float t_light = ((b-c)*a)/(a*a);


		 int material;
		 int flag = 1;
		 for(const parser::Sphere& sphere : scene.spheres){

			 float intersect = INT_MAX;
			 parser::Vec3f z;

			 ray_sphere(sphere, c + a*scene.shadow_ray_epsilon, a, 0, intersect, material,z);

			 if( intersect < t_light){
				 flag = 0;
				 break;
			 }
		 }

		 for(const parser::Triangle& triangle : scene.triangles){
			 float intersect = INT_MAX;
			 parser::Vec3f dummy;
			 triangle_ray(triangle, c + a*scene.shadow_ray_epsilon, a, 0,INT_MAX, intersect, material, dummy);

			 if(intersect < t_light){
				 flag = 0;
				 break;
			 }
		 }

		 for(const parser::Mesh& mesh : scene.meshes){
			 float intersect = INT_MAX;
			 parser::Vec3f dummy;
			 mesh_ray(mesh, c + a*scene.shadow_ray_epsilon, a , 0,INT_MAX, intersect, material, dummy);

			 if(intersect < t_light){
				 flag = 0;
				 break;
			 }
		 }

		 if(flag == 1){
			 float dist = p_intersect.distance(light_pos);

			 // diffuse shading
			 float d_costheta = std::max((float) 0, (float)(((b-c)*normal_min)/ sqrt( ((b-c)*(b-c))*(normal_min * normal_min)) )) ;
			 red += pointlight.intensity.x  * scene.materials[m_min - 1].diffuse.x  * d_costheta/ dist;
			 green += pointlight.intensity.y  * scene.materials[m_min - 1].diffuse.y * d_costheta/ dist;
			 blue += pointlight.intensity.z * scene.materials[m_min - 1].diffuse.z  * d_costheta/ dist;


			 //specular shading
			 parser::Vec3f l = (light_pos - p_intersect);
			 l = l/sqrt(l*l);
			 parser::Vec3f v = (o - p_intersect);
			 v = v/sqrt(v*v);
			 parser::Vec3f half =  (l + v)/sqrt((l+v)*(l+v));

			 parser::Vec3f n0 = normal_min/sqrt(normal_min*normal_min);

			 float s_costheta = pow((std::max((float) 0, (float) (((half)*n0)/ sqrt( ((half)*(half)) * (n0 * n0)) ))), scene.materials[m_min - 1].phong_exponent);
			 red += pointlight.intensity.x  * scene.materials[m_min - 1].specular.x * s_costheta/ dist;
			 green += pointlight.intensity.y  * scene.materials[m_min - 1].specular.y * s_costheta/ dist;
			 blue += pointlight.intensity.z  * scene.materials[m_min - 1].specular.z * s_costheta/ dist;

		 }

	 }
	 if(scene.materials[m_min - 1].mirror != parser::Vec3f{0,0,0}){
		 // reflect it
		 if(bounce_count < scene.max_recursion_depth){
			 parser::Vec3f w0 = o - p_intersect;
			 w0 = w0/sqrt(w0*w0);
			 parser::Vec3f n0 = normal_min/sqrt(normal_min*normal_min);
			 parser::Vec3f wr =   n0*2*(n0*w0) -w0;
			 wr = (wr)/sqrt(wr*wr);

			 //recursive
			 parser::Vec3f col = get_color(p_intersect + wr*scene.shadow_ray_epsilon, wr, bounce_count  + 1);

			 red += scene.materials[m_min - 1].mirror.x * col.x;
			 green += scene.materials[m_min - 1].mirror.y * col.y;
			 blue += scene.materials[m_min - 1].mirror.z * col.z;
		 }

	 }
	 if(red > 255)
		 red = 255;
	 if(green > 255)
		 green = 255;
	 if(blue > 255)
		 blue = 255;
}else{
	// no object hit, set to bg color
	red = scene.background_color.x;
	green = scene.background_color.y;
	blue = scene.background_color.z;
}
 return parser::Vec3f{red,green,blue};
}



int main(int argc, char* argv[])
{
	scene.loadFromXml(argv[1]);


	int number_of_cameras = scene.cameras.size();
	parser::Vec3i scene_bg_color = scene.background_color;

	for(const parser::Camera& camera : scene.cameras){
		int width = camera.image_width;
		int height = camera.image_height;

		unsigned char* image =  new unsigned char[width*height*3];
		int ip = 0;

		parser::Vec4f image_plane = camera.near_plane; // left right bottom top, xyzw

		for(int j = 0; j < height; j++){
			for(int i = 0; i < width; i++){

				parser::Vec3f crossprod = camera.up.crossProduct(-camera.gaze);

				 float s_u = (image_plane.y - image_plane.x) * (i + 0.5) / width;
				 float s_v =  (image_plane.w - image_plane.z) * (j + 0.5) / height;


				 parser::Vec3f o = camera.position;
				 parser::Vec3f m = o + camera.gaze* camera.near_distance;
				 parser::Vec3f q = m  + crossprod* image_plane.x  + camera.up * image_plane.w;
				 parser::Vec3f s = q + crossprod * s_u - camera.up* s_v;
				 parser::Vec3f d = s - o; // ray vector in form o + d.t


				 parser::Vec3f color = get_color(o,d, 0);

					 image[ip++] = color.x;
					 image[ip++] = color.y;
					 image[ip++] = color.z;


			}
			
		}

		write_ppm((camera.image_name).c_str(), image, width, height);
		delete[] image;
	}





	/*
    // Sample usage for reading an XML scene file
    parser::Scene scene;

    scene.loadFromXml(argv[1]);

    // The code below creates a test pattern and writes
    // it to a PPM file to demonstrate the usage of the
    // ppm_write function.

    const RGB BAR_COLOR[8] =
    {
        { 255, 255, 255 },  // 100% White
        { 255, 255,   0 },  // Yellow
        {   0, 255, 255 },  // Cyan
        {   0, 255,   0 },  // Green
        { 255,   0, 255 },  // Magenta
        { 255,   0,   0 },  // Red
        {   0,   0, 255 },  // Blue
        {   0,   0,   0 },  // Black
    };

    int width = 640, height = 480;
    int columnWidth = width / 8;

    unsigned char* image = new unsigned char [width * height * 3];

    int i = 0;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int colIdx = x / columnWidth;
            image[i++] = BAR_COLOR[colIdx][0];
            image[i++] = BAR_COLOR[colIdx][1];
            image[i++] = BAR_COLOR[colIdx][2];
        }
    }

    write_ppm(argv[2], image, width, height);
*/
}
