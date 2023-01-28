#include <fights/Game.hpp>

#include <sstream>
#include <stdexcept>

namespace fights
{
void WallBoard::PlaceHorizontalWall(int x, int y)
{
    walls_[pointToHorizontalWallIndex(x, y)] |= HORIZONTAL_WALL;
    walls_[pointToHorizontalWallIndex(x + 1, y)] |= HORIZONTAL_WALL;
    clearBoundary();
}

void WallBoard::PlaceVerticalWall(int x, int y)
{
    walls_[pointToVerticalWallIndex(x, y)] |= VERTICAL_WALL;
    walls_[pointToVerticalWallIndex(x, y + 1)] |= VERTICAL_WALL;
    clearBoundary();
}

void WallBoard::Rotate(int x, int y)
{
    auto tmpWall = walls_;

    for (int dx = 0; dx <= 4; ++dx)
    {
        for (int dy = 0; dy <= 4; ++dy)
        {
            const int nx = x + dx - 1;
            const int ny = y + dy - 1;

            if (dx > 0)
                tmpWall[pointToHorizontalWallIndex(nx, ny)] &= ~HORIZONTAL_WALL;

            if (dy > 0)
                tmpWall[pointToVerticalWallIndex(nx, ny)] &= ~VERTICAL_WALL;
        }
    }

    for (int dx = 0; dx <= 4; ++dx)
    {
        for (int dy = 0; dy <= 4; ++dy)
        {
            const int ox = x + dx - 1;
            const int oy = y + dy - 1;

            if (dx > 0 && walls_[pointToHorizontalWallIndex(ox, oy)] & HORIZONTAL_WALL)
            {
                const int tx = x + 3 - dy;
                const int ty = y + dx - 1;

                tmpWall[pointToVerticalWallIndex(tx, ty)] |= VERTICAL_WALL;
            }
            if (dy > 0 && walls_[pointToVerticalWallIndex(ox, oy)] & VERTICAL_WALL)
            {
                const int tx = x + 4 - dy;
                const int ty = y + dx - 1;

                tmpWall[pointToHorizontalWallIndex(tx, ty)] |= HORIZONTAL_WALL;
            }
        }
    }

    walls_ = std::move(tmpWall);

    clearBoundary();
}

bool WallBoard::CheckReachability(Point playerPos, int target) const
{
    return false;
}

std::string WallBoard::ToString() const
{
    std::stringstream ss;

    // build top line
    for (int x = 0; x < BOARD_SIZE + 1; ++x)
    {
        if (x == 0)
        {
            ss << "┌";
        }
        else if (x == BOARD_SIZE)
        {
            ss << "───┐";
        }
        else
        {
            ss << "───┬";
        }
    }
    ss << "\n";

    // build middle
    for (int y = 1; y <= BOARD_SIZE; ++y)
    {
        // build players or vertical lines
        for (int x = 0; x < BOARD_SIZE + 1; ++x)
        {
            if (x == 0)
            {
                ss << "│";
            } 
            else
            {
                ss << "   ";

                if (IsVerticalWallPlaced(x, y))
                    ss << "┃";
                else
                    ss << "│";
            }
        }
        ss << "\n";

        // build horizontal lines
        if (y <= BOARD_SIZE - 1)
        {
            for (int x = 0; x < BOARD_SIZE + 1; ++x)
            {
                if (x == 0)
                {
                    ss << "├";
                } 
                else
                {
                    if (IsHorizontalWallPlaced(x, y))
                        ss << "━━━";
                    else
                        ss << "───";

                    if (x == BOARD_SIZE)
                    {
                        ss << "┤";
                    }
                    else
                    {
                        ss << "┼";
                    }
                }
            }
            ss << "\n";
        }
    }

    // build bottom line
    for (int x = 0; x < BOARD_SIZE + 1; ++x)
    {
        if (x == 0)
        {
            ss << "└";
        }
        else if (x == BOARD_SIZE)
        {
            ss << "───┘";
        }
        else
        {
            ss << "───┴";
        }
    }
    ss << "\n";


    return ss.str();
}

void WallBoard::clearBoundary()
{
    for (int i = 0; i <= BOARD_SIZE + 1; ++i)
    {
        walls_[pointToHorizontalWallIndex(i, 0)] &= ~HORIZONTAL_WALL;
        walls_[pointToHorizontalWallIndex(i, BOARD_SIZE)] &= ~HORIZONTAL_WALL;

        walls_[pointToVerticalWallIndex(0, i)] &= ~VERTICAL_WALL;
        walls_[pointToVerticalWallIndex(BOARD_SIZE, i)] &= ~VERTICAL_WALL;
    }

    for (int i = 0; i <= BOARD_SIZE; ++i)
    {
        walls_[pointToHorizontalWallIndex(0, i)] &= ~HORIZONTAL_WALL;
        walls_[pointToHorizontalWallIndex(BOARD_SIZE + 1, i)] &=
            ~HORIZONTAL_WALL;

        walls_[pointToVerticalWallIndex(i, 0)] &= ~VERTICAL_WALL;
        walls_[pointToVerticalWallIndex(i, BOARD_SIZE + 1)] &= ~VERTICAL_WALL;
    }
}

Game::Game()
{
    setPlayerPosition(Player::BLUE, Point((1 + BOARD_SIZE) / 2, BOARD_SIZE));
    setPlayerPosition(Player::RED, Point((1 + BOARD_SIZE) / 2, 1));
}

Player Game::GetCurrentPlayer() const
{
    return player_;
}

Player Game::GetOpponentPlayer() const
{
    return PlayerUtils::Opponent(player_);
}

int Game::GetRemainWallCount(Player player) const
{
    return remainWallCounts_[static_cast<int>(player)];
}

Point Game::GetPlayerPosition(Player player) const
{
    return playerPositions_[static_cast<int>(player)];
}

bool Game::IsHorizontalWallPlaced(Point pos) const
{
    return wallBoard_.IsHorizontalWallPlaced(pos.X(), pos.Y());
}

bool WallBoard::IsHorizontalWallPlaced(int x, int y) const
{
    return walls_[pointToHorizontalWallIndex(x, y)] & HORIZONTAL_WALL;
}

bool Game::IsVerticalWallPlaced(Point pos) const
{
    return wallBoard_.IsVerticalWallPlaced(pos.X(), pos.Y());
}

bool WallBoard::IsVerticalWallPlaced(int x, int y) const
{
    return walls_[pointToVerticalWallIndex(x, y)] & VERTICAL_WALL;
}

Player Game::GetWinnter() const
{
    if (GetPlayerPosition(Player::BLUE).Y() == 1)
        return Player::BLUE;

    if (GetPlayerPosition(Player::RED).Y() == BOARD_SIZE)
        return Player::RED;

    // not finished
    return Player::NONE;
}

std::string Game::ToString() const
{
    std::stringstream ss;

    // build top line
    for (int x = 0; x < BOARD_SIZE + 1; ++x)
    {
        if (x == 0)
        {
            ss << "┌";
        }
        else if (x == BOARD_SIZE)
        {
            ss << "───┐";
        }
        else
        {
            ss << "───┬";
        }
    }
    ss << "\n";

    for (int y = 0; y <= BOARD_SIZE + 1; ++y)
    {
        for (int x = 0; x <= BOARD_SIZE + 1; ++x)
        {
        }
    }

    return ss.str();
}

bool Game::IsValidAction(const Action& action, Player player) const
{
    if (player == Player::NONE)
    {
        player = player_;
    }

    switch (action.GetType())
    {
        case ActionType::MOVE:
            return isValidMove(static_cast<const Actions::Move&>(action),
                               player);
        case ActionType::PLACE_HORIZONTAL_WALL:
            return isValidPlaceHorizontalWall(
                static_cast<const Actions::PlaceHorizontalWall&>(action),
                player);
        case ActionType::PLACE_VERTICAL_WALL:
            return isValidPlaceVerticalWall(
                static_cast<const Actions::PlaceVerticalWall&>(action), player);
        case ActionType::ROTATE:
            return isValidRotate(static_cast<const Actions::Rotate&>(action),
                                 player);
    }

    throw std::invalid_argument("invalid action type");
}

void Game::Play(const Action& action, Player player)
{
}

bool Game::isValidMove(const Actions::Move& action, Player player) const
{
    const Point currentPos = GetPlayerPosition(player);
    Point newPos = currentPos;

    if (action.GetDirection() == Actions::Move::Direction::UP)
    {
        if (wallBoard_.IsHorizontalWallPlaced(currentPos.X(),
                                              currentPos.Y() - 1))
        {
            return false;
        }

        newPos.Y() += 1;
    }
    else if (action.GetDirection() == Actions::Move::Direction::DOWN)
    {
        if (wallBoard_.IsHorizontalWallPlaced(currentPos.X(), currentPos.Y()))
        {
            return false;
        }

        newPos.Y() -= 1;
    }
    else if (action.GetDirection() == Actions::Move::Direction::LEFT)
    {
        if (wallBoard_.IsVerticalWallPlaced(currentPos.X() - 1, currentPos.Y()))
        {
            return false;
        }

        newPos.X() -= 1;
    }
    else if (action.GetDirection() == Actions::Move::Direction::RIGHT)
    {
        if (wallBoard_.IsVerticalWallPlaced(currentPos.X(), currentPos.Y()))
        {
            return false;
        }

        newPos.X() += 1;
    }

    const bool inBoard = (newPos.X() > 0 && newPos.X() <= BOARD_SIZE) &&
                         (newPos.Y() > 0 && newPos.X() <= BOARD_SIZE);

    return inBoard;
}

bool Game::isValidPlaceHorizontalWall(
    const Actions::PlaceHorizontalWall& action,
    [[maybe_unused]] Player player) const
{
    const Point pos = action.GetPosition();

    const bool inBoard = (pos.X() > 0 && pos.X() <= WALL_PLACEABLE_SIZE) &&
                         (pos.Y() > 0 && pos.Y() <= WALL_PLACEABLE_SIZE);
    if (!inBoard)
        return false;

    if (wallBoard_.IsVerticalWallPlaced(pos.X(), pos.Y()) &&
        wallBoard_.IsVerticalWallPlaced(pos.X(), pos.Y() + 1))
        return false;

    if (wallBoard_.IsHorizontalWallPlaced(pos.X(), pos.Y()) ||
        wallBoard_.IsHorizontalWallPlaced(pos.X() + 1, pos.Y()))
        return false;

    WallBoard tmpBoard{ wallBoard_ };
    tmpBoard.PlaceHorizontalWall(pos.X(), pos.Y());

    if (!tmpBoard.CheckReachability(GetPlayerPosition(Player::BLUE), 1) ||
        !tmpBoard.CheckReachability(GetPlayerPosition(Player::RED), BOARD_SIZE))
        return false;

    return true;
}

bool Game::isValidPlaceVerticalWall(const Actions::PlaceVerticalWall& action,
                                    [[maybe_unused]] Player player) const
{
    const Point pos = action.GetPosition();

    const bool inBoard = (pos.X() > 0 && pos.X() <= WALL_PLACEABLE_SIZE) &&
                         (pos.Y() > 0 && pos.Y() <= WALL_PLACEABLE_SIZE);
    if (!inBoard)
        return false;

    if (wallBoard_.IsVerticalWallPlaced(pos.X(), pos.Y()) ||
        wallBoard_.IsVerticalWallPlaced(pos.X(), pos.Y() + 1))
        return false;

    if (wallBoard_.IsHorizontalWallPlaced(pos.X(), pos.Y()) &&
        wallBoard_.IsHorizontalWallPlaced(pos.X() + 1, pos.Y()))
        return false;

    WallBoard tmpBoard{ wallBoard_ };
    tmpBoard.PlaceVerticalWall(pos.X(), pos.Y());

    if (!tmpBoard.CheckReachability(GetPlayerPosition(Player::BLUE), 1) ||
        !tmpBoard.CheckReachability(GetPlayerPosition(Player::RED), BOARD_SIZE))
        return false;

    return true;
}

bool Game::isValidRotate(const Actions::Rotate& action,
                         [[maybe_unused]] Player player) const
{
    const Point pos = action.GetPosition();

    const bool inBoard = (pos.X() > 0 && pos.X() <= ROTATABLE_SIZE) &&
                         (pos.Y() > 0 && pos.Y() <= ROTATABLE_SIZE);
    if (!inBoard)
        return false;

    WallBoard tmpBoard{ wallBoard_ };
    tmpBoard.Rotate(pos.X(), pos.Y());

    if (!tmpBoard.CheckReachability(GetPlayerPosition(Player::BLUE), 1) ||
        !tmpBoard.CheckReachability(GetPlayerPosition(Player::RED), BOARD_SIZE))
        return false;

    return true;
}

void Game::setPlayerPosition(Player player, Point newPosition)
{
    playerPositions_[static_cast<int>(player)] = newPosition;
}
}  // namespace fights
