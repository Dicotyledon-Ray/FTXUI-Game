#include "Game.hpp"
#include <cstdlib>
#include <ctime>
#include <algorithm>

namespace Game2048 {

Game::Game() {
    std::srand(std::time(nullptr));
    reset();
}

void Game::reset() {
    board = std::vector<std::vector<int>>(4, std::vector<int>(4, 0));
    score = 0;
    addRandomTile();
    addRandomTile();
}

void Game::addRandomTile() {
    std::vector<std::pair<int, int>> emptyCells;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (board[i][j] == 0) emptyCells.push_back({i, j});
        }
    }
    if (!emptyCells.empty()) {
        int idx = std::rand() % emptyCells.size();
        board[emptyCells[idx].first][emptyCells[idx].second] = (std::rand() % 10 == 0) ? 4 : 2;
    }
}

// 辅助函数：逆时针旋转棋盘 90 度
void rotateLeft(std::vector<std::vector<int>>& b) {
    std::vector<std::vector<int>> temp = b;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            b[3 - j][i] = temp[i][j];
        }
    }
}

// 核心逻辑：基础的向左合并算法
bool slideLeft(std::vector<std::vector<int>>& b, int& currentScore) {
    bool moved = false;
    for (int i = 0; i < 4; ++i) {
        std::vector<int> row;
        // 1. 抽离出所有非空的数字
        for (int j = 0; j < 4; ++j) {
            if (b[i][j] != 0) row.push_back(b[i][j]);
        }
        
        // 2. 合并相同的数字
        std::vector<int> newRow;
        for (size_t j = 0; j < row.size(); ++j) {
            if (j + 1 < row.size() && row[j] == row[j + 1]) {
                newRow.push_back(row[j] * 2);
                currentScore += row[j] * 2; // 加分！
                j++; // 跳过下一个已合并的数字
                moved = true;
            } else {
                newRow.push_back(row[j]);
            }
        }
        
        // 3. 补齐末尾的 0
        while (newRow.size() < 4) newRow.push_back(0);
        
        if (newRow != b[i]) {
            b[i] = newRow;
            moved = true;
        }
    }
    return moved;
}

// ✨ 四向移动大统一：通过旋转完美适配所有方向
bool Game::move(GameDirection dir) {
    bool moved = false;

    if (dir == GameDirection::Left) {
        // 本来就是向左
        moved = slideLeft(board, score);
    } 
    else if (dir == GameDirection::Up) {
        // 向上：逆时针转1次 -> 向左滑 -> 顺时针转1次（即逆时针转3次）
        rotateLeft(board);
        moved = slideLeft(board, score);
        rotateLeft(board); rotateLeft(board); rotateLeft(board);
    } 
    else if (dir == GameDirection::Right) {
        // 向右：逆时针转2次 -> 向左滑 -> 再逆时针转2次
        rotateLeft(board); rotateLeft(board);
        moved = slideLeft(board, score);
        rotateLeft(board); rotateLeft(board);
    } 
    else if (dir == GameDirection::Down) {
        // 向下：逆时针转3次 -> 向左滑 -> 再逆时针转1次
        rotateLeft(board); rotateLeft(board); rotateLeft(board);
        moved = slideLeft(board, score);
        rotateLeft(board);
    }

    // 如果棋盘发生过位移，就额外诞生一个随机新方块
    if (moved) {
        addRandomTile();
    }
    return moved;
}

bool Game::isGameOver() const {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (board[i][j] == 0) return false;
            if (i + 1 < 4 && board[i][j] == board[i + 1][j]) return false;
            if (j + 1 < 4 && board[i][j] == board[i][j + 1]) return false;
        }
    }
    return true;
}

} // namespace Game2048