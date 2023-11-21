#include "BitmapRenderer.h"
#include "Random.h"
#include "Window.h"
#include "Event.h"

#include <vector>
#include <chrono>
#include <algorithm>

namespace {
	constexpr auto WINDOW_WIDTH = 800;
	constexpr auto WINDOW_HEIGHT = 800;

	constexpr auto SEGMENT_SIZE = 80;
	constexpr auto SNAKE_PADDING_SIZE = 3;
	constexpr auto SNAKE_SIZE_WITHOUT_PADDING = SEGMENT_SIZE - SNAKE_PADDING_SIZE * 2;

	constexpr auto FOOD_PADDING_SIZE = 10;
	constexpr auto FOOD_SIZE_WITHOUT_PADDING = SEGMENT_SIZE - FOOD_PADDING_SIZE * 2;

	constexpr auto GRID_ROWS = WINDOW_HEIGHT / SEGMENT_SIZE;
	constexpr auto GRID_COLUMNS = WINDOW_WIDTH / SEGMENT_SIZE;
	constexpr auto MAXIMUM_SNAKE_LENGTH = GRID_ROWS * GRID_COLUMNS;

	constexpr auto TARGET_FRAME_RATE = 10;
	constexpr auto MS_REQUIRED_BETWEEN_FRAMES = 1000 / TARGET_FRAME_RATE;

	constexpr auto MINIMUM_LENGTH_FOR_SELF_COLLISION = 5;

	constexpr auto CLEAR_COLOR = Pixel{ 203, 217, 167, 255 };
	constexpr auto SNAKE_COLOR = Pixel{ 19, 118, 194, 255 };
	constexpr auto FOOD_COLOR = Pixel{ 19, 179, 194, 255 };

	enum class Direction {
		Left,
		Right,
		Up,
		Down
	};

	enum class GameState {
		Paused,
		Running
	};

	struct Position {
		i32 x{};
		i32 y{};

		Position& operator+=(const Position& right) {
			x += right.x;
			y += right.y;

			return *this;
		}

		Position operator+(const Position& right) const {
			return Position{ x + right.x, y + right.y };
		}

		Position operator*(i32 right) const {
			return Position{ x * right, y * right };
		}

		bool operator==(const Position& right) const {
			return x == right.x && y == right.y;
		}
	};

	struct Snake {
		std::vector<Position> body{};
		Direction direction{};
		Position food{};
		GameState state{};
	};

	bool isPositionOverlappingSnake(const Snake& snake, const Position& position) {
		return std::find(snake.body.begin(), snake.body.end(), position) != snake.body.end();
	}

	Position getRandomFoodPosition() {
		auto x = generateRange(0, GRID_COLUMNS - 1) * SEGMENT_SIZE;
		auto y = generateRange(0, GRID_ROWS - 1) * SEGMENT_SIZE;

		return Position{ x, y };
	}

	Position getNewFoodPosition(const Snake& snake) {
		auto position = getRandomFoodPosition();
		while (isPositionOverlappingSnake(snake, position)) {
			position = getRandomFoodPosition();
		}

		return position;
	}

	Position getDirectionVector(Direction direction) {
		switch (direction) {
		case Direction::Left:
			return Position{ -1, 0 };
		case Direction::Right:
			return Position{ 1, 0 };
		case Direction::Up:
			return Position{ 0, -1 };
		case Direction::Down:
			return Position{ 0, 1 };
		}
	}

	void renderSnake(const Snake& snake, BitmapRenderer& renderer) {
		renderer.setFillColor(CLEAR_COLOR);
		renderer.clear();

		renderer.setFillColor(SNAKE_COLOR);

		for (auto& segment : snake.body) {
			renderer.fillRect(
				segment.x + SNAKE_PADDING_SIZE,
				segment.y + SNAKE_PADDING_SIZE,
				SNAKE_SIZE_WITHOUT_PADDING,
				SNAKE_SIZE_WITHOUT_PADDING
			);
		}

		renderer.setFillColor(FOOD_COLOR);

		renderer.fillRect(
			snake.food.x + FOOD_PADDING_SIZE, 
			snake.food.y + FOOD_PADDING_SIZE, 
			FOOD_SIZE_WITHOUT_PADDING, 
			FOOD_SIZE_WITHOUT_PADDING
		);
	}

	void updateSnakePosition(Snake& snake) {
		auto directionVector = getDirectionVector(snake.direction);
		auto newHeadLocation = snake.body.back() + directionVector * SEGMENT_SIZE;

		if (newHeadLocation.x < 0) {
			newHeadLocation.x = WINDOW_WIDTH - SEGMENT_SIZE;
		} else if (newHeadLocation.x == WINDOW_WIDTH) {
			newHeadLocation.x = 0;
		} else if (newHeadLocation.y < 0) {
			newHeadLocation.y = WINDOW_HEIGHT - SEGMENT_SIZE;
		} else if (newHeadLocation.y == WINDOW_HEIGHT) {
			newHeadLocation.y = 0;
		}

		std::rotate(snake.body.begin(), snake.body.begin() + 1, snake.body.end());

		snake.body.back() = newHeadLocation;
	}

	void updateSnakeDirectionAndState(Snake& snake, const KeyPressed& e) {
		if (e.key == Key::D && snake.direction != Direction::Left) {
			snake.direction = Direction::Right;
		} else if (e.key == Key::S && snake.direction != Direction::Up) {
			snake.direction = Direction::Down;
		} else if (e.key == Key::A && snake.direction != Direction::Right) {
			snake.direction = Direction::Left;
		} else if (e.key == Key::W && snake.direction != Direction::Down) {
			snake.direction = Direction::Up;
		} else if (e.key == Key::Space) {
			snake.state = snake.state == GameState::Paused ? GameState::Running : GameState::Paused;
		}
	}

	void eatFoodIfPossible(Snake& snake) {
		if (snake.body.back() == snake.food) {
			snake.body.push_back(snake.body.back());
			snake.food = getNewFoodPosition(snake);
		}
	}

	bool isGameOver(const Snake& snake) {
		if (snake.body.size() < MINIMUM_LENGTH_FOR_SELF_COLLISION) {
			return false;
		}

		auto end = snake.body.end() - MINIMUM_LENGTH_FOR_SELF_COLLISION + 1;
		return std::find(snake.body.begin(), end, snake.body.back()) != end;
	}

	Snake startNewGame() {
		Snake snake{};
		snake.food = getNewFoodPosition(snake);
		snake.direction = Direction::Right;
		snake.state = GameState::Paused;

		snake.body.push_back(Position{});

		return snake;
	}
}

int main() {
	using Duration = std::chrono::duration<f32, std::milli>;

	WindowProperties properties{};
	properties.width = WINDOW_WIDTH;
	properties.height = WINDOW_HEIGHT;
	properties.title = "Snake";

	Window window{ properties };
	BitmapRenderer renderer{ window.getWidth(), window.getHeight() };
	
	auto snake = startNewGame();
	auto startTime = std::chrono::steady_clock::now();

	while (!window.isCloseRequested()) {
		Event e{};

		while (window.getEvent(e)) {
			if (auto closeEvent = e.getValue<WindowClosed>(); closeEvent.has_value()) {
				window.destroy();
			} else if (auto keyPressedEvent = e.getValue<KeyPressed>(); keyPressedEvent.has_value()) {
				auto& keyPressed = keyPressedEvent.value();
				updateSnakeDirectionAndState(snake, keyPressed);
			}
		}

		auto currentTime = std::chrono::steady_clock::now();;
		auto msElapsedSinceStart = std::chrono::duration_cast<Duration>(currentTime - startTime).count();

		if (msElapsedSinceStart >= MS_REQUIRED_BETWEEN_FRAMES) {
			startTime = std::chrono::steady_clock::now();

			if (isGameOver(snake) || snake.body.size() == MAXIMUM_SNAKE_LENGTH) {
				snake = startNewGame();
				continue;
			}

			eatFoodIfPossible(snake);
			renderSnake(snake, renderer);

			window.blit(renderer.getImageData());

			if (snake.state != GameState::Paused) {
				updateSnakePosition(snake);
			}
		}
	}
}
