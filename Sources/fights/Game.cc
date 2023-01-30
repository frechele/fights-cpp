#include <fights/Game.hpp>

#include <bitset>
#include <queue>
#include <sstream>
#include <stdexcept>

#include <iostream>

namespace fights
{
bool WallBoard::IsIntersection(int x, int y) const
{
    return intersection_[pointToIntersectionIndex(x, y)];
}

void WallBoard::PlaceHorizontalWall(int x, int y)
{
    walls_[pointToHorizontalWallIndex(x, y)] |= HORIZONTAL_WALL;
    walls_[pointToHorizontalWallIndex(x + 1, y)] |= HORIZONTAL_WALL;
    intersection_[pointToIntersectionIndex(x, y)] = true;
    clearBoundary();
}

void WallBoard::PlaceVerticalWall(int x, int y)
{
    walls_[pointToVerticalWallIndex(x, y)] |= VERTICAL_WALL;
    walls_[pointToVerticalWallIndex(x, y + 1)] |= VERTICAL_WALL;
    intersection_[pointToIntersectionIndex(x, y)] = true;
    clearBoundary();
}

void WallBoard::Rotate(int x, int y)
{
    auto tmpWall = walls_;
    auto tmpIntersection = intersection_;

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

            tmpIntersection[pointToIntersectionIndex(nx, ny)] = false;
        }
    }

    // move walls
    for (int dx = 0; dx <= 4; ++dx)
    {
        for (int dy = 0; dy <= 4; ++dy)
        {
            const int ox = x + dx - 1;
            const int oy = y + dy - 1;

            if (dx > 0 &&
                walls_[pointToHorizontalWallIndex(ox, oy)] & HORIZONTAL_WALL)
            {
                const int tx = x + 3 - dy;
                const int ty = y + dx - 1;

                tmpWall[pointToVerticalWallIndex(tx, ty)] |= VERTICAL_WALL;
            }
            if (dy > 0 &&
                walls_[pointToVerticalWallIndex(ox, oy)] & VERTICAL_WALL)
            {
                const int tx = x + 4 - dy;
                const int ty = y + dx - 1;

                tmpWall[pointToHorizontalWallIndex(tx, ty)] |= HORIZONTAL_WALL;
            }
        }
    }

    // move intersections
    for (int dx = 0; dx <= 4; ++dx)
    {
        for (int dy = 0; dy <= 4; ++dy)
        {
            const int ox = x + dx - 1;
            const int oy = y + dy - 1;

            const int tx = x + 3 - dy;
            const int ty = y + dx - 1;

            if (!IsIntersection(ox, oy))
                continue;

            if ((dx == 0 || dx == 4) && IsHorizontalWallPlaced(ox, oy) &&
                IsHorizontalWallPlaced(ox + 1, oy) && IsIntersection(ox, oy))
                continue;

            if ((dy == 0 || dy == 4) && IsVerticalWallPlaced(ox, oy) &&
                IsVerticalWallPlaced(ox, oy + 1) && IsIntersection(ox, oy))
                continue;

            tmpIntersection[pointToIntersectionIndex(tx, ty)] = true;
        }
    }

    walls_ = std::move(tmpWall);
    intersection_ = std::move(tmpIntersection);

    clearBoundary();
}

bool WallBoard::CheckReachability(Point playerPos, int target) const
{
    std::bitset<EXTENDED_BOARD_SIZE * EXTENDED_BOARD_SIZE> visited;

    std::queue<Point> Q;
    Q.emplace(playerPos);

    while (!Q.empty())
    {
        const auto pt = Q.front();
        Q.pop();
        visited.set(pointToBoardIndex(pt.X(), pt.Y()));

        if (target > playerPos.Y() && pt.Y() >= target)
            return true;
        if (target < playerPos.Y() && pt.Y() <= target)
            return true;

        if (pt.Y() > 1 && !IsHorizontalWallPlaced(pt.X(), pt.Y() - 1))
        {
            if (!visited.test(pointToBoardIndex(pt.X(), pt.Y() - 1)))
                Q.emplace(pt.X(), pt.Y() - 1);
        }
        if (pt.Y() < BOARD_SIZE && !IsHorizontalWallPlaced(pt.X(), pt.Y()))
        {
            if (!visited.test(pointToBoardIndex(pt.X(), pt.Y() + 1)))
                Q.emplace(pt.X(), pt.Y() + 1);
        }
        if (pt.X() > 1 && !IsVerticalWallPlaced(pt.X() - 1, pt.Y()))
        {
            if (!visited.test(pointToBoardIndex(pt.X() - 1, pt.Y())))
                Q.emplace(pt.X() - 1, pt.Y());
        }
        if (pt.X() < BOARD_SIZE && !IsVerticalWallPlaced(pt.X(), pt.Y()))
        {
            if (!visited.test(pointToBoardIndex(pt.X() + 1, pt.Y())))
                Q.emplace(pt.X() + 1, pt.Y());
        }
    }

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
                        if (IsIntersection(x, y))
                            ss << "╋";
                        else
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

    for (int i = 0; i <= BOARD_SIZE; ++i)
    {
        intersection_[pointToIntersectionIndex(i, 0)] = false;
        intersection_[pointToIntersectionIndex(i, BOARD_SIZE)] = false;
        intersection_[pointToIntersectionIndex(0, i)] = false;
        intersection_[pointToIntersectionIndex(i, BOARD_SIZE)] = false;
    }
}

Game::Game(bool flipped) : flipped_(flipped)
{
    constexpr int TOP_LINE = 1;
    constexpr int BOTTOM_LINE = BOARD_SIZE;

    if (flipped)
    {
        setPlayerPosition(Player::BLUE, Point((1 + BOARD_SIZE) / 2, TOP_LINE));
        setPlayerPosition(Player::RED,
                          Point((1 + BOARD_SIZE) / 2, BOTTOM_LINE));
        setPlayerTarget(Player::BLUE, BOTTOM_LINE);
        setPlayerTarget(Player::RED, TOP_LINE);
    }
    else
    {
        setPlayerPosition(Player::BLUE,
                          Point((1 + BOARD_SIZE) / 2, BOTTOM_LINE));
        setPlayerPosition(Player::RED, Point((1 + BOARD_SIZE) / 2, TOP_LINE));
        setPlayerTarget(Player::BLUE, TOP_LINE);
        setPlayerTarget(Player::RED, BOTTOM_LINE);
    }
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

int Game::GetPlayerTarget(Player player) const
{
    return playerTargets_[static_cast<int>(player)];
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

Player Game::GetWinner() const
{
    const Player goUpPlayer =
        (GetPlayerTarget(Player::BLUE) > GetPlayerTarget(Player::RED))
            ? Player::RED
            : Player::BLUE;
    const Player goDownPlayer = PlayerUtils::Opponent(goUpPlayer);

    if (GetPlayerPosition(goUpPlayer).Y() <= GetPlayerTarget(goUpPlayer))
        return goUpPlayer;

    if (GetPlayerPosition(goDownPlayer).Y() >= GetPlayerTarget(goDownPlayer))
        return goDownPlayer;

    // not finished
    return Player::NONE;
}

bool Game::IsEnd() const
{
    if (GetTurns() > 100)
        return true;

    return GetWinner() != Player::NONE;
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
                if (GetPlayerPosition(Player::BLUE) == Point(x, y))
                    ss << " B ";
                else if (GetPlayerPosition(Player::RED) == Point(x, y))
                    ss << " R ";
                else
                    ss << "   ";

                if (wallBoard_.IsVerticalWallPlaced(x, y))
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
                    if (wallBoard_.IsHorizontalWallPlaced(x, y))
                        ss << "━━━";
                    else
                        ss << "───";

                    if (x == BOARD_SIZE)
                    {
                        ss << "┤";
                    }
                    else
                    {
                        if (wallBoard_.IsIntersection(x, y))
                            ss << "╋";
                        else
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
    ss << "\n"
       << "remain walls: [blue] " << GetRemainWallCount(Player::BLUE)
       << " [red] " << GetRemainWallCount(Player::RED) << "\n"
       << "current player: " << PlayerUtils::ToString(GetCurrentPlayer())
       << "\n";

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
    if (!IsValidAction(action, player))
    {
        throw std::invalid_argument("invalid action");
    }

    if (player == Player::NONE)
    {
        player = player_;
    }

    switch (action.GetType())
    {
        case ActionType::MOVE:
            playMove(static_cast<const Actions::Move&>(action), player);
            break;

        case ActionType::PLACE_HORIZONTAL_WALL:
            playPlaceHorizontalWall(
                static_cast<const Actions::PlaceHorizontalWall&>(action),
                player);
            break;

        case ActionType::PLACE_VERTICAL_WALL:
            playPlaceVerticalWall(
                static_cast<const Actions::PlaceVerticalWall&>(action), player);
            break;

        case ActionType::ROTATE:
            playRotate(static_cast<const Actions::Rotate&>(action), player);
            break;
    }

    history_.emplace_back(const_cast<Action*>(&action));
    player_ = PlayerUtils::Opponent(player);
    ++turns_;
}

int Game::GetTurns() const
{
    return turns_;
}

const std::vector<Action*> Game::GetHistory() const
{
    return history_;
}

bool Game::isValidMove(const Actions::Move& action, Player player) const
{
    const Point currentPos = GetPlayerPosition(player);
    const Point opponentPos = GetPlayerPosition(PlayerUtils::Opponent(player));
    Point newPos = currentPos;

    if (action.GetDirection() == Actions::Move::Direction::UP)
    {
        if (wallBoard_.IsHorizontalWallPlaced(currentPos.X(),
                                              currentPos.Y() - 1))
        {
            return false;
        }

        newPos.Y() -= 1;

        if (opponentPos == newPos)
        {
            if (newPos.Y() > 1 &&
                wallBoard_.IsHorizontalWallPlaced(newPos.X(), newPos.Y() - 1))
            {
                return false;
            }

            newPos.Y() -= 1;
        }
    }
    else if (action.GetDirection() == Actions::Move::Direction::DOWN)
    {
        if (wallBoard_.IsHorizontalWallPlaced(currentPos.X(), currentPos.Y()))
        {
            return false;
        }

        newPos.Y() += 1;

        if (opponentPos == newPos)
        {
            if (newPos.Y() < BOARD_SIZE &&
                wallBoard_.IsHorizontalWallPlaced(newPos.X(), newPos.Y()))
            {
                return false;
            }

            newPos.Y() += 1;
        }
    }
    else if (action.GetDirection() == Actions::Move::Direction::LEFT)
    {
        if (wallBoard_.IsVerticalWallPlaced(currentPos.X() - 1, currentPos.Y()))
        {
            return false;
        }

        newPos.X() -= 1;

        if (opponentPos == newPos)
        {
            if (newPos.X() > 1 &&
                wallBoard_.IsVerticalWallPlaced(newPos.X() - 1, newPos.Y()))
            {
                return false;
            }

            newPos.X() -= 1;
        }
    }
    else if (action.GetDirection() == Actions::Move::Direction::RIGHT)
    {
        if (wallBoard_.IsVerticalWallPlaced(currentPos.X(), currentPos.Y()))
        {
            return false;
        }

        newPos.X() += 1;

        if (opponentPos == newPos)
        {
            if (newPos.X() < BOARD_SIZE &&
                wallBoard_.IsVerticalWallPlaced(newPos.X(), newPos.Y()))
            {
                return false;
            }

            newPos.X() += 1;
        }
    }
    else if (action.GetDirection() == Actions::Move::Direction::L_UP)
    {
        if (opponentPos == Point(currentPos.X() - 1, currentPos.Y()) &&
            !wallBoard_.IsVerticalWallPlaced(
                currentPos.X() - 1, currentPos.Y()))  // left first case
        {
            if (currentPos.X() <= 2 || !wallBoard_.IsVerticalWallPlaced(
                                           currentPos.X() - 2, currentPos.Y()))
            {
                return false;
            }
        }
        else if (opponentPos == Point(currentPos.X(), currentPos.Y() - 1) &&
                 !wallBoard_.IsHorizontalWallPlaced(
                     currentPos.X(),
                     currentPos.Y() - 1))  // up first case
        {
            if (currentPos.Y() <= 2 || !wallBoard_.IsHorizontalWallPlaced(
                                           currentPos.X(), currentPos.Y() - 2))
            {
                return false;
            }
        }
        else
        {
            return false;
        }

        newPos.X() -= 1;
        newPos.Y() -= 1;
    }
    else if (action.GetDirection() == Actions::Move::Direction::R_UP)
    {
        if (opponentPos == Point(currentPos.X() + 1, currentPos.Y()) &&
            !wallBoard_.IsVerticalWallPlaced(
                currentPos.X(), currentPos.Y()))  // right first case
        {
            if (currentPos.X() >= WALL_PLACEABLE_SIZE ||
                !wallBoard_.IsVerticalWallPlaced(currentPos.X() + 1,
                                                 currentPos.Y()))
            {
                return false;
            }
        }
        else if (opponentPos == Point(currentPos.X(), currentPos.Y() - 1) &&
                 !wallBoard_.IsHorizontalWallPlaced(
                     currentPos.X(),
                     currentPos.Y() - 1))  // up first case
        {
            if (currentPos.Y() <= 2 || !wallBoard_.IsHorizontalWallPlaced(
                                           currentPos.X(), currentPos.Y() - 2))
            {
                return false;
            }
        }
        else
        {
            return false;
        }

        newPos.X() += 1;
        newPos.Y() -= 1;
    }
    else if (action.GetDirection() == Actions::Move::Direction::L_DOWN)
    {
        if (opponentPos == Point(currentPos.X() - 1, currentPos.Y()) &&
            !wallBoard_.IsVerticalWallPlaced(
                currentPos.X() - 1, currentPos.Y()))  // left first case
        {
            if (currentPos.X() <= 2 || !wallBoard_.IsVerticalWallPlaced(
                                           currentPos.X() - 2, currentPos.Y()))
            {
                return false;
            }
        }
        else if (opponentPos == Point(currentPos.X(), currentPos.Y() + 1) &&
                 !wallBoard_.IsHorizontalWallPlaced(
                     currentPos.X(),
                     currentPos.Y()))  // down first case
        {
            if (currentPos.Y() >= WALL_PLACEABLE_SIZE ||
                !wallBoard_.IsHorizontalWallPlaced(currentPos.X(),
                                                   currentPos.Y() + 1))
            {
                return false;
            }
        }
        else
        {
            return false;
        }

        newPos.X() -= 1;
        newPos.Y() += 1;
    }
    else if (action.GetDirection() == Actions::Move::Direction::R_DOWN)
    {
        if (opponentPos == Point(currentPos.X() + 1, currentPos.Y()) &&
            !wallBoard_.IsVerticalWallPlaced(
                currentPos.X(), currentPos.Y()))  // right first case
        {
            if (currentPos.X() >= WALL_PLACEABLE_SIZE ||
                !wallBoard_.IsVerticalWallPlaced(currentPos.X() + 1,
                                                 currentPos.Y()))
            {
                return false;
            }
        }
        else if (opponentPos == Point(currentPos.X(), currentPos.Y() + 1) &&
                 !wallBoard_.IsHorizontalWallPlaced(
                     currentPos.X(),
                     currentPos.Y()))  // down first case
        {
            if (currentPos.Y() >= WALL_PLACEABLE_SIZE ||
                !wallBoard_.IsHorizontalWallPlaced(currentPos.X(),
                                                   currentPos.Y() + 1))
            {
                return false;
            }
        }
        else
        {
            return false;
        }

        newPos.X() += 1;
        newPos.Y() += 1;
    }

    const bool inBoard = (newPos.X() > 0 && newPos.X() <= BOARD_SIZE) &&
                         (newPos.Y() > 0 && newPos.Y() <= BOARD_SIZE);

    return inBoard;
}

bool Game::isValidPlaceHorizontalWall(
    const Actions::PlaceHorizontalWall& action, Player player) const
{
    if (GetRemainWallCount(player) < 1)
        return false;

    const Point pos = action.GetPosition();

    const bool inBoard = (pos.X() > 0 && pos.X() <= WALL_PLACEABLE_SIZE) &&
                         (pos.Y() > 0 && pos.Y() <= WALL_PLACEABLE_SIZE);
    if (!inBoard)
        return false;

    if (wallBoard_.IsIntersection(pos.X(), pos.Y()))
        return false;

    if (wallBoard_.IsHorizontalWallPlaced(pos.X(), pos.Y()) ||
        wallBoard_.IsHorizontalWallPlaced(pos.X() + 1, pos.Y()))
        return false;

    WallBoard tmpBoard{ wallBoard_ };
    tmpBoard.PlaceHorizontalWall(pos.X(), pos.Y());

    if (!tmpBoard.CheckReachability(GetPlayerPosition(Player::BLUE),
                                    GetPlayerTarget(Player::BLUE)) ||
        !tmpBoard.CheckReachability(GetPlayerPosition(Player::RED),
                                    GetPlayerTarget(Player::RED)))
        return false;

    return true;
}

bool Game::isValidPlaceVerticalWall(const Actions::PlaceVerticalWall& action,
                                    Player player) const
{
    if (GetRemainWallCount(player) < 1)
        return false;

    const Point pos = action.GetPosition();

    const bool inBoard = (pos.X() > 0 && pos.X() <= WALL_PLACEABLE_SIZE) &&
                         (pos.Y() > 0 && pos.Y() <= WALL_PLACEABLE_SIZE);
    if (!inBoard)
        return false;

    if (wallBoard_.IsIntersection(pos.X(), pos.Y()))
        return false;

    if (wallBoard_.IsVerticalWallPlaced(pos.X(), pos.Y()) &&
        wallBoard_.IsVerticalWallPlaced(pos.X(), pos.Y() + 1))
        return false;

    WallBoard tmpBoard{ wallBoard_ };
    tmpBoard.PlaceVerticalWall(pos.X(), pos.Y());

    if (!tmpBoard.CheckReachability(GetPlayerPosition(Player::BLUE),
                                    GetPlayerTarget(Player::BLUE)) ||
        !tmpBoard.CheckReachability(GetPlayerPosition(Player::RED),
                                    GetPlayerTarget(Player::RED)))
        return false;

    return true;
}

bool Game::isValidRotate(const Actions::Rotate& action, Player player) const
{
    if (GetRemainWallCount(player) < 2)
        return false;

    const Point pos = action.GetPosition();

    const bool inBoard = (pos.X() > 0 && pos.X() <= ROTATABLE_SIZE) &&
                         (pos.Y() > 0 && pos.Y() <= ROTATABLE_SIZE);
    if (!inBoard)
        return false;

    WallBoard tmpBoard{ wallBoard_ };
    tmpBoard.Rotate(pos.X(), pos.Y());

    if (!tmpBoard.CheckReachability(GetPlayerPosition(Player::BLUE),
                                    GetPlayerTarget(Player::BLUE)) ||
        !tmpBoard.CheckReachability(GetPlayerPosition(Player::RED),
                                    GetPlayerTarget(Player::RED)))
        return false;

    return true;
}

void Game::playMove(const Actions::Move& action, Player player)
{
    Point pt = GetPlayerPosition(player);
    const Point opponentPt = GetPlayerPosition(PlayerUtils::Opponent(player));

    if (action.GetDirection() == Actions::Move::Direction::LEFT ||
        action.GetDirection() == Actions::Move::Direction::L_UP ||
        action.GetDirection() == Actions::Move::Direction::L_DOWN)
    {
        pt.X() -= 1;

        if (action.GetDirection() == Actions::Move::Direction::LEFT &&
            opponentPt == pt)
        {
            pt.X() -= 1;
        }
    }

    if (action.GetDirection() == Actions::Move::Direction::RIGHT ||
        action.GetDirection() == Actions::Move::Direction::R_UP ||
        action.GetDirection() == Actions::Move::Direction::R_DOWN)
    {
        pt.X() += 1;

        if (action.GetDirection() == Actions::Move::Direction::RIGHT &&
            opponentPt == pt)
        {
            pt.X() += 1;
        }
    }

    if (action.GetDirection() == Actions::Move::Direction::UP ||
        action.GetDirection() == Actions::Move::Direction::L_UP ||
        action.GetDirection() == Actions::Move::Direction::R_UP)
    {
        pt.Y() -= 1;

        if (action.GetDirection() == Actions::Move::Direction::UP &&
            opponentPt == pt)
        {
            pt.Y() -= 1;
        }
    }

    if (action.GetDirection() == Actions::Move::Direction::DOWN ||
        action.GetDirection() == Actions::Move::Direction::L_DOWN ||
        action.GetDirection() == Actions::Move::Direction::R_DOWN)
    {
        pt.Y() += 1;

        if (action.GetDirection() == Actions::Move::Direction::DOWN &&
            opponentPt == pt)
        {
            pt.Y() += 1;
        }
    }

    setPlayerPosition(player, pt);
}

void Game::playPlaceHorizontalWall(const Actions::PlaceHorizontalWall& action,
                                   Player player)
{
    getRemainWallCount(player) -= 1;

    const Point pt = action.GetPosition();
    wallBoard_.PlaceHorizontalWall(pt.X(), pt.Y());
}

void Game::playPlaceVerticalWall(const Actions::PlaceVerticalWall& action,
                                 Player player)
{
    getRemainWallCount(player) -= 1;

    const Point pt = action.GetPosition();
    wallBoard_.PlaceVerticalWall(pt.X(), pt.Y());
}

void Game::playRotate(const Actions::Rotate& action, Player player)
{
    getRemainWallCount(player) -= 2;

    const Point pt = action.GetPosition();
    wallBoard_.Rotate(pt.X(), pt.Y());
}

void Game::setPlayerPosition(Player player, Point newPosition)
{
    playerPositions_[static_cast<int>(player)] = newPosition;
}

void Game::setPlayerTarget(Player player, int newTarget)
{
    playerTargets_[static_cast<int>(player)] = newTarget;
}

int& Game::getRemainWallCount(Player player)
{
    return remainWallCounts_[static_cast<int>(player)];
}
}  // namespace fights
