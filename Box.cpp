#include "Box.h"



void Box::setPosition(vec3 position)
{
	Position = position;
	TranslationMatrix = translate(glm::mat4(1.0f), Position);
}


Box::Box(vec3 initialPosition, const char* imagePath, const string vertexShader, const string fragmentShader, char* modelPath, bool m, float sp, vec3 s, bool vis) :GameObject(vertexShader, fragmentShader)
{
	Position = initialPosition;
	vertexBufferData = VertexBufferData(modelPath);
	scaleVector = s;
	speed = sp/250;
	master = m;
	Visible = vis;
	radius = (s.x + s.y + s.z) / 3;

	direction = vec3(0.0f, 0.0f, 0.0f);
	Hit = false;
	rotationAngle = 0.0f;

	RotationMatrix = mat4(1.0f);
	OrbitMatrix = mat4(1.0f);
	ScaleMatrix = mat4(1.0f);

	ScaleMatrix = glm::scale(mat4(1.0f), scaleVector);
	TranslationMatrix = glm::translate(mat4(1.0f), Position);

	if (!shaderProgram->initialize())
	{
		std::cerr << "Could not initialize the shaders" << std::endl;
	}

	shaderProgram->bindShader();

	glGenVertexArrays(1, &vertexBufferData.VAO);
	glBindVertexArray(vertexBufferData.VAO);

	printf("Reading image %s\n", imagePath);
	int img_width, img_height, channels;
	unsigned char *img = SOIL_load_image(imagePath, &img_width, &img_height, &channels, 0);
	glGenTextures(1, &vertexBufferData.Texture);
	glBindTexture(GL_TEXTURE_2D, vertexBufferData.Texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenBuffers(1, &vertexBufferData.elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBufferData.elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexBufferData.elementSize, &vertexBufferData.elements[0], GL_STATIC_DRAW);

	glGenBuffers(1, &vertexBufferData.vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferData.vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertexBufferData.verticiesSize, &vertexBufferData.verticies[0], GL_STATIC_DRAW);

	glGenBuffers(1, &vertexBufferData.textbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferData.textbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertexBufferData.texcoordSize, &vertexBufferData.texcoords[0], GL_STATIC_DRAW);
}


void Box::Draw(float deltaTime)
{
	if (Visible)
	{
		Position += direction * deltaTime * speed;
		shaderProgram->bindShader();
		glBindVertexArray(vertexBufferData.VAO);

		int size;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

		glBindTexture(GL_TEXTURE_2D, vertexBufferData.Texture);
		shaderProgram->sendUniform("TextureSampler", 0);

		shaderProgram->sendUniform4x4("MVP", glm::value_ptr(MVP));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferData.vertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferData.textbuffer);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBufferData.elementBuffer);

		glDrawElements(GL_TRIANGLES, size / sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}
	
}


void Box::OnCollide(Box *box)
{
	if (master)//you are the master cube
	{
		/*We do nothing if we are the master cube*/
		return;
	}
	else if (box->master)//hit the master cube
	{
		Hit = true;
	}
	else//hit another small cube
	{
		direction.x = -direction.x;
		direction.y = direction.y;
	}
}

bool Box:: operator!=(Box b)
{
	if (Position != b.Position)
		return  true;
	else
		return false;
}

float Box::getXBuffer()
{
	return  2 * scaleVector.x;
}

float Box::getYBuffer()
{
	return  2 * scaleVector.y;
}

float Box::getZBuffer()
{
	return  2 * scaleVector.z;
}

void Box::setMVPMatrix(mat4 viewMatrix, mat4 projectionMatrix)
{
	MVP = projectionMatrix * viewMatrix * getModelMatrix();
}

void Box::setScaleMatrix(vec3 scale)
{
	scaleVector = scale;
	ScaleMatrix = glm::scale(mat4(1.0f), scaleVector);
}

void Box::setTranslationMatrix(vec3 pos)
{
	Position = pos;
	TranslationMatrix = glm::translate(glm::mat4(1.0f), Position);
}

void Box::setRotationMatrix(float angle, float x, float y, float z)
{
	RotationMatrix = glm::rotate(mat4(1.0f), angle, vec3(x, y, z));
}

void Box::setOrbitMatrix(vec3 pos, float angle, float x, float y, float z)
{
	OrbitMatrix = glm::rotate(glm::translate(glm::mat4(1.0f), pos), angle, vec3(x, y, z));
}

void Box::ChangeTexture()
{
	int img_width, img_height, channels;
	unsigned char *img = SOIL_load_image("data/images/metal.bmp", &img_width, &img_height, &channels, 0);
	glBindTexture(GL_TEXTURE_2D, vertexBufferData.Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
}
