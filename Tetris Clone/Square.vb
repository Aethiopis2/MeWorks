Public Class Square
    ' class represents the unit/atom/cell of a tertirs block. Every block in the game of Tetris is
    '   made up of squares. The square is something that is defined by size, location, color and 
    '   display: show/hide properties.
    ' 
    ' By: Code Red
    '
    ' Date created: Sept. 22 2016, Thursday.
    ' Last update:
    '

    Public location As Point           ' the location of the square
    Public size As size                ' width/height of the square
    Public inner_color As Color        ' the square inner color
    Public border_color As Color       ' the square border color

    Public Sub Show(ByVal win_handle As System.IntPtr)
        ' this sub-routine draws the square on space; i.e. the picture box used as a game universe. 
        '   It uses GDI+ libraries to draw and fill the squares as paths or better known as GDI+
        '   path-gradients, this way the effect of multiple colors for a single object could be acheived.

        Dim rectangle As Rectangle                      ' reprsents the size of the square
        Dim graphics_handle As Graphics                 ' this is GDI+ way of abstracting the graphics hardware, i.e. handle
        Dim graphics_path As Drawing2D.GraphicsPath     ' the graphics path to draw
        Dim path_brush As Drawing2D.PathGradientBrush   ' the path fill color
        Dim surround_color() As Color                   ' array of border colors

        ' step I: In GDI+ the very first step to drawing is to acquire the graphics handle, 
        '   hence acquire the handle to the space window
        graphics_handle = Graphics.FromHwnd(win_handle)

        ' step II: Create the path object to use gradient fill, in this case I am using it for
        '   square shapes hence the need for a rectangle structure
        rectangle = New Rectangle(location.X, location.Y, size.Width, size.Height)
        graphics_path = New Drawing2D.GraphicsPath

        ' add the constructed rectangle into the graphics_path object
        graphics_path.AddRectangle(rectangle)

        ' step III: Create the brush and fill the path with the specified brush colors
        '   in this case I am using array of colors as border color for the square
        path_brush = New Drawing2D.PathGradientBrush(graphics_path)
        path_brush.CenterColor = inner_color
        surround_color = New Color() {border_color}     ' array of border color for the borders
        path_brush.SurroundColors = surround_color

        ' step IV: Draw the square using the graphics_handle
        graphics_handle.FillPath(path_brush, graphics_path)

    End Sub

    Public Sub Hide(ByVal win_handle As System.IntPtr)
        ' this sub-routine hides the currently rendering square by simply over-drawing it with
        '   the background fill color, this way the square's visiblity will not be shown.
        ' The sub-routine does not use path-gradients it uses solid colors from space.

        Dim graphics_handle As Graphics         ' GDI+ way of handles
        Dim rectangle As Rectangle              ' reprsents the square

        ' first get the handle from space
        graphics_handle = Graphics.FromHwnd(win_handle)

        ' now create and set the dimensions of the square
        rectangle = New Rectangle(location.X, location.Y, size.Width, size.Height)

        ' finally using the graphics handle and the rectangle fill using the background color
        graphics_handle.FillRectangle(New SolidBrush(Game_Feild.Backcolor), rectangle)

    End Sub

    Public Sub New(ByVal inital_size As size, ByVal inital_in_color As Color, ByVal inital_back_color As Color)
        ' this sub-routine is a synonym of a default constructor in C++, it simply initalizes the public data members
        '   to the values sent in the parameter

        size = inital_size                  ' set the size
        inner_color = inital_in_color       ' the inner color for the block
        border_color = inital_back_color    ' the outer color for the block

    End Sub
End Class
