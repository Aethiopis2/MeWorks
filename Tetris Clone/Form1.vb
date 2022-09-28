Public Class Form1
    Inherits System.Windows.Forms.Form

#Region " Windows Form Designer generated code "

    Public Sub New()
        MyBase.New()

        'This call is required by the Windows Form Designer.
        InitializeComponent()

        'Add any initialization after the InitializeComponent() call

    End Sub

    'Form overrides dispose to clean up the component list.
    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing Then
            If Not (components Is Nothing) Then
                components.Dispose()
            End If
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    Friend WithEvents Space As System.Windows.Forms.PictureBox
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents lbl_score As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents btn_start As System.Windows.Forms.Button
    Friend WithEvents Timer1 As System.Windows.Forms.Timer
    Friend WithEvents Preview As System.Windows.Forms.PictureBox
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents lbl_level As System.Windows.Forms.Label
    Friend WithEvents Label4 As System.Windows.Forms.Label
    <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container
        Me.Space = New System.Windows.Forms.PictureBox
        Me.Label1 = New System.Windows.Forms.Label
        Me.lbl_score = New System.Windows.Forms.Label
        Me.Preview = New System.Windows.Forms.PictureBox
        Me.Label2 = New System.Windows.Forms.Label
        Me.btn_start = New System.Windows.Forms.Button
        Me.Timer1 = New System.Windows.Forms.Timer(Me.components)
        Me.Label3 = New System.Windows.Forms.Label
        Me.lbl_level = New System.Windows.Forms.Label
        Me.Label4 = New System.Windows.Forms.Label
        Me.SuspendLayout()
        '
        'Space
        '
        Me.Space.BackColor = System.Drawing.SystemColors.ActiveCaptionText
        Me.Space.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Space.Location = New System.Drawing.Point(8, 8)
        Me.Space.Name = "Space"
        Me.Space.Size = New System.Drawing.Size(320, 640)
        Me.Space.TabIndex = 0
        Me.Space.TabStop = False
        '
        'Label1
        '
        Me.Label1.ForeColor = System.Drawing.Color.FromArgb(CType(192, Byte), CType(0, Byte), CType(0, Byte))
        Me.Label1.Location = New System.Drawing.Point(392, 24)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(128, 40)
        Me.Label1.TabIndex = 1
        Me.Label1.Text = "Score:"
        '
        'lbl_score
        '
        Me.lbl_score.ForeColor = System.Drawing.Color.Red
        Me.lbl_score.Location = New System.Drawing.Point(336, 72)
        Me.lbl_score.Name = "lbl_score"
        Me.lbl_score.Size = New System.Drawing.Size(232, 32)
        Me.lbl_score.TabIndex = 2
        Me.lbl_score.Text = "0"
        Me.lbl_score.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Preview
        '
        Me.Preview.BackColor = System.Drawing.SystemColors.ActiveCaptionText
        Me.Preview.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Preview.Location = New System.Drawing.Point(400, 128)
        Me.Preview.Name = "Preview"
        Me.Preview.Size = New System.Drawing.Size(100, 100)
        Me.Preview.TabIndex = 3
        Me.Preview.TabStop = False
        '
        'Label2
        '
        Me.Label2.Font = New System.Drawing.Font("Consolas", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label2.Location = New System.Drawing.Point(400, 240)
        Me.Label2.Name = "Label2"
        Me.Label2.TabIndex = 4
        Me.Label2.Text = "Next Block"
        '
        'btn_start
        '
        Me.btn_start.Location = New System.Drawing.Point(392, 304)
        Me.btn_start.Name = "btn_start"
        Me.btn_start.Size = New System.Drawing.Size(128, 40)
        Me.btn_start.TabIndex = 5
        Me.btn_start.Text = "Start"
        '
        'Timer1
        '
        '
        'Label3
        '
        Me.Label3.Location = New System.Drawing.Point(344, 536)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(100, 40)
        Me.Label3.TabIndex = 6
        Me.Label3.Text = "Level"
        '
        'lbl_level
        '
        Me.lbl_level.Location = New System.Drawing.Point(448, 536)
        Me.lbl_level.Name = "lbl_level"
        Me.lbl_level.Size = New System.Drawing.Size(120, 32)
        Me.lbl_level.TabIndex = 7
        Me.lbl_level.Text = "0"
        '
        'Label4
        '
        Me.Label4.Font = New System.Drawing.Font("Consolas", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label4.ForeColor = System.Drawing.Color.Green
        Me.Label4.Location = New System.Drawing.Point(352, 616)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(216, 23)
        Me.Label4.TabIndex = 8
        Me.Label4.Text = "Program Author: Dr. Rediet Worku. ARK Video Games."
        '
        'Form1
        '
        Me.AutoScaleBaseSize = New System.Drawing.Size(16, 33)
        Me.ClientSize = New System.Drawing.Size(576, 650)
        Me.Controls.Add(Me.Label4)
        Me.Controls.Add(Me.lbl_level)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.btn_start)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.Preview)
        Me.Controls.Add(Me.lbl_score)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.Space)
        Me.Font = New System.Drawing.Font("Consolas", 21.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.KeyPreview = True
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "Form1"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
        Me.Text = "TETRIS: CLONE"
        Me.ResumeLayout(False)

    End Sub

#End Region

    ' class globals
    Dim current_block As Block                  ' the currently falling block
    Dim next_block As Block                     ' the next block that appears in time

    Dim game_paused As Boolean                  ' checks if the status of the game is paused
    Dim still_ticking As Boolean = False        ' prevents from running while processing previous tick

    Dim dx As Integer, dy As Integer            ' the dimensions of the blocks as a rectangluar sections; 
    '   used for calculating center of masses of blocks during drawing on the "Preview"

    Sub Form1_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        ' this routine fires up when the form window first initalizes
        ' simply sets the properties of certain parameters
        Game_Feild.backcolor = Space.BackColor
        Game_Feild.win_handle = Space.Handle

    End Sub

    Private Sub btn_start_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btn_start.Click
        ' This is the event that fires up when the "Start" button is clicked

        ' clear the game feilds
        Game_Feild.Clear_All()
        Space.Invalidate()

        ' create and preview the next block
        next_block = New Block(New Point(Preview.Width / 2 - 10, Preview.Width / 2 - 10))
        next_block.Show(Preview.Handle)

        ' create a new block base on next block and show it on "space"
        current_block = New Block(New Point(Game_Feild.square_size * 6, 0), next_block.BlockType)
        current_block.Show(Space.Handle)

        ' adjust the timer effects and all
        Timer1.Enabled = True
        Timer1.Interval = 200
        btn_start.Enabled = False
        lbl_score.Text = 0
        lbl_level.Text = 1

        ' satutus of game
        game_paused = False

    End Sub

    Private Sub Form1_KeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles MyBase.KeyDown
        ' This routine fires up whenever user does something to the mapped keys of the 
        '   keybord; i.e. the arrow keys plus some others
        Select Case e.KeyCode

            Case Keys.A
                If Not game_paused Then
                    ' user is pressing the right arrow key
                    current_block.Left()   ' move 1 square to the right
                End If

            Case Keys.D
                If Not game_paused Then
                    ' pressing left arrow
                    current_block.Right()    ' shift 1 square to the left
                End If

            Case Keys.W
                If Not game_paused Then
                    ' user attempting to rotate
                    current_block.Rotate()  ' rotate where you at
                End If

            Case Keys.S
                If Not game_paused Then
                    ' user is pressing the down arrow key
                    current_block.Down()    ' fall 1 square down
                End If

            Case Keys.Escape
                ' pasue game
                Timer1.Enabled = Not Timer1.Enabled
                If Timer1.Enabled Then
                    Me.Text = "TETRIS CLONE"
                    game_paused = False
                Else
                    Me.Text = "GAME PAUSED -- Esc to continue..."
                    game_paused = True
                End If
        End Select
    End Sub

    Private Sub Timer1_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Timer1.Tick
        ' fires up every tick of the timer

        Dim erased_lines As Integer ' the number of erased lines 

        ' prevents processing if previous tick is active
        If still_ticking Then Exit Sub

        still_ticking = True    ' start ticking

        ' controls the block falling
        If Not current_block.Down() Then

            ' test for game over
            If current_block.Top() = 0 Then

                Timer1.Enabled = False      ' disable timer
                btn_start.Enabled = True    ' enable the start button

                still_ticking = False       ' stop processing this function

                Space.Invalidate()          ' clear regions
                Preview.Invalidate()

                lbl_level.Text = 0          ' re-set the level

                ' prompt game over message
                MessageBox.Show("G A M E O V E R ! ! !", "TETRIS CLONE", MessageBoxButtons.OK, MessageBoxIcon.Stop)

                Exit Sub

            End If

            ' increase the score based on the number of erased lines
            erased_lines = Game_Feild.Check_Lines
            lbl_score.Text += 100 * erased_lines

            ' clear the game feild
            If erased_lines > 0 Then
                Space.Invalidate()
                Application.DoEvents()
                Game_Feild.Redraw()
            End If

            ' update the game by the score count
            If lbl_score.Text / 1000 >= lbl_level.Text And Timer1.Interval > 50 Then
                ' update level and increase speed
                lbl_level.Text += 1
                Timer1.Interval -= 100  ' decrement by a hundered each time

            End If ' level needs update

            ' relase the current block from memory
            current_block = Nothing

            ' create a new current block
            current_block = New Block(New Point(Game_Feild.square_size * 6, 0), next_block.BlockType)
            current_block.Show(Game_Feild.win_handle)

            ' release next block from memory and get a new one
            next_block.Hide(Preview.Handle)
            next_block = Nothing

            ' create a new next_block and display on the preview
            next_block = New Block(New Point(Preview.Width / 2 - 10, Preview.Height / 2 - 10))
            next_block.Show(Preview.Handle)

        End If

        still_ticking = False   ' I'm no longer working

    End Sub

    Private Sub Form1_Activated(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Activated
        ' this message fires up when window gets covered or minimized or such; hence
        ' reactivate by redrawing
        Game_Feild.Redraw()
    End Sub

End Class
