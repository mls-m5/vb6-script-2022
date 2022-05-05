VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "Ship"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Dim i As Byte, FireRate As Integer
Public HP%, num As Byte, X%, Y%, xSpeed&, ySpeed&, AI  As Long, AIData As Long

Private Sub Class_Initialize()
    i = 1
    frmScreen.CreateShip Me
End Sub

Public Sub Display()
    'frmScreen.PaintPicture i, X, Y
    frmScreen.Pictures.FastDisplay (X), (Y), (i)
End Sub

Public Sub TimeTab()
    X = X + xSpeed
    Y = Y + ySpeed
    
    If AI Then
        CPU.Move Me, AI - 1
    End If
    
    If Y > 740 Then frmScreen.Explotion X + 15, Y + 15
End Sub

Public Sub SetPicPlayer()
    'Set i = frmScreen.Image4
    i = 0
End Sub

Public Sub Fire(FireRate1 As Long, Optional Target As Ship)
    Dim nShot As shot, plX%, plY%, mX%, mY%, Total%, isX%
    
    If FireRate > 0 Then
        FireRate = FireRate - 1
        Exit Sub
    End If
    
    Randomize
    FireRate = FireRate1
    
    Set nShot = New shot
    
    plX = frmScreen.Player.X + 15
    plY = frmScreen.Player.Y + 15
    
    mX = X + 15
    mY = Y + 30
    
    plX = plX - mX
    plY = plY - mY
    
    If plX < 0 Then
        plX = -plX
        isX = -1
    Else
        isX = 1
    End If
    
    Total = plX + plY
    
    nShot.xSpeed = (plX / Total) * isX * 15
    nShot.ySpeed = (plY / Total) * 15
    
    nShot.X = mX
    nShot.Y = mY
    Set nShot.Target = Target
    
    nShot.Range = 100
    
    frmScreen.PlaySound frmScreen.sndEnemyshot
End Sub

Public Sub SetPic(Pic As Byte)
    i = Pic
End Sub
