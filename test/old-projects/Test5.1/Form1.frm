VERSION 5.00
Begin VB.Form frmScreen 
   AutoRedraw      =   -1  'True
   BackColor       =   &H00000000&
   BorderStyle     =   1  'Fixed Single
   ClientHeight    =   11400
   ClientLeft      =   15
   ClientTop       =   210
   ClientWidth     =   15240
   ControlBox      =   0   'False
   DrawMode        =   6  'Mask Pen Not
   DrawWidth       =   10
   FillColor       =   &H00FF8080&
   ForeColor       =   &H00FF0000&
   Icon            =   "Form1.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   MouseIcon       =   "Form1.frx":030A
   MousePointer    =   99  'Custom
   ScaleHeight     =   760
   ScaleMode       =   3  'Pixel
   ScaleWidth      =   1016
   WindowState     =   2  'Maximized
   Begin VB.Timer Tidtagarur 
      Enabled         =   0   'False
      Interval        =   1
      Left            =   1440
      Top             =   6000
   End
   Begin VB.Image Image4 
      Height          =   495
      Left            =   5760
      Top             =   10680
      Width           =   855
      Visible         =   0   'False
   End
   Begin VB.Image Image3 
      Height          =   480
      Index           =   3
      Left            =   4920
      Picture         =   "Form1.frx":045C
      Top             =   10680
      Width           =   480
      Visible         =   0   'False
   End
   Begin VB.Image Image3 
      Height          =   480
      Index           =   2
      Left            =   4320
      Picture         =   "Form1.frx":089E
      Top             =   10680
      Width           =   480
      Visible         =   0   'False
   End
   Begin VB.Image Image3 
      Height          =   480
      Index           =   1
      Left            =   3600
      Picture         =   "Form1.frx":0CE0
      Top             =   10680
      Width           =   480
      Visible         =   0   'False
   End
   Begin VB.Image Image3 
      Height          =   480
      Index           =   0
      Left            =   2880
      Picture         =   "Form1.frx":1122
      Top             =   10680
      Width           =   480
      Visible         =   0   'False
   End
   Begin VB.Image Image2 
      Height          =   480
      Left            =   1920
      Picture         =   "Form1.frx":1564
      Top             =   10560
      Width           =   480
      Visible         =   0   'False
   End
   Begin VB.Image Image1 
      Height          =   480
      Left            =   1080
      Picture         =   "Form1.frx":19A6
      Top             =   10560
      Width           =   480
      Visible         =   0   'False
   End
End
Attribute VB_Name = "frmScreen"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Const Mitt = 1001 / 2
Const ShotConst = 150
Const SP = "Data\SndFx\" '"d:\mattias\projekt\projekt\test 5\Data\SndFx\"

Dim Ship(50) As Ship, shot(ShotConst) As shot, Shipcount As Byte, ShotCount As Integer, CurrentShot As Long
Dim Starx(11) As Integer, Stary(11) As Single, Star11 As Long, StarySpeed(10) As Single

Public Player As Ship
Dim Score As Long
Dim Fire2, FireSide As Boolean, NewAttack As Integer

Dim group(7) As Ship

Dim MouseControl As Boolean
Dim MouseY As Integer

Dim FireRate As Byte
Dim Fireing As Byte
Dim ShotGun As Boolean
Dim ASCR As Boolean
Dim TargetX As Long
Dim TargetY As Long
Dim rTargetX As Long
Dim rTargetY As Long
Dim rTarget As Ship
Dim LastShot As shot
Dim Explosives As Integer
Dim TimeElapsed As Long

'--------------------------------------------------------------------------
Dim DX7 As New DirectX7
Dim DX7Sound As DirectSound

Dim sndExp As DirectSoundBuffer, sndMissileLaunch As DirectSoundBuffer
Dim sndLaser As DirectSoundBuffer, sndCrash As DirectSoundBuffer, DisableSound As Boolean
Public sndEnemyshot As DirectSoundBuffer

'--------------------------------------------------------------------------
Public Pictures As New GraficClass

Sub InitSound()
    On Error GoTo fel
    Set DX7Sound = DX7.DirectSoundCreate("")
    DX7Sound.SetCooperativeLevel Me.hWnd, DSSCL_PRIORITY
    
    Set sndExp = createSound(SP + "sndexp.wav")
    Set sndLaser = createSound(SP + "gunshot.wav")
    Set sndMissileLaunch = createSound(SP + "launch.wav")
    Set sndEnemyshot = createSound(SP + "enemy.wav")
    Set sndCrash = createSound(SP + "crash.wav")
    
    Exit Sub
fel:
    DisableSound = 1
End Sub

Function createSound(FileName As String) As DirectSoundBuffer
    Dim buffdesk As DSBUFFERDESC
    Dim waveform As WAVEFORMATEX
    'buffdesk.lFlags = DSBCAPS_STATIC
    Set createSound = DX7Sound.CreateSoundBufferFromFile(FileName, buffdesk, waveform)
End Function

Public Sub CreateShip(Objekt As Ship)
    Shipcount = Shipcount + 1
    If Shipcount > 50 Then Shipcount = 50
    Set Ship(Shipcount) = Objekt
    Objekt.num = Shipcount
End Sub

Public Sub RemoveShip(num As Byte)
    Dim i As Integer
    Set Ship(num) = Nothing
    For i = num To Shipcount - 1
        Set Ship(i) = Ship(i + 1)
    Next
    Shipcount = Shipcount - 1
End Sub

Public Sub CreateShot(Objekt As shot)
    If ShotCount < -1 Then ShotCount = -1
    ShotCount = ShotCount + 1
    If ShotCount = ShotConst + 1 Then ShotCount = ShotConst
    Set shot(ShotCount) = Objekt
    Objekt.num = ShotCount
End Sub

Public Sub RemoveShot(num As Byte)
    Dim i As Integer
    Set shot(num) = Nothing
    If num <= CurrentShot Then CurrentShot = CurrentShot - 1
    For i = num To ShotCount - 1
        Set shot(i) = shot(i + 1)
        shot(i).num = i
    Next

    ShotCount = ShotCount - 1
End Sub


Private Sub Form_KeyDown(KeyCode As Integer, Shift As Integer)
    If KeyCode = vbKeyEscape Then End
    Select Case KeyCode
        Case vbKeyLeft
            Player.xSpeed = -10
        Case vbKeyRight
            Player.xSpeed = 10
        Case vbKeySpace
            Fireing = 1
        Case vbKeyControl
            ASCR = Not ASCR
        Case vbKeyReturn
            ShotGun = Not ShotGun
        Case vbKeyBack
            If Explosives Then Explosives = 0 Else Explosives = 2
    End Select
    If KeyCode = vbKeyF2 Then Start
End Sub

Private Sub Form_KeyUp(KeyCode As Integer, Shift As Integer)
    Select Case KeyCode
        Case vbKeyLeft
            If Player.xSpeed = -10 Then Player.xSpeed = 0
        Case vbKeyRight
            If Player.xSpeed = 10 Then Player.xSpeed = 0
        Case vbKeySpace
            Fireing = 0
            FireRate = 0
    End Select
End Sub

Private Sub Form_Load()
    Dim i As Integer
    For i = 0 To 10
        Randomize
        Starx(i) = 1016 * Rnd
        Stary(i) = 760 * Rnd
        DrawMode = 6
        DrawWidth = 2
        PSet (Starx(i), Stary(i))
    Next
    CPU.Start
    
    'Image4 = LoadPicture("D:\Mattias\projekt\Projekt\Test 5\Data\player.cur")
    'Image1 = LoadPicture("D:\Mattias\projekt\Projekt\Test 5\Data\enemy.ico")
    
    'ChDir "D:\Mattias\projekt\Projekt\Test 5"
    
    InitSound
    InitGrafic Me
    Pictures.LoadGrafic "bilder"
    
    CurrentX = 0
    CurrentY = 0
    Print "tryck F2"
End Sub

Private Sub Form_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
    Fireing = Button
End Sub

Private Sub Form_MouseMove(Button As Integer, Shift As Integer, X As Single, Y As Single)
    If MouseControl = True Then
        Player.X = X / ScreenRect.Right * BackRect.Right - 16
    End If
    MouseY = Y
End Sub

Private Sub Form_MouseUp(Button As Integer, Shift As Integer, X As Single, Y As Single)
    Fireing = 0
End Sub

Sub Tidtagarur_Timer()
    Dim i
    
    BackSurface.SetForeColor 0
    BackSurface.SetFillColor 0
    BackSurface.SetFillStyle 0
    BackSurface.DrawBox 0, 0, ScreenRect.Right, ScreenRect.Bottom
    
    Randomize
    
    TimeElapsed = TimeElapsed + 1
    
    BackSurface.setDrawWidth 1
    BackSurface.SetForeColor vbWhite
    For i = 0 To 10
        BackSurface.DrawLine Starx(i), Stary(i), Starx(i), Stary(i) + 1
        Stary(i) = Stary(i) + StarySpeed(i)
        If Stary(i) > 400 Then
            Starx(i) = Rnd * 400
            Stary(i) = 0
        End If
    Next
    
    Stary(11) = Stary(11) + 20
    If Stary(11) > 4000 Then
        Starx(11) = Rnd * 640
        Stary(11) = -32 * 10
        Randomize
        Image2 = Image3(Rnd * 3)
    End If
    Pictures.FastDisplay Starx(11), Stary(11) / 10, 9
    'PaintPicture Image2, Starx(11), Stary(11) / 10
    
    
    
    If Fireing > 0 And FireRate = 0 Then
        Randomize
        If Fireing = 2 Then
            If ShotGun Then SmallExplotion Player.X + 7 + 18 * -FireSide, Player.Y, 0, -16 Else Fire Player.X + 7 + 18 * -FireSide, Player.Y, 0, -16, 40 / (Explosives + 1), (Explosives)
            Pictures.FastDisplay Player.X - 9 + 18 * -FireSide, Player.Y - 16, 5
            FireSide = Not FireSide
            
            PlaySound sndMissileLaunch
        Else
            If ASCR Then FireASCR Player.X + 15 Else FireLaser Player.X + 15
            Pictures.FastDisplay Player.X, Player.Y - 16, 5
        End If
        FireRate = Fireing
    ElseIf FireRate > 0 Then
        FireRate = FireRate - 1
    End If
    
    If ShotCount > -1 Then
        For CurrentShot = 0 To ShotCount
            If CurrentShot > ShotCount Then Exit For
            shot(CurrentShot).Display
            shot(CurrentShot).TimeTab
        Next
    End If
    
    For i = 0 To Shipcount
        Ship(i).TimeTab
        Ship(i).Display
    Next
    
    If ASCR Then
        If Not rTarget Is Nothing Then
            rTargetX = rTarget.X + 16
            rTargetY = rTarget.Y + 16
        End If
        If TargetX Then
            TargetX = TargetX + (rTargetX - TargetX) / 3
            TargetY = TargetY + (rTargetY - TargetY) / 3
        Else
            TargetX = rTargetX + 1000 * Rnd - 500
            TargetY = rTargetY + 1000 * Rnd - 500
        End If
        Pictures.FastDisplay TargetX - 16, TargetY - 16, 6
    Else
        TargetX = 0
        TargetY = 0
    End If
    
    CurrentX = 10
    CurrentY = 710
    BackSurface.SetForeColor vbWhite
    BackSurface.DrawText 0, ScreenRect.Bottom - 30, "Sköld:" & Player.HP, False
    BackSurface.DrawText 0, ScreenRect.Bottom - 45, "Poäng:" & Score, False
    BackSurface.DrawText 0, ScreenRect.Bottom - 60, "ASCR:" & OnOff(ASCR), False
    
    If NewAttack < 0 Then
        CreateGroup
        NewAttack = 100 '350
    End If
    
    NewAttack = NewAttack - 1
    
    UpDateScreen
End Sub

Function OnOff(Value As Boolean) As String
    If Value Then
        OnOff = "På"
    Else
        OnOff = "Av"
    End If
End Function

Sub Init()
    Dim i As Byte
    Static first As Boolean
    
    If first < 0 Then
        For i = 0 To 20
            Set Ship(i) = Nothing
        Next
    Else
        first = 1
    End If
        
    Set Ship(0) = New Ship
    Set Player = Ship(0)
    ShotCount = -1
    Player.X = Mitt
    Player.Y = BackRect.Bottom - 32
    Player.HP = 100
    Player.Display
    Player.SetPicPlayer
    Score = 0
    Shipcount = 0
    
    For i = 0 To 10
        Randomize
        Starx(i) = 1016 * Rnd
        Stary(i) = 741 * Rnd
        StarySpeed(i) = 0.5 * Rnd
        DrawStyle = 6
        DrawWidth = 1
        PSet (Starx(i), Stary(i))
    Next
    
    Randomize
    Stary(11) = 10
    Starx(11) = Rnd * 1016
    PaintPicture Image2, Starx(11), Stary(11) / 10
    
    'CreateGroup
    
    MouseControl = True
    
    CPU.Start
    NewAttack = 0
    
    TimeElapsed = 0
End Sub

Sub Fire(X As Integer, Y As Integer, xSpeed As Integer, ySpeed As Integer, Range As Integer, Optional FromEx As Long)
    Dim nShot As shot
    Set nShot = New shot
    Randomize
    nShot.xSpeed = xSpeed
    nShot.ySpeed = ySpeed
    nShot.X = X
    nShot.Y = Y
    nShot.Range = Range
    nShot.FromEx = FromEx
    If ASCR And Not rTarget Is Nothing Then Set nShot.Target = rTarget
    Set LastShot = nShot
End Sub

Sub CreateShot2(X As Integer, Y As Integer, xSpeed As Integer, ySpeed As Integer, Range As Integer, Optional FromEx As Boolean)
    Dim nShot As shot
    Set nShot = New shot
    Randomize
    nShot.xSpeed = xSpeed
    nShot.ySpeed = ySpeed
    nShot.X = X
    nShot.Y = Y
    nShot.Range = Range
    nShot.FromEx = FromEx
End Sub

Sub FireLaser(X As Integer)
    Dim i As Byte, Hit As Byte
    
    BackSurface.SetForeColor vbWhite
    For i = 1 To Shipcount
        If Ship(i).X < X And X < Ship(i).X + 30 Then
            DrawLaser X, i
            Damage i, 3
            Exit For
        End If
    Next
    
    If i > Shipcount Then BackSurface.DrawLine X, 0, X, 400 - 20
    
    
    
    PlaySound sndLaser
End Sub

Sub FireASCR(X As Integer)
    Dim i As Byte, Hit As Byte
    For i = 1 To Shipcount
        If Ship(i).X - 40 < X And X < Ship(i).X + 70 Then
            Set rTarget = Ship(i)
            'SmallExplotion Ship(i).X + Rnd * 200 - 116, Ship(i).Y + Rnd * 200 - 116, 0, 0
            SmallExplotion TargetX + Rnd * 60 - 30, TargetY + Rnd * 60 - 30, 0, 0
            Exit For
        End If
    Next
    
    PlaySound sndLaser
End Sub

Sub DrawLaser(X As Integer, Hit As Byte)
    BackSurface.SetForeColor vbWhite
    BackSurface.DrawLine X, Ship(Hit).Y + 30, X, 400 - 20
    Fire X, Ship(Hit).Y + 40, 20 * Rnd - 10, 15, 3, 1
    Ship(Hit).Y = Ship(Hit).Y - 3
End Sub

Sub Start()
    Init
    Tidtagarur.Enabled = 1
End Sub

Public Function isHit(X&, Y&) As Integer
    Dim i As Byte
    
    For i = 0 To Shipcount
        If X > Ship(i).X And X < Ship(i).X + 30 And _
            Y > Ship(i).Y And Y < Ship(i).Y + 30 Then
                isHit = i
                Exit For
        End If
    If i = Shipcount Then isHit = -1
    Next
    
End Function

Public Sub Damage(num As Byte, Damagecount)
    Ship(num).HP = Ship(num).HP - Damagecount
    If Ship(num).HP < 0 Then
        Explotion Ship(num).X + 15, Ship(num).Y + 15
        If Ship(num) Is Player Then
            MsgBox "Du fick " & Score & " poäng!"
            Start
        Else
            RemoveShip num
            Score = Score + 15
            If Score / 105 = Score \ 105 Then Player.HP = Player.HP + 20
        End If
    End If
End Sub

Public Sub Explotion(X As Integer, Y As Integer)
    Dim i As Byte
    
    For i = 0 To 20
        Randomize
        CreateShot2 X, Y, Rnd * 20 - 10, Rnd * 20 - 10, 10 + 10 * Rnd, True
    Next
        
    PlaySound sndExp
End Sub

Public Sub SmallExplotion(X As Integer, Y As Integer, xSpeed As Integer, ySpeed As Integer)
    Dim i As Byte
    
    For i = 0 To 5
        Randomize
        CreateShot2 X, Y, Rnd * 4 - 2 + xSpeed, Rnd * 4 - 2 + ySpeed, 10 + 10 * Rnd, True
    Next
    
    Pictures.FastDisplay X - 16, Y - 16, 5
    
    PlaySound sndCrash
End Sub

Sub CreateGroup()
    Dim i As Byte
    For i = 0 To 5
        Set group(i) = New Ship
        group(i).HP = 20 '150
        group(i).AI = 1
        group(i).X = 50 + i * 100
        group(i).Y = -20 - Rnd * 100
        group(i).SetPic 1
    Next
    Set group(i) = New Ship
    group(i).HP = 10 '50
    group(i).AI = 2
    group(i).AIData = 0
    group(i).X = 380
    group(i).Y = -20
    group(i).SetPic 7
    If TimeElapsed < 2 Then Exit Sub
    Set group(i + 1) = New Ship
    With group(i + 1)
        .HP = 50
        .AI = 3
        .AIData = 0
        .X = BackRect.Right / 2
        .Y = -32
        .SetPic 8
    End With
End Sub

Public Sub PlaySound(Sound As DirectSoundBuffer)
    If DisableSound Then Exit Sub
    Sound.SetCurrentPosition 0
    Sound.Play DSBPLAY_DEFAULT
End Sub
