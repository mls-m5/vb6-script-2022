Private Structure Sapa
    X as Integer
    Y as Integer
End Structure

Sub Main()
    Dim S as Sapa
    With S
        .X = 10
        .Y = .X
    End With

    Assert S.X = 10
    Assert S.Y = 10
End Sub
