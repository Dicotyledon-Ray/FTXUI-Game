#include <memory>
#include <string>
#include "Game.hpp"

#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

int main() {
    auto screen = ScreenInteractive::TerminalOutput();
    Game2048::Game game;

    auto component = CatchEvent(Renderer([&]() {
        Elements rows;
        auto board = game.getBoard();
        
        for (int i = 0; i < 4; ++i) {
            Elements row;
            for (int j = 0; j < 4; ++j) {
                std::string val = (board[i][j] == 0) ? "" : std::to_string(board[i][j]);
                row.push_back(window(text(" " + val + " ") | hcenter, text("")) | size(WIDTH, EQUAL, 8) | size(HEIGHT, EQUAL, 3));
            }
            rows.push_back(hbox(std::move(row)));
        }

        return vbox({
            text("FTXUI 2048 游戏演示") | bold | hcenter,
            text("得分: " + std::to_string(game.getScore())) | hcenter,
            separator(),
            vbox(std::move(rows)) | bgcolor(Color::BlueLight) | border,
            separator(),
            text("使用 W, A, S, D 或 方向键 进行移动，按 Q 退出游戏。") | hcenter
        });
    }), [&](Event event) {
        if (event == Event::Character('q') || event == Event::Escape) {
            screen.ExitLoopClosure()();
            return true;
        }
        
        // 捕获按键并安全映射到我们的 GameDirection
        if (event == Event::ArrowLeft || event == Event::Character('a')) {
            game.move(Game2048::GameDirection::Left);
            return true;
        }
        if (event == Event::ArrowRight || event == Event::Character('d')) {
            game.move(Game2048::GameDirection::Right);
            return true;
        }
        if (event == Event::ArrowUp || event == Event::Character('w')) {
            game.move(Game2048::GameDirection::Up);
            return true;
        }
        if (event == Event::ArrowDown || event == Event::Character('s')) {
            game.move(Game2048::GameDirection::Down);
            return true;
        }
        return false;
    });

    screen.Loop(component);
    return 0;
}