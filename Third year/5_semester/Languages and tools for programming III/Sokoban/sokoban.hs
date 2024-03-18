{-# LANGUAGE OverloadedStrings #-}
-- Wyświetlanie planszy.

import qualified Data.Text
import System.IO
type Program = IO () -- program wykonuje IO i nie daje wartości 

main :: Program
main = program

program :: Program
program = etap5

type DrawFun = Integer -> Integer -> Char
type Picture = DrawFun -> DrawFun
blank = id
(&) = (.)

-- Definicja elementów graficznych planszy.

wall, ground, storage :: Picture
wall drawFun 0 0 = '#'
wall drawFun x y = drawFun x y

ground drawFun 0 0 = ' '
ground drawFun x y = drawFun x y

storage drawFun 0 0 = '.'
storage drawFun x y = drawFun x y

box drawFun 0 0 = '$'
box drawFun x y = drawFun x y

-- Graficzna definicja wskaźnika gracza.

player :: Picture
player drawFun 0 0 = '@'
player drawFun x y = drawFun x y

-- Definicja przykładowego poziomu.

data Tile = Wall | Ground | Storage | Box | Blank deriving Eq

drawTile :: Tile -> Picture
drawTile Wall    = wall
drawTile Ground  = ground
drawTile Storage = storage
drawTile Box     = box
drawTile Blank   = blank

type MazeF = Coord -> Tile

translated :: Integer -> Integer -> Picture -> Picture
translated mx my picture = picture'
    where picture' fun x y = picture (\x' y' -> fun (x' + mx) (y' - my)) (x - mx) (y + my)

pictures :: [Picture] -> Picture
pictures = foldr (&) blank

pictureOfMaze :: Maze -> Picture
pictureOfMaze (Maze coord mazeF) = pictures[translated (fromIntegral x) (fromIntegral y) (drawTile (mazeF (C x y))) | (C x y) <- getReachable [coord] [] (mazeNeighbors mazeF)]

data Direction = R | U | L | D deriving Eq

data Coord = C Int Int deriving Eq

-- Funkcje do przesuwania.

atCoord :: Coord -> Picture -> Picture
atCoord (C x y) = translated (fromIntegral x) (fromIntegral y)

adjacentCoord :: Direction -> Coord -> Coord
adjacentCoord R (C x y) = C (x + 1) y
adjacentCoord U (C x y) = C x (y + 1)
adjacentCoord L (C x y) = C (x - 1) y
adjacentCoord D (C x y) = C x (y - 1)

-- Ekran startowy i resetowanie poziomu.

convertToScreen :: String -> Screen
convertToScreen s = replicate 11 '\n' ++ replicate ((78 - length s) `div` 2) ' ' ++ s ++ replicate 12 '\n'

startScreen :: Screen
startScreen = convertToScreen "To start press space"

data SSState world = StartScreen | Running world

type Screen = String
data Event = KeyPress String

data Activity world = Activity {
    actState  :: world,
    actHandle :: Event -> world -> world,
    actDraw   :: world -> Screen
    }

resettable :: Activity s -> Activity s
resettable (Activity state0 handle draw)
  = Activity state0 handle' draw
  where handle' (KeyPress key) _ | key == "Esc" = state0
        handle' e s = handle e s

withStartScreen :: Activity s -> Activity (SSState s)
withStartScreen (Activity state0 handle draw)
  = Activity state0' handle' draw'
  where
    state0' = StartScreen

    handle' (KeyPress key) StartScreen
         | key == " "                  = Running state0
    handle' _              StartScreen = StartScreen
    handle' e              (Running s) = Running (handle e s)

    draw' StartScreen = startScreen
    draw' (Running s) = draw s

cleanWrite :: String -> IO ()
cleanWrite s = putStr "\ESC[1K" >> hFlush stdout >> putStr "\ESCc" >> putStr s >> hFlush stdout

runActivity :: Activity s -> IO ()
runActivity (Activity state0 handle draw) = do
    hSetBuffering stdin NoBuffering
    hSetBuffering stdout (BlockBuffering (Just (23*79-1)))
    input <- getContents
    cleanWrite (draw state0)
    go input state0
  where
    matched key ps state = let newState = handle (KeyPress key) state in cleanWrite (draw newState) >> go ps newState
    go (' ':ps) state = matched " " ps state
    go ('n':ps) state = matched "N" ps state
    go ('w':ps) state = matched "Up" ps state
    go ('d':ps) state = matched "Right" ps state
    go ('a':ps) state = matched "Left" ps state
    go ('s':ps) state = matched "Down" ps state
    go ('\ESC':'[':'A':ps) state = matched "Up" ps state
    go ('\ESC':'[':'C':ps) state = matched "Right" ps state
    go ('\ESC':'[':'D':ps) state = matched "Left" ps state
    go ('\ESC':'[':'B':ps) state = matched "Down" ps state
    go ('u':ps) state = matched "U" ps state
    go ('r':ps) state = matched "Esc" ps state
    go ('q':ps) state = return ()
    go (_:ps) state = cleanWrite (draw state) >> go ps state
    go [] state = return ()

-- Obsługa logiki gry.

data State = S Coord Direction [Coord] Integer Integer deriving Eq

getMazeF :: Integer -> MazeF
getMazeF n = mazeF where (Maze _ mazeF) = nth mazes n

initialBoxes :: Maze -> [Coord]
initialBoxes (Maze coord mazeF) = filter (\x -> mazeF x == Box) (getReachable [coord] [] (mazeNeighboursWalled mazeF))

initialCoord :: Maze -> Coord
initialCoord (Maze coord _) = coord

getStartState :: Integer -> State
getStartState n = S (initialCoord maze) R (initialBoxes maze) n 0 where maze = nth mazes n

initialState :: State
initialState = getStartState 0

removeBoxes :: MazeF -> MazeF
removeBoxes m = f
  where
    isReachable coord = mazeNeighboursWalled m coord /= []
    f coord
      | m coord == Box && isReachable coord = Ground
      | otherwise = m coord

addBoxes :: [Coord] -> MazeF -> MazeF
addBoxes boxes mazeF = mazeF' where mazeF' b = if elem b boxes then Box else mazeF b

winningScreen :: Integer -> Screen
winningScreen x = convertToScreen ("Level complete, number of moves: " ++ show x)

drawPicture :: Picture -> Screen
drawPicture picture = [picture (\x y -> if x == 39 then '\n' else ' ') x y | y <- [-11 .. 11], x <- [-39 .. 39]]

draw :: State -> Screen
draw (S coord direction boxes n moves)
  | isWinning (S coord direction boxes n moves) = winningScreen moves
  | otherwise = drawPicture (atCoord coord player & pictureOfMaze (Maze (C 0 0) (addBoxes boxes (removeBoxes (getMazeF n)))))

checkMove :: Coord -> State -> State
checkMove _ (S coord direction boxes n moves)
  | checkCoord coord == Ground || checkCoord coord == Storage = S coord direction boxes n (moves + 1)
  | checkCoord coord == Box && checkBox coord = S coord direction moveBox n (moves + 1)
  where checkCoord c = addBoxes boxes (removeBoxes (getMazeF n)) c
        checkBox c = let tile = checkCoord (adjacentCoord direction c) in tile == Ground || tile == Storage
        moveBox = map (\c -> if c == coord then adjacentCoord direction c else c) boxes
checkMove old (S _ direction boxes n moves) = S old direction boxes n moves

handleEvent :: Event -> State -> State
handleEvent (KeyPress key) (S coord direction boxes n moves)
  | key == "N" = nextLevel
  | isWinning (S coord direction boxes n moves) && key == " " = nextLevel
  | isWinning (S coord direction boxes n moves) = S coord direction boxes n moves
  | key == "Up" = tryMove U
  | key == "Right" = tryMove R
  | key == "Down" = tryMove D
  | key == "Left" = tryMove L
  where tryMove d = checkMove coord (S (adjacentCoord d coord) d boxes n moves)
        nextLevel = getStartState (mod (n+1) (fromIntegral (length mazes)))
handleEvent _ s = s

isWinning :: State -> Bool
isWinning (S _ _ boxes n moves) = all (\c -> getMazeF n c == Storage) boxes

data WithUndo a = WithUndo a [a]

withUndo :: Eq a => Activity a -> Activity (WithUndo a)
withUndo (Activity state0 handle draw) = Activity state0' handle' draw' where
    state0' = WithUndo state0 []
    handle' (KeyPress key) (WithUndo s stack) | key == "U"
      = case stack of s':stack' -> WithUndo s' stack'
                      []        -> WithUndo s []
      | key == "N" || key == " " = WithUndo (handle (KeyPress key) s) []
    handle' e              (WithUndo s stack)
       | s' == s = WithUndo s stack
       | otherwise = WithUndo (handle e s) (s:stack)
      where s' = handle e s
    draw' (WithUndo s _) = draw s

data Maze = Maze Coord MazeF
level0 :: Maze
level0 = Maze (C (-2) 0) level0F
level0F :: MazeF
level0F (C x y)
  | abs x > 3 || abs y > 1   = Blank
  | abs x == 3 || abs y == 1 = Wall
  | x == 0 && y == 0         = Box
  | x == 2 && y == 0         = Storage
  | otherwise                = Ground

level1 :: Maze
level1 = Maze (C 2 0) level1F
level1F :: MazeF
level1F (C x y)
  | abs x > 3 || abs y > 3   = Blank
  | abs x == 3 || abs y == 3 = Wall
  | abs y == 2 && x == 0     = Wall
  | y == 0 && x == 1         = Wall
  | x == 0                   = Box
  | x == -2 && abs y == 2    = Storage
  | x == -2 && y == 0        = Storage
  | otherwise                = Ground

level2 :: Maze
level2 = Maze (C (-3) 3) level2F
level2F :: MazeF
level2F (C x y)
  | abs x > 4  || abs y > 4  = Blank
  | abs x == 4 || abs y == 4 = Wall
  | x ==  2 && y <= 0        = Wall
  | x ==  3 && y <= 0        = Storage
  | x >= -2 && y == 0        = Box
  | otherwise                = Ground

level3 :: Maze
level3 = Maze (C 0 3) level3F
level3F :: MazeF
level3F (C x y)
  | abs x > 3  || y > 4 || y < -5     = Blank
  | abs x == 3 || y == 4 || y == -5   = Wall
  | x ==  0 && y == -4                = Wall
  | abs x == 2 && (y == 0 || y == 1)  = Wall
  | x == 0 && (y == -1 || y == 2)     = Storage
  | x == 0 && y /= 3                  = Box
  | abs x == 1 && (y == 2 || y == -1) = Box
  | abs x == 1 && y == 1              = Storage
  | abs x == 1 && y == -2             = Storage
  | abs x == 1 && y == -3             = Storage
  | otherwise                         = Ground

level4 :: Maze
level4 = Maze (C (-3) 3) level4F
level4F :: MazeF
level4F (C x y)
  | abs x > 4  || abs y > 4  = Blank
  | abs x == 4 || abs y == 4 = Wall
  | x ==  2 && y <= 0        = Wall
  | x ==  3 && y <= 0        = Storage
  | x >= -2 && y == 0        = Box
  | x == -3 && y == -3       = Box
  | x == -3 && y == -2       = Wall
  | x == -2 && y == -3       = Wall
  | otherwise                = Ground

badLevel0 :: Maze
badLevel0 = Maze (C (-1) 0) badLevel0F
badLevel0F :: MazeF
badLevel0F (C x y)
  | x == -2 && y == 0 = Blank
  | otherwise        = level0F (C x y)

badLevel1 :: Maze
badLevel1 = Maze (C 2 0) badLevel1F
badLevel1F :: MazeF
badLevel1F (C x y)
  | x == -1 && abs y <= 1 = Wall
  | otherwise            = level1F (C x y)

badLevel2 :: Maze
badLevel2 = Maze (C 2 0) badLevel2F
badLevel2F :: MazeF
badLevel2F (C x y)
  | x == 3 && y == 0     = Blank
  | otherwise            = badLevel1F (C x y)

badLevel3 :: Maze
badLevel3 = Maze (C 2 0) badLevel3F
badLevel3F :: MazeF
badLevel3F (C x y)
  | x == 2 && y == 0 = Wall
  | otherwise = level1F (C x y)

mazes :: [Maze]
mazes = [level0, level1, level2, level3, level4]

badMazes :: [Maze]
badMazes = [badLevel0, badLevel1, badLevel2, badLevel3]

elemList :: Eq a => a -> [a] -> Bool
elemList a [] = False
elemList a (b:bs) = a == b || elemList a bs

appendList :: [a] -> [a] -> [a]
appendList [] y = y
appendList (x:xs) y = x : appendList xs y

listLength :: [a] -> Integer
listLength [] = 0
listLength (x:xs) = 1 + listLength xs

filterList :: (a -> Bool) -> [a] -> [a]
filterList _ [] = []
filterList f (x:xs) = if f x then x : filterList f xs else filterList f xs

nth :: [a] -> Integer -> a
nth (x:xs) 0 = x
nth (x:xs) n = nth xs (n-1)

mapList :: (a -> b) -> [a] -> [b]
mapList _ [] = []
mapList f (x:xs) = f x : mapList f xs

andList :: [Bool] -> Bool
andList [] = True
andList (x:xs) = x && andList xs

allList :: (a-> Bool) -> [a] -> Bool
allList _ [] = True
allList f (x:xs) = f x && allList f xs

foldList :: (a -> b -> b) -> b -> [a] -> b
foldList f b [] = b
foldList f b (x:xs) = foldList f (f x b) xs

getReachable :: Eq a => [a] -> [a] -> (a -> [a]) -> [a]
getReachable [] visited _ = visited
getReachable (v:vs) visited neighbors
  | elem v visited = getReachable vs visited neighbors
  | otherwise = getReachable (vs ++ neighbors v) (v:visited) neighbors

isGraphClosed :: Eq a => a -> (a -> [a]) -> (a -> Bool) -> Bool
isGraphClosed initial neighbors isOk = all isOk (getReachable [initial] [] neighbors)

reachable :: Eq a => a -> a -> (a -> [a]) -> Bool
reachable v initial neighbours = elem v (getReachable [initial] [] neighbours)

allReachable :: Eq a => [a] -> a -> (a -> [a]) -> Bool
allReachable vs initial neighbours = all (\x -> reachable x initial neighbours) vs

mazeNeighbors :: MazeF -> Coord -> [Coord]
mazeNeighbors mazeF coord
  | mazeF coord == Blank = []
  | otherwise = [adjacentCoord x coord | x <- [R,U,L,D]]

mazeNeighboursWalled :: MazeF -> Coord -> [Coord]
mazeNeighboursWalled mazeF coord = filter (\x -> mazeF x /= Wall) (mazeNeighbors mazeF coord)

isClosed :: Maze -> Bool
isClosed (Maze coord mazeF)
  | start /= Ground && start /= Storage = False
  | otherwise = isGraphClosed coord (mazeNeighboursWalled mazeF) (\x -> mazeF x /= Blank)
  where start = mazeF coord

isSane :: Maze -> Bool
isSane (Maze coord mazeF) = countTile Storage >= countTile Box
  where reachableList = getReachable [coord] [] (mazeNeighboursWalled mazeF)
        countTile tile = length (filter (\x -> mazeF x == tile) reachableList)

etap5 :: IO()
etap5 = runActivity (resettable (withStartScreen (withUndo (Activity initialState handleEvent draw))))