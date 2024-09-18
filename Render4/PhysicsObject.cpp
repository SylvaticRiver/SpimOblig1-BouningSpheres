#include "PhysicsObject.h"

PhysicsObject::PhysicsObject()
{

}

PhysicsObject::PhysicsObject(Mesh& mesh, glm::vec3 startPos, glm::vec3 size, Rotator rotation, Shader& shaderProgram, bool hasGravity, float mass)
{
	this->mesh = mesh;
	this->currPos = startPos;
	this->size = size;
	this->rotation = rotation;
	this->velocity = glm::vec3(0, 0, 0);
	this->hasGravity = hasGravity;
	this->mass = mass;

	this->mesh.VAO.Bind();
	glm::mat4 objectModel = glm::mat4(1.0f);
	objectModel = glm::scale(glm::translate(objectModel, this->currPos), this->size);
	objectModel = glm::rotate(objectModel, glm::radians(this->rotation.amount), this->rotation.Axis);

	shaderProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(objectModel));

	this->model = objectModel;
}

void PhysicsObject::updateMovement()
{
	if (hasGravity && this->velocity.y > -0.04) {
		this->velocity += glm::vec3(0, -0.0004, 0);
	}
	Object::updateMovement();
}

bool PhysicsObject::isColliding(Object object)
{
	for (Triangle t : object.getTriangles()) {
		Vertex v1 = t.verticies[0];
		Vertex v2 = t.verticies[1];
		Vertex v3 = t.verticies[2];
		glm::vec3 TriPos = glm::vec3((v1.pos + v2.pos).x / 2, (v1.pos + v2.pos).y / 2, (v1.pos + v2.pos).z / 2);
		TriPos = glm::vec3((TriPos + v3.pos).x / 2, (TriPos + v3.pos).y / 2, (TriPos + v3.pos).z / 2);

		glm::vec3 normal = glm::cross(v2.pos - v1.pos, v3.pos - v1.pos);

		glm::vec3 y = glm::vec3(this->currPos.x - ((TriPos.x * object.size.x) + object.currPos.x), this->currPos.y - ((TriPos.y * object.size.y) + object.currPos.y), this->currPos.z - ((TriPos.z * object.size.z) + object.currPos.z));
		float r = (glm::normalize(this->mesh.vertices[0].pos + this->mesh.vertices[1].pos) * this->size).x + 0.1;
		float f = glm::l1Norm((y * normal));
		if (f <= r) {
			this->lastCollided = t;
			return true;
		}
	}
	return false;
}

void PhysicsObject::onCollisionWall(Object object)
{	
	Triangle tri = this->lastCollided;
		Vertex v1 = tri.verticies[0];
		Vertex v2 = tri.verticies[1];
		Vertex v3 = tri.verticies[2];
		glm::vec3 TriPos = glm::vec3((v1.pos + v2.pos).x / 2, (v1.pos + v2.pos).y / 2, (v1.pos + v2.pos).z / 2);
		TriPos = glm::vec3((TriPos + v3.pos).x / 2, (TriPos + v3.pos).y / 2, (TriPos + v3.pos).z / 2);

		glm::vec3 normal = glm::cross(v2.pos - v1.pos, v3.pos - v1.pos);
		float r = (glm::normalize(this->mesh.vertices[0].pos + this->mesh.vertices[1].pos) * this->size).x;

		glm::vec3 y = (glm::vec3(this->currPos.x - ((TriPos.x * object.size.x) + object.currPos.x), this->currPos.y - ((TriPos.y * object.size.y) + object.currPos.y), this->currPos.z - ((TriPos.z * object.size.z) + object.currPos.z)) * normal) * normal;
		glm::vec3 a = (this->velocity * normal) * normal;
		glm::vec3 x = this->velocity - (glm::vec3(0.125 * a.x, 0.125 * a.y, 0.125 * a.z));

		//av en eller annen grunn teleporterer dette ballene out of bounds, saa jeg bruker heller bare den forrige posisjonsdataen til ballen
		//this->move((((r - y) / r) * this->velocity) + ((y / r) * x));
		this->currPos = this->prevPos;
		this->velocity = x * glm::vec3(0.95);
}

//Provde metoden fra forelesninger/Vism notater, men uansett hvor mye jeg provde fikk jeg det ikke til, brukte 2 hele dager paa dette for jeg ga opp og heller bare brukte squareDistance mellom kulene
//for kollisjonsdetektering

//bool PhysicsObject::isCollidingBall(Object object)
//{
//
//	glm::vec3 A = this->currPos - object.currPos;
//	glm::vec3 B = this->velocity - object.velocity;
//
//	float AB = multVec(A, B);
//	float AA = multVec(A, A);
//	float BB = multVec(B, B);
//
//	float d = 0.1f + 0.1f;
//	float rot = (AB * AB) - (BB * (AA - d * d));
//	float t = -1.0f;
//	if (rot >= 0.0f) {
//		t = -AB - std::sqrt(rot);
//	}
//	if (BB != 0.0) {
//		t = t / BB;
//	}
//
//	return 0 < t < 1;
//}

bool PhysicsObject::isCollidingBall(Object* object)
{
	float d = 0.1f + 0.1f;
	float dd = glm::distance2(this->currPos, object->currPos);
	return dd < d * d;
}

//bruker 1.0f istedet for 2.0f fordi 2.0f 
//fikk dem til aa konstant oke bevegelsesmengden deres
void PhysicsObject::onBallCollision(PhysicsObject* object) {

	glm::vec3 v0 = this->velocity * (this->mass - object->mass) + object->velocity * object->mass * 1.0f;
	v0 = v0 / (object->mass + this->mass);
	glm::vec3 v1 = this->velocity * (object->mass - this->mass) + this->velocity * this->mass * 1.0f;
	v1 = v1 / (this->mass + object->mass);
	cout << "hit" << endl;
	this->velocity = v0;
	this->currPos = this->prevPos;
	object->velocity = v1;
	object->currPos = object->prevPos;
}

