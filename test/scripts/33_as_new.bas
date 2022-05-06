' TestImport Class33

Dim X As New Class33

Sub Main()
    Dim Y as New Class33

    Assert Not X Is Nothing
    Assert Not Y Is Nothing

    Y.X = 10

    Assert Y.X
End Sub