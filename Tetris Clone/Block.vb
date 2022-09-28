Public Class Block
    ' This class represents the very element in the game of tertis; the blocks. A block is 
    '   composed of four squares and can do either of the following in a given moment: Free-Fall, 
    '   move horizontally to the right or the left, and finally accelarated fall. Quite obviously
    '   a block knows how to draw itself since a square knows how to draw.

    ' the following constant/enum defines the various possible blocks in this game
    Public Enum BLOCK_TYPE
        UNDEFINED = 0
        SQUARE = 1
        LINE = 2
        J = 3
        L = 4
        T = 5
        Z = 6
        S = 7
    End Enum

    ' the following enumuration is used for rotation; there are four ways a block can rotate
    '   NORTH, EAST, SOUTH AND WEST.
    Public Enum ROTATE_BLOCK
        NORTH = 1
        EAST = 2
        SOUTH = 3
        WEST = 4
    End Enum

    Public location As Point                    ' the x,y point of this block
    Public square_size As Integer = 20          ' the dimensions of the tetris squares (10x10 pixels).

    Private inner_color As Color                ' the colors of the block
    Private border_color As Color

    ' the squares that compose/form a block
    Public square1 As Square
    Public square2 As Square
    Public square3 As Square
    Public square4 As Square

    ' declare an instance of the constant's block types and rotate
    Public BlockType As BLOCK_TYPE
    Public rotation_status As ROTATE_BLOCK = ROTATE_BLOCK.NORTH     ' the default direction

    ' declare array of colors to be used for filling our blocks
    Private border_colors() As Color = {Nothing, Color.DarkRed, Color.DarkOrange, Color.DarkViolet, Color.DarkBlue, _
                                        Color.DarkGreen, Color.Purple, Color.Indigo}
    Private inner_colors() As Color = {Nothing, Color.LightPink, Color.LightGoldenrodYellow, Color.Beige, Color.Aqua, _
                                        Color.LightGreen, Color.PaleVioletRed, Color.Violet}

    Public Sub New(ByVal location As Point, Optional ByVal new_block_type As BLOCK_TYPE = BLOCK_TYPE.UNDEFINED)
        ' this function creates a new block randomly at the loction passed by the parameter. It can also be forced
        '   to produce a specific type of block each time its called by passing the block_type in the optional secondary
        '   parameter.

        ' unless the caller told you so, please randomly choose a block type 
        '   and...
        If new_block_type = BLOCK_TYPE.UNDEFINED Then
            Randomize()
            BlockType = Int(Rnd(7) * 7) + 1
        Else
            BlockType = new_block_type          ' use caller specifed block type
        End If

        ' ... create a new block based on the blocktype
        square1 = New Square(New Size(square_size, square_size), inner_colors(BlockType), border_colors(BlockType))
        square2 = New Square(New Size(square_size, square_size), inner_colors(BlockType), border_colors(BlockType))
        square3 = New Square(New Size(square_size, square_size), inner_colors(BlockType), border_colors(BlockType))
        square4 = New Square(New Size(square_size, square_size), inner_colors(BlockType), border_colors(BlockType))

        ' now set the locations: this is highly dependant on the blocktype
        Select Case BlockType

            ' create a squared block
        Case BLOCK_TYPE.SQUARE
                square1.location = New Point(location.X, location.Y)
                square2.location = New Point(location.X + square_size, location.Y)
                square3.location = New Point(location.X, location.Y + square_size)
                square4.location = New Point(location.X + square_size, location.Y + square_size)

                ' create the line or bar block
            Case BLOCK_TYPE.LINE
                square1.location = New Point(location.X, location.Y)
                square2.location = New Point(location.X, location.Y + square_size)
                square3.location = New Point(location.X, location.Y + square_size * 2)
                square4.location = New Point(location.X, location.Y + square_size * 3)

                ' create the J block
            Case BLOCK_TYPE.J
                square1.location = New Point(location.X, location.Y)
                square2.location = New Point(location.X, location.Y + square_size)
                square3.location = New Point(location.X, location.Y + square_size * 2)
                square4.location = New Point(location.X - square_size, location.Y + square_size * 2)

                ' create the L block
            Case BLOCK_TYPE.L
                square1.location = New Point(location.X, location.Y)
                square2.location = New Point(location.X, location.Y + square_size)
                square3.location = New Point(location.X, location.Y + square_size * 2)
                square4.location = New Point(location.X + square_size, location.Y + square_size * 2)

                ' create the Z block
            Case BLOCK_TYPE.Z
                square1.location = New Point(location.X, location.Y)
                square2.location = New Point(location.X + square_size, location.Y)
                square3.location = New Point(location.X + square_size * 2, location.Y + square_size)
                square4.location = New Point(location.X + square_size, location.Y + square_size)

                ' create the S block
            Case BLOCK_TYPE.S
                square1.location = New Point(location.X, location.Y)
                square2.location = New Point(location.X - square_size, location.Y)
                square3.location = New Point(location.X - square_size * 2, location.Y + square_size)
                square4.location = New Point(location.X - square_size, location.Y + square_size)

                ' create the final T block
            Case BLOCK_TYPE.T
                square1.location = New Point(location.X, location.Y)
                square2.location = New Point(location.X + square_size, location.Y)
                square3.location = New Point(location.X + square_size * 2, location.Y)
                square4.location = New Point(location.X + square_size, location.Y + square_size)

        End Select

    End Sub

    Public Function Down() As Boolean
        ' this function moves the block down by 1 square each time it's called.
        Down = True         ' VB's weird way of return

        ' now test if the current block can fall down
        If Game_Feild.Is_Empty(square1.location.X / square_size, square1.location.Y / square_size + 1) And _
            Game_Feild.Is_Empty(square2.location.X / square_size, square2.location.Y / square_size + 1) And _
            Game_Feild.Is_Empty(square3.location.X / square_size, square3.location.Y / square_size + 1) And _
            Game_Feild.Is_Empty(square4.location.X / square_size, square4.location.Y / square_size + 1) Then

            ' this block can move down; hence perform the falling act
            '   first aside though; hide the block
            Hide(Game_Feild.win_handle)

            ' now fall
            square1.location = New Point(square1.location.X, square1.location.Y + square_size)
            square2.location = New Point(square2.location.X, square2.location.Y + square_size)
            square3.location = New Point(square3.location.X, square3.location.Y + square_size)
            square4.location = New Point(square4.location.X, square4.location.Y + square_size)

            ' finally show the block
            Show(Game_Feild.win_handle)

        Else
            ' this block cant fall any further stop all the squares
            Down = False
            Game_Feild.Stop_Square(square1, square1.location.X / square_size, square1.location.Y / square_size)
            Game_Feild.Stop_Square(square2, square2.location.X / square_size, square2.location.Y / square_size)
            Game_Feild.Stop_Square(square3, square3.location.X / square_size, square3.location.Y / square_size)
            Game_Feild.Stop_Square(square4, square4.location.X / square_size, square4.location.Y / square_size)

        End If

    End Function

    Public Function Right() As Boolean
        ' this function moves the block one square to the right each time it's called
        Right = True

        ' now test if the current block can move right
        If Game_Feild.Is_Empty(square1.location.X / square_size + 1, square1.location.Y / square_size) And _
            Game_Feild.Is_Empty(square2.location.X / square_size + 1, square2.location.Y / square_size) And _
            Game_Feild.Is_Empty(square3.location.X / square_size + 1, square3.location.Y / square_size) And _
            Game_Feild.Is_Empty(square4.location.X / square_size + 1, square4.location.Y / square_size) Then

            ' this block can move down; hence perform the right movement
            '   first aside though; hide the block
            Hide(Game_Feild.win_handle)

            square1.location = New Point(square1.location.X + square_size, square1.location.Y)
            square2.location = New Point(square2.location.X + square_size, square2.location.Y)
            square3.location = New Point(square3.location.X + square_size, square3.location.Y)
            square4.location = New Point(square4.location.X + square_size, square4.location.Y)

            ' finally show the blocks
            Show(Game_Feild.win_handle)

        Else
            ' the block cannot move right any further
            Right = False

        End If

    End Function

    Public Function Left() As Boolean
        ' this function moves the block one square to the left each time it's called
        Left = True

        ' now test if the current block can move left
        If Game_Feild.Is_Empty(square1.location.X / square_size - 1, square1.location.Y / square_size) And _
            Game_Feild.Is_Empty(square2.location.X / square_size - 1, square2.location.Y / square_size) And _
            Game_Feild.Is_Empty(square3.location.X / square_size - 1, square3.location.Y / square_size) And _
            Game_Feild.Is_Empty(square4.location.X / square_size - 1, square4.location.Y / square_size) Then

            ' this block can move down; hence perform the left movement
            '   first aside though; hide the block
            Hide(Game_Feild.win_handle)

            square1.location = New Point(square1.location.X - square_size, square1.location.Y)
            square2.location = New Point(square2.location.X - square_size, square2.location.Y)
            square3.location = New Point(square3.location.X - square_size, square3.location.Y)
            square4.location = New Point(square4.location.X - square_size, square4.location.Y)

            ' finally show the blocks
            Show(Game_Feild.win_handle)
        Else
            ' the block cannot move right any further
            Left = False

        End If
    End Function

    Public Sub Rotate()
        ' this function/sub-routine rotates the block based on the current direction and location
        '   of each square of the block. The direction of rotation is colck-wise; N-E-S-W.

        ' store the old positions for resetting
        Dim old_pos1 As Point = square1.location
        Dim old_pos2 As Point = square2.location
        Dim old_pos3 As Point = square3.location
        Dim old_pos4 As Point = square4.location
        Dim old_status_rotation As ROTATE_BLOCK = rotation_status

        ' hide the visiblity of the current block
        Hide(Game_Feild.win_handle)

        ' rotate the blocks based on their current direction
        Select Case BlockType
            Case BLOCK_TYPE.SQUARE
                ' all good this one don't rotate; i.e. rotation brings no visible difference hence
                '   simply ignore outcome

            Case BLOCK_TYPE.LINE
                ' a line/bar block can rotate one of the 2 ways; east or north
                ' hence rotate accordingly based on current formation
                Select Case rotation_status
                    Case ROTATE_BLOCK.NORTH
                        ' this one is having a bar shape rotate to line taking square 2 as an axis
                        '   of rotation
                        square1.location = New Point(square2.location.X - square_size, square2.location.Y)
                        square3.location = New Point(square2.location.X + square_size, square2.location.Y)
                        square4.location = New Point(square2.location.X + 2 * square_size, square2.location.Y)

                        ' face east
                        rotation_status = ROTATE_BLOCK.EAST

                    Case ROTATE_BLOCK.EAST
                        ' turn the flat line shape to horizontal bar based on
                        '   square 2 as the axis of rotation
                        square1.location = New Point(square2.location.X, square2.location.Y - square_size)
                        square3.location = New Point(square2.location.X, square2.location.Y + square_size)
                        square4.location = New Point(square2.location.X, square2.location.Y - square_size * 2)

                        ' turn north
                        rotation_status = ROTATE_BLOCK.NORTH
                End Select

            Case BLOCK_TYPE.Z
                ' Like the line block Z block's can rotate 2 ways; east or north
                Select Case rotation_status
                    Case ROTATE_BLOCK.NORTH
                        ' the default Z-shaped block rotate to its counterpart, again using
                        '   square2 as the axis of rotation
                        square1.location = New Point(square2.location.X, square2.location.Y - square_size)
                        square3.location = New Point(square2.location.X - square_size, square2.location.Y + square_size)
                        square4.location = New Point(square2.location.X - square_size, square2.location.Y)

                        ' block is now looking east
                        rotation_status = ROTATE_BLOCK.EAST

                    Case ROTATE_BLOCK.EAST
                        ' this is now faceing inverted Amharic "Na" alphabet, and hence turn back to
                        '   Z-shaped block
                        square1.location = New Point(square2.location.X - square_size, square2.location.Y)
                        square3.location = New Point(square2.location.X + square_size, square2.location.Y + square_size)
                        square4.location = New Point(square2.location.X, square2.location.Y + square_size)

                        ' block now faces north
                        rotation_status = ROTATE_BLOCK.NORTH
                End Select

            Case BLOCK_TYPE.S
                ' like all before it this one too can turn 2 ways; east or north
                Select Case rotation_status
                    Case ROTATE_BLOCK.NORTH
                        ' must face eastwards
                        rotation_status = ROTATE_BLOCK.EAST

                        ' move squares around
                        square1.location = New Point(square2.location.X, square2.location.Y - square_size)
                        square3.location = New Point(square2.location.X + square_size, square2.location.Y + square_size)
                        square4.location = New Point(square2.location.X + square_size, square2.location.Y)

                    Case ROTATE_BLOCK.EAST
                        'face north
                        rotation_status = ROTATE_BLOCK.NORTH

                        ' move around
                        square1.location = New Point(square2.location.X + square_size, square2.location.Y)
                        square3.location = New Point(square2.location.X - square_size, square2.location.Y + square_size)
                        square4.location = New Point(square2.location.X, square2.location.Y + square_size)
                End Select

            Case BLOCK_TYPE.T
                ' the following block can be rotated either of the four directions in clock-wise
                '   starting from North -> East -> South -> West.
                Select Case rotation_status
                    Case ROTATE_BLOCK.NORTH
                        ' block is currently facing the north side face east
                        rotation_status = ROTATE_BLOCK.EAST

                        ' rotate squares using square2 as an axis
                        square1.location = New Point(square2.location.X, square2.location.Y - square_size)
                        square3.location = New Point(square2.location.X, square2.location.Y + square_size)
                        square4.location = New Point(square2.location.X - square_size, square2.location.Y)

                    Case ROTATE_BLOCK.EAST
                        ' block is faceing east turn south
                        rotation_status = ROTATE_BLOCK.SOUTH

                        ' rotate squares wrt square 2
                        square1.location = New Point(square2.location.X + square_size, square2.location.Y)
                        square3.location = New Point(square2.location.X - square_size, square2.location.Y)
                        square4.location = New Point(square2.location.X, square2.location.Y - square_size)

                    Case ROTATE_BLOCK.SOUTH
                        ' now its faceing south turn west
                        rotation_status = ROTATE_BLOCK.WEST

                        ' rotate squares
                        square1.location = New Point(square2.location.X, square2.location.Y + square_size)
                        square3.location = New Point(square2.location.X, square2.location.Y - square_size)
                        square4.location = New Point(square2.location.X + square_size, square2.location.Y)

                    Case ROTATE_BLOCK.WEST
                        ' facing west finally turn north
                        rotation_status = ROTATE_BLOCK.NORTH

                        ' do the rotation
                        square1.location = New Point(square2.location.X - square_size, square2.location.Y)
                        square3.location = New Point(square2.location.X + square_size, square2.location.Y)
                        square4.location = New Point(square2.location.X, square2.location.Y + square_size)
                End Select

            Case BLOCK_TYPE.J
                ' The J_block can rotate four ways in clock-wise direction begining from NORTH
                Select Case rotation_status
                    Case ROTATE_BLOCK.NORTH
                        ' facing north turn east
                        rotation_status = ROTATE_BLOCK.EAST

                        ' do the rotation using square3 as an axis of rotation
                        square1.location = New Point(square3.location.X + square_size * 2, square3.location.Y)
                        square2.location = New Point(square3.location.X + square_size, square3.location.Y)
                        square4.location = New Point(square3.location.X, square3.location.Y - square_size)

                    Case ROTATE_BLOCK.EAST
                        ' block must be facing east turn south
                        rotation_status = ROTATE_BLOCK.SOUTH

                        ' using square3 as axis of rotation perform the rotation
                        square1.location = New Point(square3.location.X, square3.location.Y + square_size * 2)
                        square2.location = New Point(square3.location.X, square3.location.Y + square_size)
                        square4.location = New Point(square3.location.X + square_size, square3.location.Y)

                    Case ROTATE_BLOCK.SOUTH
                        ' block is facing south turn west
                        rotation_status = ROTATE_BLOCK.WEST

                        ' do rotation on square3
                        square1.location = New Point(square3.location.X - 2 * square_size, square3.location.Y)
                        square2.location = New Point(square3.location.X - square_size, square3.location.Y)
                        square4.location = New Point(square3.location.X, square3.location.Y + square_size)

                    Case ROTATE_BLOCK.WEST
                        ' block is facing west turn to north finally
                        rotation_status = ROTATE_BLOCK.NORTH

                        ' do rotation using square3
                        square1.location = New Point(square3.location.X, square3.location.Y - square_size * 2)
                        square2.location = New Point(square3.location.X, square3.location.Y - square_size)
                        square4.location = New Point(square3.location.X - square_size, square3.location.Y)
                End Select

            Case BLOCK_TYPE.L
                ' like its complicated brethen's this too can rotate four ways in clock-wise
                Select Case rotation_status
                    Case ROTATE_BLOCK.NORTH
                        ' facing north turn east
                        rotation_status = ROTATE_BLOCK.EAST

                        ' do the rotation using square3 as an axis of rotation
                        square1.location = New Point(square3.location.X - square_size * 2, square3.location.Y)
                        square2.location = New Point(square3.location.X - square_size, square3.location.Y)
                        square4.location = New Point(square3.location.X, square3.location.Y - square_size)

                    Case ROTATE_BLOCK.EAST
                        ' block must be facing east turn south
                        rotation_status = ROTATE_BLOCK.SOUTH

                        ' using square3 as axis of rotation perform the rotation
                        square1.location = New Point(square3.location.X, square3.location.Y + square_size * 2)
                        square2.location = New Point(square3.location.X, square3.location.Y + square_size)
                        square4.location = New Point(square3.location.X - square_size, square3.location.Y)

                    Case ROTATE_BLOCK.SOUTH
                        ' block is facing south turn west
                        rotation_status = ROTATE_BLOCK.WEST

                        ' do rotation on square3
                        square1.location = New Point(square3.location.X + 2 * square_size, square3.location.Y)
                        square2.location = New Point(square3.location.X + square_size, square3.location.Y)
                        square4.location = New Point(square3.location.X, square3.location.Y + square_size)

                    Case ROTATE_BLOCK.WEST
                        ' block is facing west turn to north finally
                        rotation_status = ROTATE_BLOCK.NORTH

                        ' do rotation using square3
                        square1.location = New Point(square3.location.X, square3.location.Y - square_size * 2)
                        square2.location = New Point(square3.location.X, square3.location.Y - square_size)
                        square4.location = New Point(square3.location.X + square_size, square3.location.Y)
                End Select

        End Select

        ' after rotation test if they overlap with other squares
        If Not (Game_Feild.Is_Empty(square1.location.X / square_size + 1, square1.location.Y / square_size) And _
            Game_Feild.Is_Empty(square2.location.X / square_size + 1, square2.location.Y / square_size) And _
            Game_Feild.Is_Empty(square3.location.X / square_size + 1, square3.location.Y / square_size) And _
            Game_Feild.Is_Empty(square4.location.X / square_size + 1, square4.location.Y / square_size)) Then

            ' reset the old positions
            square1.location = old_pos1
            square2.location = old_pos2
            square3.location = old_pos3
            square4.location = old_pos4
            rotation_status = old_status_rotation

        End If

        ' show the current block
        Show(Game_Feild.win_handle)

    End Sub

    Public Function Top() As Integer
        ' returns the top of the line
        Top = Math.Min(square1.location.Y, Math.Min(square2.location.Y, Math.Min(square3.location.Y, square4.location.Y)))
    End Function

    Public Sub Show(ByVal win_handle As System.IntPtr)
        ' this sub-routine draws each square on the screen

        square1.Show(win_handle)
        square2.Show(win_handle)
        square3.Show(win_handle)
        square4.Show(win_handle)

    End Sub

    Public Sub Hide(ByVal win_handle As System.IntPtr)
        ' hides each square by simply calling their individual methods

        square1.Hide(win_handle)
        square2.Hide(win_handle)
        square3.Hide(win_handle)
        square4.Hide(win_handle)

    End Sub
End Class