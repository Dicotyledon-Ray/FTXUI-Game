#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace Game2048 {

// 严格限定作用域的枚举，代表移动方向
enum class Direction {
    Up,
    Down,
    Left,
    Right
};

// 游戏状态
enum class GameState {
    Playing,
    Victory,
    Defeat
};

class Game {
public:
    static constexpr int BOARD_SIZE = 4;
    using BoardType = std::array<std::array<int, BOARD_SIZE>, BOARD_SIZE>;

    // 构造函数，初始化并生成开局的数字
    Game();

    // 核心操作接口
    bool move(Direction dir);
    void restart();

    // 状态查询接口 (使用 [[nodiscard]] 提醒调用者处理返回值)
    [[nodiscard]] const BoardType& getBoard() const { return board_; }
    [[nodiscard]] int getScore() const { return score_; }
    [[nodiscard]] GameState getState() const { return state_; }

private:
    BoardType board_{};
    int score_{0};
    GameState state_{GameState::Playing};

    // 内部原子操作
    void spawnTile();
    void updateGameState();
    bool canMove() const;

    // 为了保持代码 DRY (Don't Repeat Yourself)，我们将所有方向的移动
    // 抽象为“矩阵旋转”配合“向左滑动合并”的操作。
    bool slideLeft();
    void rotateBoardClockwise(int times);
};

} // namespace Game2048