Public Class Game_Feild
    ' this class represents the rules for the game of Tetris. This is like a container class to the
    '   "Block" class that is the sole object in this game but instead it contains methods/functions
    '   or sub-routines that validate the various states of the game.
    '
    ' By: Code Red
    '
    ' Date created: Sept. 22 2016, Thursday
    ' Last update:
    '
    Public Const width As Integer = 16          ' the width of the game universe
    Public Const height As Integer = 32         ' height of the game universe
    Public Const square_size As Integer = 20    ' size of a square in pixels

    Private Const bit_empty As Integer = &H0&   ' 00000000 00000000
    Private Const bit_full As Integer = &HFFFF& ' 11111111 11111111

    Public Shared backcolor As Color            ' stores the background color of the "Space"
    Public Shared win_handle As System.IntPtr   ' pointer to the window handle; i.e. the "Space"
    Public Shared counter As Integer            ' counts the number of lines made

    ' the game universe as a 2D array of squares
    Private Shared game_universe(width, height) As Square
    Private Shared bit_feild(height) As Integer ' USED FOR collosion detection

    Public Shared Function Is_Empty(ByVal x As Integer, ByVal y As Integer) As Boolean
        ' this function tests if the currently falling block can fall further once more or
        '   not, its like Shakespear; "To be or not to be, that is the question?"
        Is_Empty = True

        ' test for boundary conditions
        If (x < 0 Or x >= width) Or (y < 0 Or y >= height) Then
            Is_Empty = False ' reject

            ' test if the array bit_feild has been set for collosions
        ElseIf bit_feild(y) And (2 ^ x) Then
            Is_Empty = False
        End If
    End Function

    Public Shared Function Check_Lines() As Integer
        ' this function tests for any complete horizontal lines and removes them according 
        '   to the rules of the game.
        Check_Lines = 0

        Dim y As Integer, x As Integer, i As Integer

        ' start from the bottom of the game_universe array (space) and...
        y = height - 1

        ' ...look for any horizontal lines in the game_universe
        Do While y >= 0

            ' stop this loop if ya reached a blank line
            If bit_feild(y) = bit_empty Then
                y = 0   ' this line is blank

                ' if all the bits of the line are set, then remove them and
                '   adjust anything above it.
            ElseIf bit_feild(y) = bit_full Then

                Check_Lines += 1        ' increment the count of lines removed

                ' move all the above blocks down
                For i = y To 0 Step -1
                    ' if this line is not the first of the game feild copy the lines above
                    If i > 0 Then
                        bit_feild(i) = bit_feild(i - 1)

                        ' now copy each square from the lines above
                        For x = 0 To width - 1
                            ' copy the square
                            game_universe(x, i) = game_universe(x, i - 1)
                            ' update the location property
                            Try
                                With game_universe(x, i)
                                    .location = New Point(.location.X, .location.Y + square_size)
                                End With
                            Catch
                                ' igonre error
                            End Try
                        Next
                    Else
                        ' this is the first line just clear it
                        bit_feild(i) = bit_empty
                        For x = 0 To width - 1
                            game_universe(x, i) = Nothing
                        Next
                    End If
                Next
           
            End If

            y -= 1
        Loop

    End Function

    Public Shared Function Stop_Square(ByVal square As Square, ByVal x As Integer, ByVal y As Integer) As Boolean
        ' this function is rather an ill-named one, but it nonetheless records the position of each square
        '   in the "game_universe" array and it set's the approprite bit in the "bit_feild" array which is 
        '   used for fast collosion detection.
        '
        ' NOTE: function assumes that the block has reached the bottom of the screen or at the top of the piled
        '   up blocks; i.e. function "Down()" returned a false.
        '
        Stop_Square = True
        '
        bit_feild(y) = bit_feild(y) Or (2 ^ x)      ' turn the yth array element's xth bit to on/1
        game_universe(x, y) = square                ' record this square in the array

    End Function

    Public Shared Function Redraw() As Boolean
        ' function draws all the squares
        Dim x As Integer
        Dim y As Integer

        ' loop and draw everything from the "game_universe" 
        '   using the ol'skool 2 layer deep loop for 2D arrays.
        For y = height - 1 To 0 Step -1

            ' dont process loop 2 if this line is empty
            If bit_feild(y) = bit_empty Then
                Exit Function
            End If

            ' do for the width
            For x = width - 1 To 0 Step -1

                ' test for unfinished widths and avoid them
                Try
                    game_universe(x, y).Show(win_handle)
                Catch
                    ' ignore errors
                End Try

            Next    ' end for x
        Next    ' end for y

    End Function

    Public Shared Sub Clear_All()
        ' this sub-routine clears all the contents of the "game_universe" and "bit_feild" set

        ' a few locals used for loops
        Dim y As Integer, x As Integer

        ' loop and clear everything
        For y = height - 1 To 0 Step -1

            bit_feild(y) = bit_empty    ' reset

            For x = 0 To width - 1
                game_universe(x, y) = Nothing
            Next

        Next

    End Sub
End Class
