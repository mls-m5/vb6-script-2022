Attribute VB_Name = "CPU"
Public Type AIData
    CurrentMethod As Long
End Type

Dim Player As Ship

Public Sub Start()
    Set Player = frmScreen.Player
End Sub


Public Sub Move(Obj As Ship, MoveVariant As Integer)
    Dim nRnd As Single
    
    Select Case MoveVariant
        Case 0
            Select Case Obj.AIData
                Case 6
                    If Obj.Y > BackRect.Bottom / 3 Then Obj.ySpeed = Obj.ySpeed - 1 Else Obj.ySpeed = Obj.ySpeed + 1
                    If Obj.X > Player.X Then Obj.xSpeed = Obj.xSpeed - 1 Else Obj.xSpeed = Obj.xSpeed + 1
                    Obj.Fire 40
                    If Obj.X < 0 Then Obj.xSpeed = 1
                    If Obj.X > BackRect.Right Then Obj.xSpeed = -1
                Case 5
                    Obj.ySpeed = 2
                    Obj.xSpeed = 0
                    'frmScreen.Explotion Obj.X + 15, Obj.Y + 15
                    If Obj.Y > BackRect.Bottom / 1.5 Then Obj.AIData = 4
                Case 4
                    Randomize
                    Obj.ySpeed = 0
                    'If Player.X > Obj.X Then
                    '    Obj.xSpeed = Obj.xSpeed + Rnd
                    'Else
                    '    Obj.xSpeed = Obj.xSpeed - Rnd
                    'End If
                    Obj.AIData = 6
                Case 3
                    Randomize
                    Obj.xSpeed = 0
                    Obj.ySpeed = 2
                    Obj.Fire 50
                    If Obj.Y > BackRect.Bottom / 4 Then Obj.AIData = 5
                Case 2
                    Randomize
                    Obj.ySpeed = 3
                    If Player.X > Obj.X Then
                        Obj.xSpeed = 1
                    Else
                        Obj.xSpeed = -1
                    End If
                    Obj.Fire 50
                    If Obj.Y > BackRect.Bottom / 5 Then Obj.AIData = 3
                Case 1
                    Obj.ySpeed = 4
                    'Obj.Fire
                    If Obj.Y > 150 Then Obj.AIData = 2
                Case 0
                    Obj.ySpeed = 6
                    'Obj.Fire
                    If Obj.Y > BackRect.Bottom / 12 Then Obj.AIData = 1
            End Select
        Case 1
            Select Case Obj.AIData
                Case 0
                    Obj.ySpeed = 2
                    If Obj.Y > BackRect.Bottom / 4 Then
                        Obj.AIData = 1
                        Obj.xSpeed = -3
                        Obj.ySpeed = 0
                    End If
                Case 1
                    If Obj.xSpeed = 3 Then
                        If Obj.X > BackRect.Right - 32 Then Obj.xSpeed = -3
                    Else
                        If Obj.X < 0 Then
                            Obj.xSpeed = 3
                            Obj.X = 0
                        End If
                    End If
                    If Rnd > 0.7 Then Obj.Fire 10, Player Else Obj.Fire 10
                    If Obj.Y < 180 Then Obj.AIData = 0
            End Select
        Case 2
            Select Case Obj.AIData
                Case 0
                    nRnd = Rnd
                    Obj.ySpeed = 6
                    frmScreen.CreateShot2 Obj.X, Obj.Y + 32, 20 * nRnd - 10, 20, 20, 1
                    frmScreen.CreateShot2 Obj.X + 32, Obj.Y + 32, 20 * nRnd - 10, 20, 20, 1
                    If Obj.X > Player.X Then Obj.xSpeed = -10 Else Obj.xSpeed = 10
                    If Obj.X > Player.X - 16 And Obj.X < Player.Y + 16 Then Obj.xSpeed = 0
            End Select
    End Select
End Sub
