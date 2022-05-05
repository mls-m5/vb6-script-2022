VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "Shot"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Dim i As IPictureDisp, W As Integer
Public Range%, num As Byte, xSpeed&, ySpeed&, X&, Y&, FromEx As Long, Target As Ship

Public Sub Display()
    With frmScreen.Pictures
        '.FastDisplay X - xSpeed * 4 - 16, Y - ySpeed * 4 - 16, 4
        '.FastDisplay X - xSpeed * 2 - 16, Y - ySpeed * 2 - 16, 3
        '.FastDisplay X - 16, Y - 16, 2
        .DDLine X, Y, X - xSpeed, Y - ySpeed, 1, vbYellow
        '.FastDisplay X - xSpeed - 16, Y - ySpeed - 16, 2
        '.FastDisplay X - xSpeed * 3 - 16, Y - ySpeed * 3 - 16, 3
    End With
End Sub

Private Sub Class_Initialize()
    Set i = frmScreen.Image2
    W = i.Width / 2
    frmScreen.CreateShot Me
End Sub

Public Sub TimeTab()
    Dim hitNum As Integer, hitnum2 As Byte
    hitNum = frmScreen.isHit(X, Y)
    If hitNum > -1 Then
        hitnum2 = hitNum
        frmScreen.Damage hitnum2, 3
        frmScreen.RemoveShot num
        'frmScreen.SmallExplotion X - xSpeed, Y - ySpeed, -xSpeed / 3, -ySpeed / 3
        If FromEx = 0 Then
            frmScreen.SmallExplotion X - xSpeed, Y - ySpeed, -xSpeed / 3, -ySpeed / 3
        End If
    End If
    
    If Not Target Is Nothing Then
        With Target
            If .X + 16 <> X And Y + 16 <> .Y Then
                xSpeed = (.X - X + 16) / ((.X - X + 16) ^ 2 + (.Y - Y + 16) ^ 2) ^ 0.5 * 20 + .xSpeed
                ySpeed = (.Y - Y + 16) / ((.X - X + 16) ^ 2 + (.Y - Y + 16) ^ 2) ^ 0.5 * 20 + .ySpeed
            End If
        End With
    End If
    X = X + xSpeed
    Y = Y + ySpeed
    
    Range = Range - 1
    If Range < 0 Then
        frmScreen.RemoveShot num
        frmScreen.Pictures.FastDisplay X - 16, Y - 16, 5
        If FromEx = 2 Then frmScreen.Explotion (X), (Y)
    End If
End Sub

