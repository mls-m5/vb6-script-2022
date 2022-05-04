

Sub Main()
    Dim X As Long

    If 1 Then X = 10 Else X = 20

    Assert X = 10

    If 0 Then X = 30 Else X = 40

    Assert X = 40

    If 1 Then X = 50

    Assert X = 50

End Sub