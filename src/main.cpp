#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <string>

#include "Game.hpp"

using namespace ftxui;
using namespace Game2048;

// 辅助函数：根据方块数值返回对应的背景颜色
Color GetTileColor(int value) {
    switch (value) {
        case 2: return Color::RGB(238, 228, 218);
        case 4: return Color::RGB(237, 224, 200);
        case 8: return Color::RGB(242, 177, 121);
        case 16: return Color::RGB(245, 149, 99);
        case 32: return Color::RGB(246, 124, 95);
        case 64: return Color::RGB(246, 94, 59);
        case 128: return Color::RGB(237, 207, 114);
        case 256: return Color::RGB(237, 204, 97);
        case 512: return Color::RGB(237, 200, 80);
        case 1024: return Color::RGB(237, 197, 63);
        case 2048: return Color::RGB(237, 194, 46);
        default: return Color::RGB(205, 193, 180); // 空白方块或超过2048的方块
    }
}

// 辅助函数：浅色背景用深色字，深色背景用浅色字
Color GetTextColor(int value) {
    if (value <= 4) return Color::RGB(119, 110, 101);
    return Color::White;
}

int main() {
    // 实例化游戏核心逻辑 (Model)
    Game game;
    
    // 创建一个自适应组件大小的交互式屏幕
    auto screen = ScreenInteractive::FitComponent();

    // 1. 构建 UI 渲染器 (View)
    auto ui_renderer = Renderer([&] {
        Elements rows;
        const auto& board = game.getBoard();

        // 动态生成 4x4 的 DOM 树
        for (int i = 0; i < Game::BOARD_SIZE; ++i) {
            Elements cells;
            for (int j = 0; j < Game::BOARD_SIZE; ++j) {
                int val = board[i][j];
                std::string txt = (val == 0) ? "" : std::to_string(val);
                
                // 单个方块的样式定义
                cells.push_back(
                    text(txt) 
                    | center 
                    | size(WIDTH, EQUAL, 8) 
                    | size(HEIGHT, EQUAL, 4) 
                    | border 
                    | bgcolor(GetTileColor(val))
                    | color(GetTextColor(val))
                );
            }
            // 将一行方块用水平盒子 (hbox) 包装起来
            rows.push_back(hbox(std::move(cells)));
        }

        // 将所有行用垂直盒子 (vbox) 包装起来，形成完整棋盘
        auto board_ui = vbox(std::move(rows)) | border | center;

        // 侧边栏/顶部信息
        auto info_ui = vbox({
            text("2048 TUI") | bold | center,
            separator(),
            text("Score: " + std::to_string(game.getScore())) | center,
            separator(),
            text("操作指南:") | center,
            text("↑ ↓ ← → 或 WASD : 移动") | dim | center,
            text("R : 重新开始") | dim | center,
            text("Q / Esc : 退出") | dim | center,
        }) | border | center;

        // 游戏结束/胜利状态提示
        Element status_ui = text("");
        if (game.getState() == GameState::Victory) {
            status_ui = text(" You Win! ") | bold | color(Color::Green) | blink | center;
        } else if (game.getState() == GameState::Defeat) {
            status_ui = text(" Game Over! ") | bold | color(Color::Red) | center;
        }

        // 最终布局组合
        return vbox({
            info_ui,
            status_ui,
            board_ui
        }) | center;
    });

    // 2. 拦截键盘事件 (Controller)
    auto interactive_component = CatchEvent(ui_renderer, [&](Event event) {
        // 退出逻辑
        if (event == Event::Character('q') || event == Event::Character('Q') || event == Event::Escape) {
            screen.ExitLoopClosure()();
            return true; 
        }
        
        // 重置逻辑
        if (event == Event::Character('r') || event == Event::Character('R')) {
            game.restart();
            return true;
        }
        
        // 游戏处于进行中时，才响应方向键
        if (game.getState() == GameState::Playing) {
            bool moved = false;
            if (event == Event::ArrowUp || event == Event::Character('w') || event == Event::Character('W')) {
                moved = game.move(Direction::Up);
            } else if (event == Event::ArrowDown || event == Event::Character('s') || event == Event::Character('S')) {
                moved = game.move(Direction::Down);
            } else if (event == Event::ArrowLeft || event == Event::Character('a') || event == Event::Character('A')) {
                moved = game.move(Direction::Left);
            } else if (event == Event::ArrowRight || event == Event::Character('d') || event == Event::Character('D')) {
                moved = game.move(Direction::Right);
            }
            
            // 如果移动成功（状态发生改变），返回 true 让屏幕重绘
            if (moved) return true;
        }
        
        // 返回 false 表示该事件未被处理，可以传递给更外层的组件
        return false;
    });

    // 3. 启动主事件循环
    screen.Loop(interactive_component);

    return 0;
}