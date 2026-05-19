#ifndef GAME_HPP
#define GAME_HPP

#include <vector>
#include <ftxui/component/component.hpp>

namespace Game2048 {

// 1. 把普通的 Direction 改为特定的 GameDirection，完美避开冲突
enum class GameDirection {
    Up,
    Down,
    Left,
    Right
};

class Game {
public:
    Game();
    void reset();
    bool move(GameDirection dir); // 修改参数类型
    void addRandomTile();
    bool isGameOver() const;
    int getScore() const { return score; }
    const std::vector<std::vector<int>>& getBoard() const { return board; }

private:
    std::vector<std::vector<int>> board;
    int score;
};

} // namespace Game2048

#endif // GAME_HPP