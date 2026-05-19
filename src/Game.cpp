#include "Game.hpp"
#include <random>
#include <algorithm>

namespace Game2048 {

Game::Game() {
    restart();
}

void Game::restart() {
    // 清空棋盘和分数
    for (auto& row : board_) {
        row.fill(0);
    }
    score_ = 0;
    state_ = GameState::Playing;

    // 初始生成两个数字
    spawnTile();
    spawnTile();
}

void Game::spawnTile() {
    // 收集所有空位
    std::vector<std::pair<int, int>> emptyCells;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board_[i][j] == 0) {
                emptyCells.emplace_back(i, j);
            }
        }
    }

    if (emptyCells.empty()) return;

    // 使用现代 C++ 的随机数生成器
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> posDist(0, emptyCells.size() - 1);
    std::uniform_int_distribution<> valDist(0, 9);

    auto [x, y] = emptyCells[posDist(gen)];
    // 10% 概率生成 4，90% 概率生成 2
    board_[x][y] = (valDist(gen) == 0) ? 4 : 2;
}

bool Game::move(Direction dir) {
    if (state_ != GameState::Playing) return false;

    int rotations = 0;
    switch (dir) {
        case Direction::Left:  rotations = 0; break;
        case Direction::Down:  rotations = 1; break; // 顺时针转1次，相当于把下变成左
        case Direction::Right: rotations = 2; break;
        case Direction::Up:    rotations = 3; break;
    }

    rotateBoardClockwise(rotations);
    bool changed = slideLeft();
    rotateBoardClockwise((4 - rotations) % 4); // 转回去

    if (changed) {
        spawnTile();
        updateGameState();
    }

    return changed;
}

bool Game::slideLeft() {
    bool changed = false;

    for (int i = 0; i < BOARD_SIZE; ++i) {
        // 1. 压缩零（将所有非零元素推到左边）
        int insertPos = 0;
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board_[i][j] != 0) {
                if (insertPos != j) {
                    board_[i][insertPos] = board_[i][j];
                    board_[i][j] = 0;
                    changed = true;
                }
                insertPos++;
            }
        }

        // 2. 合并相同的相邻元素
        for (int j = 0; j < BOARD_SIZE - 1; ++j) {
            if (board_[i][j] != 0 && board_[i][j] == board_[i][j + 1]) {
                board_[i][j] *= 2;
                score_ += board_[i][j];
                board_[i][j + 1] = 0;
                changed = true;
                
                if (board_[i][j] == 2048) {
                    state_ = GameState::Victory;
                }
            }
        }

        // 3. 再次压缩（填补合并产生的空隙）
        insertPos = 0;
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board_[i][j] != 0) {
                if (insertPos != j) {
                    board_[i][insertPos] = board_[i][j];
                    board_[i][j] = 0;
                }
                insertPos++;
            }
        }
    }
    return changed;
}

void Game::rotateBoardClockwise(int times) {
    times %= 4;
    for (int t = 0; t < times; ++t) {
        BoardType temp{};
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                temp[j][BOARD_SIZE - 1 - i] = board_[i][j];
            }
        }
        board_ = temp;
    }
}

bool Game::canMove() const {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board_[i][j] == 0) return true;
            if (j < BOARD_SIZE - 1 && board_[i][j] == board_[i][j + 1]) return true;
            if (i < BOARD_SIZE - 1 && board_[i][j] == board_[i + 1][j]) return true;
        }
    }
    return false;
}

void Game::updateGameState() {
    if (state_ == GameState::Victory) return; // 已达标
    if (!canMove()) {
        state_ = GameState::Defeat;
    }
}

} // namespace Game2048