#include <iostream>
#include <iomanip>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>


void PrintMatrix(const glm::mat4& M)
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			std::cout
				<< std::setw(10)
				<< std::setprecision(4)
				<< std::fixed
				<< M[j][i] << " ";
		}
		std::cout << std::endl;
	}
}

void TranslationMatrix()
{
	std::cout << std::endl;
	std::cout << "=================" << std::endl;
	std::cout << "Translation Matrix" << std::endl;
	std::cout << "=================" << std::endl;

	glm::mat4 I = glm::identity<glm::mat4>();
	glm::vec3 T{ 10, 10, 10 };
	glm::mat4 Translation = glm::translate(I, T);

	PrintMatrix(Translation);

	glm::vec4 Position{ 10, 10, 10, 1 };
	glm::vec4 Direction{ 10, 10, 10, 0 };

	std::cout << std::endl;

	Position = Translation * Position;

	std::cout << glm::to_string(Position) << std::endl;

	//PrintMatrix(Translation);
}

void ScaleMatrix()
{
	std::cout << std::endl;
	std::cout << "=================" << std::endl;
	std::cout << "Scale Matrix" << std::endl;
	std::cout << "=================" << std::endl;

	glm::mat4 I = glm::identity<glm::mat4>();
	glm::vec3 ScaleAmount{ 2, 2, 2 };
	glm::mat4 Scale = glm::scale(I, ScaleAmount);

	PrintMatrix(Scale);

	glm::vec4 Position{ 100, 100, 0, 1 };
	glm::vec4 Direction{ 100, 100, 0, 0 };

	Position = Scale * Position;
	Direction = Scale * Direction;
	std::cout << glm::to_string(Position) << std::endl;
	std::cout << glm::to_string(Direction) << std::endl;

	std::cout << std::endl;
}

void RotationMatrix()
{
	std::cout << std::endl;
	std::cout << "=================" << std::endl;
	std::cout << "Rotation Matrix" << std::endl;
	std::cout << "=================" << std::endl;

	glm::mat4 I = glm::identity<glm::mat4>();
	constexpr float Angle = glm::radians(90.0f);
	glm::vec3 Axis{ 0, 0, 1 };
	glm::mat4 Rotation = glm::rotate(I, Angle, Axis);

	PrintMatrix(Rotation);

	glm::vec4 Position{ 100, 0, 0, 1 };
	glm::vec4 Direction{ 100, 0, 0, 0 };

	Position = Rotation * Position;
	Direction = Rotation * Direction;

	std::cout << glm::to_string(Position) << std::endl;
	std::cout << glm::to_string(Direction) << std::endl;
	std::cout << std::endl;
}

void ComposedMatrices()
{
	std::cout << std::endl;
	std::cout << "=================" << std::endl;
	std::cout << "Composed Matrices" << std::endl;
	std::cout << "=================" << std::endl;

	glm::mat4 I = glm::identity<glm::mat4>();

	//Translation Matrix
	glm::vec3 T{ 0, 10, 0 };
	glm::mat4 Translation = glm::translate(I, T);

	//Rotation Matrix
	constexpr float Angle = glm::radians(45.0f);
	glm::vec3 Axis{ 0, 0, 1 };
	glm::mat4 Rotation = glm::rotate(I, Angle, Axis);

	//Scale Matrix
	glm::vec3 ScaleAmount{ 2, 2, 0 };
	glm::mat4 Scale = glm::scale(I, ScaleAmount);

	std::cout << "Translation " << std::endl;
	PrintMatrix(Translation);
	std::cout << std::endl;

	std::cout << "Rotation " << std::endl;
	PrintMatrix(Rotation);
	std::cout << std::endl;

	std::cout << "Scale " << std::endl;
	PrintMatrix(Scale);
	std::cout << std::endl;

	glm::vec4 Position{ 1, 1, 0, 1 };
	glm::vec4 Direction{ 1, 1, 0, 0 };

	//Transform
	//This sequence: Translation * Rotation * Scale
	glm::mat4 ModelMatrix = Translation * Rotation * Scale;

	std::cout << "Model Matrix " << std::endl;
	PrintMatrix(ModelMatrix);
	std::cout << std::endl;

	Position = ModelMatrix * Position;
	Direction = ModelMatrix * Direction;
	std::cout << glm::to_string(Position) << std::endl;
	std::cout << glm::to_string(Direction) << std::endl;
}

int main()
{
	TranslationMatrix();
	ScaleMatrix();
	RotationMatrix();
	ComposedMatrices();
	return 0;
}
